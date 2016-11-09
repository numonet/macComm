//////////////////////////////////////////////////////////////////////////////
//
// Description:		This is for clock calculation. The function is
//                      same as td_clock.m file which is in Matlab project.
//
// Version:		V1.0
//
// Author:		Mian Tang
//
// Date:		05/10/2016
//
// Comment:
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>





////////////////////////////////////////////////////////////////////////////
//
// Extern Global Variables
//
///////////////////////////////////////////////////////////////////////////









///////////////////////////////////////////////////////////////////////////
//
// Function Name:	tda_clock
//
// Description:		It's used to read the system clock and convert it
//                      to number or array of char.
// Parameter:
//        INPUT:
//                      clock_dif:	clock difference
//                      clock_type: 	the format of clock for output
//                                  	char: array or char
//                                  	num: number
//       OUTPUT:
//                      clock_in_char:	the clock in char array format
//                      clock_in_num:	the clock in number format
//
// Return Value:
//                      0:		OK
//			else:		Error
//
//////////////////////////////////////////////////////////////////////////
unsigned int tda_clock(float clock_dif, char* clock_type, char* clock_in_char, int* clock_in_num)
{
    struct timespec systime;
    struct tm* time_vec;
    unsigned int ui_err = 0;
    unsigned int temp, clock_num;


    clock_gettime(CLOCK_REALTIME, &systime);
    time_vec = localtime(&systime.tv_sec);
    //printf("The current time: %d, %d, %d, %d:%d:%d:%dms.\r\n", 
    //        time_vec->tm_year + 1900, time_vec->tm_mon +1, time_vec->tm_mday,
    //        time_vec->tm_hour, time_vec->tm_min, time_vec->tm_sec, (int)(systime.tv_nsec / 1000000));

    // Convert nanosecond to the number of 0.1s
    temp = systime.tv_nsec / 100000000;
    if (strcmp(clock_type, "char") == 0) {
        clock_num = (10 * (time_vec->tm_min * 60 + time_vec->tm_sec) + temp + (int)(10 * clock_dif)) % 36000;
        clock_in_char[0] = (char)(clock_num / 256);
        clock_in_char[1] = (char)(clock_num % 256);
        //printf("The current time: %d:%d:%d:%dms.\r\n", time_vec->tm_hour, time_vec->tm_min, time_vec->tm_sec, systime.tv_nsec / 1000000);
        //printf("Time (0.1s) in char is 0x%x, 0x%x.\r\n", clock_in_char[0], clock_in_char[1]);
    }
    else if (strcmp(clock_type, "num") == 0) {
        *clock_in_num = (int)((time_vec->tm_min * 60 + time_vec->tm_sec) * 10 + temp);
    }
    else {
        ui_err = 0x1000;
    }


    return ui_err;
}










