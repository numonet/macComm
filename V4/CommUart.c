///////////////////////////////////////////////////
// File name:	CommUart.c
//
//
//  Created on: 05/02/2016
//      Author: Mian Tang
//
//
///////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "Include.h"






/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////////////
// Local variable
/////////////////////////////////////////////////////////////////////////////////////

static int uart_fd;





 
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:               Com_Init
// Function Description:        Initialize Uart port for transmission
// Function Parameters:
//                      INPUT:                  None
//                      OUTPUT:                 None
// Return Value:
//                                                      0: no error
//                                                      1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
int Com_Init(const char* dev_path, int baudrate, int data_bits, int stop_bits, char parity)
{
    const int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800};
    const int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800};
    struct termios opt;
    int i ,err = 0;


    uart_fd = open(dev_path, O_RDWR);
    if (uart_fd >= 0) {
        if(tcgetattr(uart_fd, &opt) == -1) {
            printf("Get Uart Attribute Error.\r\n");
            err = -2;
        } else {
            // Configure Baud rate
            for (i = 0; i < sizeof(name_arr) / sizeof(int); ++i) {
                if (baudrate == name_arr[i]) {
                    tcflush(uart_fd, TCIOFLUSH);
                    cfsetispeed(&opt, speed_arr[i]);
                    cfsetospeed(&opt, speed_arr[i]);
                    if (tcsetattr(uart_fd, TCSANOW, &opt) == -1 ) {
                       printf("Configure baudrate error.\r\n");
                       err = -3;
                    }
                    break;
                }
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


unsigned int Com_ConfigureTimeout(unsigned char uc_time)
{
    unsigned int ui_err;
    struct termios opt;


    ui_err = 0;
    if (tcgetattr(uart_fd, &opt) == -1) {
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
        if(tcsetattr(uart_fd, TCSANOW, &opt) == -1) {
            printf("read_Timeout/tcsetattr");
            ui_err = 1;
        }
    }

    return ui_err;
}


unsigned int Com_Send(char* puc_ptr, int ui_len)
{
    unsigned int num;

    num = write(uart_fd, puc_ptr, ui_len);

    return num;
}


unsigned int Com_Receive(char* puc_ptr, int ui_len)
{
    int num;

    num = read(uart_fd, puc_ptr, ui_len);

    return num;
}


unsigned int Com_Close()
{
    close(uart_fd);

    return 0;
}
