//******************************************************************
//
// File Name:				CommMem.c
// File Description:		It serves RS485 Communication between MU and the devices
//							connected through Optical Module. 
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************
#include "CommUart.h"
#include "UdasDatabase.h"
#include "UdasNetwork.h"
#include "CommMem.h"


extern unsigned int g_ui_devid__;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// DownStream Function Interface /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ComD_DetectCmd(unsigned int ui_Port,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = 0x00;
	puc_buff[5] = 0x00;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_CHECKDEV >> 8);
	puc_buff[9] = (unsigned char)CMD_CHECKDEV;
	puc_buff[10] = (unsigned char)(CMDLEN_4 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_4;
	puc_buff[12] = (unsigned char)ui_Port;
	puc_buff[13] = puc_buff[14] = puc_buff[15] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}


void ComD_ConFirmCmd(unsigned int ui_DevID, unsigned int ui_Port, unsigned int ui_assignID, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_CONFIRM >> 8);
	puc_buff[9] = (unsigned char)CMD_CONFIRM;
	puc_buff[10] = (unsigned char)(CMDLEN_4 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_4;
	puc_buff[12] = (unsigned char)ui_Port;
	puc_buff[13] = 0x00;
	puc_buff[14] = (unsigned int)(ui_assignID >> 8);
	puc_buff[15] = (unsigned int)ui_assignID;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}

void ComD_Upload1Cmd(unsigned int ui_DevID,unsigned int ui_filetype,unsigned int ui_subtype,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_UPLOAD >> 8);
	puc_buff[9] = (unsigned char)CMD_UPLOAD;
	puc_buff[10] = (unsigned char)(CMDLEN_4 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_4;
	puc_buff[12] = (unsigned char)ui_filetype;
	puc_buff[13] = (unsigned char)(ui_subtype >> 16);
	puc_buff[14] = (unsigned char)(ui_subtype >> 8);
	puc_buff[15] = (unsigned char)ui_subtype;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}

