//////////////////////////////////////////////////////////////////////////////
//
// Description:		This is used to send data to the modem. The function is
//                      same as td_send.m file which is in Matlab project.
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
#include "td_func.h"
#include "CommUart.h"



////////////////////////////////////////////////////////////////////////////
//
// Extern Global Variables
//
///////////////////////////////////////////////////////////////////////////












///////////////////////////////////////////////////////////////////////////
//
// Function Name:	td_send
//
// Description:		It's used to send data to the modem
//               
// Parameter:
//        INPUT:
//                      tx_ptr:		Buffer data for sending
//       OUTPUT:
//                      None
//
// Return Value:
//                      0:		OK
//			else:		Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int td_send(struct modemPacket* tx_ptr)
{
    unsigned int ui_err = 0;
    unsigned int i, total_len;
    char tx_bytes[512];
    char* cur_ptr;


    memset(tx_bytes, 0, sizeof(tx_bytes));
    tx_bytes[0] = '@';
    tx_bytes[1] = tx_ptr->xid;
    tx_bytes[2] = tx_ptr->type;
    tx_bytes[3] = tx_ptr->numParam;
    cur_ptr = &tx_bytes[4];
    for (i = 0; i < tx_ptr->numParam; i ++) {
        *cur_ptr ++ = tx_ptr->subsys[i];
        *cur_ptr ++ = tx_ptr->field[i];
        if (tx_ptr->type != 'g') {
            *cur_ptr ++ = (char)((tx_ptr->len[i]) >> 8);
            *cur_ptr ++ = (char)(tx_ptr->len[i]);
            memcpy(cur_ptr, tx_ptr->message[i], tx_ptr->len[i]);
            cur_ptr += tx_ptr->len[i];
        }
        else {
            cur_ptr += 2;
        }
    }
    total_len = cur_ptr - tx_bytes;
    //For debug
    printf("Send data to Modem: ");
    for (i = 0; i < total_len; i ++) {
        printf("0x%x, ", tx_bytes[i]);
    }
    printf("\r\n");

    // Send command over Uart
    Com_Send(tx_bytes, total_len);


    return ui_err;
}



///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_notify
//
// Description:         It's used to receive data from the modem
//               
// Parameter:
//        INPUT:
//                      None
//       OUTPUT:
//                      rx_ptr:         Received data from modem
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int td_notify(struct modemPacket* rx_ptr)
{
    unsigned int ui_err = 0;
    unsigned int i, loop, total_len;
    char msglen[2];
    char* cur_ptr;

    total_len = loop = 0;
    // First, try to receive '@' symbol
    do {
        usleep(50 * 1000);
        Com_Receive(&(rx_ptr->header), 1);
    } while ((rx_ptr->header != '@') && (++ loop <= 40));
    // Then, receive the rest
    if (rx_ptr->header == '@') {
        Com_Receive(&(rx_ptr->xid), 1);
        Com_Receive(&(rx_ptr->type), 1);
        Com_Receive(&(rx_ptr->numParam), 1);
        total_len += 4;
        usleep(100 * 1000);
        for (i = 0; i < rx_ptr->numParam; i ++) {
            Com_Receive(&(rx_ptr->subsys[i]), 1);
            Com_Receive(&(rx_ptr->field[i]), 1);
            Com_Receive(msglen, 2);
            rx_ptr->len[i] = ((int)msglen[0] << 8) + (int)msglen[1];
            total_len += 4;
            if (rx_ptr->len[i] != 0) {
                Com_Receive(rx_ptr->message[i], rx_ptr->len[i]);
                total_len += rx_ptr->len[i];
            }
            else {
                memset(rx_ptr->message[i], 0, MESSAGE_LEN);
            }
        }
    }
    else {
        rx_ptr->header = 0x0;
        ui_err = 1;
    }
    //For debug
    cur_ptr = (char*)rx_ptr;
    printf("Data received from Modem: ");
    for (i = 0; i < total_len; i ++) {
        printf("0x%x, ", cur_ptr[i]);
    }
    printf("\r\n");


    return ui_err;
}


