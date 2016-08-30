/*
 * com.c
 *
 *  Created on: 2012-11-24
 *      Author: zhang
 */
#include "Include.h"


/////////////////////////////////////////////////////////////////////////////////////
// the local global variable
/////////////////////////////////////////////////////////////////////////////////////
unsigned int g_ui_srcid;
unsigned int g_ui_srctype;
unsigned int g_ui_destid;
unsigned int g_ui_destype;
unsigned int g_ui_msgid;
unsigned int g_ui_msgtype;
unsigned int g_ui_msglen;
unsigned int g_ui_recvsize;
unsigned int g_ui_upsize;
unsigned int g_ui_filecrc;
unsigned int g_ui_downfile_type;
unsigned int g_ui_downfile_size;
unsigned int g_ui_upfile_size;



/////////////////////////////////////////////////////////////////////////////////////
// the extern variable
/////////////////////////////////////////////////////////////////////////////////////
extern int g_sockfd;
extern struct sockaddr_in g_serveraddr;
extern unsigned int g_ui_flag;
extern unsigned int g_ui_mode;



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			restart_Child
// Function Description:	restart the child process
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int restart_Child()
{
	int fd;
	int error;
	char buff[] = "001";


	error = 0;
	if((fd = open(FIFO,O_RDWR)) < 0) {
		perror("open");
		error = -1;
	} else {
		if(write(fd,buff,sizeof(buff)) < 0) {
			perror("write");
			error = -1;
		}
	}
	close(fd);

	return error;
}




/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			do_FileChange
// Function Description:	base of the g_downfile_type,then do something
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
unsigned int do_FileChange()
{
	unsigned int ui_error;

	ui_error = 0;
	//0001 应用程序 0002 库文件 0003其他文件++++++++++++++
	switch(g_ui_downfile_type) {
		case APP_FILE:
			restart_Child();
			break;
		case CONFIG_FILE:
			//+++++
			break;
		case DATA_FILE:
			//+++++
			break;
		case LOG_FILE:
			//+++++
			break;
		case OTHER_FILE:
			//+++++
			break;
		default:
			//不支持的文件类型
			break;
	}

	return ui_error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			res_MyId
// Function Description:	response device ID to host
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int res_MyId()
{
	unsigned char wr_buff[BUFFSIZE];
	unsigned int ui_error;
	int send_num;

	ui_error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_GET_DESTID >> 8 & 0xFF;
	wr_buff[17] = RESPON_GET_DESTID & 0xFF;
	wr_buff[18] = RESPON_LEN1 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN1 & 0xFF;
	wr_buff[20] = DEST_ID_H;
	wr_buff[21] = DEST_ID_H;
	wr_buff[22] = DES_ID >> 8;
	wr_buff[23] = DES_ID;
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN1,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		ui_error = -1;
	}
#ifdef DEBUG
	else {
		printf("send my id to client\n");
	}
#endif

	return ui_error;
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_FileTypeError
// Function Description:	To the host to send the file type error
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_FileTypeError()
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;


	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = MSG_TYPEERR >> 8;
	wr_buff[15] = MSG_TYPEERR;
	wr_buff[16] = COMMAND_ERR >> 8 & 0xFF;
	wr_buff[17] = COMMAND_ERR & 0xFF;
	wr_buff[18] = RESPON_LEN3 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN3 & 0xFF;
	wr_buff[20] = DES_ID >> 8;
	wr_buff[21] = DES_ID;
	wr_buff[22] = FILE_TYPEERR >> 8;
	wr_buff[23] = FILE_TYPEERR;
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN3,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	}
#ifdef DEBUG
	else {
		printf("-------filetype error-----------\n");
	}
#endif
	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_CommandError
// Function Description:	To the host to send the command error
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_CommandError()
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;


	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = MSG_TYPEERR >> 8;
	wr_buff[15] = MSG_TYPEERR;
	wr_buff[16] = COMMAND_ERR >> 8 & 0xFF;
	wr_buff[17] = COMMAND_ERR & 0xFF;
	wr_buff[18] = RESPON_LEN3 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN3 & 0xFF;
	wr_buff[20] = DES_ID >> 8;
	wr_buff[21] = DES_ID;
	wr_buff[22] = NOT_SUPPORTCMD >> 8;
	wr_buff[23] = NOT_SUPPORTCMD;
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN3,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	}
#ifdef DEBUG
	else {
		printf("-------command error-----------\n");
	}
