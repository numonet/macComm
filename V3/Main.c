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


#define SLAVE_ADDRESS			1


#define PROBABILITY_THRESHOLD		0.01	

#define CMD_LENGTH			16
#define BUFFER_LENGTH			256


/////////////////////////////////////////////////////////////////////////////////////
// the local global variable
/////////////////////////////////////////////////////////////////////////////////////
unsigned int g_ui_flag;
unsigned int g_ui_mode;

int g_sockfd;
struct sockaddr_in g_serveraddr;
pthread_mutex_t mut_queue;


// For Timer
timer_t first_timer;
timer_t second_timer;
timer_t third_timer;

struct timespec g_timer005 = {0, 0};
struct timespec g_timer2 = {0, 0};


// For Buffer
static unsigned char g_buffer[CMD_LENGTH * BUFFER_LENGTH];
static unsigned char* g_puc_readptr;
static unsigned char* g_puc_writeptr;
static unsigned int g_buffercounter;

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
					printf("%s\n",rd_buff);
					// Run the related commands
					//sock_DataAnalysize(rd_num,rd_buff);
				}
			}
		}
	}
}



int Com_Init(const char* dev_path, int baudrate, int data_bits, int stop_bits, char parity)
{
    const int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800};
    const int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800};
    struct termios opt;
    int fd, i ,err;


    fd = open(dev_path, O_RDWR);
    if (fd >= 0) {
        err = fd;
        if(tcgetattr(fd, &opt) == -1) {
            printf("Get Uart Attribute Error.\r\n");
            err = -2;
        } else {
            // Configure Baud rate
            for (i = 0; i < sizeof(name_arr) / sizeof(int); ++i) {
                if (baudrate == name_arr[i]) {
                    tcflush(fd, TCIOFLUSH);
                    cfsetispeed(&opt, speed_arr[i]);
                    cfsetospeed(&opt, speed_arr[i]);
                    if (tcsetattr(fd, TCSANOW, &opt) == -1 ) {
                       printf("Configure baudrate error.\r\n");
                       err = -3;
                    }
                }
                break;
            }
            // Configure Parity, Stop bit, Data bit
            opt.c_cflag &= ~CSIZE;
            switch (data_bits) {
                case 5:
                    opt.c_cflag |= CS5;
                    break;
                case 6:
                    opt.c_cflag |= CS6;
                    break;
                case 7:
                    opt.c_cflag |= CS7;
                    break;
                case 8:
                    opt.c_cflag |= CS8;
                    break;
                default:
                    printf("Unsupported data bits\n");
                    err = -4;
            }
            switch (parity) {
                case 'n':
                case 'N':
                    opt.c_cflag &= ~PARENB;
                    opt.c_iflag &= ~INPCK;
                    break;
                case 'o':
                case 'O':
                    opt.c_cflag |= PARODD | PARENB;
                    opt.c_iflag |= INPCK;
                    break;
                case 'e':
                case 'E':
                    opt.c_cflag = opt.c_cflag | PARENB & ~PARODD;
                    opt.c_iflag |= INPCK;
                    break;
                default:
                    printf("Unsupported parity\n");
                    err = -5;
            }
            switch (stop_bits) {
                case 1:
                    opt.c_cflag &= ~CSTOPB;
                    break;
                case 2:
                    opt.c_cflag &= CSTOPB;
                    break;
                default:
                    printf("Unsupporter stop bits\n");
                    err = -6;
            }
        }
    }
    else {
        printf("Open Uart port error.\r\n");
        err = -1;
    }
    


    return err;
}


unsigned int Com_ConfigureTimeout(int fd, unsigned char uc_time)
{
    unsigned int ui_err;
    struct termios opt;


    ui_err = 0;
    if (tcgetattr(fd, &opt) == -1) {
        printf("read_Timeout/tcgetattr");
        ui_err = 1;
    } else {
        //RAW mode
        opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        opt.c_oflag &= ~OPOST;
        //Close IXON,IXOFF  0x11 0x13
        opt.c_iflag &= ~(IXANY | IXON | IXOFF);
        opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
        //set read timeout, 1 means 100ms
        opt.c_cc[VTIME] = uc_time;
        opt.c_cc[VMIN] = 0;
        if(tcsetattr(fd, TCSANOW, &opt) == -1) {
            printf("read_Timeout/tcsetattr");
            ui_err = 1;
        }
    }

    return ui_err;
}



