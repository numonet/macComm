//******************************************************************
//
// File Name:				CommMem.h
// File Description:		Header file of the Communication between Devices
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************

#ifndef __COMMMEM_H_
#define __COMMMEM_H_



// Command Header
#define PDUHEADER				0xFFE2
#define EXTOHEADER				0xFFE1
#define REMOHEADER				0xFFE0						
#define DEVHEADER				0xFFF0
#define DEVEND					0x0FFF
#define PDUEND					0xE2FF
#define EXTOEND					0xE1FF
#define REMOEND					0xE0FF









// Definition of ERROR Code
#define COMMUNICATE_OK			0x0000
#define UNKNOWN_ERROR			0x0001
#define RS485HOST_TIMEOUT		0x0002
#define RS485SLAVE_TIMEOUT		0x0003
#define ETHERNET_TIMEOUT		0x0004
#define CRC_ERROR				0x0005
#define UNSUPPORTED_CMD			0x0006
#define PARAMETER_ERROR			0x0007
#define OUTOFRANGE_ERROR		0x0008
#define NORIGHT_ERROR			0x0009
#define CMDEXE_ERROR			0x000A
#define PRODUCT_UNSUPPORT		0x000B
#define HEADER_ERROR			0x000C






//cmd type
#define CMDPUSH					0x0001
#define CMDRESPONSE				0x0002
#define CMDFORWARD				0x0003
#define CMDURENT				0x0004
#define CMDBOARDCAST			0xFFFF



//poll cmd id
#define CMDDEV_INTERACTION_START			0x0100
#define CMDDEV_INTERACTION_END				0x01FF

#define CMD_CHECKDEV			0x0100
#define CMD_CHECKDEV_HANDLER	0x0101
#define CMD_UPLOAD				0x0103
#define CMD_PREUPLOAD			0x0104
#define CMD_UPLOADOVER			0x0105
#define CMD_CHECKCONFIG			0x0106
#define CMD_CHECKERR			0x0109
#define CMD_DEVDETECT			0x010B
#define CMD_CHECKSTATE			0x010C
#define CMD_CONFIRM				0x010D

//cmd len
#define CMDLEN_0				0x0
#define CMDLEN_2				0x2
#define CMDLEN_4				0x4
#define CMDLEN_8				0x8
#define CMDLEN_512				512

#define RESPONSOVER				0xAC1529C0









//unsigned int CRC16(unsigned char* ptr, unsigned int len);



// DownStream Function Interface
void ComD_DetectCmd(unsigned int ui_Port,unsigned char *puc_buff);
void ComD_ConFirmCmd(unsigned int ui_DevID, unsigned int ui_Port,unsigned int ui_assignID, unsigned char *puc_buff);
void ComD_Upload1Cmd(unsigned int ui_DevID,unsigned int ui_filetype,unsigned int ui_subtype,unsigned char *puc_buff);
void ComD_Upload2Cmd(unsigned int ui_DevID,unsigned char *puc_buff);
void ComD_Upload3Cmd(unsigned int ui_DevID,unsigned char *puc_buff);
void ComD_PollCmd(unsigned int ui_DevID,unsigned char uc_Port,unsigned char *puc_buff);
void ComD_ConfigCheck1Cmd(unsigned int ui_DevID,unsigned char *puc_buff);
void ComD_ConfigCheck2Cmd(unsigned int ui_DevID,unsigned char *puc_buff);
void ComD_StateCheck1Cmd(unsigned int ui_DevID, unsigned char *puc_buff);
void ComD_StateCheck2Cmd(unsigned int ui_DevID, unsigned char *puc_buff);
void ComD_StateCheck3Cmd(unsigned int ui_DevID, unsigned int ui_port, unsigned char *puc_buff);
void ComD_ErrCheckCmd(unsigned int ui_DevID, unsigned char *puc_buff);

// UpStream function Interface
void ComD_ProbResCmd(unsigned int ui_DevID, unsigned int ui_diagno, unsigned int ui_totalsize, unsigned char* puc_buff);
void ComD_ConfResCmd(unsigned int ui_DevID, unsigned int ui_supervport, unsigned char* puc_buff);
void ComD_PollResCmd(unsigned int ui_DevID, unsigned char* puc_buff);
void ComD_StatUpload1Cmd(unsigned int ui_DevID, unsigned int ui_totalsize, unsigned char* puc_buff);
void ComD_StatUpload2Cmd(unsigned int ui_DevID, unsigned int ui_datalen, unsigned char* puc_data, unsigned char* puc_buff);
void ComD_StatUpload3Cmd(unsigned int ui_DevID, unsigned char* puc_buff);
void ComD_FileUpload1Cmd(unsigned int ui_DevID, unsigned int ui_filesize, unsigned char* puc_buff);
void ComD_FileUpload2Cmd(unsigned int ui_DevID, unsigned int ui_datalen, unsigned char* puc_data, unsigned char* puc_buff);
void ComD_FileUpload3Cmd(unsigned int ui_DevID, unsigned char* puc_buff);
void ComD_ResErrMsgCmd(unsigned int ui_DevID, unsigned char* puc_errmsg, unsigned int ui_errnum, unsigned char* puc_buff);
void ComD_ResponseCmd(unsigned int ui_DevID, unsigned char *puc_data, unsigned char *puc_buff);





#endif  /*  __COMMMEM_H_  */
