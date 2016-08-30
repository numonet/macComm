//////////////////////////////////////////////////////////////////////////////
//
// Description:		This file includes all functions for TDT layer.
//                      It's ported from Matlab project.
//
// Version:		V1.0
//
// Author:		Mian Tang
//
// Date:		05/10/2015
//
// Comment:
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "Config.h"
#include "td_func.h"
#include "tda_func.h"
#include "tdt_func.h"
#include "Common.h"





#define MESSAGE_LEN_PING		6
#define ADDRESS_BOOK_LEN		128
#define PING_QUEUE_LEN			128
#define TX_QUEUE_LEN			128
#define GENERAL_PAYLOAD_SIZE		6


#define NUM_PEER_INDEX			12
#define ADDRESS_INIT_LOC		16
#define ABSENCE_DELETE			2
#define RANGE_ADDR_LOC			2
#define RANGE_DIST_INDEX		5


#define MAX_NUM_OF_ACK			8



#define NUM_OF_ACKS			5
#define ACK_TIMEOUT			35




////////////////////////////////////////////////////////////////////////////
//
// Extern Global Variables
//
///////////////////////////////////////////////////////////////////////////













//////////////////////////////////////////////////////////////////////////
//
// Local Global Variables
//
//////////////////////////////////////////////////////////////////////////

// Configuralbe variables
static float 				prob_ping_threshold = 0.0001;
static float				optNodeProb = 0;
static float				clock_diff = 0;
static float				packet_rate = 0.04;
static float				T_packet = 1.8;
static float				P_ALOHA	= 0.25;
static float				T_slot = 2;
static char				Tx_Enable = 0;
static char				Master_node = 17;
static char				XID = 13;
static char				Ping_Enable = 0;
static char				Auto_Ping = 0;
static char				Auto_Sync = 0;
static int				dataAcks = 0;
static int				TARS_retrans_timeout = 45;
static float				TARS_I = 0.0;
static int				mac_protocol = 4;		// 1: Aloha; 2: Slotted Aloha; 3: TARS; 4: LISS
static int				Packet_Gen_Counter = 0;
static int				packets_produced = 0;
static int				last_packet_TX_time = 0;
static int				Ping_queue_woffset = 0;
static int				Ping_queue_roffset = 0;
static int				Tx_queue_Notify_woffset = 0;
static int				Tx_queue_Notify_roffset = 0;


static char				localAddr;
static char				addrbook_counter = 0;


static float				optNodeProb_GUI = 0;
static int				packets_received = 0;


static int				dedicated_ack_cnt = 0;




static struct def_state			state;
static struct def_queue_length		queue_length;
static struct def_Ping_queue		Ping_queue[PING_QUEUE_LEN];
static struct def_Ping_queue		Tx_queue_Notify[ADDRESS_BOOK_LEN];
static struct def_Addr_book		Addr_book[ADDRESS_BOOK_LEN];
static struct def_Tx_queue		Tx_queue[TX_QUEUE_LEN];
static struct def_Time_Table		TimeTable[ADDRESS_BOOK_LEN];
static struct def_Q_empty		Q_empty[ADDRESS_BOOK_LEN];
static struct def_Rx_queue		Rx_queue[ADDRESS_BOOK_LEN];
static struct def_delays		delays[ADDRESS_BOOK_LEN];
static struct def_TimerCounter		TimerCounter;





/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_getModemByAddr
//
// Description:		It's used to look up the address from address book
//                      
//
// Parameter:
//        Input:
//			addr:		Address to be found
//        Output:
//			index:		the offset in address book
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int tdt_getModemByAddr(int addr, int* index)
{
    struct def_Addr_book* pAddrBook;
    int i;

    *index = -1;
    pAddrBook = &Addr_book[0];
    for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
        if (addr == pAddrBook->address) {
            *index = i;
            break;
        }
        else {
            ++ pAddrBook;
        }
    }


    return 0;
}


/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_addAddrToBook
//
// Description:		It's used to add address to the book
//                      
//
// Parameter:
//        Input:
//			dataPacket:	The data packet to be added
//                                      or other kind of data
//                      callFrom:	The command
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int tdt_addAddrToBook(char* dataPacket, char* callFrom)
{
    unsigned int j, ui_err = 0;
    struct def_Addr_book* bookptr;
    struct def_Tx_queue* txqueueptr;
    int i, numPeers, distance, offset, address;
    int addrIndex;
    char* ptrNum;

    if (strncmp(callFrom, "Ping", 4) == 0) {
        addrIndex = 0;
        numPeers = (int)dataPacket[NUM_PEER_INDEX - 1];       
        for (i = 0; i < numPeers; i ++) {
            address = (int)dataPacket[ADDRESS_INIT_LOC + (i * 8) - 1];
            distance = 0;
            for (j = 0; j < 4; j ++) {
                distance += (int)(dataPacket[ADDRESS_INIT_LOC + (i * 8) + j]) * ((256^(3-j)) / 10);
            }
            
            tdt_getModemByAddr(address, &addrIndex);
            if (addrIndex == -1) {
                bookptr = &Addr_book[0];
                for (j = 0; j < ADDRESS_BOOK_LEN; j ++) {
                    if (bookptr->address == 0) {
                        bookptr->address = address;
                        bookptr->distance = distance;
                        bookptr->absent = 0;
                        bookptr->lastPkt = '0';
                        tda_clock(clock_diff, "char", bookptr->timeLastPkt, NULL);
                        offset = j;
                        ++ addrbook_counter;
                        break;
                    }
                    else {
                       ++ bookptr;
                    }
                }
                
            }
            else {
                Addr_book[addrIndex].absent = 0;
                offset = addrIndex;
            }            
        }
        // Update absent info
        bookptr = &Addr_book[0];
        for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
            if (bookptr->address != 0) {
                bookptr->absent += 1;
                if (bookptr->absent > ABSENCE_DELETE) {
                    bookptr->address = 0;
                    memset(bookptr->timeLastPkt, 0, sizeof(bookptr->timeLastPkt));
                    -- addrbook_counter;
                }
            }
            ++ bookptr;
        }
        // Adjust Ping_queue ???
        Com_Printf("Modems added to address book via Ping Response", 5);
    }
    else if (strncmp(callFrom, "Range", 5) == 0) {
        address = (int)(dataPacket[RANGE_ADDR_LOC - 1]);
        distance = 0;
        for (i = 0; i < 4; i ++) {
            distance += dataPacket[RANGE_DIST_INDEX + i] * ((256^(4-1-i)) / 10);
        }

        tdt_getModemByAddr(address, &addrIndex);
        if (addrIndex == -1) {
            bookptr = &Addr_book[0];
            for (j = 0; j < ADDRESS_BOOK_LEN; j ++) {
                if (bookptr->address == 0) {
                    bookptr->address = address;
                    bookptr->distance = distance;
                    bookptr->absent = 0;
                    bookptr->lastPkt = '0';
                    tda_clock(clock_diff, "char", bookptr->timeLastPkt, NULL);
                    offset = j;
                    ++ addrbook_counter;
                    break;
                }
                else {
                    ++ bookptr;
                }
            }
            txqueueptr = &Tx_queue[0];
            for (j = 0; j < ADDRESS_BOOK_LEN; j ++) {
                if (txqueueptr->address == 0) {
                    txqueueptr->length = 0;
                    txqueueptr->counter = 1;
                    txqueueptr->address = address;
                    txqueueptr->Buffer_len = 0;
                    memset(txqueueptr->Buffer_Retries, 0, sizeof(txqueueptr->Buffer_Retries));
                    break;
                }
                else {
                    ++ txqueueptr;
                }
            }
            
        }
        Com_Printf("Modem added to address book via Range Update", 5);
    }
    else if (strncmp(callFrom, "justAddr", 8) == 0) {
        ptrNum = dataPacket;
        address = ((int)(ptrNum[0] - 0x30) * 10) + (int)(ptrNum[1] - 0x30);
        tdt_getModemByAddr(address, &addrIndex);
        if (addrIndex == -1) {
            bookptr = &Addr_book[0];
            for (j = 0; j < ADDRESS_BOOK_LEN; j ++) {
                if (bookptr->address == 0) {
                    bookptr->address = address;
                    bookptr->distance = -1;
                    bookptr->absent = 0;
                    bookptr->lastPkt = '0';
                    tda_clock(clock_diff, "char", bookptr->timeLastPkt, NULL);
                    offset = j;
                    ++ addrbook_counter;
                    break;
                }
                else {
                    ++ bookptr;
                }
            }
            txqueueptr = &Tx_queue[0];
            for (j = 0; j < TX_QUEUE_LEN; j ++) {
                if (txqueueptr->address == 0) {
                    txqueueptr->length = 0;
                    txqueueptr->counter = 1;
                    txqueueptr->address = address;
                    txqueueptr->Buffer_len = 0;
                    memset(txqueueptr->Buffer_Retries, 0, sizeof(txqueueptr->Buffer_Retries));
                    break;
                }
                else {
                    ++ txqueueptr;
                }
            } 
        }
        else {
            Addr_book[addrIndex].absent = 0;
            offset = addrIndex;
        }
        Com_Printf("Modem added to address book via justAddr", 5);
    }

    if (addrIndex == -1) {
        memset(TimeTable[offset].Gen, 0, TIMETABLE_LENGTH);
        memset(TimeTable[offset].Sent, 0, TIMETABLE_LENGTH);
        memset(TimeTable[offset].Tx_count, 0, TIMETABLE_LENGTH);

        Q_empty[offset].address = Addr_book[offset].address;
        Q_empty[offset].out = 0.0;
        Q_empty[offset].in = 0;
        Q_empty[offset].I = 1;

        memset(&Rx_queue[offset], 0, sizeof(struct def_Rx_queue));
        //Rx_queue[offset].counter = 0;
        //Rx_queue[offset].length = 0;
        //memset(Rx_queue[offset].PacketList, 0, sizeof(Rx_queue[offset].PacketList));
        //memset(RX_queue[offset].PacketTime, 0, sizeof(RX_queue[offset].PacketTime));

        memset(delays[offset].total, 0, sizeof(delays[offset].total));
    }



    return ui_err;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_data_rcvd
