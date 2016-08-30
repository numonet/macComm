///////////////////////////////////////////////////
// File name:	main.c
//
//
//  Created on: 05/02/2016
//      Author: Mian Tang
//
//
///////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Common.h"
#include "td_func.h"
#include "tda_func.h"
#include "tdt_func.h"
#include "Config.h"




#define SLAVE_ADDRESS			1


#define PROBABILITY_THRESHOLD		0.01	

#define CMD_LENGTH			16
#define BUFFER_LENGTH			256


#define INTERVAL_40MS			2 * 1000
#define INTERVAL_50MS			50 * 1000


/////////////////////////////////////////////////////////////////////////////////////
// the local global variable
/////////////////////////////////////////////////////////////////////////////////////
unsigned int g_ui_flag;
unsigned int g_ui_mode;

int g_sockfd;
struct sockaddr_in g_serveraddr;


// For Basic Initialization
int Master_Node = 17;
int timer_counter1 = 0;
int timer_counter2 = 0;

// For Timer
timer_t first_timer;
timer_t second_timer;
timer_t third_timer;

struct timespec g_timer005 = {0, 0};
struct timespec g_timer004 = {0, 0};



/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////












 
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:                       socket_Init
// Function Description:        init the socket/use UDP protocol
// Function Parameters:
//                      INPUT:                  None
//                      OUTPUT:                 None
// Return Value:
//                                                      0: no error
//                                                      1: an error occur
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


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:		pth_SocketFunction
// Function Description:	Thread 1 for socket communication
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//				None
/////////////////////////////////////////////////////////////////////////////////////
static void *pth_SocketFunction()
{
	int ret;
	int rd_num;
	int status;
	socklen_t socklen;
	fd_set read_fds;
	struct timeval tv;
	char rd_buff[256];


	status = -1;
	g_ui_flag = NORMAL_COMMAND;
	g_ui_mode = COMMAND;
	memset(rd_buff,0,sizeof(rd_buff));
	socklen = sizeof(struct sockaddr);
	// Initialize socket
	socket_Init();
	//seclect mechanism
	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(g_sockfd,&read_fds);
		tv.tv_sec = 0;
		tv.tv_usec = 800000; //800ms
		ret = select(g_sockfd + 1,&read_fds,NULL,NULL,&tv);
		//seclect error
		if(ret < 0) {
			printf("Select error!!!\n");
			pthread_exit(&status);
		} else if(0 == ret){
			// Wait timeout
			//printf("Socket select timeout...\n");
			g_ui_flag = NORMAL_COMMAND;
			g_ui_mode = COMMAND;
		} else {
			if(FD_ISSET(g_sockfd,&read_fds)) {
				// Receive command
				rd_num = recvfrom(g_sockfd,rd_buff,sizeof(rd_buff),0,(struct sockaddr *)&g_serveraddr,&socklen);
				rd_buff[rd_num] = '\0';
				//PRI_DEBUG("-------%s-------\n",rd_buff);
				//printf("-------%s-------\n",rd_buff);
				if(rd_num < 0) {
					printf("Read socket error!!!\n");
				} else {
					printf("%s\n",rd_buff);
					// Run the related commands
					//sock_DataAnalysize(rd_num,rd_buff);
				}
			}
		}
	}
}



//////////////////////////////////////////////////////////
//
// Description: Thread 2 to be determined
//
/////////////////////////////////////////////////////////
static void *pth_UpperUartFunction()
{
    int time_interval;
    struct timespec timer_005;


    do {
        //printf("Thread 2 ..............................................\r\n");
        // Clock check for 0.05s interval
        clock_gettime(CLOCK_REALTIME, &timer_005);
        if (timer_005.tv_sec >= g_timer005.tv_sec) {
            time_interval = 1000000 * (timer_005.tv_sec - g_timer005.tv_sec);
            time_interval -= (g_timer005.tv_nsec / 1000);
            time_interval += (timer_005.tv_nsec / 1000);
        }
        else {
            time_interval = (int)(timer_005.tv_nsec / 1000 - g_timer005.tv_nsec / 1000);
        }
        if (time_interval <= INTERVAL_50MS) {
            usleep(INTERVAL_50MS - time_interval);
        }
        // Backup time
        clock_gettime(CLOCK_REALTIME, &g_timer005);
        // Run related work
        tdt_dataprodTask();
        if (++ timer_counter2 == 11) {
            //printf("Run tdt_qmonitorTask............................\r\n");
            tdt_qmonitorTask();
            timer_counter2 = 0;
        }
       
    } while (1);


    return NULL;
}



