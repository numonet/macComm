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
#include <time.h>

#include "Include.h"
#include "CommMain.h"
#include "Init.h"
	



/////////////////////////////////////////////////////////////////////////////////////
// the local global variable
/////////////////////////////////////////////////////////////////////////////////////
pthread_t pth_socket,pth_Downuart,pth_Upperuart;
pthread_attr_t g_attr;
sem_t sem1, sem2;
unsigned int g_ui_flag;
unsigned int g_ui_mode;
unsigned int g_ui_devid__;
int g_sockfd;
struct sockaddr_in g_serveraddr;
pthread_mutex_t mut, mut_Cmd;


/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////
extern unsigned int g_ui_recvsize;
extern unsigned int g_ui_upsize;
extern unsigned int g_ui_flag_sem;
extern int g_uartfd;
extern int g_Upperuartfd;





////////////////////////////////////////////
timer_t first_timer;
timer_t second_timer;
timer_t third_timer;




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
	unsigned int ui_error;
	int ret;
	int rd_num;
	int status;
	socklen_t socklen;
	fd_set read_fds;
	struct timeval tv;
	char rd_buff[BUFFSIZE];


	status = -1;
	g_ui_flag = NORMAL_COMMAND;
	g_ui_mode = COMMAND;
	memset(rd_buff,0,sizeof(rd_buff));
	socklen = sizeof(struct sockaddr);
	// Initialize socket
	ui_error = socket_Init();
	// Confirm that the file is not existed
	if(access(CHILD_UPDATE,F_OK) == 0) {
		remove(CHILD_UPDATE);
	}
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
			g_ui_recvsize = 0;
			g_ui_upsize = 0;
			g_ui_flag = NORMAL_COMMAND;
			g_ui_mode = COMMAND;
		} else {
			if(FD_ISSET(g_sockfd,&read_fds)) {
				// Receive command
				rd_num = recvfrom(g_sockfd,rd_buff,BUFFSIZE,0,(struct sockaddr *)&g_serveraddr,&socklen);
				rd_buff[rd_num] = '\0';
				//PRI_DEBUG("-------%s-------\n",rd_buff);
				//printf("-------%s-------\n",rd_buff);
				if(rd_num < 0) {
					printf("Read socket error!!!\n");
				} else {
					//printf("%s\n",rd_buff);
					//解析命令
					sock_DataAnalysize(rd_num,rd_buff);
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
    // test
    do {
        sleep(2);
        printf("Thread 2...\r\n");		
    } while (1);
	
}



/////////////////////////////////////////////////////////////////////
//
// Description: Thread 3 to be determined
//
////////////////////////////////////////////////////////////////////
static void *pth_DownUartFunction()
{

    do {
        sleep(2);
        printf("Thread 3......\r\n");
    } while (1);
}



static void timerHandler(int sig, siginfo_t *si, void *uc)
{
    printf("timerHandler.\r\n");
#if 1
    timer_t *tidp;

    tidp = si->si_value.sival_ptr;
    if (*tidp == first_timer) {
        printf("The first Timer.\r\n");
    } else if (*tidp == second_timer) {
        printf("The second Timer.\r\n");
    } else if (*tidp == third_timer) {
        printf("The third Timer.\r\n");
    }
#else
    printf("Timer Handler.\r\n");
#endif
}



static int makeTimer(timer_t* timerID, int expireMS, int intervalMS, int sigNo)
{
    struct sigevent         te;
    struct itimerspec       its;
    struct sigaction        sa;
    //int                     sigNo = SIGRTMIN;

    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sigNo, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed to setup signal handling.\r\n");
        return 1;
    }
    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = intervalMS * 1000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = expireMS * 1000000;
    timer_settime(*timerID, 0, &its, NULL);

    return 0;
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
int main()
{
    int error,ret;
    void *status;
    struct sched_param param;
    struct timespec timeinfo = {0, 0};
    int sigNo = SIGRTMIN;


    makeTimer(&first_timer, 200, 500, sigNo);
    makeTimer(&second_timer, 200, 1000, sigNo + 1);
    makeTimer(&third_timer, 200, 2000, sigNo + 2);
    //clock_getres(CLOCK_REALTIME, &timeinfo);



    pthread_attr_init(&g_attr);
    // Use the default scheduler
    pthread_attr_setinheritsched(&g_attr,PTHREAD_EXPLICIT_SCHED);
    // Configure Scheduler
    ret = pthread_attr_setschedpolicy(&g_attr,SCHED_FIFO);
    if(ret != 0) {
        fprintf(stderr,"can't set thread policy!!!the error num is %s\n",strerror(ret));
        printf("Now use default policy SCHED_OTHER\n");
    }
	
    // Mutex Init
    pthread_mutex_init(&mut,NULL);
    pthread_mutex_init(&mut_Cmd,NULL);
	
#if 1
    // Create socket thread
    param.sched_priority=PRIORITY;
    // Configure Priority
    pthread_attr_setschedparam(&g_attr,&param);
    // Create socket thread
    error = pthread_create(&pth_socket,&g_attr,pth_SocketFunction,NULL);
    if(error != 0) {
        printf("create pth_socket error!!!\n");
        exit(1);
    }
#endif

#if 1
    // Create thread 2
    param.sched_priority=1;
    pthread_attr_setschedparam(&g_attr,&param);
    error = pthread_create(&pth_Upperuart,&g_attr,pth_UpperUartFunction,NULL);
    if(error != 0) {
        printf("create pth_UpperUart error!!!\n");
        exit(1);
    }
#endif

#if 1
    // Create thread 3
    param.sched_priority=2;
    pthread_attr_setschedparam(&g_attr,&param);
    error = pthread_create(&pth_Downuart,&g_attr,pth_DownUartFunction,NULL);
    if(error != 0) {
        printf("create pth_uart error!!!\n");
        exit(1);
    }
#endif



    // Wait for the terminiation of threads
    error = pthread_join(pth_socket,&status);
    if(error != 0) {
        printf("exit pth_socket error,with error code:%d\n",(int)status);
    }
    error = pthread_join(pth_Upperuart,&status);
    if(error != 0) {
        printf("exit pth_Upperuart error,with error code:%d\n",(int)status);
    }
    error = pthread_join(pth_Downuart,&status);
    if(error != 0) {
        printf("exit pth_Downuart error,with error code:%d\n",(int)status);
    }
    pthread_attr_destroy(&g_attr);


    return 0;
}