#endif

	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			update_File
// Function Description:	update the process
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int update_File()
{
	int error = 0;


	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			write_File
// Function Description:	write data to file from memery
// Function Parameters:
//			INPUT:			pc_mem: the memery base address of file
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int write_File(char* pc_mem)
{
	int error;
	int file_fd;
	char *pc_mem_tmp;
	unsigned int ui_calnum,ui_count;
	unsigned int ui_calsize;

	error = 0;
	if((file_fd = open(CHILD_UPDATE,O_CREAT | O_EXCL | O_RDWR,00777)) < 0) {
		perror("open");
		error = -1;
	} else {
		//这里不能用ptr++,free的时候会出错
		pc_mem_tmp = pc_mem;
		ui_count = 0;
		ui_calnum = g_ui_recvsize / WR_SIZE;
		ui_calsize = g_ui_recvsize % WR_SIZE;
		if(ui_calnum) {
			do {
				if(write(file_fd,pc_mem_tmp,WR_SIZE) < 0) {
					perror("write");
					remove(CHILD_UPDATE);
					error = -1;
					break;
				}
				pc_mem_tmp += WR_SIZE;
			} while((++ui_count) < ui_calnum);
		}
		if(ui_calsize) {
			if(write(file_fd,pc_mem_tmp,ui_calsize) < 0) {
				perror("write");
				error = -1;
				remove(CHILD_UPDATE);
			}
		}
		close(file_fd);
	}
#ifdef DEBUG
			printf("write over...!!!\n");
#endif
	return error;
}

static int write_configFile(char* pc_mem)
{
	int error;
	int cfgfile_fd;
	char *pc_mem_tmp;
	unsigned int ui_calnum,ui_count;
	unsigned int ui_calsize;

	error = 0;
	if((cfgfile_fd = open(CONFIG_SOURCE, O_CREAT | O_EXCL | O_RDWR, 00777)) < 0) {
		perror("cfgfile_fd open");
		error = -1;
	} else {
		//这里不能用ptr++,free的时候会出错
		pc_mem_tmp = pc_mem;
		ui_count = 0;
		ui_calnum = g_ui_recvsize / WR_SIZE;
		ui_calsize = g_ui_recvsize % WR_SIZE;
		if(ui_calnum) {
			do {
				if(write(cfgfile_fd,pc_mem_tmp,WR_SIZE) < 0) {
					perror("cfgfile_fd ui_calnum write");
					remove(CHILD_UPDATE);
					error = -1;
					break;
				}
				pc_mem_tmp += WR_SIZE;
			} while((++ui_count) < ui_calnum);
		}
		if(ui_calsize) {
			if(write(cfgfile_fd,pc_mem_tmp,ui_calsize) < 0) {
				perror("cfgfile_fd ui_calsize write");
				error = -1;
				remove(CONFIG_SOURCE);
			}
		}
		close(cfgfile_fd);
	}
#ifdef DEBUG
			printf("write over...!!!\n");
#endif
	return error;
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			update_File
// Function Description:	update the process
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int update_configFile(void)
{
	int error = 0;


	if(access(SYSDEFAULT_CFGFILE, F_OK) == 0 && access(USER_CFGFILE, F_OK) == 0) {
		if(remove(USER_CFGFILE) < 0) {
			printf("delete USER_CONFIG_SOURCE error!\n");
			error = -1;
		}else {
			if(rename(CONFIG_SOURCE,USER_CFGFILE) < 0) {
				printf("rename update error!!!\n");
				error = -1;
			}
		}
	} else if(access(USER_CFGFILE, F_OK) == 0) {
		if(system("cp USER_CFGFILE SYSDEFAULT_CFGFILE") != 0) {
			error = -1;
		}else {
			if(remove(USER_CFGFILE) < 0) {
				printf("delete USER_CONFIG_SOURCE error!\n");
				error = -1;
			}else {
				if(rename(CONFIG_SOURCE, USER_CFGFILE) < 0) {
					printf("rename update error!!!\n");
					error = -1;
				}
			}
		}
	} else if (access(SYSDEFAULT_CFGFILE, F_OK) == 0) {
		if(rename(CONFIG_SOURCE, USER_CFGFILE) < 0) {
			printf("rename update error!!!\n");
			error = -1;
		}
	}else {
		if(rename(CONFIG_SOURCE, SYSDEFAULT_CFGFILE) < 0) {
				printf("rename update error!!!\n");
				error = -1;
		}
		else {
			if(system("cp SYSDEFAULT_CFGFILE USER_CFGFILE") != 0) {
				error = -1;
			}else {
				
			}
		}
	}
	

	return error;
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_ResponPrepare
// Function Description:	To the host to send preparing up/down file
// Function Parameters:
//			INPUT:			pc_mem: the memery base address of file
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_ResponPrepare(char *pc_mem)
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;

	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_DOWNFILE >> 8 & 0xFF;
	wr_buff[17] = RESPON_DOWNFILE & 0xFF;
	wr_buff[18] = RESPON_LEN2 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN2 & 0xFF;
	if(pc_mem != NULL) {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = SUCCESS;
	} else {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = ERROR;
	}
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN2,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	}

#ifdef DEBUG
	else {
		printf("recv command down file and now start down...\n");
	}
#endif

	return error;
}




/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_ResponDownFile
// Function Description:	To the host to response down file
// Function Parameters:
//			INPUT:			num: write to file number
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_ResponDownFile(int num)
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;

	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_DOWNSTART >> 8 & 0xFF;
	wr_buff[17] = RESPON_DOWNSTART & 0xFF;
	wr_buff[18] = RESPON_LEN2 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN2 & 0xFF;
	if(g_ui_recvsize != 0) {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = SUCCESS;
	} else {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = ERROR;
	}
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN2,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	}
#ifdef DEBUG
		printf("write file data at %d\n",num);
