/*
 * CommPlatform.c
 *
 *  Created on: 2013-3-1
 *      Author: Zhang Qiang
 *		Company: Telestone
 */

#include "UdasDatabase.h"
#include "CommUart.h"
#include "Include.h"




extern int g_uartfd , g_Upperuartfd;
extern pthread_mutex_t mut, mut_Cmd;
extern sem_t sem1;
extern unsigned int g_ui_flag_sem;

/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			open_dev
// Function Description:	open the /dev/tty* device
// Function Parameters:
//			INPUT:			the path of device
//			OUTPUT:			None
// Return Value:
//							fd: the file description or an error occur
/////////////////////////////////////////////////////////////////////////////////////
int open_Dev(const char *dev_path)
{
	int fd;


	fd = open(dev_path, O_RDWR);
	if (-1 == fd) {
		perror("Can't open serial port");
	}

	return fd;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			set_speed
// Function Description:	set the uart's speed
// Function Parameters:
//			INPUT:			fd : file description
//							speed: the uart's speed
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
unsigned int set_Speed(int fd, int speed)
{
	int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800};
	int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800};
	unsigned int ui_err;
	int i, status;
	struct termios opt;


	ui_err = 0;
	if(tcgetattr(fd, &opt) == -1) {
		perror("set_speed/tcgetattr");
		ui_err = 1;
	} else {
		for (i = 0; i < sizeof(name_arr) / sizeof(int); ++i) {
			if (speed == name_arr[i]) {
				//刷新输入输出缓冲
				tcflush(fd, TCIOFLUSH);
				cfsetispeed(&opt, speed_arr[i]);
				cfsetospeed(&opt, speed_arr[i]);
				//立刻写入fd
				status = tcsetattr(fd, TCSANOW, &opt);
				if (status == -1) {
					perror("set_speed/tcsetattr");
					ui_err = 1;
				}
			}
		}
	}

	return ui_err;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			set_parity
// Function Description:	set the uart's parity
// Function Parameters:
//			INPUT:			fd : file description
//							data_bits: the data bits
//							stop_bits: the stop bits
//							parity: the uart's parity
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
unsigned int set_Parity(int fd, int data_bits, int stop_bits,char parity)
{
	unsigned int ui_err;
	struct termios opt;


	ui_err = 0;
	if (tcgetattr(fd, &opt) == -1) {
		perror("set_parity/tcgetattr");
		ui_err = 1;
	} else {
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
				fprintf(stderr, "Unsupported data bits\n");
				ui_err = 1;
		}
		switch (parity)	{
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
				fprintf(stderr, " Unsupported parity\n");
				ui_err = 1;
		}
		switch (stop_bits) {
			case 1:
				opt.c_cflag &= ~CSTOPB;
				break;
			case 2:
				opt.c_cflag &= CSTOPB;
				break;
			default:
				fprintf(stderr, "Unsupporter stop bits\n");
				ui_err = 1;
		}
	}

	return ui_err;
}

/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			set_ReadTimeout
// Function Description:	set the uart's readtimeout
// Function Parameters:
//			INPUT:			fd : file description
//							uc_time : timeout(unsigned char)
//			OUTPUT:			None
// Return Value:
//							ui_err : 0 no error
//									 1 an error occur
/////////////////////////////////////////////////////////////////////////////////////
unsigned int set_ReadTimeout(int fd, unsigned char uc_time)
{
	unsigned int ui_err;
	struct termios opt;


	ui_err = 0;
	if (tcgetattr(fd, &opt) == -1) {
		perror("read_Timeout/tcgetattr");
		ui_err = 1;
	} else {
		//RAW mode
		opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		opt.c_oflag &= ~OPOST;
		//Close IXON,IXOFF  0x11 0x13
		opt.c_iflag &= ~(IXANY | IXON | IXOFF);
		//0x0d->>>0x0a
		opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
		//set read timeout
		opt.c_cc[VTIME] = uc_time;//this... 0.1s为单位
		opt.c_cc[VMIN] = 0;
		if(tcsetattr(fd, TCSANOW, &opt) == -1) {
			perror("read_Timeout/tcsetattr");
			ui_err = 1;
		}
	}


	return ui_err;
}