/////////////////////////////////////////////////////////////////////
//
// Description: Thread 3 to be determined
//
////////////////////////////////////////////////////////////////////
static void *pth_DownUartFunction()
{
    int time_interval;
    struct timespec timer_004;

    do {
        //printf("Thread 3 ................................................................\r\n");
        // Clock check for 40ms interval;
        clock_gettime(CLOCK_REALTIME, &timer_004);
        if (timer_004.tv_sec >= g_timer004.tv_sec) {
            time_interval = 1000000 * (timer_004.tv_sec - g_timer004.tv_sec);
            time_interval -= (g_timer004.tv_nsec / 1000);
            time_interval += (timer_004.tv_nsec / 1000);
        }
        else {
            time_interval = (int)(timer_004.tv_nsec / 1000 - g_timer004.tv_nsec / 1000);
        }
        if (time_interval <= INTERVAL_40MS) {
            usleep(INTERVAL_40MS - time_interval);
        }
        // Backup Timer
        clock_gettime(CLOCK_REALTIME, &g_timer004);
        // Run related work
        tdt_notifyTask();
        if (++ timer_counter1 == 400) {
            //printf("run tdt_pingTask.................................\r\n");
            tdt_pingTask();
            tdt_txTask();
            timer_counter1 = 0;
        }
        //printf("Thread 3, timer: %ds.\r\n", (int)timer_004.tv_sec);


    } while (1);



    return NULL;
}




/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			main
// Function Description:	main function
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int i, error, ret, numClock, macPara;
    char charClock[2], txPara, pingPara, xidPara;
    float packetratePara;
    void *status;
    struct sched_param param;

    pthread_t pth_Downuart, pth_Upperuart;
    pthread_attr_t pth_attr;


    // Clock function test
    tda_clock(0, "char", charClock, NULL);
    tda_clock(0, "num", NULL, &numClock);
    printf("The clock num is %d.\r\n", numClock);




    macPara = 4;
    txPara = 0;
    pingPara = 0;
    xidPara = 13;
    packetratePara = 0.004;
    // Arguments......
    if (argc > 1) {
        for (i = 1; i < argc; i += 2) {
            if (strcmp(argv[i], "-protocol") == 0) {
                macPara = atoi(argv[i + 1]);
                printf("Protocol is %d.\r\n", macPara);
            }
            else if (strcmp(argv[i], "-packetrate") == 0) {
                packetratePara = atof(argv[i + 1]);
                printf("packet rate is %f.\r\n", packetratePara);
            }
            else if (strcmp(argv[i], "-txenable") == 0) {
                txPara = atoi(argv[i + 1]);
                printf("tx enable is %d.\r\n", txPara);
            }
            else if (strcmp(argv[i], "-pingenable") == 0) {
                pingPara = atoi(argv[i + 1]);
                printf("ping enable is %d.\r\n", pingPara);
            }
            else if (strcmp(argv[i], "-xid") == 0) {
                xidPara = (char)atoi(argv[i + 1]);
                printf("XID is %d.\r\n", xidPara);
            }
        }
    }


    // Initialize global time and counter
    timer_counter1 = 0;
    timer_counter2 = 0;
    clock_gettime(CLOCK_REALTIME, &g_timer004);
    clock_gettime(CLOCK_REALTIME, &g_timer005);
    // Initialize Modem
    tdt_autoPing(1);
    tdt_autoSync(1);
    tdt_modemInit(macPara, txPara, pingPara, xidPara, packetratePara);
    //tda_getTime(&hour, &minute, &second);



    printf("Start the threads.....\r\n");
    pthread_attr_init(&pth_attr);
    // Use the default scheduler
    pthread_attr_setinheritsched(&pth_attr,PTHREAD_EXPLICIT_SCHED);
    // Configure Scheduler
    ret = pthread_attr_setschedpolicy(&pth_attr,SCHED_FIFO);
    if(ret != 0) {
        fprintf(stderr,"can't set thread policy!!!the error num is %s\n",strerror(ret));
        printf("Now use default policy SCHED_OTHER\n");
    }

	
#if 0
    // Create socket thread
    param.sched_priority=PRIORITY;
    // Configure Priority
    pthread_attr_setschedparam(&pth_attr,&param);
    // Create socket thread
    error = pthread_create(&pth_socket,&pth_attr,pth_SocketFunction,NULL);
    if(error != 0) {
        printf("create pth_socket error!!!\n");
        exit(1);
    }
#endif

#if 1
    // Create thread 2
    param.sched_priority=1;
    pthread_attr_setschedparam(&pth_attr,&param);
    error = pthread_create(&pth_Upperuart, &pth_attr, pth_UpperUartFunction,NULL);
    if(error != 0) {
        printf("create pth_UpperUart error!!!\n");
        exit(1);
    }

    // Create thread 3
    param.sched_priority=2;
    pthread_attr_setschedparam(&pth_attr,&param);
    error = pthread_create(&pth_Downuart, &pth_attr, pth_DownUartFunction,NULL);
    if(error != 0) {
        printf("create pth_uart error!!!\n");
        exit(1);
    }
#endif


    // Here we do time schronization if Master node...
    tdt_doSync();





    // Wait for the terminiation of threads
    error = pthread_join(pth_Upperuart,&status);
    if(error != 0) {
        printf("exit pth_Upperuart error,with error code:%d\n",(int)status);
    }
    error = pthread_join(pth_Downuart,&status);
    if(error != 0) {
        printf("exit pth_Downuart error,with error code:%d\n",(int)status);
    }
    pthread_attr_destroy(&pth_attr);


    return 0;
}