//
// Description:		It's used to calculate the time difference, and 
//                      adjust the time of the modem for synchronization
//                      
//
// Parameter:
//        Input:
//			msgptr:		The pointer to the message
//                      signature:	the signature of the message
//                      masterNode:	the address of Master Node
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int tdt_data_rcvd(char* msgptr, char* signature, char masterNode)
{
    unsigned int com_flag, ui_err = 0;
    char pktType, counter, sender, packetno, check_flag;
    char timeRcv[2], clock_diff_msg[4];
    int i, j, t1_num, t2_num, t3_num, t4_num;
    int num_of_ack,  quant, offset, addrIndex, packet;
    int timeProd_num, timeTx_num, timeRcv_num, timeDelay_total, timeDelay_Tx;
    float clock_diff_num;
 

    com_flag = 0;
    pktType = msgptr[8];
    if (pktType == 254) {
        // Sync Packet
        counter = msgptr[9];
        if (localAddr == masterNode) {
            sender = msgptr[1];
            tda_clock(clock_diff, "num", NULL, &t4_num);
            t1_num = ((int)msgptr[10] << 8) + (int)msgptr[11];
            t2_num = ((int)msgptr[12] << 8) + (int)msgptr[13];
            t3_num = ((int)msgptr[14] << 8) + (int)msgptr[15];
            clock_diff_num = ((float)((t4_num - t3_num) - (t2_num - t1_num))) / 2;
            quant = ((int)(clock_diff_num * 10)) % 65536;
            clock_diff_msg[0] = 254;
            clock_diff_msg[1] = 3;
            clock_diff_msg[2] = (char)(quant >> 8);
            clock_diff_msg[3] = (char)quant;
            // Send command
            com_flag = 1;
            //tda_remSendData(XID, sender, clock_diff_msg, 4);
        }
        else {
            if (counter == 1) {
                clock_diff_msg[0] = 254;
                clock_diff_msg[1] = 2;
                clock_diff_msg[2] = msgptr[10];
                clock_diff_msg[3] = msgptr[11];
                tda_clock(clock_diff, "char", &clock_diff_msg[4], NULL);
                Com_Sleep(1);
                tda_clock(clock_diff, "char", &clock_diff_msg[6], NULL);
                // Send command
                com_flag = 2;
                //tda_remSendData(XID, masterNode, clock_diff_msg, 8);
            }
            else if (counter == 3) {
                Com_MutexLock();
                //clock_diff = (((float)((int)msgptr[10] << 8)) + ((float)msgptr[11])) / 100;
                clock_diff = ((float)(((int)msgptr[10] << 8) + (int)msgptr[11])) / 100;
                if (clock_diff > 327.68) {
                    clock_diff -= 655.36;
                }
                Com_MutexUnlock();
                printf("Adjusting the clock by %f.\r\n", clock_diff);
                Tx_Enable = 1;
            }
        } 
    }
    else if (pktType == 253) {
        // Dedicated ACK packet, Format: | Packet Flag(253) 1B | Reserved 1B | Number of ACKs 1B | ACK1 1B | ACK2 1B | .... |
        printf("******** Received Dedicated ack: 0x%x, 0x%x, 0x%x. ********\r\n", msgptr[11], msgptr[12], msgptr[13]);
        num_of_ack = (unsigned int)msgptr[10];
        num_of_ack = num_of_ack < MAX_NUM_OF_ACK ? num_of_ack : MAX_NUM_OF_ACK;
        sender = (signature[9] - 0x30) * 10 + (signature[10] - 0x30);
        tdt_getModemByAddr(sender, &addrIndex);
        //printf("Teh addrIndex is 0x%x, the num_of_ack is 0x%x.\r\n", addrIndex, num_of_ack);
        if (addrIndex != -1) {  
            for (i = 0; i < num_of_ack; i ++) {
                // remove the packet from buffer queue
                for (packet = 0; packet < 32; packet ++) {
                    packetno = Tx_queue[addrIndex].Buffer_payload[packet][0];
                    //printf("packetno is 0x%x.\r\n", packetno);
                    if ((packetno == msgptr[11 + i]) && (msgptr[11 + i] != 0)) {
                        //printf("The buffer len is 0x%x.\r\n", Tx_queue[addrIndex].Buffer_len);
                        Tx_queue[addrIndex].Buffer_len -= 1;
                        memset(Tx_queue[addrIndex].Buffer_payload[packet], 0, 32);
                        Tx_queue[addrIndex].Buffer_time[packet] = 0;
                        Tx_queue[addrIndex].Buffer_Retries[packet] = 0;
                        Tx_queue[addrIndex].Buffer_flag[packet] = 0;
                        if (packet < 31) {
                            for (j = packet; j < (32 -1); j ++) {
                                memcpy(Tx_queue[addrIndex].Buffer_payload[j], Tx_queue[addrIndex].Buffer_payload[j + 1], 32);
                                Tx_queue[addrIndex].Buffer_time[j] = Tx_queue[addrIndex].Buffer_time[j + 1];
                                Tx_queue[addrIndex].Buffer_Retries[j] = Tx_queue[addrIndex].Buffer_Retries[j + 1];
                                Tx_queue[addrIndex].Buffer_flag[j] = Tx_queue[addrIndex].Buffer_flag[j + 1];
                            }
                            Tx_queue[addrIndex].Buffer_flag[31] = 0;
                            memset(Tx_queue[addrIndex].Buffer_payload[31], 0, 32);
                        }
                        break;
                    }
                }
            }
        }
    }
    else {
        Com_MutexLock();
        sender = (signature[9] - 0x30) * 10 + (signature[10] - 0x30);
        tdt_getModemByAddr(sender, &addrIndex);
        if (addrIndex != -1) {
            Q_empty[addrIndex].in = ((float)msgptr[16]) / 255;
            Q_empty[addrIndex].I = ((float)msgptr[17]) / 51;
            // Read timing info, find delay and clock drift
            tda_clock(clock_diff, "char", timeRcv, NULL);
            timeRcv_num = (((int)timeRcv[0] << 8) + (int)timeRcv[1]) / 10;
            timeProd_num = (((int)msgptr[9] << 8) + (int)msgptr[10]) / 10;
            timeTx_num = (((int)msgptr[14] << 8) + (int)msgptr[15]) / 10;
            timeDelay_total = (timeRcv_num - timeProd_num) % 3600;
            timeDelay_Tx = (timeRcv_num - timeTx_num) % 3600;
           
            printf("Notify: Data Packet, pktNum received from sender %d.\r\n", sender);
            printf("	Notify: Message: Packet type: %d. ACK info: 0x%x, 0x%x, 0x%x.\r\n", pktType, msgptr[18], msgptr[19], msgptr[20]);
            printf("	Transmission took: %ds and %ds in Tx. Time Produce: 0x%x, 0x%x. Time Tx: 0x%x, 0x%x. Time receive: 0x%x, 0x%x.\r\n", 
                                             timeDelay_total, timeDelay_Tx, msgptr[9], msgptr[10], msgptr[14], msgptr[15], timeRcv[0], timeRcv[1]);


            for (i = 0; i < NUM_OF_ACKS; i ++) {
                // remove the packet from buffer queue
                if (msgptr[18 + i] != 0) {
                    for (packet = 0; packet < 32; packet ++) {
                        packetno = Tx_queue[addrIndex].Buffer_payload[packet][0];
                        if (packetno == msgptr[18 + i]) {
                            Tx_queue[addrIndex].Buffer_len -= 1;
                            memset(Tx_queue[addrIndex].Buffer_payload[packet], 0, 32);
                            Tx_queue[addrIndex].Buffer_time[packet] = 0;
                            Tx_queue[addrIndex].Buffer_Retries[packet] = 0;
                            Tx_queue[addrIndex].Buffer_flag[packet] = 0;
                            if (packet < 31) {
                                for (j = packet; j < (32 -1); j ++) {
                                    memcpy(Tx_queue[addrIndex].Buffer_payload[j], Tx_queue[addrIndex].Buffer_payload[j + 1], 32);
                                    Tx_queue[addrIndex].Buffer_time[j] = Tx_queue[addrIndex].Buffer_time[j + 1];
                                    Tx_queue[addrIndex].Buffer_Retries[j] = Tx_queue[addrIndex].Buffer_Retries[j + 1];
                                    Tx_queue[addrIndex].Buffer_flag[j] = Tx_queue[addrIndex].Buffer_flag[j + 1];
                                }
                                Tx_queue[addrIndex].Buffer_flag[31] = 0;
                                memset(Tx_queue[addrIndex].Buffer_payload[31], 0, 32);
                            }
                            break;
                        }
                    }
                }
            }

            // include the received packet in the ack buffer
            check_flag = 0;
            for (i = 0; i < RXPACKET_LENGTH; i ++) {
                if (Rx_queue[addrIndex].PacketList[i] == pktType) {
                    check_flag = 1;
                }
            }
            if (check_flag == 0) {
                for (i = 0; i < RXPACKET_LENGTH; i ++) {
                    if (Rx_queue[addrIndex].PacketList[i] == 0) {
                        Rx_queue[addrIndex].PacketList[i] = pktType;
                        Rx_queue[addrIndex].PacketTime[i] = timeRcv_num;
                        Rx_queue[addrIndex].length += 1;
                        break;
                    }
                }
            }
            printf("So far, the accumulated ACKs are: ");
            for (i = 0; i < RXPACKET_LENGTH; i ++) {
                if (Rx_queue[addrIndex].PacketList[i] != 0) {
                    printf("0x%x, ", Rx_queue[addrIndex].PacketList[i]);
                    printf("Time is %ds. ", Rx_queue[addrIndex].PacketTime[i]);
                }
            }
            printf("\r\n");
            Addr_book[addrIndex].lastPkt = pktType;
            memcpy(Addr_book[addrIndex].timeLastPkt, timeRcv, sizeof(timeRcv));
            
            // Store the delay info to delay buffer for the calculation of average delay
            packets_received += 1;
            offset = delays[addrIndex].num;
            delays[addrIndex].total[offset] = timeDelay_total;
            delays[addrIndex].propagation[offset] = timeDelay_Tx;
            delays[addrIndex].GenTime[offset] = timeProd_num;
            delays[addrIndex].Received[offset] = 1;
            if (offset == DELAYBUFFER_LENGTH - 1) {
                delays[addrIndex].num = 0;
            }
            else {
                delays[addrIndex].num = offset + 1;
            }
        }
        else {
            printf("tdt_data_rcvd function: cannot find index when calling tdt_getModemByAddr function.\r\n");
        }
        Com_MutexUnlock();
    }

    if (com_flag == 1) {
        tda_remSendData(XID, sender, clock_diff_msg, 4);
    }
    else if (com_flag == 2) {
        tda_remSendData(XID, masterNode, clock_diff_msg, 8);
    }
    

    return ui_err;
}

