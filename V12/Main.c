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




#define SLAVE_ADDRESS			1


#define PROBABILITY_THRESHOLD		0.01	

#define CMD_LENGTH			16
#define BUFFER_LENGTH			256


#define INTERVAL_2MS			2 * 1000
#define INTERVAL_50MS			50 * 1000


#define PIPE_FILE			"/tmp/macFifo"





/////////////////////////////////////////////////////////////////////////////////////
// the local global variable
/////////////////////////////////////////////////////////////////////////////////////
unsigned int g_ui_flag;
unsigned int g_ui_mode;


// For Basic Initialization
int Master_Node = 17;
unsigned int timer_counter1 = 0;
unsigned int timer_counter2 = 0;

// For Timer
timer_t first_timer;
timer_t second_timer;
timer_t third_timer;

struct timespec g_timer050 = {0, 0};
struct timespec g_timer002 = {0, 0};


// For Pipe
int fifo_fd;





/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////












/////////////////////////////////////////////////////////////////////////////////////
// Function Name:		pth_CommFunc
// Function Description:	Thread 1 for communication to another process
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//				None
/////////////////////////////////////////////////////////////////////////////////////
static void *pth_CommFunc()
{
    char shareBuffer[100];


    memset(shareBuffer, 0, sizeof(shareBuffer));
    fifo_fd = open(PIPE_FILE, O_WRONLY | O_NONBLOCK);
    if (fifo_fd > 0) {
        printf("Pipe file open OK.\r\n");
        do {
            memset(shareBuffer, 0, sizeof(shareBuffer));
            tdt_dataCopy(shareBuffer);
            write(fifo_fd, shareBuffer, 100);
            Com_Sleep(1);        
        } while (1);
    }


    return NULL;
}