#if 0
unsigned int ComP_ReadTimeout(int fd, unsigned char* puc_buffer, unsigned int ui_len)
{
	unsigned int ui_err;
	int len;
	struct termios opt;


	ui_err = 0;
	if (tcgetattr(fd, &opt) == -1) {
		perror("read_Timeout/tcgetattr");
		ui_err = 1;
	} else {
		//RAW mode
		opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		opt.c_oflag &= ~OPOST;
		//Close IXON,IXOFF  0x11 0x13
		opt.c_iflag &= ~(IXANY | IXON | IXOFF);
		//0x0d->>>0x0a
		opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
		//opt.c_oflag &= ~(ONLCR | OCRNL);
		//set read timeout
		opt.c_cc[VTIME] = 8;//this... 0.1s为单位
		opt.c_cc[VMIN] = ui_len;
		if(tcsetattr(fd, TCSANOW, &opt) == -1) {
			perror("read_Timeout/tcsetattr");
			ui_err = 1;
		}
	}

	if(puc_buffer != NULL) {
		len = read(fd,puc_buffer,ui_len);
		if(len <= 0) {
			ui_err = 1;
		}
	}

	return ui_err;
}
#endif
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_Sleep
// Function Description:	uart sleep nms
// Function Parameters:
//			INPUT:
//							ui_nms : the sleep time(ms)
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_Sleep(unsigned int nMs)
{
	usleep(nMs * 1000);
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_DownDeInit
// Function Description:	Down uart initaltion 
// Function Parameters:
//			INPUT:
//							None
//			OUTPUT:			None
// Return Value:
//							0
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_DownDeInit(void)
{
	close(g_uartfd);
	
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_UpDeInit
// Function Description:	Upper uart dele initaltion 
// Function Parameters:
//			INPUT:
//							None
//			OUTPUT:			None
// Return Value:
//							0
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_UpDeInit(void)
{
	close(g_Upperuartfd);
	
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_UpWrite
// Function Description:	Upper uart write 
// Function Parameters:
//			INPUT:
//							*puc_Cmd 
//							ui_CmdLen
//			OUTPUT:			None
// Return Value:
//							ui_err: 0-sucess
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_UpWrite(unsigned char* puc_Cmd, unsigned int ui_CmdLen)
{
	unsigned int ui_err, i, ui_len,j;

	ui_err = 2;
	
	if (write(g_Upperuartfd, puc_Cmd, ui_CmdLen)) {
		ui_err = 0;
	}
	else {
		printf("******************************************Upper write uart error\n");
	}

	return ui_err;
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_UpRead
// Function Description:	Upper uart read
// Function Parameters:
//			INPUT:
//							ui_ReturnLen
//			OUTPUT:			*puc_Payload 
// Return Value:
//							ui_err: 0-sucess
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_UpRead(unsigned char* puc_Payload, unsigned int ui_ReturnLen)
{
	int  i_cnt, i;
	unsigned int ui_err, ui_num, ui_count, ui_flag;
	unsigned char *puc_ptr;
	
	ui_err = 2;
	puc_ptr = puc_Payload;
	i_cnt = ui_ReturnLen;
	ui_count = 0;
	ui_num = 0;
	ui_flag = 1;
	if (g_ui_flag_sem == 1) {
		do {
			ui_num = read(g_Upperuartfd, puc_ptr, i_cnt);
			if (ui_num > 0) {		
				puc_ptr += ui_num;
				i_cnt -= ui_num;
			}
			else if (ui_num == 0) {
				printf("****************************************Upper Uart read Timeout!\n");
				ui_flag = 0;
			}
			else {
				printf("****************************************Upper Uart read error!\n");
				ui_flag = 0;
			}
		
		} while((i_cnt > 0) && (ui_flag != 0));
		g_ui_flag_sem = 0;
		ComP_Sleep(100);
	}
	
	if (i_cnt == 0) {
		ui_err = 0;
	}
	
	
	return ui_err;
}
unsigned int ComP_UpReadClear(void)
{
	tcflush(g_Upperuartfd, TCIFLUSH);
	
	return 0;
}
unsigned int ComP_ReadClear(void)
{
	tcflush(g_uartfd, TCIFLUSH);
	
	return 0;
}

static unsigned int Uart_ReadData(int fd, unsigned char *puc_Payload, unsigned int ui_ReturnLen) 
{
	fd_set fds;
	struct timeval uart_timeout;
	unsigned int ui_Revlength, ui_out;
	int nread, i_cnt;
	int ret;
	unsigned char *puc_ptr;
	
	nread = 0;
	ui_Revlength = 0;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	uart_timeout.tv_sec = 1;
	uart_timeout.tv_usec = 0;
	ret = select(fd + 1, &fds, NULL, NULL, &uart_timeout);
	if(ret < 0) {
		perror("+++++++++++++++++++++++++++++++++++++++++++++++++++select\n");
	}
	else if(ret == 0) {
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++timeout\n");
		nread = 0;
		ui_Revlength = 0;
	}
	else {
		if(FD_ISSET(fd, &fds)) {	
			i_cnt = ui_ReturnLen;
			ui_out = 0;
			puc_ptr = puc_Payload;
			do {	
				usleep(200000);	
				nread = read(fd, puc_ptr, i_cnt);
				if(nread > 0) {
					puc_ptr += nread;
					i_cnt -= nread;
				}
			} while((i_cnt > 0) && (++ ui_out < 40));
			printf("timeout: %d\n", ui_out);
			if(ui_out < 40) {
				ui_Revlength = ui_ReturnLen;
			}
			else {
				nread = 0;	
			}
		}
		else {
			printf("******************************FD_ISSET error.\n");
		}
	}
	
	return ui_Revlength;
} 
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			ComP_Transfer
// Function Description:	use the uart transfering and receiving data
// Function Parameters:
//			INPUT:			puc_Cmd : the base address of transfering data
//							ui_CmdLen : the length of transfering data
//							ui_ReturnLen : the length of receiving data
//			OUTPUT:
//							puc_Payload : the base address of receiving data
// Return Value:
//							ui_err : 0 no error
//									 1 an error occur
/////////////////////////////////////////////////////////////////////////////////////
unsigned int ComP_Transfer(unsigned char* puc_Cmd, unsigned int ui_CmdLen, unsigned int ui_ReturnLen, unsigned char* puc_Payload)
{
	unsigned int ui_err,ui_i;
	unsigned char* puc_tmp;
	unsigned int len,flag;


	ui_err = 0;
	puc_tmp = puc_Payload;
	if(write(g_uartfd,puc_Cmd,ui_CmdLen)) {
		ComP_Sleep(300);	
		if(Uart_ReadData(g_uartfd, puc_tmp, ui_ReturnLen) != ui_ReturnLen) {
			ui_err = 1;
			printf("ReadData failed\n");
		}
		ComP_Sleep(100);
	}
	else {
		ui_err = 1;
		printf("Down Uart Write error\n");
	}

	return ui_err;
}


unsigned int ComP_EnterCriticalSection()
{
	pthread_mutex_lock(&mut);
	
	return 0;
}


unsigned int ComP_LeaveCriticalSection()
{
	 pthread_mutex_unlock(&mut);
	
	return 0;
}

unsigned int ComP_CmdEnterCriticalSection()
{
	pthread_mutex_lock(&mut_Cmd);
	
	return 0;
}

unsigned int ComP_CmdLeaveCriticalSection()
{
	pthread_mutex_unlock(&mut_Cmd);
	
	return 0;
}