/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_notify_to_file
//
// Description:		It's used to write the log received from Modem
//                      to the file
//                      
//
// Parameter:
//        Input:
//			dataPacket:	The data to be written
//                      type:		1: modem packet
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int tdt_notify_to_file(void* dataPacket, char type)
{
    unsigned int total, i, ui_err = 0;
    struct modemPacket* pmPkt;
    char log_buf[1024];
    char* logptr;
    int len;

    total = 0;
    if (type == 1) {
        pmPkt = (struct modemPacket*)dataPacket;
        logptr = log_buf;
        memset(log_buf, 0, sizeof(log_buf));
        Com_Clock(logptr, &len);
        logptr += len;
        total += len;
        len = sprintf(logptr, "xid: %d\n", pmPkt->xid);
        logptr += len;
        total += len;
        len = sprintf(logptr, "type: %d\n", pmPkt->type);
        logptr += len;
        total += len;
        len = sprintf(logptr, "numParam: %d\n", pmPkt->numParam);
        logptr += len;
        total += len;
        for (i = 0; i < pmPkt->numParam; i ++) {
            len = sprintf(logptr, "  subsys: %d\n", pmPkt->subsys[i]);
            logptr += len;
            total += len;
            len = sprintf(logptr, "  field: %d\n", pmPkt->field[i]);
            logptr += len;
            total += len;
            len = sprintf(logptr, "  message len: %d\n", pmPkt->len[i]);
            logptr += len;
            total += len;
            len = sprintf(logptr, "  message: %s\n", pmPkt->message[i]);
            logptr += len;
            total += len;
        }
        len = sprintf(logptr, " \n\n");
        total += len;
        // Write to file
        Com_fileWrite(log_buf, total); 
    }


    return ui_err;
}


/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_doSync
//
// Description:		It's used to synchronize the time with each modem
//                      based on the time of Master node.
//                      
//
// Parameter:
//        Input:
//			dataPacket:	The data to be written
//                      type:		1: modem packet
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_doSync(void)
{
    unsigned int i, ui_err;
    char time_char[2], modemAddr[ADDRESS_BOOK_LEN], message[32];
    char* ptraddr;

    memset(modemAddr, 0, sizeof(modemAddr));
    ptraddr = modemAddr;
    if (localAddr == Master_node) {
        // Wait for 70 seconds in order to get the address book filled from
        // the packet sent by each modem.
        Com_Sleep(70);
        // Check address book and get the valid modem address
        memset(message, 0, sizeof(message));
        Com_MutexLock();
        for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
            if (Tx_queue[i].address != 0) {
                *ptraddr ++ = Tx_queue[i].address;
            }
        }
        Com_MutexUnlock();
        // Send sync packet to each modem existed
        ptraddr = modemAddr;
        for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
            if (*ptraddr ++ != 0) {
                tda_clock(clock_diff, "char", time_char, NULL);
                message[0] = 254;
                message[1] = 1;
                message[2] = time_char[0];
                message[3] = time_char[1];
                ui_err = tda_remSendData(XID, Tx_queue[i].address, message, 4);
                Com_Sleep(8);
            }
            else {
                break;
            }
        }
    }

    return ui_err;
}