#endif

	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_ResponDownFileOver
// Function Description:	To the host to response down over file
// Function Parameters:
//			INPUT:			pc_mem: the memery base address of file
//							ui_calcrc: calculate the crc value
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_ResponDownFileOver(char *pc_mem,unsigned int ui_calcrc)
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;

	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = MSG_TYPEERR >> 8;
	wr_buff[15] = MSG_TYPEERR;
	wr_buff[16] = RESPON_DOWNOVER >> 8 & 0xFF;
	wr_buff[17] = RESPON_DOWNOVER & 0xFF;
	wr_buff[18] = RESPON_LEN2 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN2 & 0xFF;
	if(g_ui_recvsize == g_ui_downfile_size && g_ui_filecrc == ui_calcrc) {
		wr_buff[20] = SUCCESS >> 8;
		wr_buff[21] = SUCCESS;
	} else {
		wr_buff[20] = SUCCESS >> 8;
		wr_buff[21] = ERROR;
	}
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN2,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	} else {
		if(wr_buff[21] != ERROR) {
#ifdef DEBUG
			printf("check ok,now write to file...!!!\n");
#endif

#if 0
			if(write_File(pc_mem) < 0) {
				printf("write file error!!!...\n");
				error = -1;
			} else {
				if(update_File() < 0) {
#ifdef DEBUG
					printf("update file error!!!\n");
#endif
					error = -1;
				} else {
#ifdef DEBUG
					printf("update file ok!!!\n");
#endif
					do_FileChange(g_ui_downfile_type);
				}
			}
#endif


#if 1   // TEST
		switch(g_ui_downfile_type) {
			case APP_FILE:
				if(write_File(pc_mem) < 0) {
					printf("write file error!!!...\n");
					error = -1;
				} 
				else {
					if(update_File() < 0) {
						printf("update file error!!!\n");
						error = -1;
					} else {
						//printf("update file ok!!!\n");
						restart_Child();
					}
				}
				
				break;
			case CONFIG_FILE:
				//+++++
				if (write_configFile(pc_mem) < 0) {
					printf("write configuration file error!!!...\n");
					error = -1;
				}
				else {
					if (update_configFile() < 0) {
						printf("update file error!!!\n");
						error = -1;
					}
					else {
                                                printf("Configure EU para........................\r\n");
					}
				}
				break;
			case DATA_FILE:
				//+++++
				break;
			case LOG_FILE:
				//+++++
				break;
			case OTHER_FILE:
				//+++++
				break;
			default:
				//不支持的文件类型
				break;
		}

#endif // TEST OVER
		}
	}

	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_ResponUpFilePrepare
