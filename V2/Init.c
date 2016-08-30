/*
 * init.c
 *
 *  Created on: 2012-11-24
 *      Author: zhang
 */
#include "Include.h"

/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////
extern int g_sockfd;
extern struct sockaddr_in g_serveraddr;
int g_uartfd, g_Upperuartfd, g_file_fd;


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			socket_Init
// Function Description:	init the socket/use UDP protocol
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
unsigned int socket_Init(void)
{
	unsigned int ui_error;


	ui_error = 0;
	if((g_sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		ui_error = 1;
	} else {
		bzero(&g_serveraddr,sizeof(g_serveraddr));
		g_serveraddr.sin_family = AF_INET;
		g_serveraddr.sin_port = htons(SERVER_PORT);
		g_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if(bind(g_sockfd,(struct sockaddr*)&g_serveraddr,sizeof(g_serveraddr)) < 0) {
			perror("bind");
			ui_error = 1;
		}
	}

	return ui_error;
}