//////////////////////////////////////////////////////////
//
// Description: Thread 2 to be determined
//
/////////////////////////////////////////////////////////
static void *pth_macTask1Func()
{
    int time_interval;
    struct timespec timer_050;


    do {
        //printf("Thread 2 ..............................................\r\n");
        // Clock check for 0.05s interval
        clock_gettime(CLOCK_REALTIME, &timer_050);
        if (timer_050.tv_sec >= g_timer050.tv_sec) {
            time_interval = 1000000 * (timer_050.tv_sec - g_timer050.tv_sec);
            time_interval -= (g_timer050.tv_nsec / 1000);
            time_interval += (timer_050.tv_nsec / 1000);
        }
        else {
            time_interval = (int)(timer_050.tv_nsec / 1000 - g_timer050.tv_nsec / 1000);
        }
        if (time_interval <= INTERVAL_50MS) {
            usleep(INTERVAL_50MS - time_interval);
        }
        // Backup time
        clock_gettime(CLOCK_REALTIME, &g_timer050);
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
static void *pth_macTask2Func()
{
    int time_interval, sync_delay_in_ms, slot_time_in_sec, slot_time_in_msec;
    int slot_time_in_ms, accu_time_in_ms;
    struct timespec timer_sync;
    struct timespec timer_002;
    

    // Here is used to confirm that the timer slot start from the beginning of each slot time
    sync_delay_in_ms = 0;
    accu_time_in_ms = 0;
    slot_time_in_ms = tdt_getSlotCounter() * 2;
    slot_time_in_sec = slot_time_in_ms / 1000;
    slot_time_in_msec = (slot_time_in_ms) % 1000;
    printf("*********** %d, %d.\r\n", slot_time_in_sec, slot_time_in_msec);
    clock_gettime(CLOCK_REALTIME, &timer_sync);
    if (slot_time_in_sec != 0) {
        sync_delay_in_ms = (timer_sync.tv_sec % slot_time_in_sec)  * 1000;
    }
    if (slot_time_in_msec != 0) {
        sync_delay_in_ms += ((timer_sync.tv_nsec / 1000000) % slot_time_in_msec);  
    }
    else {
        sync_delay_in_ms += timer_sync.tv_nsec / 1000000;
    }
    sync_delay_in_ms = slot_time_in_ms - sync_delay_in_ms;

    usleep(sync_delay_in_ms * 1000);
    printf("********Right now, the time is %ds:%dms, delay is needed for %dms.", timer_sync.tv_sec, (timer_sync.tv_nsec / 1000000), sync_delay_in_ms);

    do {
        //printf("Thread 3 ................................................................\r\n");
        // Clock check for 2ms interval;
        clock_gettime(CLOCK_REALTIME, &timer_002);
        if (timer_002.tv_sec >= g_timer002.tv_sec) {
            time_interval = 1000000 * (timer_002.tv_sec - g_timer002.tv_sec);
            time_interval -= (g_timer002.tv_nsec / 1000);
            time_interval += (timer_002.tv_nsec / 1000);
        }
        else {
            time_interval = (int)(timer_002.tv_nsec / 1000 - g_timer002.tv_nsec / 1000);
        }
        if (time_interval <= INTERVAL_2MS) {
            usleep(INTERVAL_2MS - time_interval);
        }
        // Backup Timer
        clock_gettime(CLOCK_REALTIME, &g_timer002);
        // Run related work
        tdt_notifyTask();
        if (++ timer_counter1 == tdt_getSlotCounter()) {
            //printf("run tdt_pingTask.................................\r\n");
            tdt_pingTask();
            printf("$$$$$$$$$$$$$$$$$$$$ Before tdt_txTask, the time is %ds : %dms. $$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n", g_timer002.tv_sec, g_timer002.tv_nsec / 1000000);
            tdt_txTask();
            timer_counter1 = 0;
        }
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
    int i, error, ret, macPara, masterPara;
    char txPara, pingPara, xidPara;
    float packetratePara, slotPara;
    void *status;
    struct sched_param param;

    pthread_t pth_Comm, pth_macTask1, pth_macTask2;
    pthread_attr_t pth_attr;




    // MAC Protocol: 1: Aloha; 2: Slotted Aloha; 3: TARS; 4: LiSS
    macPara = 4;
    // TX Enable: 0: Disable; 1: Enable
    txPara = 0;
    // Ping Enable: 0: Disable; 1: Enable
    pingPara = 0;
    // XID:
    xidPara = 13;
    // Packet rate: 
    packetratePara = 0.004;
    // Master Node: If it equals local address of Modem, it is Master Node
    masterPara = 17;
    // Time of slot in second
    slotPara = 2.0;

    // Arguments input to support parameter input from the command line
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
            else if (strcmp(argv[i], "-master") == 0) {
                masterPara = (char)atoi(argv[i + 1]);
                printf("Master Node is 0x%x.\r\n", masterPara);
            }
            else if (strcmp(argv[i], "-slot") == 0) {
                slotPara = atof(argv[i + 1]);
                printf("Slot time is %f second.\r\n", slotPara);
            }
            else {
                printf("Help: example...\r\n");
                printf("        macComm -protocol 4 -packetrate 0.02 -txenable 0 -pingenable 0 -xid 8 -master 17 -slot 2.0\r\n");
                return 0;
            }
        }
    }
    else {
        printf("Run as default...................\r\n");
    }


    // Initialize global time and counter
    timer_counter1 = 0;
    timer_counter2 = 0;
    clock_gettime(CLOCK_REALTIME, &g_timer002);
    clock_gettime(CLOCK_REALTIME, &g_timer050);
    // Initialize Modem
    tdt_autoPing(1);
    tdt_autoSync(1);
    tdt_modemInit(macPara, txPara, pingPara, xidPara, packetratePara, masterPara, slotPara);


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

	
    // Configure Priority of thread
    param.sched_priority=PRIORITY;
    pthread_attr_setschedparam(&pth_attr,&param);
    // Create communication thread which communicates with another application.
    error = pthread_create(&pth_Comm, &pth_attr, pth_CommFunc, NULL);
    if(error != 0) {
        printf("create communication thread error!!!\n");
        exit(1);
    }
    // Create thread 2
    param.sched_priority=1;
    pthread_attr_setschedparam(&pth_attr,&param);
    error = pthread_create(&pth_macTask1, &pth_attr, pth_macTask1Func, NULL);
    if(error != 0) {
        printf("create MAC task1 thread error!!!\n");
        exit(1);
    }
    // Create thread 3
    param.sched_priority=2;
    pthread_attr_setschedparam(&pth_attr,&param);
    error = pthread_create(&pth_macTask2, &pth_attr, pth_macTask2Func, NULL);
    if(error != 0) {
        printf("create MAC task2 thread error!!!\n");
        exit(1);
    }

    // Here we do time schronization if Master node...
    tdt_doSync();



    // Wait for the terminiation of threads
    error = pthread_join(pth_Comm, &status);
    if (error != 0) {
        printf("Exit pth_CommFunc thread error, with error code: %d.\r\n", (int)status);
    }
    error = pthread_join(pth_macTask1, &status);
    if(error != 0) {
        printf("Exit pth_macTask1Func thread error, with error code: %d.\r\n", (int)status);
    }
    error = pthread_join(pth_macTask2, &status);
    if(error != 0) {
        printf("Exit pth_macTask2Func thread error, with error code: %d.\r\n", (int)status);
    }
    pthread_attr_destroy(&pth_attr);


    return 0;
}