unsigned int printmsg(unsigned char* str, unsigned char* puc_ptr, unsigned int length)
{
    unsigned int loop;

    if (str != NULL) {
        printf("%s: ", str);
    }
    for (loop = 0; loop < length; loop ++) {
        printf("0x%x, ", puc_ptr[loop]);
    }
    printf("\r\n");

    return 0;
}




//////////////////////////////////////////////////////////
//
// Description: Thread 2 to be determined
//
/////////////////////////////////////////////////////////
static void *pth_UpperUartFunction()
{
    float probability;
    int time_interval;
    struct timespec timer_005;

    clock_gettime(CLOCK_REALTIME, &g_timer005);
    // Initialize buffer
    memset(g_buffer, 0, sizeof(g_buffer));
    g_puc_readptr = g_puc_writeptr = g_buffer;
    g_buffercounter = 1;

    do {
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
        if (time_interval <= 50 * 1000) {
            usleep(50 * 1000 - time_interval);
        }
        // Backup time
        clock_gettime(CLOCK_REALTIME, &g_timer005);
        //printf("Thread 2: time: %ds, %dus.\r\n", (int)timer_005.tv_sec, (int)(timer_005.tv_nsec / 1000));
        // Run probability 0.01
        probability = ((float)rand() / RAND_MAX);
        //printf("Thread 2: probability: %.4f.\r\n", probability);
        if (probability <= PROBABILITY_THRESHOLD) {
            pthread_mutex_lock(&mut_queue);
            g_puc_writeptr[0] = (unsigned char)(g_buffercounter >> 24);
            g_puc_writeptr[1] = (unsigned char)(g_buffercounter >> 16);
            g_puc_writeptr[2] = (unsigned char)(g_buffercounter >> 8);
            g_puc_writeptr[3] = (unsigned char)g_buffercounter;
            g_puc_writeptr[7] = SLAVE_ADDRESS;
            // print
            printmsg("Thread 2, fill data", g_puc_writeptr, CMD_LENGTH);
            if ((g_buffercounter - 1) % BUFFER_LENGTH == (BUFFER_LENGTH - 1)) {
                g_puc_writeptr = g_buffer;
                g_buffercounter = 1;
            }
            else {
                g_puc_writeptr += CMD_LENGTH;
                g_buffercounter += 1;
            }
            pthread_mutex_unlock(&mut_queue);
        }

       
    } while (1);
}