// Function Description:	To the host to response preparing up file
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_ResponUpFilePrepare()
{
	unsigned char wr_buff[BUFFSIZE];
	int send_num;

	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_UPFILE >> 8 & 0xFF;
	wr_buff[17] = RESPON_UPFILE & 0xFF;
	wr_buff[18] = RESPON_LEN1 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN1 & 0xFF;
	wr_buff[20] = g_ui_upfile_size >> 24;
	wr_buff[21] = g_ui_upfile_size >> 16;
	wr_buff[22] = g_ui_upfile_size >> 8;
	wr_buff[23] = g_ui_upfile_size;
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN1,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		return -1;
	} else {
#ifdef DEBUG
		printf("up filesize to web\n");
#endif
		return 0;
	}
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_UpFileStart
// Function Description:	To the host to start up file
// Function Parameters:
//			INPUT:			fd: file description
//							ui_file_num: up file number 0,1,2...
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_UpFileStart(int fd,unsigned int ui_file_num)
{
	unsigned char wr_buff[PACKAGE_SIZE + FILE_NUM + RD_SIZE + CRC_SIZE];
	int send_num;
	int rd_num;
	int error;
	unsigned int ui_count,ui_len,ui_off,ui_cnt;
	unsigned long cal_file_crc;
	unsigned char *ptr;


	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_UPSTART >> 8 & 0xFF;
	wr_buff[17] = RESPON_UPSTART & 0xFF;
	wr_buff[18] = RESPON_LEN4 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN4 & 0xFF;
	//wr_buff[20],wr_buff[21]为序列号
	wr_buff[20] = ui_file_num >> 8;
	wr_buff[21] = ui_file_num;
	if((rd_num = read(fd,&wr_buff[22],RD_SIZE)) < 0) {
		perror("read");
	} else {
		g_ui_upsize += rd_num;
		cal_file_crc = CRC32(&wr_buff[22],rd_num);
		wr_buff[22 + rd_num] = cal_file_crc >> 24;
		wr_buff[22 + rd_num + 1] = cal_file_crc >> 16;
		wr_buff[22 + rd_num + 2] = cal_file_crc >> 8;
		wr_buff[22 + rd_num + 3] = cal_file_crc;
		ptr = wr_buff;
		ui_count = 0;
		ui_len = PACKAGE_SIZE + FILE_NUM + rd_num + CRC_SIZE;
		ui_cnt = ui_len / SEND_SIZE;
		ui_off = ui_len % SEND_SIZE;
		if(ui_cnt){
			do {
				send_num = sendto(g_sockfd,ptr,SEND_SIZE,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
				if(send_num < 0) {
					perror("send");
					error = -1;
					close(fd);
					break;
				} else {
					ptr += SEND_SIZE;
				}
			} while ((++ ui_count) < ui_cnt);
		}
		if(ui_off){
			send_num = sendto(g_sockfd,ptr,ui_off,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
			if(send_num < 0) {
				perror("send");
				error = -1;
				close(fd);
			}
		}

#ifdef DEBUG
		else {

			printf("up filesize to web\n");
		}
#endif
	}

	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			send_UpFileOver
// Function Description:	To the host to up file over
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							0: no error
//							-1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int send_UpFileOver()
{
	unsigned char wr_buff[BUFFSIZE];
	int error;
	int send_num;


	error = 0;
	wr_buff[0] = DES_ID >> 8;
	wr_buff[1] = DES_ID;
	wr_buff[2] = g_ui_destype >> 8;
	wr_buff[3] = g_ui_destype;
	wr_buff[4] = g_ui_srcid >> 8;
	wr_buff[5] = g_ui_srcid;
	wr_buff[6] = g_ui_srctype >> 8;
	wr_buff[7] = g_ui_srctype;
	wr_buff[8] = PROTO_VER >> 8 & 0xFF;
	wr_buff[9] = PROTO_VER & 0xFF;
	wr_buff[10] = SYS_ID >> 8 & 0xFF;
	wr_buff[11] = SYS_ID & 0xFF;
	wr_buff[12] = RESERVE >> 8 & 0xFF;
	wr_buff[13] = RESERVE & 0xFF;
	wr_buff[14] = g_ui_msgtype >> 8;
	wr_buff[15] = g_ui_msgtype;
	wr_buff[16] = RESPON_UPOVER >> 8 & 0xFF;
	wr_buff[17] = RESPON_UPOVER & 0xFF;
	wr_buff[18] = RESPON_LEN2 >> 8 & 0xFF;
	wr_buff[19] = RESPON_LEN2 & 0xFF;
	if(g_ui_upsize == g_ui_upfile_size) {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = SUCCESS;
	} else {
		wr_buff[20] = SUCCESS;
		wr_buff[21] = ERROR;
	}
	send_num = sendto(g_sockfd,wr_buff,PACKAGE_SIZE + RESPON_LEN2,0,(struct sockaddr *)&g_serveraddr,sizeof(struct sockaddr));
	if(send_num < 0) {
		perror("send");
		error = -1;
	}
#ifdef DEBUG
	else {
		printf("up filesize to web over\n");
	}
#endif

	return error;
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			check_FileType
// Function Description:	check the get file type
// Function Parameters:
//			INPUT:			ui_getfile_type: Obtained file type
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static unsigned int check_FileType(unsigned int ui_getfile_type)
{
	unsigned int ui_error;


	ui_error = 0;
	switch(g_ui_downfile_type) {
		case APP_FILE:
			if(ui_getfile_type != ELF) {
				ui_error = 1;
			}
			break;
		case CONFIG_FILE:
			if(ui_getfile_type != TELE) {
				ui_error = 1;
			}
			break;
		case DATA_FILE:
			if(ui_getfile_type != LIB) {
				ui_error = 1;
			}
			break;
		case LOG_FILE:
			if(ui_getfile_type != LOG) {
				ui_error = 1;
			}
			break;
		case OTHER_FILE:
			ui_error = 0;
			//if(ui_getfile_type != OTHER) {
				//ui_error = 1;
			//}
			break;
		default:
			//不支持的文件类型
			break;
	}

	return ui_error;
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			open_File
// Function Description:	open the file depend on file type
// Function Parameters:
//			INPUT:			ui_file_type: the file type APP,CONFIG,LOG...
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int open_File(unsigned int ui_file_type)
{
	int fd;

	switch(ui_file_type) {
		case APP_FILE:
			fd = open(APP_SOURCE,O_RDONLY);
			break;
		case CONFIG_FILE:
			fd = open(CONFIG_SOURCE,O_RDONLY);
			break;
		case DATA_FILE:
			fd = open(LIB_SOURCE,O_RDONLY);
			break;
		case LOG_FILE:
			fd = open(LOG_SOURCE,O_RDONLY);
			break;
		case OTHER_FILE:
			fd = open(OTHER_SOURCE,O_RDONLY);
			break;
		default:
			//不支持的文件类型
			break;
	}

	return fd;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			check_Command
// Function Description:	Check whether the command
// Function Parameters:
//			INPUT:			buff: the base address of command
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int check_Command(unsigned char *buff)
{
	int error = 0;


	g_ui_srcid = (buff[SRCID_ADDR_H] << 8) | buff[SRCID_ADDR_S];
	g_ui_srctype = (buff[SRCTYPE_ADDR_H] << 8) | buff[SRCTYPE_ADDR_S];
	g_ui_destid = (buff[DESID_ADDR_H] << 8) | buff[DESID_ADDR_S];
	g_ui_destype = (buff[DESTYPE_ADDR_H] << 8) | buff[DESTYPE_ADDR_S];
	g_ui_msgtype = (buff[MSGTYPE_ADDR_H] << 8) | buff[MSGTYPE_ADDR_S];
	g_ui_msgid = (buff[MSGID_ADDR_H] << 8) | buff[MSGID_ADDR_S];
	g_ui_msglen = (buff[MSGLEN_ADDR_H] << 8) | buff[MSGLEN_ADDR_S];
	//网管或web
	if(g_ui_srcid == SRCID_WEB || g_ui_srcid == SRCID_CNET) {
		//广播或本设备ID
		//if(g_ui_destid == DES_ID || g_ui_destid == BROAD_DESTID) {
			//单播(client->sever),单播(server->client),广播(client->server)
			//if(g_ui_msgtype == SINGLE_MSG) {
				error = 0;
			//}
		//} else {
			//error = -1;
		//}
	//} else {
		//error = -1;
	//}
	}

	return error;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			pre_DownFile
// Function Description:	prepare to down the file
// Function Parameters:
//			INPUT:			pc_mem: the point to the memery base address of file
//							rx_buff:the base of command
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
static void pre_DownFile(unsigned char **pc_mem,unsigned char *rx_buff)
{
	//unsigned int ui_downfile_size;


	g_ui_downfile_size = (rx_buff[FILESIZE_ADDR_H16] << 24) | (rx_buff[FILESIZE_ADDR_H8] << 16) |
							(rx_buff[FILESIZE_ADDR_S16] << 8) | rx_buff[FILESIZE_ADDR_S8];
	if(g_ui_downfile_size > FILE_SIZE) {
		printf("File is too large!!!\n");
	} else  {
		g_ui_downfile_type = (rx_buff[FILETYPE_ADDR_H] << 8) | rx_buff[FILETYPE_ADDR_S];
		//去掉4个ｂｙｔｅｓ的ＣＲＣ校验
		g_ui_downfile_size -= CRC_SIZE;
		*pc_mem = (char *)malloc(FILE_SIZE);
		if(!(*pc_mem)) {
			perror("No memery!");
		} else {
			memset(*pc_mem,0,FILE_SIZE);
			if(send_ResponPrepare(*pc_mem) < 0) {
				printf("send_ResponPrepare error...\n");
				free(*pc_mem);
			} else {
				g_ui_flag = DOWNFILE_COMMAND;
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			down_File
// Function Description:	down the file to local
// Function Parameters:
//			INPUT:			ptr: the file memery address
//							buff: the base address of command
//							rd_num: the number of the file
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
static void down_File(unsigned char *ptr,unsigned char *buff,int rd_num)
{
	unsigned int ui_file_num,ui_getfile_type;
	unsigned int ui_calcrc;
	unsigned long cal_crc_tmp;
	unsigned int ui_result;
	static int recv_filesize;
	static unsigned char *pc_mem_tmp;

	if(pc_mem_tmp == NULL) {
		pc_mem_tmp = ptr;
	}
	if(g_ui_mode == COMMAND) {
		if(check_Command(buff) == 0) {
			if(g_ui_msgid == MSGID_DOWNSTART) {
				ui_file_num = buff[FILENUM_ADDR_H] << 8 | buff[FILENUM_ADDR_S];
				//recv_filesize为一次接收到的文件大小
				recv_filesize = rd_num - COMMAND_LEN - FILE_NUM;
				memcpy(pc_mem_tmp,buff + COMMAND_LEN + FILE_NUM,recv_filesize);
				//printf("ptr[0]=0x%x ptr[1]=0x%x ptr[2]=0x%x ptr[3]=0x%x\n",ptr[0],ptr[1],ptr[2],ptr[3]);
				pc_mem_tmp += recv_filesize;
				//计算接受到文件的大小
				g_ui_recvsize += recv_filesize;
				if((g_ui_msglen - FILE_NUM) != recv_filesize) {
					g_ui_mode = DATA;
				}
				if(send_ResponDownFile(ui_file_num) < 0) {
#ifdef DEBUG
					printf("down file error!!!\n");
#endif
					free(ptr);
					recv_filesize = 0;
					//g_ui_downfile_size = 0;
					pc_mem_tmp = NULL;
				}
#ifdef DEBUG
				printf("---msglen=%d----recv_filesize=%d-----\n",g_ui_msglen,recv_filesize);
#endif
			} else if(g_ui_msgid == MSGID_DOWNOVER){
				//CRC在最后4个字节
				g_ui_filecrc = (ptr)[g_ui_recvsize - 4] << 24 | (ptr)[g_ui_recvsize - 3] << 16 |
						(ptr)[g_ui_recvsize - 2] << 8 | (ptr)[g_ui_recvsize - 1];
				//前4个字节.ＥＬＦ/ＴＥＬＥ...
				ui_getfile_type = (ptr)[0] << 24 |(ptr)[1] << 16 | (ptr)[2] << 8 | (ptr)[3];
				//printf("ptr[0]=0x%x ptr[1]=0x%x ptr[2]=0x%x ptr[3]=0x%x\n",ptr[0],ptr[1],ptr[2],ptr[3]);
				ui_result = check_FileType(ui_getfile_type);
				if(ui_result == 0) {
					//去掉4bytes的CRC校验
					g_ui_recvsize -= CRC_SIZE;
					cal_crc_tmp = CRC32(ptr,g_ui_recvsize);
					ui_calcrc = cal_crc_tmp & 0xFFFFFFFF;
					if(send_ResponDownFileOver(ptr,ui_calcrc) < 0) {
						printf("down file error!!!\n");
					}
				} else {
					send_FileTypeError();
				}
				recv_filesize = 0;
				//g_ui_downfile_size = 0;
				pc_mem_tmp = NULL;
				g_ui_recvsize = 0;
				g_ui_flag = NORMAL_COMMAND;
				free(ptr);
			}
		} else {
			send_CommandError();
		}
	} else if(g_ui_mode == DATA) {
		recv_filesize += rd_num;
		memcpy(pc_mem_tmp,buff,rd_num);
		pc_mem_tmp += rd_num;
		g_ui_recvsize += rd_num;
		if(recv_filesize == (g_ui_msglen - FILE_NUM)) {
			g_ui_mode = COMMAND;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			pre_UpFile
// Function Description:	prepare to up file
// Function Parameters:
//			INPUT:			buff: the base address of command
//			OUTPUT:			None
// Return Value:
//							0: no error
//							1: an error occur
/////////////////////////////////////////////////////////////////////////////////////
static int pre_UpFile(unsigned char *buff)
{
	unsigned int ui_upfile_type;
	int up_file_fd;
	off_t off_upfile_size;


	up_file_fd = 0;
	ui_upfile_type = (buff[UP_FILETYPE_ADDR_H] << 8) | buff[UP_FILETYPE_ADDR_S];
	up_file_fd = open_File(ui_upfile_type);
	if(up_file_fd < 0) {
		perror("open");
	} else {
		g_ui_upsize = 0;
		off_upfile_size = lseek(up_file_fd,0,SEEK_END);
		g_ui_upfile_size = off_upfile_size;
		//重新把文件指针定到文件头位置
		lseek(up_file_fd,0,SEEK_SET);
		if(send_ResponUpFilePrepare() < 0) {
			printf("up file error!!!!!\n");
			close(up_file_fd);
		} else {
			g_ui_flag = UPFILE_COMMAND;
		}
	}


	return up_file_fd;
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			up_File
// Function Description:	up the file
// Function Parameters:
//			INPUT:			up_filefd: the up file description
//							buff: the base address of command
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
static void up_File(int up_filefd,unsigned char *buff)
{
	static unsigned int ui_file_num;


	if(check_Command(buff) == 0) {
		if(g_ui_msgid == MSGID_UPSTART) {
			ui_file_num++;
			if(send_UpFileStart(up_filefd,ui_file_num) < 0) {
				printf("up file error!!!!!\n");
				g_ui_upsize = 0;
				ui_file_num = 0;
				close(up_filefd);
			}
		} else if(g_ui_msgid == MSGID_UPOVER) {
			if(send_UpFileOver() < 0) {
				printf("up file error!!!!!\n");
			}
			g_ui_upsize = 0;
			ui_file_num = 0;
			g_ui_flag = NORMAL_COMMAND;
			close(up_filefd);
		}
	} else {
		send_CommandError();
	}
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			sock_DataAnalysize
// Function Description:	Analysize the socket data
// Function Parameters:
//			INPUT:			rd_num: the size of the socket data
//							rx_buff: the base address of socket data
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
void sock_DataAnalysize(int rd_num, unsigned char *rx_buff)
{
	static unsigned char *pc_mem;
	//unsigned int ui_downfilesize;
	//unsigned int ui_upfilesize;
	static int up_filefd;


	if(g_ui_flag == NORMAL_COMMAND) {
		//检查命令头是否是正确的命令
		if(check_Command(rx_buff) == 0) {
			//是本模块的命令
			switch(g_ui_msgid) {
				//获取设备ID命令
				case GET_DESTID:
					res_MyId();
					break;
				//下载命令
				case MSGID_DOWNFILE:
					pc_mem = NULL;
					pre_DownFile(&pc_mem,rx_buff);
					break;
				//上传命令
				case MSGID_UPFILE:
					up_filefd = pre_UpFile(rx_buff);
					break;
				//不支持的命令
				default :
					break;
			}
		} else {
			//不是模块命令
		}
	} else if(g_ui_flag == DOWNFILE_COMMAND) {
		down_File(pc_mem,rx_buff,rd_num);
	} else if(g_ui_flag == UPFILE_COMMAND) {
		up_File(up_filefd,rx_buff);
	}
}