unsigned int tdt_txEnable(char enable)
{
    Com_MutexLock();
    Tx_Enable = enable;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_pingEnable(char enable)
{
    Com_MutexLock();
    Ping_Enable = enable;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_packetRate(float rate)
{
    Com_MutexLock();
    packet_rate = rate;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_retransTimeout(int retrans)
{
    Com_MutexLock();
    TARS_retrans_timeout = retrans;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_protocol(int macProtocol)
{
    Com_MutexLock();
    mac_protocol = macProtocol;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_xidIs(char xid__)
{
    Com_MutexLock();
    XID = xid__;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_autoPing(char ping)
{
    Com_MutexLock();
    Auto_Ping = ping;
    Com_MutexUnlock();


    return 0;
}


unsigned int tdt_autoSync(char sync)
{
    Com_MutexLock();
    Auto_Sync = sync;
    Com_MutexUnlock();


    return 0;
}



static unsigned int tdt_findMaxPktTime(int reftime, int* p_addrIndex, int* p_offset)
{
    int addrLoop, pktLoop, difference;
    int addrIndex, offset;
    struct def_Rx_queue* pRxqueue;


    addrIndex = offset = 0;
    difference = 0;
    for (addrLoop = 0; addrLoop < ADDRESS_BOOK_LEN; addrLoop ++) {
        pRxqueue = &Rx_queue[addrLoop];
        if (pRxqueue->length != 0) {
            for (pktLoop = 0; pktLoop < RXPACKET_LENGTH; pktLoop ++) {
                if (pRxqueue->PacketTime[pktLoop] != 0) {
                    if ((reftime - pRxqueue->PacketTime[pktLoop]) > difference) {
                        addrIndex = addrLoop;
                        offset = pktLoop;
                        difference = reftime - pRxqueue->PacketTime[pktLoop];
                    }
                }
            }
        }
    }
    *p_addrIndex = addrIndex;
    *p_offset = offset;


    return 0;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_modemInit
//
// Description:		It's used to initialize modem and other variables
//                      
//
// Parameter:
//        Input:
//			macProtocol:	MAC layer protocol
//					1: Aloha
//                                      2: Slotted Aloha
//                                      3: TARS
//                                      4: LISS
//                      txEnable:	0: disable; 1: enable
//                      pingEnable:	0: disable; 1: enable
//                      xid__:		XID
//                      packetRate:	packet rate
//
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_modemInit(int macProtocol, char txEnable, char pingEnable, char xid__, float packetRate)
{
    unsigned int i, ui_err = 0;
    char sniffer_mode, dataRetrans;

    tda_modemInit();
    tda_getLocalAddr(0x31, &localAddr);
    printf("Modem Local Address is 0x%x.\r\n", localAddr);
    tda_getSnifferMode(24, &sniffer_mode);
    if (sniffer_mode == 1) {
        printf("Modem Sniffer Mode is ON.\r\n");
    }
    else {
        printf("Modem Sniffer Mode is OFF.\r\n");
    }

    dataAcks = 0;
    dataRetrans = 0;
    if (tda_transmitInit(16, 17, dataAcks, dataRetrans) == 0) {
        printf("ACK and Retransmission has been initialized.\r\n");
    }
    else {
        ui_err = 1;
        printf("ACK and Retransmission initilzation error.\r\n");
    }
    // Initialize Global Variables
    Ping_queue_woffset = Ping_queue_roffset = 0;
    Tx_queue_Notify_woffset = Tx_queue_Notify_roffset = 0;
    addrbook_counter = 0;
    packets_received = 0;
    packets_produced = 0;
    dedicated_ack_cnt = 0;
    prob_ping_threshold = 0.0001;
    optNodeProb = 0;
    optNodeProb_GUI = 0;
    last_packet_TX_time = 0;
    T_packet = 1.8;
    TARS_I = 0.0;
    clock_diff = 0;
    TARS_retrans_timeout = 45;
    Packet_Gen_Counter = 0;
    Master_node = 17;
    Tx_Enable = txEnable;
    Ping_Enable = pingEnable;
    packet_rate = packetRate;
    XID = xid__;
    mac_protocol = macProtocol;
    if (mac_protocol == 1) {
        T_slot = 0.05;
        P_ALOHA = 1/4;
    }
    else {
        T_slot = 2;
        P_ALOHA = 1/3;
    }
    
    memset(&state, 0, sizeof(struct def_state));
    memset(&queue_length, 0, sizeof(struct def_queue_length));
    memset(&TimerCounter, 0, sizeof(struct def_TimerCounter));    
    for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
        memset(&Ping_queue[i], 0, sizeof(struct def_Ping_queue));
        memset(&Tx_queue_Notify[i], 0, sizeof(struct def_Ping_queue));
        memset(&Addr_book[i], 0, sizeof(struct def_Addr_book));
        memset(&Tx_queue[i], 0, sizeof(struct def_Tx_queue));
        memset(&TimeTable[i], 0, sizeof(struct def_Time_Table));
        memset(&Q_empty[i], 0, sizeof(struct def_Q_empty));
        memset(&Rx_queue[i], 0, sizeof(struct def_Rx_queue));
        memset(&delays[i], 0, sizeof(struct def_delays));
    }

    if ((Auto_Ping == 1) && (Master_node == localAddr)) {
        state.Ping = 10;
    }

    // Create log file
    Com_fileInit("/root/log.txt");



    return ui_err;
}



unsigned int tdt_syncTask(void)
{
    unsigned int flag;



    if (Auto_Sync == 1) {
    }
    else {
        flag = 0;
    }


    return flag;
}




/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_txTask
//
// Description:		It's used to read data from the queue and send
//                      data to the modem.
//                      
//
// Parameter:
//        Input:
//			NONE
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_txTask(void)
{
    int time_waited, time_now, time_elapsed, msglen, Packet_sent_at;
    int i, j, record_num, packet_addr, offset, roffset, temp_retries;
    float probability, time_elapsed_mod;
    char record[ADDRESS_BOOK_LEN], time_char[2], temp_payload[GENERAL_PAYLOAD_SIZE];
    char transmit_now, transmit_from_buffer, transmit_dedicated_ack, timePktSent[2];
    char com_flag, transmit_from_queue, do_not_transmit;
    char xid_, addr_, Qempty, TarsITx, acks[NUM_OF_ACKS], message[32];
    char* msgptr;
    struct def_Tx_queue* pTx_queue;
    struct def_Rx_queue* pRx_queue;


    com_flag = 0;
    TimerCounter.Tx += 1;
    transmit_dedicated_ack = 0;
    transmit_now = transmit_from_buffer = 0;
    transmit_from_queue = do_not_transmit = 0;
    probability = (float)rand() / RAND_MAX;
    Com_MutexLock();
    if((state.Tx == 0) && (state.Ping == 0) && (probability < optNodeProb)) {
        // check the buffer first.  Does a packet need retransmission?
        tda_clock(clock_diff, "char", time_char, NULL);
        time_now = (((int)time_char[0] << 8) + (int)time_char[1]) / 10;
        for (packet_addr = 0; packet_addr < ADDRESS_BOOK_LEN; packet_addr ++) {
            if (Tx_queue[packet_addr].Buffer_len > 0) {
                time_waited = (time_now - Tx_queue[packet_addr].Buffer_time[0]) % 36000;
                if (time_waited > TARS_retrans_timeout) {
                    // we should retransmit this packet at this time
                    if (mac_protocol == 1) { // ALOHA
                        // We need to find the last transmitted packet
                        time_elapsed = (time_now - last_packet_TX_time) % 36000;
                        time_elapsed_mod = fmodf((float)time_elapsed, T_packet);
                        if (time_elapsed_mod < T_slot) {
                            probability = (float)rand() / RAND_MAX;
                            if (probability >= P_ALOHA) {
                                do_not_transmit = 1;
                            }
                        }
                        else {
                            do_not_transmit = 1;
                        }
                    }
                    else if (mac_protocol == 2) { // slotted ALOHA
                        probability = (float)rand() / RAND_MAX;
                        if (probability <= P_ALOHA) {
                            do_not_transmit = 1;
                        }
                    }
                    transmit_from_buffer = 1;
                    transmit_now = 1;
                    pTx_queue = &Tx_queue[packet_addr];
                    pRx_queue = &Rx_queue[packet_addr];
                    break;
                }
            }
        }

        if (transmit_now == 0) {
            record_num = 0;
            if (mac_protocol == 1) { // ALOHA
                tda_clock(clock_diff, "num", NULL, &time_now);
                time_now /= 10;
                time_elapsed = (time_now - last_packet_TX_time) % 36000;
                if ((queue_length.Tx > 0) && (time_elapsed > T_packet)) {
                    for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
                        if ((Addr_book[i].address != 0) && (Tx_queue[i].roffset != Tx_queue[i].woffset)) {
                            record[record_num] = (char)i;
                            ++ record_num;
                        }
                    }
                    offset = (char)(rand() % record_num + 1) - 1;
                    packet_addr = (int)record[offset];
                    transmit_from_queue = 1;
                    transmit_now = 1;
                    pTx_queue = &Tx_queue[packet_addr];
                    pRx_queue = &Rx_queue[packet_addr];
                }
            }
            else if (queue_length.Tx > 0) {
                for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
                    if ((Addr_book[i].address != 0) && (Tx_queue[i].roffset != Tx_queue[i].woffset)) {
                        record[record_num] = i;
                        ++ record_num;
                    }
                }
                offset = (char)(rand() % record_num + 1) - 1;
                packet_addr = (int)record[offset];
                transmit_from_queue = 1;
                transmit_now = 1;
                pTx_queue = &Tx_queue[packet_addr];
                pRx_queue = &Rx_queue[packet_addr];
            }
            else {
                if (dedicated_ack_cnt == 3) {
                    transmit_dedicated_ack = 1;
                    transmit_now = 1;
                    dedicated_ack_cnt = 0;
                }
                else {
                    ++ dedicated_ack_cnt;
                }
            }    
        }
    }

    if ((transmit_now == 1) && (do_not_transmit == 0)) {
        // prepare the data packet
        tda_clock(clock_diff, "char", timePktSent, NULL);
        if (transmit_dedicated_ack == 0) {
            Qempty = (char)(Q_empty[packet_addr].out * 255);
            TarsITx = (char)(TARS_I * 51);
            Packet_sent_at = (((int)timePktSent[0] << 8) + (int)timePktSent[1]) / 10;
            // prepare the data packet
            for (i = 0; i < 3; i ++) {
                if (pRx_queue->length > 0) {
                    // if there is an ack, include it, and remove it from queue
                    for (j = 0; j < RXPACKET_LENGTH; j ++) {
                        if (pRx_queue->PacketList[j] != 0) {
                            acks[i] = pRx_queue->PacketList[j];
                            pRx_queue->PacketList[j] = pRx_queue->PacketTime[j] = 0;
                            pRx_queue->length -= 1;
                            break;
                        }
                    }
                }
                else { // no acks pending, send -3
                    acks[i] = 0;
                }
            }
        }


        // send and remove from queue/Buffer
        if (transmit_from_buffer == 1) {
            memset(message, 0, sizeof(message));
            msgptr = message;
            for (i = 0; i < 32; i ++) {
                if (pTx_queue->Buffer_flag[i] != 0) {
                    memcpy(msgptr, pTx_queue->Buffer_payload[i], GENERAL_PAYLOAD_SIZE);
                    break;
                }
            }
            msgptr += GENERAL_PAYLOAD_SIZE;
            msgptr[0] = timePktSent[0];
            msgptr[1] = timePktSent[1];
            msgptr[2] = Qempty;
            msgptr[3] = TarsITx;
            msgptr[4] = acks[0];
            msgptr[5] = acks[1];
            msgptr[6] = acks[2];
            msgptr[7] = 0;
            msgptr[8] = 0;
            msglen = GENERAL_PAYLOAD_SIZE + 9;
            xid_ = XID;
            addr_ = pTx_queue->address;
            com_flag = 1;
            printf("sub-payload is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x.\r\n", timePktSent[0], timePktSent[1], Qempty, TarsITx, acks[0], acks[1], acks[2]);
            //tda_remSendData(XID, Tx_queue[packet_addr].address, message, msglen);
            Com_Printf("tdt_txTask: transmit from buffer, is being transmitted to ...\r\n", 5);
            printf("Qempty is 0x%x, TarsITx is 0x%x. Calculated from Q_empty[] is %f, TARS_I is %f.\r\n", Qempty, TarsITx, Q_empty[packet_addr].out, TARS_I);
#if 0
            // Update the time table
            temp = Tx_queue[packet_addr].Buffer_payload[i][0];
            TimeTable[packet_addr].Sent[temp] = (((int)timePktSent[0] << 8) + (int)timePktSent[1]) / 10;
            TimeTable[packet_addr].Tx_count[temp] += 1;
            TimeTable[packet_addr].Tx_prob[temp] = optNodeProb;
#endif
            if (i < 32) {
                // If retrans limit not met, copy the the end of the Buffer queue
                if (pTx_queue->Buffer_Retries[i] < 2) {
                    // temporarily store the info
                    temp_retries = pTx_queue->Buffer_Retries[i];
                    memcpy(temp_payload, pTx_queue->Buffer_payload[i], GENERAL_PAYLOAD_SIZE);
                    // Remove from Buffer
                    pTx_queue->Buffer_time[i] = 0;
                    pTx_queue->Buffer_Retries[i] = 0;
                    pTx_queue->Buffer_flag[i] = 0;
                    memset(pTx_queue->Buffer_payload[i], 0, 32);
                    // Re-arrange the Buffer array
                    for (j = i; j < (32 - 1); j ++) {
                        pTx_queue->Buffer_time[j] = pTx_queue->Buffer_time[j + 1];
                        pTx_queue->Buffer_Retries[j] = pTx_queue->Buffer_Retries[j + 1];
                        pTx_queue->Buffer_flag[j] = pTx_queue->Buffer_flag[j + 1];
                        memcpy(pTx_queue->Buffer_payload[j], pTx_queue->Buffer_payload[j + 1], GENERAL_PAYLOAD_SIZE);
                    }
                    pTx_queue->Buffer_flag[31] = 0;
                    memset(pTx_queue->Buffer_payload[31], 0, 32);

                    // Insert the buffer data to the end
                    for (j = 0; j < 32; j ++) {
                        if (pTx_queue->Buffer_flag[j] == 0) {
                            break;
                        }
                    }
                    memcpy(pTx_queue->Buffer_payload[j], temp_payload, GENERAL_PAYLOAD_SIZE);
                    pTx_queue->Buffer_time[j] = Packet_sent_at;
                    pTx_queue->Buffer_Retries[j] = temp_retries + 1;
                    pTx_queue->Buffer_flag[j] = 1;
                }
                else {
                    // Remove from Buffer
                    pTx_queue->Buffer_time[i] = 0;
                    pTx_queue->Buffer_Retries[i] = 0;
                    pTx_queue->Buffer_flag[i] = 0;
                    memset(pTx_queue->Buffer_payload[i], 0, 32);
                    pTx_queue->Buffer_len -= 1;
                }
                last_packet_TX_time = Packet_sent_at;
                XID += 1;
            }
        }
        else if (transmit_from_queue == 1) {
            memset(message, 0, sizeof(message));
            msgptr = message;
            roffset = pTx_queue->roffset;
            memcpy(msgptr, pTx_queue->payload[roffset], GENERAL_PAYLOAD_SIZE);
            msgptr += GENERAL_PAYLOAD_SIZE;
            msgptr[0] = timePktSent[0];
            msgptr[1] = timePktSent[1];
            msgptr[2] = Qempty;
            msgptr[3] = TarsITx;
            msgptr[4] = acks[0];
            msgptr[5] = acks[1];
            msgptr[6] = acks[2];
            msgptr[7] = 0;
            msgptr[8] = 0;
            msglen = GENERAL_PAYLOAD_SIZE + 9;
            xid_ = XID;
            addr_ = pTx_queue->address;
            com_flag = 1;
            //tda_remSendData(XID, Tx_queue[packet_addr].address, message, msglen);
            Com_Printf("tdt_txTask: transmit from queue, is being transmitted to ...\r\n", 5);
            printf("TarsITx: 0x%x, ACKs: 0x%x, 0x%x, 0x%x.\r\n", TarsITx, acks[0], acks[1], acks[2]);
#if 0
            // Update the time table
            temp = Tx_queue[packet_addr].payload[roffset][0];
            TimeTable[packet_addr].Sent[temp] = (((int)timePktSent[0] << 8) + (int)timePktSent[1]) / 10;
            TimeTable[packet_addr].Tx_count[temp] = 1;
            TimeTable[packet_addr].Tx_prob[temp] = optNodeProb;
#endif
            // Add to buffer:
            for (j = 0; j < 32; j ++) {
                if (pTx_queue->Buffer_flag[j] == 0) {
                    pTx_queue->Buffer_len += 1;
                    memcpy(pTx_queue->Buffer_payload[j], pTx_queue->payload[roffset], GENERAL_PAYLOAD_SIZE);
                    pTx_queue->Buffer_time[j] = Packet_sent_at;
                    pTx_queue->Buffer_Retries[j] = 0;
                    pTx_queue->Buffer_flag[j] = 1;
                    break;
                }
            }
            // Update read pointer for payload buffer
            if (roffset == 32 - 1) {
                pTx_queue->roffset = 0;
            }
            else {
                pTx_queue->roffset += 1;
            }
            // ...
            queue_length.Tx -= 1;
            // Tx packet started
            state.Tx = 5;
            XID += 1;
            last_packet_TX_time = Packet_sent_at;
        }
        else if (transmit_dedicated_ack == 1) {
            // First of all, find the packet with the max received time
            tdt_findMaxPktTime(time_now, &packet_addr, &offset);
            printf("The packet with max received time: addr is %d, offset is %d.\r\n", packet_addr, offset);
            // check if it is larger than the timeout, and fill ack[] ...
            pRx_queue = &Rx_queue[packet_addr];
            pTx_queue = &Tx_queue[packet_addr];
            memset(acks, 0, sizeof(acks));
            printf("The current time is %d, the packet time is %d.\r\n", time_now, pRx_queue->PacketTime[offset]);
            if (((time_now - pRx_queue->PacketTime[offset]) > ACK_TIMEOUT) && (pRx_queue->PacketTime[offset] != 0)) {
                acks[0] = pRx_queue->PacketList[offset];
                pRx_queue->PacketList[offset] = pRx_queue->PacketTime[offset] = 0;
                pRx_queue->length -= 1;
                for (i = 0; i < (NUM_OF_ACKS - 1); i ++) {
                    if (offset == RXPACKET_LENGTH - 1) {
                        offset = 0;
                    }
                    else {
                        offset += 1;
                    }
                    if (pRx_queue->PacketTime[offset] != 0) {
                    //if (((time_now - pRx_queue->PacketTime[offset]) > ACK_TIMEOUT) && (pRx_queue->PacketTime[offset] != 0)) {
                        acks[1 + i] = pRx_queue->PacketList[offset];
                        pRx_queue->PacketList[offset] = pRx_queue->PacketTime[offset] = 0;
                        pRx_queue->length -= 1;
                    }
                    else {
                        acks[1 + i] = 0;
                    }
                }
            }
            printf("The dedicated acks is 0x%x, 0x%x, 0x%x.\r\n", acks[0], acks[1], acks[2]);
            // Send dedicated ACK, acks[0] includes the maximum difference between the time of received time and the time right now
            if (acks[0] != 0) {
                Com_Printf("Transmit dedicated ACKs......\r\n", 5);
                memset(message, 0, sizeof(message));
                msgptr = message;
                msgptr[0] = 253;
                msgptr[1] = 0;
                msgptr[2] = NUM_OF_ACKS;
                memcpy(&msgptr[3], acks, NUM_OF_ACKS);
                //msgptr[3] = acks[0];
                //msgptr[4] = acks[1];
                //msgptr[5] = acks[2];
                msglen = 3 + NUM_OF_ACKS;
                xid_ = XID;
                addr_ = pTx_queue->address;
                com_flag = 1;
            }
        }
    }
    Com_MutexUnlock();

    if (com_flag) {
        printf("*****************************************************\r\n");
        printf("*****************************************************\r\n");
        tda_remSendData(xid_, addr_, message, msglen);
    }

    return 0;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_pingTask
//
// Description:		It's used to ping other modems
//                      
//
// Parameter:
//        Input:
//			NONE
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_pingTask(void)
{
    unsigned int com_flag, ui_err = 0;
    struct def_Ping_queue* pPingqueue;
    char xid_, msg[32];
    float prb;


    com_flag = 0;
    memset(msg, 0, sizeof(msg));
    prb = (float)rand() / RAND_MAX;
    // Protection Start ////////////////////////////////////////////////////////////////
    Com_MutexLock();
    if ((state.Ping == 10) || (prb < prob_ping_threshold)) {
        if ((state.Ping == 10) || ((state.Ping == 0) && (Ping_Enable == 1))) {
            com_flag = 1;
            xid_ = XID;
        }
    }
    else if (Ping_queue_roffset != Ping_queue_woffset) {
        pPingqueue = &Ping_queue[Ping_queue_roffset];
        if (strncmp(pPingqueue->signature, "00 03 03 00 00 06", 17) == 0) {
            printf("Ping: working on ping - process.\r\n");
        }
        else if (strncmp(pPingqueue->signature, "00 03 03 00 00 04", 17) == 0) {
            state.Ping = state.Ping < 1 ? state.Ping : 1;
            tdt_addAddrToBook(pPingqueue->message, "Ping");
            printf("**************************************Ping: working on ping - arrived.*******************************************\r\n");
        }
        else {
            printf("Unknown signature.\r\n");
        }

        if (Ping_queue_roffset == PING_QUEUE_LEN - 1) {
            Ping_queue_roffset = 0;
        }
        else {
            Ping_queue_roffset += 1;
        }
    }
    Com_MutexUnlock();
    // Protection End /////////////////////////////////////////////////////////////////
    if (com_flag) {
        tda_ping(xid_, msg, MESSAGE_LEN_PING);
        // Protection Start ///////////////////////////////////////////////////////////
        Com_MutexLock();
        XID = XID + 1;
        state.Ping = 1;
        Com_MutexUnlock();
        // Protection End /////////////////////////////////////////////////////////////
    }


    return ui_err;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	notify_converter
//
// Description:		It's used to generate signature                     
//
// Parameter:
//        Input:
//			c_subsys:	subsystem value
//                      c_field:        field value
//                      s_message:      message string
//        Output:
//			s_output:       signature
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int notify_converter(char c_subsys, char c_field, char* s_message, char* s_output)
{
    int len, i;
    char c_num[2];

    len = sprintf(c_num, "%d", c_subsys);
    if (len == 1) {
        s_output[0] = '0';
        s_output[1] = c_num[0];
    }
    else {
        memcpy(&s_output[0], c_num, 2);
    }
    s_output[2] = ' ';

    len = sprintf(c_num, "%d", c_field);
    if (len == 1) {
        s_output[3] = '0';
        s_output[4] = c_num[0];
    }
    else {
        memcpy(&s_output[3], c_num, 2);
    }
    s_output[5] = ' ';
    for (i = 0; i < 4; i ++) {
        len = sprintf(c_num, "%d", s_message[i]);
        if (len == 1) {
            s_output[6 + i * 3] = '0';
            s_output[6 + i * 3 + 1] = c_num[0];
        }
        else {
            memcpy(&s_output[6 + i * 3], c_num, 2);
        }
        s_output[6 + i * 3 + 2] = ' ';
    }


    return 0;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_notifyTask
//
// Description:		It's used to receive message from the modem
//                      in asynch mode, and do related update.                      
//
// Parameter:
//        Input:
//			NONE
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_notifyTask(void)
{
    unsigned int i, temp, ui_err = 0;
    int offset;
    struct modemPacket mPkt;
    struct modemPacket* pmPkt;
    struct def_Ping_queue* pPing_queue;
    struct def_Ping_queue* pTx_queue_Notify;
    char signature[32];


    //printf("In tdt_notifyTask:...\r\n");
    pmPkt = &mPkt;
    memset(pmPkt, 0, sizeof(struct modemPacket));
    memset(signature, 0, sizeof(signature));
    ui_err = tda_asyncNotify(pmPkt);
    if (ui_err == 0) {
        // Write the packet to log file
        tdt_notify_to_file(pmPkt, 1);
        for (i = 0; i < pmPkt->numParam; i ++) {
            // Generate Signature
            notify_converter(pmPkt->subsys[i], pmPkt->field[i], pmPkt->message[i], signature);
            if (pmPkt->len[i] >= 4) {
                signature[17] = 0x0;
            }
            else if (pmPkt->len[i] == 2) {
                signature[12] = signature[13] = '0';
                signature[15] = signature[16] = '0';
                signature[17] = 0x0;
            }
            else {
                signature[6] = signature[7] = '0';
                signature[9] = signature[10] = '0';
                signature[12] = signature[13] = '0';
                signature[15] = signature[16] = '0';
                signature[27] = 0x0;
            }
            if (strncmp(signature, "00 03", 5) == 0) {
                signature[13] = '0';
            }
            //printf("The signature is %s.\r\n", signature);

            // Do update based on the signature
            if (strncmp(signature, "03 16", 5) == 0) {
                Com_Printf("	Notify: InBound RTS", 2);
            }
            else if (strncmp(signature, "03 17", 5) == 0) {
                Com_Printf("	Notify: RTS sent", 2);
                Com_MutexLock();
                state.Tx = 4;
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 18", 5) == 0) {
                Com_Printf("	Notify: CTS received", 2);
                Com_MutexLock();
                state.Tx = 3;
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 19", 5) == 0) {
                Com_Printf("	Notify: Outbound CTS", 2);
            }
            else if (strncmp(signature, "03 22", 5) == 0) {
                Com_Printf("	Notify: Ack received! Transmission done!", 5);
                Com_MutexLock();
                state.Tx = (state.Tx) < 2 ? (state.Tx) : 2;
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 25", 5) == 0) {
                Com_Printf("	Notify: data sent! Hurry!", 2);
                Com_MutexLock();
                state.Tx = (state.Tx) < 2 ? (state.Tx) : 2;
                if (dataAcks == 0) {
                    state.Tx = 0;
                }
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 26", 5) == 0) {
                Com_Printf("Notify: Ping Received from modem", 1);
                Com_MutexLock();
                if (state.Ping != 0) {
                    Com_Printf("Notify: We will responde to ping (Ping: 7)", 1);
                    state.Ping = 7;
                }
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 27", 5) == 0) {
                Com_Printf("Notify: Ping Sent", 5);
                Com_MutexLock();
                offset= Ping_queue_woffset;
                pPing_queue = &Ping_queue[offset];
                pPing_queue->xid = pmPkt->xid;
                pPing_queue->type = pmPkt->type;
                pPing_queue->numParam = 1;
                pPing_queue->subsys = pmPkt->subsys[i];
                pPing_queue->field = pmPkt->field[i];
                pPing_queue->length[0] = (char)(pmPkt->len[i] >> 8);
                pPing_queue->length[1] = (char)(pmPkt->len[i]);
                memcpy(pPing_queue->message, pmPkt->message[i], pmPkt->len[i]);
                memcpy(pPing_queue->signature, signature, sizeof(signature));
                if (Ping_queue_woffset == PING_QUEUE_LEN - 1) {
                    Ping_queue_woffset = 0;
                }
                else {
                    Ping_queue_woffset += 1;
                }
                state.Ping = 6;
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 28", 5) == 0) {
                Com_Printf("Notify: Echo Received from modem", 1);
            }
            else if (strncmp(signature, "03 29", 5) == 0) {
                Com_Printf("Notify: Echo sent to modem", 1);
            }
            else if (strncmp(signature, "03 33", 5) == 0) {
                Com_Printf("Notify: wait for CTS up", 2);
            }
            else if (strncmp(signature, "03 34", 5) == 0) {
                Com_Printf("Notify: data packet heard", 5);
            }
            else if (strncmp(signature, "03 35", 5) == 0) {
                Com_Printf("Notify: wait for Ack up", 2);
            }
            else if (strncmp(signature, "03 37", 5) == 0) {
                Com_Printf("	Notify: Wait echo - up (ping)", 5);
                Com_MutexLock();
                state.Ping = 0;
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 40", 5) == 0) {
                Com_MutexLock();
                if (state.Ping != 0) {
                    Com_Printf("	Notify: Ping related (back to idle)", 1);
                    state.Ping = 0;
                }
                else if (state.Tx != 0) {
                    Com_Printf("	Notify: Tx related (back to idle) (Tx 0)", 2);
                    state.Tx = 0;
                }
                Com_MutexUnlock();
            }
            else if (strncmp(signature, "03 41", 5) == 0) {
                Com_Printf("Notify: Layer 2 state is processing events", 1);
            }
            else if (strncmp(signature, "04 01", 5) == 0) {
                Com_Printf("******************************Notify: tdt_data_rcvd.*******************************************", 2);
                tdt_data_rcvd(pmPkt->message[i], signature, Master_node);
            }
            else if (strncmp(signature, "04 07", 5) == 0) {
#if 0
                Com_Printf("TIMESTAMP rx or tx event.", 5);
                if ((pmPkt->message[0][0] == 0) && (pmPkt->message[0][1] == 0)) {
                    Com_Printf("TX event.\r\n", 5);
                }
                else if ((pmPkt->message[0][0] == 0) && (pmPkt->message[0][1] == 1)) {
                    Com_Printf("RX event.\r\n", 5);
                }
                else {
                    Com_Printf("Unknown event.\r\n", 5);
                }
#endif
            }
            else if (strncmp(signature, "04 08", 5) == 0) {
                Com_Printf("	Notify: Doppler", 2);
            }
            else if (strncmp(signature, "04 13", 5) == 0) {
                Com_Printf("Notify: Low Power Mode", 5);
            }
            else if (strncmp(signature, "04 14", 5) == 0) {
                Com_Printf("Notify: Acoustic stat received.", 2);
                temp = (int)(pmPkt->message[i][26]) + (int)(pmPkt->message[i][27]);
                if (temp == 0) {
                    Com_MutexLock();
                    tdt_addAddrToBook((void*)(signature + 9), "justAddr");
                    Com_MutexUnlock();
                }
                else {
                    Com_Printf("BAD CRC CAUGHT!", 0);
                }
            }
            else if (strncmp(signature, "04 17", 5) == 0) {
                Com_Printf("Notify: NAV_STAT", 2);
            }
            else if (strncmp(signature, "04 18", 5) == 0) {
                Com_Printf("Notify: Range update.", 2);
                Com_MutexLock();
                tdt_addAddrToBook(pmPkt->message[i], "Range");
                Com_MutexUnlock();
            }
            else {
                if (strncmp(signature, "00 03 03 00", 11) == 0) {
                    Com_Printf("Notify: ping related", 3);
                    Com_MutexLock();
                    offset = Ping_queue_woffset;
                    pPing_queue = &Ping_queue[offset];
                    pPing_queue->xid = pmPkt->xid;
                    pPing_queue->type = pmPkt->type;
                    pPing_queue->numParam = 1;
                    pPing_queue->subsys = pmPkt->subsys[i];
                    pPing_queue->field = pmPkt->field[i];
                    pPing_queue->length[0] = (char)(pmPkt->len[i] >> 8);
                    pPing_queue->length[1] = (char)(pmPkt->len[i]);
                    memcpy(pPing_queue->message, pmPkt->message[i], pmPkt->len[i]);
                    memcpy(pPing_queue->signature, signature, sizeof(signature));
                    if (Ping_queue_woffset == PING_QUEUE_LEN - 1) {
                        Ping_queue_woffset = 0;
                    }
                    else {
                        Ping_queue_woffset += 1;
                    }
                    Com_MutexUnlock();
                }
                else if (strncmp(signature, "00 03 01 07", 11) == 0) {
                    Com_Printf("Notify: data to be transmitted", 3);
                }
                else if (strncmp(signature, "04 07 00 00", 11) == 0) {
                    Com_Printf("Notify: Tx event", 2);
                    Com_MutexLock();
                    if (state.Ping != 0) {
                        offset = Ping_queue_woffset;
                        pPing_queue = &Ping_queue[offset];
                        pPing_queue->xid = pmPkt->xid;
                        pPing_queue->type = pmPkt->type;
                        pPing_queue->numParam = 1;
                        pPing_queue->subsys = pmPkt->subsys[i];
                        pPing_queue->field = pmPkt->field[i];
                        pPing_queue->length[0] = (char)(pmPkt->len[i] >> 8);
                        pPing_queue->length[1] = (char)(pmPkt->len[i]);
                        memcpy(pPing_queue->message, pmPkt->message[i], pmPkt->len[i]);
                        memcpy(pPing_queue->signature, signature, sizeof(signature));
                        if (Ping_queue_woffset == PING_QUEUE_LEN - 1) {
                            Ping_queue_woffset = 0;
                        }
                        else {
                            Ping_queue_woffset += 1;
                        }
                        Com_Printf("Notify: Tx event - Ping related", 2);
                    }
                    else if (state.Tx != 0) {
                        offset = Tx_queue_Notify_woffset;
                        pTx_queue_Notify = &Tx_queue_Notify[offset];
                        pTx_queue_Notify->xid = pmPkt->xid;
                        pTx_queue_Notify->type = pmPkt->type;
                        pTx_queue_Notify->numParam = 1;
                        pTx_queue_Notify->subsys = pmPkt->subsys[i];
                        pTx_queue_Notify->field = pmPkt->field[i];
                        pTx_queue_Notify->length[0] = (char)(pmPkt->len[i] >> 8);
                        pTx_queue_Notify->length[1] = (char)(pmPkt->len[i]);
                        memcpy(pTx_queue_Notify->message, pmPkt->message[i], pmPkt->len[i]);
                        memcpy(pTx_queue_Notify->signature, signature, sizeof(signature));
                        if (Tx_queue_Notify_woffset == PING_QUEUE_LEN - 1) {
                            Tx_queue_Notify_woffset = 0;
                        }
                        else {
                            Tx_queue_Notify_woffset += 1;
                        }
                        Com_Printf("Notify: Tx event - Tx data related", 2);
                        state.Tx = (state.Tx) < 1 ? (state.Tx) : 1;
                    }
                    else {
                        Com_Printf("Notify: unknown reason for Tx", 2);
                        Com_Printf(signature, 2);
                    }
                    Com_MutexUnlock();
                }
                else if (strncmp(signature, "04 07 00 01", 11) == 0) {
                    Com_Printf("Notify: Rx event", 3);
                    Com_MutexLock();
                    if (state.Ping != 0) {
                        offset = Ping_queue_woffset;
                        pPing_queue = &Ping_queue[offset];
                        pPing_queue->xid = pmPkt->xid;
                        pPing_queue->type = pmPkt->type;
                        pPing_queue->numParam = 1;
                        pPing_queue->subsys = pmPkt->subsys[i];
                        pPing_queue->field = pmPkt->field[i];
                        pPing_queue->length[0] = (char)(pmPkt->len[i] >> 8);
                        pPing_queue->length[1] = (char)(pmPkt->len[i]);
                        memcpy(pPing_queue->message, pmPkt->message[i], pmPkt->len[i]);
                        memcpy(pPing_queue->signature, signature, sizeof(signature));
                        if (Ping_queue_woffset == PING_QUEUE_LEN - 1) {
                            Ping_queue_woffset = 0;
                        }
                        else {
                            Ping_queue_woffset += 1;
                        }
                        Com_Printf("Notify: Rx event - Ping related", 3);
                    }
                    else if (state.Tx != 0) {
                        offset = Tx_queue_Notify_woffset;
                        pTx_queue_Notify = &Tx_queue_Notify[offset];
                        pTx_queue_Notify->xid = pmPkt->xid;
                        pTx_queue_Notify->type = pmPkt->type;
                        pTx_queue_Notify->numParam = 1;
                        pTx_queue_Notify->subsys = pmPkt->subsys[i];
                        pTx_queue_Notify->field = pmPkt->field[i];
                        pTx_queue_Notify->length[0] = (char)(pmPkt->len[i] >> 8);
                        pTx_queue_Notify->length[1] = (char)(pmPkt->len[i]);
                        memcpy(pTx_queue_Notify->message, pmPkt->message[i], pmPkt->len[i]);
                        memcpy(pTx_queue_Notify->signature, signature, sizeof(signature));
                        if (Tx_queue_Notify_woffset == PING_QUEUE_LEN - 1) {
                            Tx_queue_Notify_woffset = 0;
                        }
                        else {
                            Tx_queue_Notify_woffset += 1;
                        }
                        Com_Printf("Notify: Rx event - Rx data related", 3);
                    }
                    else {
                        Com_Printf("Notify: unknown reason for Rx", 3);
                        Com_Printf(signature, 3);
                    }
                    Com_MutexUnlock();
                }
                else {
                    Com_Printf("Notiify: unknown signature", 4);
                    Com_Printf(signature, 4);
                }
            }
        }
    }    
    else {
        ui_err = 1;
        //Com_Printf("tdt_notifyTask: No data received frome the Modem.\r\n", 4);
    }


    return ui_err;
}




/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_qmonitorTask
//
// Description:		It's used to monitor Q-value and calculate the
//                      probability.              
//
// Parameter:
//        Input:
//			NONE
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_qmonitorTask(void)
{
    char clock[2];
    float cal, sum_O_i, sum_I_i, multi, sum_I;
    int i, counter, numClock, This_Q_empty;
    struct def_Q_empty* pQempty;
    struct def_Tx_queue* pTxqueue;


    pQempty = &Q_empty[0];
    pTxqueue = &Tx_queue[0];
    sum_O_i = sum_I_i = sum_I = 0;
    counter = 0;
    multi = 1;
    tda_clock(clock_diff, "char", clock, NULL);
    numClock = ((int)clock[0] << 8) + (int)clock[1];
    numClock /= 10;
    // Protection start //////////////////////////////////////////////////////////////////////////////////
    Com_MutexLock();
    for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
        if (Addr_book[i].address != 0) {
            This_Q_empty = 1;
            if (pTxqueue->length > 0) {
                This_Q_empty = 0;
            }
            else if (pTxqueue->Buffer_len > 0) {
                if (((numClock - pTxqueue->Buffer_time[0]) % 36000) > TARS_retrans_timeout) {
                    This_Q_empty = 0;
                }
            }
            cal = pQempty->out * 0.99 + 0.01 * This_Q_empty;
            pQempty->out = cal;
            ////////////////////////////////////////////////////////////////////////////////////
            sum_O_i += (1 - pQempty->out);
            sum_I_i += (1 - pQempty->in);
            multi *= pQempty->out;
            sum_I = pQempty->I;
            ++ counter;
        }
        ++ pQempty;
        ++ pTxqueue;
    }

    TARS_I = sum_I_i;
    optNodeProb_GUI = sum_O_i / ((1 - multi) * (sum_I_i + sum_I));
    optNodeProb = (float)1 / 3;
    if ((mac_protocol == 2) || (mac_protocol == 1)) {
        optNodeProb = 1;
    }
    else if (mac_protocol == 4) {
        optNodeProb = (float)1 / (1 + counter);
    }
    Com_MutexUnlock();
    // Protection end ////////////////////////////////////////////////////////////////////////////////////


    return 0;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_dataprodTask
//
// Description:		It's used to generate packets for Tx
//
// Parameter:
//        Input:
//			NONE
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tdt_dataprodTask(void)
{
    unsigned int ui_err = 0;
    int i, record_num, offset, packet_addr, woffset;
    char pktNum2Modem, Qempty, TarsITx, lastPkt, timeLastPkt[2];
    char timePktGen[2], record[ADDRESS_BOOK_LEN];
    struct def_Tx_queue* pTxqueue;
    char* offset_ptr;
    float probability;


    // Protection Start ////////////////////////////////////////////////////////
    Com_MutexLock();
    tda_clock(clock_diff, "char", timePktGen, NULL);   
    TimerCounter.DataProd += 1;
    if (Tx_Enable == 1) {
        probability = ((float)rand() / RAND_MAX);
        if (addrbook_counter > 0) {
            if (probability < (packet_rate * 0.06 / T_packet)) {
                // tdt_gen_packet
                Packet_Gen_Counter += 1;
                record_num = 0;
                for (i = 0; i < ADDRESS_BOOK_LEN; i ++) {
                    if (Addr_book[i].address != 0) {
                        record[record_num] = (char)i;
                        ++ record_num;
                    }
                }
                offset = (int)(rand() % record_num + 1) - 1;
                packet_addr = (int)record[offset];

                pTxqueue = &Tx_queue[packet_addr];
                pktNum2Modem = (char)(pTxqueue->counter);
                Qempty = (char)(Q_empty[packet_addr].out * 255);
                TarsITx = (char)(TARS_I * 255);
                lastPkt = Addr_book[packet_addr].lastPkt;
                timeLastPkt[0] = Addr_book[packet_addr].timeLastPkt[0];
                timeLastPkt[1] = Addr_book[packet_addr].timeLastPkt[1];
                woffset = (int)(pTxqueue->woffset);
                offset_ptr = pTxqueue->payload[woffset];
                offset_ptr[0] = pktNum2Modem;
                offset_ptr[1] = timePktGen[0];
                offset_ptr[2] = timePktGen[1];
                offset_ptr[3] = lastPkt;
                offset_ptr[4] = timeLastPkt[0];
                offset_ptr[5] = timeLastPkt[1];
                if (woffset == 32 - 1) {
                    pTxqueue->woffset = 0;
                }
                else {
                    pTxqueue->woffset += 1;
                }

                printf("Data: data generate, the offset is %d, the address is 0x%x.\r\n", packet_addr, Addr_book[packet_addr].address);
                offset = pTxqueue->counter;
                // It is in Matlab codes, but seems no chance for use about TimeTable[packet_addr].Gen[]
                //TimeTable[packet_addr].Gen[offset] = ((float)(((int)timePktGen[0] << 8) + (int)timePktGen[1])) / 10;
                pTxqueue->counter += 1;
                packets_produced += 1;           
                queue_length.Tx += 1;
            }
        }
        else if (probability < packet_rate * 0.06) {
            Com_Printf("tdt_dataprodTask: No address in Address book.\r\n", 5);
        }
    }
    else {
        //Com_Printf("tdt_dataprodTask: TX is not enabled.\r\n", 5);
    }   
    Com_MutexUnlock();
    // Protection End ///////////////////////////////////////////////////////////////

    return ui_err;
}