void ComD_Upload2Cmd(unsigned int ui_DevID,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_PREUPLOAD >> 8);
	puc_buff[9] = (unsigned char)CMD_PREUPLOAD;
	puc_buff[10] = (unsigned char)(CMDLEN_0 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_0;
	ui_calcrc = CRC16(&puc_buff[2],10);
	puc_buff[12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[13] = (unsigned char)ui_calcrc;
	puc_buff[14] = (unsigned char)(DEVEND >> 8);
	puc_buff[15] = (unsigned char)DEVEND;
}

void ComD_Upload3Cmd(unsigned int ui_DevID,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_UPLOADOVER >> 8);
	puc_buff[9] = (unsigned char)CMD_UPLOADOVER;
	puc_buff[10] = (unsigned char)(CMDLEN_0 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_0;
	ui_calcrc = CRC16(&puc_buff[2],10);
	puc_buff[12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[13] = (unsigned char)ui_calcrc;
	puc_buff[14] = (unsigned char)(DEVEND >> 8);
	puc_buff[15] = (unsigned char)DEVEND;
}

void ComD_PollCmd(unsigned int ui_DevID,unsigned char uc_Port,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_DEVDETECT >> 8);
	puc_buff[9] = (unsigned char)CMD_DEVDETECT;
	puc_buff[10] = (unsigned char)(CMDLEN_4 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_4;
	puc_buff[12] = uc_Port;
	puc_buff[13] = puc_buff[14] = puc_buff[15] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}

//send check status command
void ComD_ConfigCheck1Cmd(unsigned int ui_DevID,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_CHECKCONFIG >> 8);
	puc_buff[9] = (unsigned char)CMD_CHECKCONFIG;
	puc_buff[10] = (unsigned char)(CMDLEN_0 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_0;
	ui_calcrc = CRC16(&puc_buff[2],10);
	puc_buff[12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[13] = (unsigned char)ui_calcrc;
	puc_buff[14] = (unsigned char)(DEVEND >> 8);
	puc_buff[15] = (unsigned char)DEVEND;
}
//get changed status
void ComD_ConfigCheck2Cmd(unsigned int ui_DevID,unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)(CMD_CHECKCONFIG >> 8);
	puc_buff[9] = (unsigned char)CMD_CHECKCONFIG;
	puc_buff[10] = (unsigned char)(CMDLEN_4 >> 8);
	puc_buff[11] = (unsigned char)CMDLEN_4;
	//Reverse 0xFF
	puc_buff[12] = 0xFF;
	puc_buff[13] = 0xFF;
	puc_buff[14] = 0xFF;
	puc_buff[15] = 0xFF;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = DEVEND >> 8 & 0xFF;
	puc_buff[19] = DEVEND & 0xFF;
}

void ComD_StateCheck1Cmd(unsigned int ui_DevID, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (DEVHEADER >> 8) & 0xFF;
	puc_buff[1] = DEVHEADER & 0xFF ;
	puc_buff[2] = (g_ui_devid__ >> 8) & 0xFF;
	puc_buff[3] = g_ui_devid__ & 0xFF;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = ui_DevID & 0xFF;
	puc_buff[6] = CMDRESPONSE >> 8 & 0xFF;
	puc_buff[7] = CMDRESPONSE & 0xFF;
	puc_buff[8] = CMD_CHECKSTATE >> 8 & 0xFF;
	puc_buff[9] = CMD_CHECKSTATE & 0xFF;
	puc_buff[10] = CMDLEN_0 >> 8 & 0xFF;
	puc_buff[11] = CMDLEN_0 & 0xFF;
	ui_calcrc = CRC16(&puc_buff[2],10);
	puc_buff[12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[13] = (unsigned char)ui_calcrc;
	puc_buff[14] = DEVEND >> 8 & 0xFF;
	puc_buff[15] = DEVEND & 0xFF;
}

void ComD_StateCheck2Cmd(unsigned int ui_DevID, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (DEVHEADER >> 8) & 0xFF;
	puc_buff[1] = DEVHEADER & 0xFF ;
	puc_buff[2] = (g_ui_devid__ >> 8) & 0xFF;
	puc_buff[3] = g_ui_devid__ & 0xFF;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = ui_DevID & 0xFF;
	puc_buff[6] = CMDRESPONSE >> 8 & 0xFF;
	puc_buff[7] = CMDRESPONSE & 0xFF;
	puc_buff[8] = CMD_CHECKSTATE >> 8 & 0xFF;
	puc_buff[9] = CMD_CHECKSTATE & 0xFF;
	puc_buff[10] = CMDLEN_4 >> 8 & 0xFF;
	puc_buff[11] = CMDLEN_4 & 0xFF;
	puc_buff[12] = puc_buff[13] = puc_buff[14] = puc_buff[15] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = DEVEND >> 8 & 0xFF;
	puc_buff[19] = DEVEND & 0xFF;
}

void ComD_StateCheck3Cmd(unsigned int ui_DevID, unsigned int ui_port, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (DEVHEADER >> 8) & 0xFF;
	puc_buff[1] = DEVHEADER & 0xFF ;
	puc_buff[2] = (g_ui_devid__ >> 8) & 0xFF;
	puc_buff[3] = g_ui_devid__ & 0xFF;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = ui_DevID & 0xFF;
	puc_buff[6] = CMDRESPONSE >> 8 & 0xFF;
	puc_buff[7] = CMDRESPONSE & 0xFF;
	puc_buff[8] = CMD_CHECKSTATE >> 8 & 0xFF;
	puc_buff[9] = CMD_CHECKSTATE & 0xFF;
	puc_buff[10] = CMDLEN_4 >> 8 & 0xFF;
	puc_buff[11] = CMDLEN_4 & 0xFF;
	puc_buff[12] = RESPONSOVER >> 24& 0xFF;
	puc_buff[13] = RESPONSOVER >> 16 & 0xFF;
	puc_buff[14] = RESPONSOVER >> 8 & 0xFF;
	puc_buff[15] = RESPONSOVER & 0xFF;
	ui_calcrc = CRC16(&puc_buff[2],14);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = DEVEND >> 8 & 0xFF;
	puc_buff[19] = DEVEND & 0xFF;
}

void ComD_ErrCheckCmd(unsigned int ui_DevID, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (DEVHEADER >> 8) & 0xFF;
	puc_buff[1] = DEVHEADER & 0xFF ;
	puc_buff[2] = (g_ui_devid__ >> 8) & 0xFF;
	puc_buff[3] = g_ui_devid__ & 0xFF;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = ui_DevID & 0xFF;
	puc_buff[6] = CMDRESPONSE >> 8 & 0xFF;
	puc_buff[7] = CMDRESPONSE & 0xFF;
	puc_buff[8] = CMD_CHECKERR >> 8 & 0xFF;
	puc_buff[9] = CMD_CHECKERR & 0xFF;
	puc_buff[10] = CMDLEN_0 >> 8 & 0xFF;
	puc_buff[11] = CMDLEN_0 & 0xFF;
	ui_calcrc = CRC16(&puc_buff[2],10);
	puc_buff[12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[13] = (unsigned char)ui_calcrc;
	puc_buff[14] = DEVEND >> 8 & 0xFF;
	puc_buff[15] = DEVEND & 0xFF;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// UpStream Function Interface ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ComD_ProbResCmd(unsigned int ui_DevID, unsigned int ui_diagno, unsigned int ui_totalsize, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;


	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((PROBE_NEW_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)PROBE_NEW_CMD;
	puc_buff[10] = (unsigned char)(PROBE_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)PROBE_CMDLEN;
	puc_buff[12] = EU_CFGDEV;
	puc_buff[13] = (unsigned char)ui_diagno;
	puc_buff[14] = puc_buff[15] = 0xFF;
	memset((puc_buff + 16), 0xFF, 4);
	puc_buff[20] = (unsigned char)(ui_totalsize >> 24);
	puc_buff[21] = (unsigned char)(ui_totalsize >> 16);
	puc_buff[22] = (unsigned char)(ui_totalsize >> 8);
	puc_buff[23] = (unsigned char)ui_totalsize;
	ui_calcrc = CRC16(&puc_buff[2], PROBE_CMDLEN + 10);
	puc_buff[24] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[25] = (unsigned char)ui_calcrc;
	puc_buff[26] = (unsigned char)(DEVEND >> 8);
	puc_buff[27] = (unsigned char)DEVEND;
}


void ComD_ConfResCmd(unsigned int ui_DevID, unsigned int ui_supervport, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((CONFIRM_NEW_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)CONFIRM_NEW_CMD;
	puc_buff[10] = (unsigned char)(CONFIRM_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)CONFIRM_CMDLEN;
	puc_buff[12] = (unsigned char)ui_supervport;
	puc_buff[13] = puc_buff[14] = puc_buff[15] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2], CONFIRM_CMDLEN + 10);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}


void ComD_PollResCmd(unsigned int ui_DevID, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((POLL_EXIST_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)POLL_EXIST_CMD;
	puc_buff[10] = (unsigned char)(POLL_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)POLL_CMDLEN;
	puc_buff[12] = 0x00;
	puc_buff[13] = puc_buff[14] = puc_buff[15] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2], POLL_CMDLEN + 10);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}


void ComD_StatUpload1Cmd(unsigned int ui_DevID, unsigned int ui_totalsize, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD_STAT_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD_STAT_CMD;
	puc_buff[10] = (unsigned char)(UPLOADSTAT1_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)UPLOADSTAT1_CMDLEN;
	puc_buff[12] = (unsigned char)(ui_totalsize >> 24);
	puc_buff[13] = (unsigned char)(ui_totalsize >> 16);
	puc_buff[14] = (unsigned char)(ui_totalsize >> 8);
	puc_buff[15] = (unsigned char)ui_totalsize;
	ui_calcrc = CRC16(&puc_buff[2], UPLOADSTAT1_CMDLEN + 10);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}


void ComD_StatUpload2Cmd(unsigned int ui_DevID, unsigned int ui_datalen, unsigned char* puc_data, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD_STAT_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD_STAT_CMD;
	puc_buff[10] = (unsigned char)(MAX_PACKETSIZE >> 8);
	puc_buff[11] = (unsigned char)MAX_PACKETSIZE;
	memcpy(&puc_buff[12], puc_data, ui_datalen);
	memset(&puc_buff[12 + ui_datalen], 0xFF, MAX_PACKETSIZE - ui_datalen);
	ui_calcrc = CRC16(&puc_buff[2], MAX_PACKETSIZE + 10);
	puc_buff[MAX_PACKETSIZE + 12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[MAX_PACKETSIZE + 13] = (unsigned char)ui_calcrc;
	puc_buff[MAX_PACKETSIZE + 14] = (unsigned char)(DEVEND >> 8);
	puc_buff[MAX_PACKETSIZE + 15] = (unsigned char)DEVEND;
}


void ComD_StatUpload3Cmd(unsigned int ui_DevID, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD_STAT_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD_STAT_CMD;
	puc_buff[10] = (unsigned char)(UPLOADSTAT3_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)UPLOADSTAT3_CMDLEN;
	puc_buff[12] = (unsigned char)(UPLAOD_ENDFLAG >> 24);
	puc_buff[13] = (unsigned char)(UPLAOD_ENDFLAG >> 16);
	puc_buff[14] = (unsigned char)(UPLAOD_ENDFLAG >> 8);
	puc_buff[15] = (unsigned char)UPLAOD_ENDFLAG;
	ui_calcrc = CRC16(&puc_buff[2], UPLOADSTAT3_CMDLEN + 10);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}


void ComD_FileUpload1Cmd(unsigned int ui_DevID, unsigned int ui_filesize, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD1_FILE_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD1_FILE_CMD;
	puc_buff[10] = (unsigned char)(UPLOADFILE1_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)UPLOADFILE1_CMDLEN;
	puc_buff[12] = (unsigned char)(ui_filesize >> 24);
	puc_buff[13] = (unsigned char)(ui_filesize >> 16);
	puc_buff[14] = (unsigned char)(ui_filesize >> 8);
	puc_buff[15] = (unsigned char)ui_filesize;
	ui_calcrc = CRC16(&puc_buff[2], UPLOADFILE1_CMDLEN + 10);
	puc_buff[16] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[17] = (unsigned char)ui_calcrc;
	puc_buff[18] = (unsigned char)(DEVEND >> 8);
	puc_buff[19] = (unsigned char)DEVEND;
}

void ComD_FileUpload2Cmd(unsigned int ui_DevID, unsigned int ui_datalen, unsigned char* puc_data, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD2_FILE_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD2_FILE_CMD;
	puc_buff[10] = (unsigned char)(ui_datalen >> 8);
	puc_buff[11] = (unsigned char)ui_datalen;
	memcpy(&puc_buff[12], puc_data, ui_datalen);
	ui_calcrc = CRC16(&puc_buff[2], ui_datalen + 10);
	puc_buff[ui_datalen + 12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[ui_datalen + 13] = (unsigned char)ui_calcrc;
	puc_buff[ui_datalen + 14] = (unsigned char)(DEVEND >> 8);
	puc_buff[ui_datalen + 15] = (unsigned char)DEVEND;
}


void ComD_FileUpload3Cmd(unsigned int ui_DevID, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD3_FILE_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD3_FILE_CMD;
	puc_buff[10] = (unsigned char)(UPLOADFILE3_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)UPLOADFILE3_CMDLEN;
	ui_calcrc = CRC16(&puc_buff[2], UPLOADFILE3_CMDLEN + 10);
	puc_buff[UPLOADFILE3_CMDLEN + 12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[UPLOADFILE3_CMDLEN + 13] = (unsigned char)ui_calcrc;
	puc_buff[UPLOADFILE3_CMDLEN + 14] = (unsigned char)(DEVEND >> 8);
	puc_buff[UPLOADFILE3_CMDLEN + 15] = (unsigned char)DEVEND;
}


void ComD_ResErrMsgCmd(unsigned int ui_DevID, unsigned char* puc_errmsg, unsigned int ui_errnum, unsigned char* puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = (unsigned char)(CMDRESPONSE >> 8);
	puc_buff[7] = (unsigned char)CMDRESPONSE;
	puc_buff[8] = (unsigned char)((UPLOAD_ALARMERR_CMD >> 8) | 0x80);
	puc_buff[9] = (unsigned char)UPLOAD_ALARMERR_CMD;
	puc_buff[10] = (unsigned char)(UPLOADALARMERR_CMDLEN >> 8);
	puc_buff[11] = (unsigned char)UPLOADALARMERR_CMDLEN;
	ui_calcrc = CRC16(&puc_buff[2], UPLOADALARMERR_CMDLEN + 10);
	if (ui_errnum == 0) {
		puc_buff[12] = puc_buff[13] == 0x00;
		memset(&puc_buff[14], 0xFF, (UPLOADALARMERR_CMDLEN - 2)); 
	}
	else {
		puc_buff[12] = 0x01;
		puc_buff[13] = (unsigned char)ui_errnum;
		memcpy(&puc_buff[14], puc_errmsg, (UPLOADALARMERR_CMDLEN - 2));
	}
	puc_buff[UPLOADALARMERR_CMDLEN + 12] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[UPLOADALARMERR_CMDLEN + 13] = (unsigned char)ui_calcrc;
	puc_buff[UPLOADALARMERR_CMDLEN + 14] = (unsigned char)(DEVEND >> 8);
	puc_buff[UPLOADALARMERR_CMDLEN + 15] = (unsigned char)DEVEND;
}
void ComD_ResponseCmd(unsigned int ui_DevID, unsigned char *puc_data, unsigned char *puc_buff)
{
	unsigned int ui_calcrc;

	puc_buff[0] = (unsigned char)(DEVHEADER >> 8);
	puc_buff[1] = (unsigned char)DEVHEADER;
	puc_buff[2] = (unsigned char)(g_ui_devid__ >> 8);
	puc_buff[3] = (unsigned char)g_ui_devid__;
	puc_buff[4] = (unsigned char)(ui_DevID >> 8);
	puc_buff[5] = (unsigned char)ui_DevID;
	puc_buff[6] = puc_data[0];
	puc_buff[7] = puc_data[1];
	puc_buff[8] = puc_data[2] | 0x80;
	puc_buff[9] = puc_data[3];
	puc_buff[10] = 0x00;
	puc_buff[11] = 0x02;
	puc_buff[12] = 0x00;
	puc_buff[13] = 0x00;
	ui_calcrc = CRC16(&puc_buff[2], 12);
	puc_buff[14] = (unsigned char)(ui_calcrc >> 8);
	puc_buff[15] = (unsigned char)ui_calcrc;
	puc_buff[16] = (unsigned char)(DEVEND >> 8);
	puc_buff[17] = (unsigned char)DEVEND;
}