/////////////////////////////////////////////////////////////////////
//
// Description: Thread 3 to be determined
//
////////////////////////////////////////////////////////////////////
static void *pth_DownUartFunction()
{
    int time_interval, offset;
    struct timespec timer_2;

    clock_gettime(CLOCK_REALTIME, &g_timer2);
    do {
        // Clock check for 2s interfal;
        clock_gettime(CLOCK_REALTIME, &timer_2);
        time_interval = (int)(timer_2.tv_nsec / 1000 - g_timer2.tv_nsec / 1000);
        if (time_interval < 2000 * 1000) {
            usleep(2000 * 1000 - time_interval);
        }
        // Backup Timer
        clock_gettime(CLOCK_REALTIME, &timer_2);
        printf("Thread 3, timer: %ds.\r\n", (int)timer_2.tv_sec);
        // Check buffer
        pthread_mutex_lock(&mut_queue);
        if (g_puc_readptr != g_puc_writeptr) {
            // print
            printmsg("Thread 3, get data", g_puc_readptr, CMD_LENGTH);
            offset = (g_puc_readptr - g_buffer) / CMD_LENGTH;
            if (offset == (BUFFER_LENGTH - 1)) {
                g_puc_readptr = g_buffer;
            }
            else {
                g_puc_readptr += CMD_LENGTH;
            }
        }
        pthread_mutex_unlock(&mut_queue);

    } while (1);

#if 0        // For Uart Application

    const char uart_dev1[] = "/dev/ttyO1";
    const char uart_dev2[] = "/dev/ttyO2";
    char wbuffer1[] = "Hello, World!";
    char wbuffer2[] = "Underwater acoustic communication.";
    char rbuffer1[64], rbuffer2[64];
    int num, fd1, fd2;

    
    // Uart 1
    fd1 = Com_Init(uart_dev1, 115200, 8, 1, 'N');
    if (fd1 >= 0) {
        Com_ConfigureTimeout(fd1, 5);
    }
    else {
        printf("Configure Uart error.\r\n");
    }
    // Uart 2
    fd2 = Com_Init(uart_dev2, 115200, 8, 1, 'N');
    if (fd2 >= 0) {
        Com_ConfigureTimeout(fd2, 5);
    }
    else {
        printf("Configure Uart error.\r\n");
    }


    do {
        usleep(2000 * 1000);
        if (write(fd1, wbuffer1, 13) <= 0 ) {
            printf("Send data over Uart error.\r\n");
        }
        if (write(fd2, wbuffer2, 34) <= 0 ) {
            printf("Send data over Uart error.\r\n");
        }
        memset(rbuffer1, 0, sizeof(rbuffer1));
        memset(rbuffer2, 0, sizeof(rbuffer2));
        usleep(100 * 1000);
        read(fd1, rbuffer1, 34);
        read(fd2, rbuffer2, 13);
        printf("Uart1 Read data over Uart: %s\r\n", rbuffer1);   
        printf("Uart2 Read data over Uart: %s\r\n", rbuffer2);    
        printf("Thread 3......\r\n");
    } while (1);

#endif
}



static void timerHandler(int sig, siginfo_t *si, void *uc)
{
    timer_t *tidp;

    tidp = si->si_value.sival_ptr;
    if (*tidp == first_timer) {
        printf("Timer 1.\r\n");
    } else if (*tidp == second_timer) {
        printf("Timer 2.\r\n");
    } else if (*tidp == third_timer) {
        printf("Timer 3.\r\n");
    }
}



static int makeTimer(timer_t* timerID, int expireMS, int intervalMS)
{
    struct sigevent         te;
    struct itimerspec       its;
    struct sigaction        sa;
    int                     sigNo = SIGRTMIN;

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
    if (timer_create(CLOCK_REALTIME, &te, timerID) == -1) {
        printf("Create Timer error.\r\n");
    }
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = intervalMS * 1000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = expireMS * 1000000;
    if (timer_settime(*timerID, 0, &its, NULL) == -1 ) {
        printf("Set Timer error.\r\n");
    }

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

    pthread_t pth_socket, pth_Downuart, pth_Upperuart;
    pthread_attr_t pth_attr;



    pthread_attr_init(&pth_attr);
    // Use the default scheduler
    pthread_attr_setinheritsched(&pth_attr,PTHREAD_EXPLICIT_SCHED);
    // Configure Scheduler
    ret = pthread_attr_setschedpolicy(&pth_attr,SCHED_FIFO);
    if(ret != 0) {
        fprintf(stderr,"can't set thread policy!!!the error num is %s\n",strerror(ret));
        printf("Now use default policy SCHED_OTHER\n");
    }
	
    // Mutex Init
    pthread_mutex_init(&mut_queue, NULL);
	
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
    error = pthread_create(&pth_Upperuart,&pth_attr,pth_UpperUartFunction,NULL);
    if(error != 0) {
        printf("create pth_UpperUart error!!!\n");
        exit(1);
    }
#endif

#if 1
    // Create thread 3
    param.sched_priority=2;
    pthread_attr_setschedparam(&pth_attr,&param);
    error = pthread_create(&pth_Downuart,&pth_attr,pth_DownUartFunction,NULL);
    if(error != 0) {
        printf("create pth_uart error!!!\n");
        exit(1);
    }
#endif

    //makeTimer(&first_timer, 200, 500);
    //makeTimer(&second_timer, 200, 500);
    //makeTimer(&third_timer, 200, 200);






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
    pthread_attr_destroy(&pth_attr);


    return 0;
}
