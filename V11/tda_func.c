//////////////////////////////////////////////////////////////////////////////
//
// Description:		This file includes all functions for TDA layer.
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
#include <time.h>
#include <sys/time.h>
#include "Config.h"
#include "td_func.h"
#include "Common.h"


////////////////////////////////////////////////////////////////////////////
//
// Extern Global Variables
//
///////////////////////////////////////////////////////////////////////////













/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_modemInit
//
// Description:		It's used to find and initialize modem to enter
//                      mmpd mode.
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
unsigned int tda_modemInit(void)
{
    const char uart_dev[] = "/dev/ttyO2";
    char init_cmd[] = {0x2B, 0x2B, 0x2B, 0x0A};
    char mmpd_cmd[] = {0x6D, 0x6D, 0x70, 0x64, 0x0A};
    char mmpd_res[] = {0x40, 0x00, 0x6E};
    char rbuffer[128];
    int num, loop, detected;
    unsigned int ui_err = 0;
    struct modemPacket mpkt;


    memset(rbuffer, 0, sizeof(rbuffer));
    detected = loop = 0;
    // Uart 2
    if (Com_Init(uart_dev, 9600, 8, 1, 'N') == 0) {
        Com_ConfigureTimeout(0);
        // Read log data if it outputs...
        do {
            Com_uSleep(500 * 1000);
            num = Com_Receive(rbuffer, sizeof(rbuffer));
            printf("Read Log: %d, %s.\r\n", num, rbuffer);
        } while (num > 0);
        // Send "+++" command and get response
        do {
            Com_Send(init_cmd, sizeof(init_cmd));
            Com_uSleep(200 * 1000);
            num = Com_Receive(rbuffer, sizeof(rbuffer));
            if (strstr(rbuffer, "user:") != NULL) {
                detected = 1;
                printf("Modem has been detected.\r\n");
            }
            else {
                printf("No Response found from Modem: %d, %s.\r\n", num, rbuffer);
                Com_uSleep(500 * 1000);
            }
        } while ((++ loop <= 50) && (detected == 0));

    }
    else {
        printf("Cannot configure Uart correctly.\r\n");
        ui_err = 1;
    }

    if (detected == 1) {
        // Enter mmpd mode
        memset(rbuffer, 0, sizeof(rbuffer));
        Com_Send(mmpd_cmd, sizeof(mmpd_cmd));
        Com_uSleep(200 * 1000);
        num = Com_Receive(rbuffer, sizeof(rbuffer));
        if ((rbuffer[7] == mmpd_res[0]) && (rbuffer[8] == mmpd_res[1]) && (rbuffer[9] == mmpd_res[2])) {
            printf("Entered MMPD mode.....\r\n");
            td_set("rtsAttempts", 0, 7);
            Com_uSleep(100 * 1000);
            if (td_notify(&mpkt, 5, 200) == 0) {
                if (mpkt.type == 'n') {
                    printf("Get response correctly from Modem.\r\n");
                }
                else {
                    memset(&mpkt, 0, sizeof(struct modemPacket));
                    td_set("rtsAttempts", 0, 29);
                    Com_uSleep(50 * 1000);
                    td_notify(&mpkt, 5, 200);
                    if (mpkt.type == 'n') {
                        printf("Get response correctly from Modem.\r\n");
                    }       
                }
            }
            else {
                printf("td_notify error.\r\n");
                ui_err = 4;
            }
        }
        else {
            printf("Cannot enter MMPD mode:\r\n");
            for (loop = 0; loop < num; loop ++) {
                printf("0x%x, ", rbuffer[loop]);
            }
            printf("\r\n");
            ui_err = 3;
        }
    }
    else {
        printf("No response from Modem.\r\n");
        ui_err = 2;
    }

    return ui_err;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_getLocalAddr
//
// Description:		It's used to get the local address from Modem
//
// Parameter:
//        Input:
//			xid:		XID field for Modem
//        Output:
//			local_addr:	the local address received from
//                                      the modem
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_getLocalAddr(char xid, char* local_addr)
{
    unsigned int i, ui_err = 0;
    struct modemPacket mpkt;


    *local_addr = INVALID_ADDRESS;
    // Clear Uart input buffer
    //Com_ClearRbuffer();
    if (td_get("localAddr", xid) == 0) {
        Com_uSleep(50 * 1000);
        if (td_notify(&mpkt, 5, 100) == 0) {
            for (i = 0; i < mpkt.numParam; i ++) {
                if ((mpkt.subsys[i] == 1) && (mpkt.field[i] == 18)) {
                    *local_addr = mpkt.message[i][1];
                }
            }
            if (*local_addr == INVALID_ADDRESS) {
                printf("Cannot find Local address from the message sent by the Modem.\r\n");
                ui_err = 3;
            }
        }
        else {
            printf("Cannot get notify from Modem after sending localAddr Command.\r\n");
            ui_err = 2;
        }
    }
    else {
        printf("Cannot send localAddr command to the Modem.\r\n");
        ui_err = 1;
    }


    return ui_err;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_getSnifferMode
//
// Description:		It's used to get the Sniffer Mode from Modem
//
// Parameter:
//        Input:
//			xid:		XID field for Modem
//        Output:
//			sniffer_mode:	the sniffer mode received from
//                                      the modem
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_getSnifferMode(char xid, char* sniffer_mode)
{
    unsigned int ui_err = 0;
    struct modemPacket mpkt;


    // Clear Uart input buffer
    //Com_ClearRbuffer();
    if (td_get("sniffermode", xid) == 0) {
        Com_uSleep(50 * 1000);
        if (td_notify(&mpkt, 5, 100) == 0) {
            *sniffer_mode = mpkt.message[0][3];
        }
        else {
            printf("Cannot get notify from Modem after sending sniffermode Command.\r\n");
            ui_err = 2;
        }
    }
    else {
        printf("Cannot send sniffermode command to the Modem.\r\n");
        ui_err = 1;
    }


    return ui_err;
}


/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_transmitInit
//
// Description:		It's used to initialize the transmit mode for the
//                      modem
//
// Parameter:
//        Input:
//                      xid1:		XID for ACK
//                      xid2:		XID for Retransmission
//			ack:		Enable/Disable ACK
//			retrans:	the number of retransmission time
//        Output:
//			NONE
//
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_transmitInit(char xid1, char xid2, char ack, char retrans)
{
    unsigned int ui_err;


    // Clear Uart input buffer
    //Com_ClearRbuffer();
    if (td_set("dataAcks", ack, xid1) == 0) {
        Com_uSleep(10 * 1000);
        if (td_set("dataRetrans", retrans, xid2) == 0) {
            ui_err = 0;
        }
        else {
            printf("Cannot sned dataRetrans command to the modem.\r\n");
            ui_err = 2;
        }
    }
    else {
        printf("Cannot send dataAcks command to the Modem.\r\n");
        ui_err = 1;
    }


    return ui_err;
}





/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_asyncNotify
//
// Description:		It's used to get the data from Modem in async mode
//                      
//
// Parameter:
//        Input:
//			None
//
//        Output:
//			pmPkt:		the sniffer mode received from
//                                      the modem
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_asyncNotify(struct modemPacket* pmPkt)
{
    unsigned int ui_err;

    // Only wait for 1ms to see if the data recevied from the modem
    ui_err = td_notify(pmPkt, 1, 1);


    return ui_err;
}


/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_remSendData
//
// Description:		It's used to to send command "remSendData"
//                      
//
// Parameter:
//        Input:
//			xid:		XID
//                      remoteAddr:	remote Address
//                      msgdata:	message info
//			msglen:		length of message
//
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_remSendData(char xid, char remoteAddr, char* msgdata, int msglen)
{
    unsigned int len, ui_err;
    char message[32];

 
    memset(message, 0, sizeof(message));
    message[0] = 0;
    message[1] = remoteAddr;
    message[2] = 0;
    message[3] = 0;
    message[4] = (char)(msglen >> 8);
    message[5] = (char)msglen;
    memcpy(&message[6], msgdata, msglen);
    len = 6 + msglen; 
    ui_err = td_exec("remSendData", message, len, xid);


    return ui_err;
}


/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_ping
//
// Description:		It's used to to do Ping
//                      
//
// Parameter:
//        Input:
//			xid:		XID
//			msgdata:	message
//
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
unsigned int tda_ping(char xid, char* msgdata, int msglen)
{
    unsigned int ui_err;

 
    ui_err = td_exec("Ping", msgdata, msglen, xid);


    return ui_err;
}


unsigned int tda_getTime(char* phour, char* pmin, char* psec)
{
    unsigned int ui_err;
    struct modemPacket mPkt, rmPkt;

    memset(&mPkt, 0, sizeof(struct modemPacket));
    memset(&rmPkt, 0, sizeof(struct modemPacket));
    mPkt.xid = 13;
    mPkt.type = 'g';
    mPkt.numParam = 1;
    mPkt.subsys[0] = 0x06;
    mPkt.field[0] = 0x01;
    ui_err = td_send(&mPkt);
    Com_uSleep(10 * 1000);
    if (ui_err == 0) {
        ui_err = td_notify(&rmPkt, 10, 1);
        if (ui_err == 0) {
            printf("The time is %d, %d, %d, %d:%d:%d.\r\n", ((int)rmPkt.message[0][6] << 8) + (int)rmPkt.message[0][7], 
                                                            rmPkt.message[0][4], rmPkt.message[0][5], 
                                                            (int)rmPkt.message[0][1], (int)rmPkt.message[0][2], (int)rmPkt.message[3]);
            phour[0] = rmPkt.message[0][1];
            pmin[0] = rmPkt.message[0][2];
            psec[0] = rmPkt.message[0][3];
        }
        else {
            printf("Receive time info error.\r\n");
        }
    }
    else {
        printf("Send get time info error.\r\n");
    }


    return ui_err;
}