///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_set
//
// Description:         It's used to set the parameters for the Modem
//               
// Parameter:
//        INPUT:
//                      set_request:	Request field for Modem
//                      set_data:	The related data field
//                      ID:		XID for Modem
//       OUTPUT:
//                      NONE
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int td_set(char* set_request, char set_data, char ID)
{
    unsigned int ui_err = 0;
    struct modemPacket tx_pkt;


    memset(&tx_pkt, 0, sizeof(struct modemPacket));
    tx_pkt.xid = ID;
    tx_pkt.type = 's';
    if (strcmp(set_request, "localAddr") == 0) {
        printf("td_set, Command: localAddr.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 18;
        tx_pkt.len[0] = 2;
        tx_pkt.message[0][0] = 0x0;
        tx_pkt.message[0][1] = set_data;
    }
    else if (strcmp(set_request, "remAddr") == 0) {
        printf("td_set, Command: remAddr");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 14;
        tx_pkt.len[0] = 2;
        tx_pkt.message[0][0] = 0x0;
        tx_pkt.message[0][1] = set_data;
    }
    else if (strcmp(set_request, "rtsAttempts") == 0) {
        printf("td_set, Command: rtsAttempts.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 67;
        tx_pkt.field[0] = 0;
        tx_pkt.len[0] = 6;
        tx_pkt.message[0][3] = set_data;
    }
    else if (strcmp(set_request, "dataAcks") == 0) {
        printf("td_set, Command: dataAcks.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 67;
        tx_pkt.field[0] = 13;
        tx_pkt.len[0] = 6;
        tx_pkt.message[0][3] = set_data;
        tx_pkt.message[0][5] = 3;
    }
    else if (strcmp(set_request, "dataRetrans") == 0) {
        printf("td_set, Command: dataRetrans.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 2;
        tx_pkt.len[0] = 2;
        tx_pkt.message[0][1] = set_data;
    }
    else if (strcmp(set_request, "sniffermode") == 0) {
        printf("td_set, Command: sniffermode.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 72;
        tx_pkt.field[0] = 1;
        tx_pkt.len[0] = 6;
        tx_pkt.message[0][1] = 0x01;
        tx_pkt.message[0][5] = set_data;
    }
    else {
        printf("td_set, Command: It's unknown.......\r\n");
        ui_err = 1;
    }
    // Send command
    if (ui_err == 0) {
        ui_err = td_send(&tx_pkt);
    }

    return ui_err;
}



///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_get
//
// Description:         It's used to read the parameters from the Modem
//               
// Parameter:
//        INPUT:
//                      get_request:	Request field for Modem
//                      ID:		XID for Modem
//       OUTPUT:
//                      NONE
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int td_get(char* get_request, char ID)
{
    unsigned int ui_err = 0;
    struct modemPacket tx_pkt;


    memset(&tx_pkt, 0, sizeof(struct modemPacket));
    tx_pkt.xid = ID;
    tx_pkt.type = 'g';
    if (strcmp(get_request, "localAddr") == 0) {
        printf("td_get, Command: localAddr.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 18;
        tx_pkt.len[0] = 0;
    }
    else if (strcmp(get_request, "remAddr") == 0) {
        printf("td_get, Command: remAddr.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 14;
        tx_pkt.len[0] = 0;
    }
    else if (strcmp(get_request, "mmpStat") == 0) {
        printf("td_get, Command: mmpStat.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 0;
        tx_pkt.field[0] = 0;
        tx_pkt.len[0] = 0;
    }
    else if (strcmp(get_request, "rtsAttempts") == 0) {
        printf("td_get, Command: rtsAttempts.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 67;
        tx_pkt.field[0] = 0;
        tx_pkt.len[0] = 0;
    }
    else if (strcmp(get_request, "dataRetrans") == 0) {
        printf("td_get, Command: dataRetrans.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 1;
        tx_pkt.field[0] = 2;
        tx_pkt.len[0] = 0;
    }
    else if (strcmp(get_request, "sniffermode") == 0) {
        printf("td_get, Command: sniffermode.\r\n");
        tx_pkt.numParam = 1;
        tx_pkt.subsys[0] = 72;
        tx_pkt.field[0] = 1;
        tx_pkt.len[0] = 0;
    }
    else {
        printf("td_get, Command: It's unknown.......\r\n");
        ui_err = 1;
    }
    // Send command
    if (ui_err == 0) {
        ui_err = td_send(&tx_pkt);
    }

    return ui_err;
}


///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_exec
//
// Description:         It's used to run command in the Modem
//               
// Parameter:
//        INPUT:
//                      exec_request:	Request field for Modem
//                      exec_msg:	The message field
//                      msglen:         The length of message
//                      ID:		XID for Modem
//       OUTPUT:
//                      NONE
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int td_exec(char* exec_request, char* exec_msg, char msglen, char ID)
{
    unsigned int ui_err = 0;
    struct modemPacket exec_pkt;


    memset(&exec_pkt, 0, sizeof(struct modemPacket));
    exec_pkt.xid = ID;
    exec_pkt.type = 'x';
    exec_pkt.numParam = 1;
    if (strcmp(exec_request, "remRange") == 0) {
        printf("td_exec, Command: remRange.\r\n");
        exec_pkt.subsys[0] = 2;
        exec_pkt.field[0] = 1;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remTestLink") == 0) {
        printf("td_exec, Command: remTestLink.\r\n");
        exec_pkt.subsys[0] = 1;
        exec_pkt.field[0] = 8;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remGetBatt") == 0) {
        printf("td_exec, Command: remGetBatt.\r\n");
        exec_pkt.subsys[0] = 1;
        exec_pkt.field[0] = 11;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remMMP") == 0) {
        printf("td_exec, Command: remMMP.\r\n");
        exec_pkt.subsys[0] = 1;
        exec_pkt.field[0] = 1;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remSetPower") == 0) {
        printf("td_exec, Command: remSetPower.\r\n");
        exec_pkt.subsys[0] = 1;
        exec_pkt.field[0] = 1;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remGetSreg") == 0) {
        printf("td_exec, Command: remGetSreg.\r\n");
        exec_pkt.subsys[0] = 7;
        exec_pkt.field[0] = 1;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remSendData") == 0) {
        printf("td_exec, Command: remSendData.\r\n");
        exec_pkt.subsys[0] = 1;
        exec_pkt.field[0] = 7;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "Ping") == 0) {
        printf("td_exec, Command: Ping.\r\n");
        exec_pkt.subsys[0] = 3;
        exec_pkt.field[0] = 0;
        exec_pkt.len[0] = msglen;
        memcpy(exec_pkt.message[0], exec_msg, msglen);
    }
    else {
        printf("td_exec, Command: It's unknown.......\r\n");
        ui_err = 1;
    }
    // Send command
    if (ui_err == 0) {
        ui_err = td_send(&exec_pkt);
    }

    return ui_err;
}




