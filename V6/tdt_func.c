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
#include <time.h>
#include <sys/time.h>
#include "Config.h"
#include "tda_func.h"
#include "tdt_func.h"
#include "CommUart.h"





#define MESSAGE_LEN_PING		6















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
static char				XID = 13;
static char				Ping_Enable = 0;




static struct def_state			state;
static struct def_queue_length		queue_length;
static struct def_Ping_queue		Ping_queue[8];








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
//                      callFrom:	The command
//        Output:
//			NONE
//
// Return Value:
//			0:		OK
//			else:		Error
//
////////////////////////////////////////////////////////////////////////
static unsigned int tdt_addAddrToBook(struct def_Ping_queue* dataPacket, char* callFrom)
{
    unsigned int ui_err = 0;


    return ui_err;
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
    unsigned int ui_err = 0;
    char msg[32];
    float prb;


    memset(msg, 0, sizeof(msg));
    prb = (float)rand() / RAND_MAX;
    if ((state.Ping == 10) || (prb < prob_ping_threshold)) {
        if ((state.Ping == 10) || ((state.Ping == 0) && (Ping_Enable == 1))) {
            msg[0] = 
            td_exec("Ping", msg, MESSAGE_LEN_PING, XID);
            XID = XID + 1;
            state.Ping = 1;
        }
    }
    else if (queue_length.Ping > 0) {
        if (strncmp(Ping_queue[0].signature, "00 03 03 00 00 06", 17) == 0) {
            printf("Ping: working on ping - process.\r\n");
        }
        else if (strncmp(Ping_queue[0].signature, "00  03 03 00 00 06", 17) == 0) {
            state.Ping = state.Ping < 1 ? state.Ping : 1;
            tdt_addAddrToBook(&Ping_queue[0], "Ping");
            printf("Ping: working on ping - arrived.\r\n");
        }
        else {
            printf("Unknown signature.\r\n");
        }

        queue_length.Ping -= 1;
    }


    return ui_err;
}



/////////////////////////////////////////////////////////////////////////
//
// Function Name:	tdt_notifyTask
//
// Description:		It's used to receive message from the modem
//                      in asynch mode, and do related update.
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
unsigned int tdt_notifyTask(void)
{
    unsigned int ui_err = 0;
    char msg[32];
    float prb;


    



    return ui_err;
}


