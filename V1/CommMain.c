//******************************************************************
//
// File Name:				CommMain.c
// File Description:		It serves RS485 Communication between MU and the devices
//							connected through Optical Module. Polling their state with
//							specific duration and check the dynamic topology.
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************
#include "UdasDatabase.h"
#include "CommUart.h"
#include "CommMem.h"
#include "CommMain.h"
//#include "CommExOptical.h"
//#include "CommReOptical.h"
#include "CommNetwork.h"
#include "UdasNetwork.h"
#include "CommPdu.h"
#include "CommEuBasic.h"
//#include "Configuration.h"
#include "Include.h"
#include "CommHost.h"




extern int g_uartfd;

#define CFGUPLOAD_HEADER					0xFFFFFFF0
#define STATUPLOAD_HEADER					0xFFFFFFF1



// Definition of ERROR code
#define COMM_OK								0
#define COMM_DEVERR							1
#define COMM_TRANSERR						2
#define COMM_PARAERR						3
#define COMM_MEMERR							4
#define COMM_SELFERR						5
#define COMM_PDUERR							6
#define COMM_POLLERR						7
#define COMM_ERRCHECKERR					8
#define COMM_STATECHECKERR					9
#define COMM_UPLOADERR						10
#define COMM_CONFIGCHECKERR					11

#define COMM_EOLVEERR						12







#define	DEVICE_CONFIG_FILE					0x1
#define ERROR_EXISTED						1
#define NEW_TOPOLOGY						1


#define CFG_TYPE							1
#define STATE_TYPE							2


#define MU_CFGDEV							0x1
#define EU_CFGDEV							0x2
#define RU_CFGDEV							0x3

#define MU_CFGPARA							0x1
#define EU_CFGPARA							0x2
#define RU_CFGPARA							0x3
#define DIU_CFGPARA							0x4
#define PDU_CFGPARA							0x5
#define NET_CFGPARA							0x6
#define ROPTICAL_CFGPARA					0x7
#define EOPTICAL_CFGPARA					0x8
#define RUFDD_CFGPARA						0x9
#define RUTDD_CFGPARA						0xA
#define DIURF_CFGPARA						0xB




#define CONFIG_CHANGED							1
#define TOPO_CHANGED							2
#define TOPO_NEWDEVICE							1

#define CFG_UNCHANGED							0
#define CFG_CHANGED								1
#define THREETIMES								3


// Definition of Communication Coniguration
// Number of times to try when  occurs during communication
#define NUMBER_OF_TRY							3
// Number of ports for device connection over Optical Module
#define NUMBER_OF_PORT							8
// Unknown ID used to detect new device
#define UNKNOWN_ID								0x0000
// Optical Threshold for Device detect. In general, it should be -15dB
#define OPTICAL_THR								-15



// Length of Error Message Memory
#define ERRMSG_BUFFERLEN						5120
// Length of Each Error Message
#define ERRMSG_LEN								8
// Length of Maxim Packet
#define MAX_PACKETSIZE							512

// Max Length of Command
#define MAX_CMDLEN								256

// Extern of EU Basic functions
extern unsigned int (*g_func_EuBasicFunc[4])(unsigned int, unsigned char*, unsigned int*);
extern unsigned int (*g_p_CommandExOptical[10])(unsigned int, unsigned char*, unsigned int*, unsigned char*);
extern unsigned int (*g_p_CommandReOptical[8])(unsigned int, unsigned char *, unsigned int *, unsigned char *);
extern unsigned int (*g_p_CommandPdu[3])(unsigned int, unsigned char *, unsigned int *, unsigned char *);


// ******************************** Definition of Local Variables ******************************** //

// check if the UDAS configuration database has been changed
static unsigned int g_ui_CfgChanged;
// check if the UDAS Topology has been changed
static unsigned int g_ui_TopoChanged;

//link leds 
static unsigned char uc_link_led;
//status leds 
static unsigned char uc_status_led; 

//EU ID
extern unsigned int g_ui_devid__;

extern unsigned int g_ui_CONFIG_CHANGE;








//EU ID
extern unsigned int g_ui_devid__;
extern int g_file_fd;



// ******************************** Definition of Global Variables ******************************** //

// EU Configuration Information Database
EU_DevPara									g_ks_EUPara;
EU_DevPara*									g_pks_EUPara;
// EU State Information Database
EU_DevState									g_ks_EUState;
EU_DevState*								g_pks_EUState;

// Backup of EU Configuration Information Database for Network and Upper-Device Threads 
void*										g_pvoid_bakofEUPara;
unsigned int								g_ui_paratotalsize;
// Backup of EU Configuration Information Database for Network and Upper_Device Threads 
void*										g_pvoid_bakofEUState;
unsigned int								g_ui_stattotalsize;




// Error Message Memory For Network
unsigned char g_uc_ErrNetworkMemory[ERRMSG_BUFFERLEN];
// Writing Pointer for Error Message Memory For Network
unsigned char* g_puc_ErrNetworkWptr;
// Reading Pointer for Error Message Memory For Network
unsigned char* g_puc_ErrNetworkRptr;

// Error Message Memory For Upper-Device
unsigned char g_uc_ErrUpperMemory[ERRMSG_BUFFERLEN];
// Writing Pointer for Error Message Memory For Upper-Device
unsigned char* g_puc_ErrUpperWptr;
// Reading Pointer for Error Message Memory For Upper-Device
unsigned char* g_puc_ErrUpperRptr;



// Command Buffer which includes command from Network
unsigned char g_uc_CmdNetworkBuffer[CMD_BUFFERLEN];
// Writing Pointer
unsigned char* g_puc_CmdNetworkWptr;
// Reading Pointer
unsigned char* g_puc_CmdNetworkRptr;

// Command Buffer which includes command from Upper-Device
unsigned char g_uc_CmdUpperBuffer[CMD_BUFFERLEN];
// Writing Pointer
unsigned char* g_puc_CmdUpperWptr;
// Reading Pointer
unsigned char* g_puc_CmdUpperRptr;


unsigned int g_ui_CONFIG_CHANGE;







// RS485 Device Handler
unsigned char		g_uc_DevId;
static unsigned int g_ui_PortExist[NUMBER_OF_PORT];




unsigned int Comm_Init()
{
	unsigned int ui_err;
	
	uc_link_led = 0xFF;
	uc_status_led = 0xFF;
	ui_err = COMM_DEVERR;
	g_ui_CfgChanged = CFG_UNCHANGED;
	g_ui_TopoChanged = TOPOL_UNCHANGED;
	// Initialize Error Message Memory
	memset(g_uc_ErrNetworkMemory, 0, sizeof(g_uc_ErrNetworkMemory));
	g_puc_ErrNetworkWptr = g_puc_ErrNetworkRptr = g_uc_ErrNetworkMemory;
	memset(g_uc_ErrUpperMemory, 0, sizeof(g_uc_ErrUpperMemory));
	g_puc_ErrUpperWptr = g_puc_ErrUpperRptr = g_uc_ErrUpperMemory;
	// Initialize Command Buffer
	memset(g_uc_CmdNetworkBuffer, 0, sizeof(g_uc_CmdNetworkBuffer));
	g_puc_CmdNetworkWptr = g_puc_CmdNetworkRptr = g_uc_CmdNetworkBuffer;
	memset(g_uc_CmdUpperBuffer, 0, sizeof(g_uc_CmdUpperBuffer));
	g_puc_CmdUpperWptr = g_puc_CmdUpperRptr = g_uc_CmdUpperBuffer;
	memset(g_ui_PortExist,0,NUMBER_OF_PORT);

	// Initialize EU Configuration Information
	memset((void*)&g_ks_EUPara, 0, sizeof(EU_DevPara));
	g_pks_EUPara = &g_ks_EUPara;
	g_pks_EUPara->kp_pEuB = &(g_pks_EUPara->kp_EuB);
	g_pks_EUPara->kp_pExpO = &(g_pks_EUPara->kp_ExpO);
	g_pks_EUPara->kp_pPdu = &(g_pks_EUPara->kp_Pdu);
	g_pks_EUPara->kp_pRemO = &(g_pks_EUPara->kp_RemO);
	// Initialize EU State Information
	memset((void*)&g_ks_EUState, 0, sizeof(EU_DevState));
	g_pks_EUState = &g_ks_EUState;
	g_pks_EUState->kp_pExpO = &(g_pks_EUState->kp_ExpO);
	g_pks_EUState->kp_pPdu = &(g_pks_EUState->kp_Pdu);
	g_pks_EUState->kp_pRemO = &(g_pks_EUState->kp_RemO);
	// Initialize backup parameter
	g_ui_paratotalsize = sizeof(EU_DevPara);
	g_ui_stattotalsize = sizeof(EU_DevState);
	g_pvoid_bakofEUPara = (EU_DevPara*)malloc(sizeof(EU_DevPara));
	g_pvoid_bakofEUState = (EU_DevState*)malloc(sizeof(EU_DevState));
	

	memset(g_pvoid_bakofEUPara, 0, g_ui_paratotalsize);
	memset(g_pvoid_bakofEUState, 0, g_ui_stattotalsize);

	//EUPara id
	g_pks_EUPara->kp_EuB.uc_DevID[0] = (unsigned char)(g_ui_devid__ >> 24);
	g_pks_EUPara->kp_EuB.uc_DevID[1] = (unsigned char)(g_ui_devid__ >> 16);
	g_pks_EUPara->kp_EuB.uc_DevID[2] = (unsigned char)(g_ui_devid__ >> 8);
	g_pks_EUPara->kp_EuB.uc_DevID[3] = (unsigned char)g_ui_devid__;
	//EUState id
	g_pks_EUState->uc_DevID[0] = (unsigned char)(g_ui_devid__ >> 24);
	g_pks_EUState->uc_DevID[1] = (unsigned char)(g_ui_devid__ >> 16);
	g_pks_EUState->uc_DevID[2] = (unsigned char)(g_ui_devid__ >> 8);
	g_pks_EUState->uc_DevID[3] = (unsigned char)g_ui_devid__;
	memcpy(g_pvoid_bakofEUPara, g_pks_EUPara, sizeof(EU_DevPara));
	memcpy(g_pvoid_bakofEUState, g_pks_EUState, sizeof(EU_DevState));

	// Open RS485 Device
	ui_err = ComP_DownInit();
	if (ui_err == COMM_OK) {
		//Init PDU
		Comm_PduInit();
		//Init EXOPTICAL
		Comm_ExOpticalInit();
		// Init REOPTICAL
		Comm_ReOpticalInit();
	}

	return ui_err;
}


unsigned int Comm_DeInit(void)
{
	ComP_DownDeInit();

	return 0;
}

unsigned int Write_File(unsigned char *uc_data, unsigned int ui_lenth)
{
	int file_fd;
	unsigned char *puc_ptr;
	unsigned int ui_err, ui_i, ui_cmt, ui_off;
	
	ui_err = 1;	
	puc_ptr = uc_data;
#if 0
	file_fd = open("/usr/binfile", O_CREAT | O_RDWR | O_TRUNC, 10705);
	if (file_fd < 0) {
		//printf("open file error.\r\n");
		perror("open file error: ");
	}
	else {

		ui_cmt = ui_lenth / 30;
		ui_off = ui_lenth % 30;
		
		if (ui_cmt) {
			ui_i = 0;		
			do {
				write(file_fd, &puc_ptr[ui_i * 30], 30);
				write(file_fd, "\n", 1);
			}while((++ui_i) < ui_cmt);

			if(ui_off) {
				write(file_fd, &puc_ptr[ui_i * 30], ui_off);
			}
			ui_err = 0;
		}
		else {
			write(file_fd, puc_ptr, ui_off);
			ui_err = 0;
		}
	}
#endif
#if 1
		write(g_file_fd, puc_ptr, ui_lenth);
		ui_err = 0;
#endif
		close(g_file_fd);	
	
	
	return ui_err;
}
/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			Comm_Transfer
// Function Description:	As Host, transfer data over RS485
// Function Parameters:
//			INPUT:			puc_Cmd:			Command Data
//							ui_CmdLen:			Length of Command
//							ui_ReturnLen:		Length of receiving data
//			OUTPUT:			puc_Payload:		Data received
// Return Value:
//							COMM_OK:			OK
/////////////////////////////////////////////////////////////////////////////////////
unsigned int Comm_Transfer(unsigned char* puc_Cmd, unsigned int ui_CmdLen, unsigned int ui_ReturnLen, unsigned char* puc_Payload)
{
	unsigned int ui_err;
	
	ui_err = ComP_Transfer(puc_Cmd, ui_CmdLen, ui_ReturnLen, puc_Payload);


	return ui_err;
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			Comm_DevCfgSelf
// Function Description:	Configure MU itself
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							COMM_OK:		OK
/////////////////////////////////////////////////////////////////////////////////////
unsigned int Comm_DevCfgSelf()
{
	unsigned int ui_err;

	ui_err = COMM_OK;
	// Configuration RTC

	
	// Configuration LEDs

	
	// Configuration GPIO


	// Configuration Modules




	return ui_err;
}


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			Comm_CheckSelfAlarm
// Function Description:	Check MU Alarm itself
// Function Parameters:
//			INPUT:			None
//			OUTPUT:			None
// Return Value:
//							COMM_OK:		OK
/////////////////////////////////////////////////////////////////////////////////////
unsigned int Comm_CheckSelfAlarm()
{
	unsigned int ui_err;

	ui_err = COMM_OK;
	// Check Expansion Optical Module Alarm and PDU Module Alarm by reading GPIO state.



	return ui_err;
}








unsigned int Comm_Upload(unsigned char** puc_uploadAddr, unsigned int ui_fileType, unsigned int ui_DevID, unsigned int *ui_subtype)
{
	unsigned int ui_err, ui_len, ui_crc, ui_calcrc;
	unsigned int ui_cnt, ui_off;
	unsigned char* puc_ptr;
	unsigned char uc_buffer[32], uc_payload[528];


	ui_err = COMM_UPLOADERR;
	puc_ptr = *puc_uploadAddr;
	memset(uc_buffer, 0 ,sizeof(uc_buffer));
	memset(uc_payload, 0, sizeof(uc_payload));
	ComD_Upload1Cmd(ui_DevID, ui_fileType, *ui_subtype, uc_buffer);
	ui_err = Comm_Transfer(uc_buffer, 20, 20, uc_payload);
	if (ui_err == COMM_OK) {
		ui_calcrc = CRC16(&uc_payload[2], 14);
		ui_crc = ((unsigned int)uc_payload[16] << 8 | (unsigned int)uc_payload[17]);
		if (ui_crc == ui_calcrc) {
			ui_len = ((unsigned int)uc_payload[12] << 24) | ((unsigned int)uc_payload[13] << 16) |
						((unsigned int)uc_payload[14] << 8) | (unsigned int)uc_payload[15];
			if (*puc_uploadAddr == NULL) {
				*puc_uploadAddr = (unsigned char*)malloc(ui_len * sizeof(unsigned char));
				puc_ptr = *puc_uploadAddr;
			}

			ui_cnt = ui_len / MAX_PACKETSIZE;
			ui_off = ui_len % MAX_PACKETSIZE;
			if (ui_len <= MAX_PACKETSIZE) {
				ComD_Upload2Cmd(ui_DevID, uc_buffer);
#ifdef READ
				ComP_Write(uc_buffer,16);
				ComP_Sleep(300);
				ui_err = ComP_Read(ui_len + 16,uc_payload);
#else
				ui_err = Comm_Transfer(uc_buffer, 16, ui_len + 16, uc_payload);// ??puc_ptr<----->uc_payload
#endif
				ui_calcrc = CRC16(&uc_payload[2], 10 + ui_len);
				ui_crc = ((unsigned int)uc_payload[12 + ui_len] << 8 | (unsigned int)uc_payload[13 + ui_len]);
				if (ui_calcrc != ui_crc) {
					ui_err = CRC_ERROR;
					printf("********************Comm_Upload CRC error***1\n");
				}
				else {
					memcpy(puc_ptr, &uc_payload[12], ui_len);
				}
			}
			else {
				// Receiving State Information in loop
				do {
					ComD_Upload2Cmd(ui_DevID, uc_buffer);
#ifdef READ
					ComP_Write(uc_buffer,16);
					ComP_Sleep(300);
					ui_err = ComP_Read(MAX_PACKETSIZE + 16,uc_payload);
#else
					ui_err = Comm_Transfer(uc_buffer, 16, MAX_PACKETSIZE + 16, uc_payload);
#endif
					ui_calcrc = CRC16(&uc_payload[2], 10 + MAX_PACKETSIZE);
					ui_crc = ((unsigned int)uc_payload[12 + MAX_PACKETSIZE] << 8 | (unsigned int)uc_payload[13 + MAX_PACKETSIZE]);
					if (ui_calcrc != ui_crc) {
						ui_err = CRC_ERROR;
						printf("********************Comm_Upload CRC error***2\n");
					}
					else {
						memcpy(puc_ptr, &uc_payload[12], MAX_PACKETSIZE);
						puc_ptr += MAX_PACKETSIZE;
					}
				} while ((-- ui_cnt) && (ui_err == COMM_OK));
				if (ui_err == COMM_OK) {
					ComD_Upload2Cmd(ui_DevID, uc_buffer);
#ifdef READ
					ComP_Write(uc_buffer,16);
					ComP_Sleep(300);
					ui_err = ComP_Read(ui_off + 16,uc_payload);
#else
					ui_err = Comm_Transfer(uc_buffer, 16, ui_off + 16, uc_payload);
#endif
					ui_calcrc = CRC16(&uc_payload[2], 10 + ui_off);
					ui_crc = ((unsigned int)uc_payload[12 + ui_off] << 8 | (unsigned int)uc_payload[13 + ui_off]);
					if (ui_calcrc != ui_crc) {
						ui_err = CRC_ERROR;
						printf("********************Comm_Upload CRC error***3\n");
					}
					else {
						memcpy(puc_ptr, &uc_payload[12], ui_off);
					}
				}
			}
			if (ui_err == COMM_OK) {
				ComD_Upload3Cmd(ui_DevID, uc_buffer);
#ifdef READ
				ComP_Write(uc_buffer,16);
				ComP_Sleep(300);
				ComP_Read(16,uc_payload);
#else 
				ui_err = Comm_Transfer(uc_buffer, 16, 16, uc_payload);
#endif
				ui_calcrc = CRC16(&uc_payload[2], 10);
				ui_crc = ((unsigned int)uc_payload[12] << 8 | (unsigned int)uc_payload[13]);
				if (ui_calcrc != ui_crc) {
					ui_err = CRC_ERROR;
					printf("********************Comm_Upload CRC error***4\n");
				}
			}
		}
		else {
			// Record Error into Error Message Memory
			ui_err = CRC_ERROR;
			printf("********************Comm_Upload CRC error***5\n");
		}
	}
	*ui_subtype = ui_len;

	return ui_err;	
}

#if 0
static unsigned int Comm_FindIDDev(unsigned int ui_devid, void *pvoid_ptrin, unsigned int ui_type, void* pvoid_ptrout)
{
	unsigned int ui_err, ui_cnt, ui_id;
	unsigned char* puc_ptr;
	EU_DevPara *peu;


	ui_err = 1;
	puc_ptr = (unsigned char*)pvoid_ptrin;
	ui_id = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
			((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
	if(ui_id == ui_devid) {
		pvoid_ptrout = pvoid_ptrin;
		ui_err = COMM_OK;
	} 
	else if (ui_type == DEVICE_EU) {
		ui_cnt = 0;
		peu = (EU_DevPara *)pvoid_ptrin;
		do {
			ui_err = Comm_FindIDDev(ui_devid, (void*)(peu->kp_pPort[ui_cnt]), (unsigned int)(peu->uc_PortType[ui_cnt]), pvoid_ptrout);
		}while((++ ui_cnt < NUMBER_OF_PORT) && (ui_err == 1));
	}
	
	return ui_err;
}
#endif
static unsigned int __Comm_FindIDDev(unsigned int ui_devid, void *pui_ptr, void* pvoid_ptr)
{
	unsigned int ui_err, ui_cnt;
	unsigned char* puc_ptr;
	unsigned int ui_nextid;
	EU_DevPara *peu;


	ui_cnt = 0;
	ui_err = 1;
	peu = (EU_DevPara *)pui_ptr;
	do {
		puc_ptr = (unsigned char*)peu->kp_pPort[ui_cnt];
		if(peu->uc_PortType[ui_cnt] == DEVICE_EU) {
			ui_nextid = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
						((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			if(ui_nextid != ui_devid) {
				ui_err = __Comm_FindIDDev(ui_devid, peu->kp_pPort[ui_cnt], pvoid_ptr);
			} else {
				pvoid_ptr = (void*)puc_ptr;
				ui_err = 0;
			}
		} else if(peu->uc_PortType[ui_cnt] == DEVICE_RU) {
			ui_nextid = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
						((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			if(ui_nextid == ui_devid) {
				pvoid_ptr = (void*)puc_ptr;
				ui_err = 0;
			}
		}
	} while((++ ui_cnt < NUMBER_OF_PORT) && (ui_err == 1));


	return ui_err;
}

static unsigned int Comm_FindIDDev(unsigned int ui_devid, void* pvoid_ptr, unsigned int* ui_returnptr)
{
	unsigned int ui_err;
	unsigned int ui_cnt,ui_id;
	unsigned char* puc_ptr;


	ui_err = 1;
	ui_cnt = 0;
	do {
		puc_ptr = (unsigned char*)(g_pks_EUPara->kp_pPort[ui_cnt]);
		if(g_pks_EUPara->uc_PortType[ui_cnt] == DEVICE_RU) {
			ui_id = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
					((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			if(ui_id == ui_devid) {
				ui_err = 0;
				pvoid_ptr = (void*)puc_ptr;
				*ui_returnptr = (unsigned int)puc_ptr;
			}
		} else if(g_pks_EUPara->uc_PortType[ui_cnt] == DEVICE_EU) {
			ui_id = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
					((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			if(ui_id == ui_devid) {
				ui_err = 0;
				pvoid_ptr = (void*)puc_ptr;
			} else {
				ui_err = __Comm_FindIDDev(ui_devid, g_pks_EUPara->kp_pPort[ui_cnt], pvoid_ptr);
			}
		}
	} while((++ ui_cnt < NUMBER_OF_PORT) && (ui_err == 1));

	return ui_err;
}



static unsigned int Comm_EvolveEu(void* pvoid_tobeEvolved, unsigned char* puc_bufferaddr, unsigned int ui_bufferlen, unsigned int* pui_reallen)
{
	unsigned int ui_err, ui_cnt, ui_reallen, ui_tfddnum;
	unsigned char* puc_ptr;
	EU_DevPara* peucfg;
	EU_DevPara* peu;
	RU_DevPara* pru;
	EU_DevState* peustat;
	LOAD_HEADER loadheader;
	LOAD_HEADER* pHeader;

	ui_err = COMM_OK;
	peucfg = (EU_DevPara*)pvoid_tobeEvolved;
	peustat = (EU_DevState*)pvoid_tobeEvolved;
	puc_ptr = puc_bufferaddr;
	pHeader = &loadheader;
	*pui_reallen = ui_cnt = 0;
	do {
		memcpy(pHeader, puc_ptr, sizeof(LOAD_HEADER));
		if (pHeader->ui_header == CFGUPLOAD_HEADER) {
			if (peucfg->uc_PortType[ui_cnt] != DEVICE_NONE) {
				// Check if match
				if ((pHeader->uc_portnum == ui_cnt) && (pHeader->uc_devtype == peucfg->uc_PortType[ui_cnt])) {
					if (pHeader->uc_devtype == EU_CFGDEV) {
						peucfg->kp_pPort[ui_cnt] = malloc(sizeof(EU_DevPara) * sizeof(char));
						memcpy(peucfg->kp_pPort[ui_cnt], puc_ptr + sizeof(LOAD_HEADER), sizeof(EU_DevPara));
						peu = (EU_DevPara*)(peucfg->kp_pPort[ui_cnt]);
						peu->kp_pEuB = &(peu->kp_EuB);
						peu->kp_pExpO = &(peu->kp_ExpO);
						peu->kp_pRemO = &(peu->kp_RemO);
						peu->kp_pPdu = &(peu->kp_Pdu);
						puc_ptr += (sizeof(LOAD_HEADER) + sizeof(EU_DevPara));
						*pui_reallen += (sizeof(LOAD_HEADER) + sizeof(EU_DevPara));
						Comm_EvolveEu(peucfg->kp_pPort[ui_cnt], puc_ptr, (ui_bufferlen - sizeof(EU_DevPara)), &ui_reallen);
						puc_ptr += ui_reallen;
						*pui_reallen += ui_reallen;
					}
					else if (pHeader->uc_devtype == RU_CFGDEV) {
						peucfg->kp_pPort[ui_cnt] = malloc(sizeof(RU_DevPara) * sizeof(char));
						memcpy(peucfg->kp_pPort[ui_cnt], puc_ptr + sizeof(LOAD_HEADER), sizeof(RU_DevPara));
						pru = (RU_DevPara*)(peucfg->kp_pPort[ui_cnt]);
						pru->kp_pRuB = &(pru->kp_RuB);
						pru->kp_pRemO = &(pru->kp_RemO);
						ui_tfddnum = 0;
						do {
							pru->kp_pFdd[ui_tfddnum] = &(pru->kp_Fdd[ui_tfddnum]);
						} while (++ ui_tfddnum < MAX_FDD);
						ui_tfddnum = 0;
						do {
							pru->kp_pTdd[ui_tfddnum] = &(pru->kp_Tdd[ui_tfddnum]);
						} while (++ ui_tfddnum < MAX_TDD);
						puc_ptr += (sizeof(LOAD_HEADER) + sizeof(RU_DevPara));
						*pui_reallen += (sizeof(LOAD_HEADER) + sizeof(RU_DevPara));
					}
				}
				else {
					ui_err = COMM_EOLVEERR;
				}
			}
		}
		else if (pHeader->ui_header == STATUPLOAD_HEADER) {
			if (peustat->uc_PortType[ui_cnt] != DEVICE_NONE) {
				// Check if match
				if ((pHeader->uc_portnum == ui_cnt) && (pHeader->uc_devtype == peustat->uc_PortType[ui_cnt])) {
					if (pHeader->uc_devtype == EU_CFGDEV) {
						peustat->kp_pPort[ui_cnt] = malloc(sizeof(EU_DevState) * sizeof(char));
						memcpy(peustat->kp_pPort[ui_cnt], puc_ptr + sizeof(LOAD_HEADER), sizeof(EU_DevState));
						puc_ptr += (sizeof(LOAD_HEADER) + sizeof(EU_DevState));
						*pui_reallen += (sizeof(LOAD_HEADER) + sizeof(EU_DevState));
						Comm_EvolveEu(peustat->kp_pPort[ui_cnt], puc_ptr, (ui_bufferlen - sizeof(EU_DevState)), &ui_reallen);
						puc_ptr += ui_reallen;
						*pui_reallen += ui_reallen;
					}
					else if (pHeader->uc_devtype == RU_CFGDEV) {
						peustat->kp_pPort[ui_cnt] = malloc(sizeof(RU_DevState) * sizeof(char));
						memcpy(peustat->kp_pPort[ui_cnt], puc_ptr + sizeof(LOAD_HEADER), sizeof(RU_DevState));
						puc_ptr += (sizeof(LOAD_HEADER) + sizeof(RU_DevState));
						*pui_reallen += (sizeof(LOAD_HEADER) + sizeof(RU_DevState));
					}
				}
				else {
					ui_err = COMM_EOLVEERR;
				}
			}
		}
		else {
			ui_err = COMM_EOLVEERR;
		}
	} while ((++ ui_cnt < NUMBER_OF_PORT) && (ui_err == COMM_OK));


	return ui_err;
}


static unsigned int Comm_FreeEu(void* puc_bufferaddr, unsigned int ui_type)
{
	unsigned int ui_err, ui_cnt;
	EU_DevPara* peucfg;
	EU_DevState* peustat;


	ui_err = COMM_OK;
	ui_cnt = 0;
	switch(ui_type) {
		case CFG_TYPE:
			peucfg = (EU_DevPara*)puc_bufferaddr;
			do {
				if (peucfg->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peucfg->uc_PortType[ui_cnt] == EU_CFGDEV) {
						Comm_FreeEu(peucfg->kp_pPort[ui_cnt], ui_type);			
					}
					free(peucfg->kp_pPort[ui_cnt]);
					peucfg->kp_pPort[ui_cnt] = NULL;
					peucfg->ui_PortSize[ui_cnt] = 0;
					peucfg->uc_PortType[ui_cnt] = DEVICE_NONE;
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
		case STATE_TYPE:
			peustat = (EU_DevState*)puc_bufferaddr;
			do {
				if (peustat->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peustat->uc_PortType[ui_cnt] == EU_CFGDEV) {
						Comm_FreeEu(peustat->kp_pPort[ui_cnt], ui_type);
					}
					free(peustat->kp_pPort[ui_cnt]);
					peustat->kp_pPort[ui_cnt] = NULL;
					peustat->ui_PortSize[ui_cnt] = 0;
					peustat->uc_PortType[ui_cnt] = DEVICE_NONE;
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
	}

	return ui_err;
}


static unsigned int Comm_CopyEu(unsigned int ui_supervisor, void* pvoid_baseaddr, unsigned char** puc_pointer, unsigned int ui_type)
{
	unsigned int ui_err, ui_cnt, ui_subcnt, ui_devnum, ui_superID;
	EU_DevPara* peucfg;
	EU_DevState* peustat;
	LOAD_HEADER hloader;
	LOAD_HEADER* phloader;
	unsigned char *puc_ptr;

	ui_err = COMM_OK;
	ui_cnt = 0;
	phloader = &hloader;
	switch(ui_type) {
		case CFG_TYPE:
			peucfg = (EU_DevPara*)pvoid_baseaddr;
			do {
				if (peucfg->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peucfg->uc_PortType[ui_cnt] == EU_CFGDEV) {
						phloader->ui_header = CFGUPLOAD_HEADER;
						phloader->ui_supervisorID = ui_supervisor;
						phloader->uc_portnum = ui_cnt;
						phloader->uc_devtype = EU_CFGDEV;
						// Calculate the number of device connected th EU
						ui_devnum = ui_subcnt = 0;
						do {
							if (peucfg->uc_PortType[ui_subcnt] != DEVICE_NONE) {
								++ ui_devnum;
							}
						} while (++ ui_subcnt < NUMBER_OF_PORT);
						phloader->uc_numofdev =ui_devnum;
						memcpy(*puc_pointer, (unsigned char*)phloader, sizeof(LOAD_HEADER));
						*puc_pointer += sizeof(LOAD_HEADER);
						memcpy(*puc_pointer, (unsigned char*)peucfg->kp_pPort[ui_cnt], sizeof(EU_DevPara));
						*puc_pointer += sizeof(EU_DevPara);
						ui_superID = ((unsigned int)(peucfg->kp_pEuB->uc_DevID[0]) << 24) | ((unsigned int)(peucfg->kp_pEuB->uc_DevID[1]) << 16) |
										((unsigned int)(peucfg->kp_pEuB->uc_DevID[2]) << 8) | (unsigned int)peucfg->kp_pEuB->uc_DevID[3];
						Comm_CopyEu(ui_superID, (peucfg->kp_pPort[ui_cnt]), puc_pointer, CFG_TYPE);

					}
					else {
						phloader->ui_header = CFGUPLOAD_HEADER;
						phloader->ui_supervisorID = ui_supervisor;
						phloader->uc_portnum = ui_cnt;
						phloader->uc_devtype = RU_CFGDEV;
						phloader->uc_numofdev = 0;
						memcpy(*puc_pointer, (unsigned char*)phloader, sizeof(LOAD_HEADER));
						*puc_pointer += sizeof(LOAD_HEADER);
						memcpy(*puc_pointer, (unsigned char*)(peucfg->kp_pPort[ui_cnt]), sizeof(RU_DevPara));
						*puc_pointer += sizeof(RU_DevPara);
					}
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
		case STATE_TYPE:
			peustat = (EU_DevState*)pvoid_baseaddr;
			do {
				if (peustat->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peustat->uc_PortType[ui_cnt] == EU_CFGDEV) {
						phloader->ui_header = STATUPLOAD_HEADER;
						phloader->ui_supervisorID = ui_supervisor;
						phloader->uc_portnum = ui_cnt;
						phloader->uc_devtype = EU_CFGDEV;
						// Calculate the number of device connected th EU
						ui_devnum = ui_subcnt = 0;
						do {
							if (peustat->uc_PortType[ui_subcnt] != DEVICE_NONE) {
								++ ui_devnum;
							}
						} while (++ ui_subcnt < NUMBER_OF_PORT);
						phloader->uc_numofdev =ui_devnum;
						memcpy(*puc_pointer, (unsigned char*)phloader, sizeof(LOAD_HEADER));
						*puc_pointer += sizeof(LOAD_HEADER);
						memcpy(*puc_pointer, (unsigned char*)peustat->kp_pPort[ui_cnt], sizeof(EU_DevState));
						*puc_pointer += sizeof(EU_DevState);
						ui_superID = ((unsigned int)(peustat->uc_DevID[0]) << 24) | ((unsigned int)(peustat->uc_DevID[1]) << 16) |
										((unsigned int)(peustat->uc_DevID[2]) << 8) | (unsigned int)peustat->uc_DevID[3];
						Comm_CopyEu(ui_superID, (peustat->kp_pPort[ui_cnt]), puc_pointer, STATE_TYPE);
					}
					else {
						phloader->ui_header = STATUPLOAD_HEADER;
						phloader->ui_supervisorID = ui_supervisor;
						phloader->uc_portnum = ui_cnt;
						phloader->uc_devtype = RU_CFGDEV;
						phloader->uc_numofdev = 0;
						memcpy(*puc_pointer, (unsigned char*)phloader, sizeof(LOAD_HEADER));
						*puc_pointer += sizeof(LOAD_HEADER);
						puc_ptr = *puc_pointer;
						memcpy(*puc_pointer, (unsigned char*)(peustat->kp_pPort[ui_cnt]), sizeof(RU_DevState));
						*puc_pointer += sizeof(RU_DevState);
					}
#if 0
					printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^RU_DevState\n");
					printf("port number = 0x%x\n", ui_cnt);
					printf("RU addr = 0x%x \n", peustat->kp_pPort[ui_cnt]); 
					printf("RU size = 0x%x\n", peustat->ui_PortSize[ui_cnt]);
					printf("port type = 0x%x\n", peustat->uc_PortType[ui_cnt]);
#endif
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
	}

	return ui_err;
}


static unsigned int Comm_CalSizeEu(void* puc_bufferaddr, unsigned int ui_type, unsigned int* pui_totalsize)
{
	unsigned int ui_err, ui_cnt, ui_subtotalsize;
	EU_DevPara* peucfg;
	EU_DevState* peustat;


	ui_err = COMM_OK;
	*pui_totalsize = ui_cnt = 0;
	switch(ui_type) {
		case CFG_TYPE:
			peucfg = (EU_DevPara*)puc_bufferaddr;
			do {
				if (peucfg->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peucfg->uc_PortType[ui_cnt] == EU_CFGDEV) {
						Comm_CalSizeEu(peucfg->kp_pPort[ui_cnt], ui_type, &ui_subtotalsize);
						*pui_totalsize += ui_subtotalsize;
						*pui_totalsize += sizeof(EU_DevPara);
					}
					else {
						*pui_totalsize += sizeof(RU_DevPara);
					}
					*pui_totalsize += sizeof(LOAD_HEADER);
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
		case STATE_TYPE:
			peustat = (EU_DevState*)puc_bufferaddr;
			do {
				if (peustat->uc_PortType[ui_cnt] != DEVICE_NONE) {
					if (peustat->uc_PortType[ui_cnt] == EU_CFGDEV) {
						Comm_CalSizeEu(peustat->kp_pPort[ui_cnt], ui_type, &ui_subtotalsize);
						*pui_totalsize += ui_subtotalsize;
						*pui_totalsize += sizeof(EU_DevState);
					}
					else {
						*pui_totalsize += sizeof(RU_DevState);
					}
					*pui_totalsize += sizeof(LOAD_HEADER);
				}
			} while (++ ui_cnt < NUMBER_OF_PORT);
			break;
	}

	return ui_err;
}


static unsigned int Comm_DevDetect(unsigned int ui_Port, unsigned int* pui_suberr, unsigned char* puc_DetectInfo)
{
	unsigned int ui_err, ui_crc, ui_calcrc;
	unsigned char uc_buffer[32];

	ui_err = COMM_MEMERR;
	if (puc_DetectInfo != NULL) {
		memset(uc_buffer, 0 ,sizeof(uc_buffer));
		ComD_DetectCmd(ui_Port, uc_buffer);
		ui_err = Comm_Transfer(uc_buffer, 20, 28, puc_DetectInfo);
		if (ui_err == COMM_OK) {
			ui_calcrc = CRC16(&puc_DetectInfo[2], 22);
			ui_crc = ((unsigned int)puc_DetectInfo[24] << 8 | (unsigned int)puc_DetectInfo[25]);
			if (ui_crc != ui_calcrc) {
				*pui_suberr = CRC_ERROR;
				ComP_ReadClear();
				printf("********************Comm_DevDetect CRC error\n");
			}
		}
	}

	return ui_err;
}

static unsigned int Comm_FindMaxId(unsigned int* pui_maxid)
{
	unsigned int ui_err, ui_cnt, ui_CalId;
	unsigned char* puc_ptr;


	ui_err = ui_cnt = 0;
	*pui_maxid = 0;
	do 
	{
		if(g_pks_EUPara->uc_PortType[ui_cnt] != DEVICE_NONE) {
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_cnt];
			ui_CalId = (unsigned int)puc_ptr[0] << 24 | (unsigned int)puc_ptr[1] << 16 |
				(unsigned int)puc_ptr[2] << 8 | (unsigned int)puc_ptr[3];
			if(ui_CalId > *pui_maxid) {
				*pui_maxid = ui_CalId;
			}

		}
	} while (++ ui_cnt < NUMBER_OF_PORT);


	return ui_err;
}
static unsigned int Comm_AssignedID(unsigned int ui_DevId, unsigned int* pui_assignedId)
{
	unsigned int ui_CalId, ui_err, ui_cnt;
	unsigned int ui_MaxId;
	unsigned char* puc_ptr;
	

	ui_err = NOTFOUND;
	ui_cnt = 0;
	do 
	{
		if(g_pks_EUPara->uc_PortType[ui_cnt] != DEVICE_NONE) {
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_cnt];
			ui_CalId = (unsigned int)puc_ptr[0] << 24 | (unsigned int)puc_ptr[1] << 16 |
						(unsigned int)puc_ptr[2] << 8 | (unsigned int)puc_ptr[3];
			if(ui_CalId == ui_DevId) {
				Comm_FindMaxId(&ui_MaxId);
				*pui_assignedId = ui_MaxId + 1;
				ui_err = FOUND;
			}
			
		}
	} while ((++ui_cnt < NUMBER_OF_PORT) && (ui_err == NOTFOUND));
	

	return ui_err;
}
static unsigned int Comm_Confirm(unsigned int* pui_DevID, unsigned int ui_Port, unsigned char* puc_buffer)
{
	unsigned int ui_err, ui_origID;
	unsigned char uc_buffer[32];
	unsigned int ui_calcrc, ui_crc;


	ui_err = COMM_MEMERR;
	if (puc_buffer != NULL) {
		memset(uc_buffer, 0 ,sizeof(uc_buffer));
		ui_origID = *pui_DevID;
		Comm_AssignedID(ui_origID, pui_DevID);
		ComD_ConFirmCmd(ui_origID, ui_Port, *pui_DevID, uc_buffer);
		ui_err = Comm_Transfer(uc_buffer, 20, 20, puc_buffer);
		//crc
		if (ui_err == COMM_OK) {
			ui_calcrc = CRC16(&puc_buffer[2], 14);
			ui_crc = ((unsigned int)puc_buffer[16] << 8 | (unsigned int)puc_buffer[17]);
			if (ui_crc != ui_calcrc) {
				ui_err = CRC_ERROR;
				ComP_ReadClear();
				printf("********************Comm_Confirm CRC error\n");
			}
		}
	}

	return ui_err;
}



static unsigned int Comm_DevPoll(unsigned int ui_Port, unsigned int ui_DevID, unsigned int* pui_exist, unsigned int* pui_suberr)
{
	unsigned int ui_err, ui_crc, ui_calcrc;
	unsigned char uc_buffer[32], uc_payload[32];


	ui_err = COMM_POLLERR;
	memset(uc_buffer, 0 ,sizeof(uc_buffer));
	memset(uc_payload, 0, sizeof(uc_payload));
	ComD_PollCmd(ui_DevID, ui_Port, uc_buffer);
	ui_err = Comm_Transfer(uc_buffer, 20, 20, uc_payload);
	
	if (ui_err == COMM_OK) {
		ui_calcrc = CRC16(&uc_payload[2], 14);
		ui_crc = ((unsigned int)uc_payload[16] << 8 | (unsigned int)uc_payload[17]);
		
		if (ui_crc == ui_calcrc) {
			*pui_exist = (unsigned int)uc_payload[12];
		}
		else {
			ComP_ReadClear();
			*pui_suberr = CRC_ERROR;
			printf("=====ui_calcrc: 0x%x, ui_crc: 0x%x \n", ui_calcrc, ui_crc);
		}
	}
	else{
		printf("Comm_DevPoll ui_err ========= 0x%x\n", ui_err);
		ui_err = 11;
	}
 

	return ui_err;
}


unsigned int Comm_PollDevState(void)
{
	int i_opticalData;
	unsigned int ui_err, ui_suberr, ui_exist, ui_cnt, ui_len, ui_DevID, ui_reallen, ui_tfddnum, ui_subtype;
	unsigned char uc_detectinfo[32],uc_buffer[32];
	unsigned char* puc_ptr;
	unsigned char* puc_buffer;
	RU_DevPara* pru;
	EU_DevPara* peu;


	ui_cnt = 0;
	memset(uc_buffer,0,sizeof(uc_buffer));
	memset(uc_detectinfo,0,sizeof(uc_detectinfo));

	do {
		//ui_err = Comm_GetStateExpO(g_pks_EUState->kp_pExpO,&ui_suberr);
		if (g_pks_EUPara->uc_PortType[ui_cnt] == DEVICE_NONE) {
			printf("port =============================================== %d\n", ui_cnt);
			ui_err = switch_OneExpO(ui_cnt,EXPOUL_ONE_ON);
			printf("switch_OneExpO ui_err : %d\n", ui_err);
			if(ui_err == COMM_OK) {
				ComP_Sleep(800);
				ui_err = Comm_GetStateExpO(g_pks_EUState->kp_pExpO, &ui_suberr);
				if (ui_err == COMM_OK) {	
					// Didn't detect device before, and detect it in real-time
					i_opticalData = (g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt] & 0x80) ? (int)(0 - (g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt] & 0x7F)) : (int)(g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt]);
					//i_opticalData = (g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt] > 0x80) ? ((int)(g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt]) | 0xFFFFFF00) : (int)(g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt]);
					//i_opticalData = (int)(g_pks_EUState->kp_pExpO->uc_DlRecvP[ui_cnt]) | 0xFFFFFF00;
					//i_opticalData = 14;
					//if ((ui_cnt == 4) || (ui_cnt == 0)) {
						printf("port = %d, ui_err = %d, power = %d\n", ui_cnt, ui_err, i_opticalData);
					//}
					if (i_opticalData < OPTICAL_THR) {
						ui_err = switch_OneExpO(ui_cnt,EXPOUL_ONE_OFF);
						printf("%d Optical power THR\n", ui_cnt);
						//ComP_Sleep(100);
					} else {
						ui_suberr = COMM_OK;
						ui_err = Comm_DevDetect(ui_cnt + 1, &ui_suberr, uc_detectinfo);
						printf("port ==== %d, ui_err ===== %d\n", ui_cnt, ui_err);
						if ((ui_err == COMM_OK) && (ui_suberr == COMM_OK)) {
							// Device Type detected
							ui_len = ((unsigned int)uc_detectinfo[20] << 24) | ((unsigned int)uc_detectinfo[21] << 16) |
								((unsigned int)uc_detectinfo[22] << 8) | (unsigned int)uc_detectinfo[23];
							puc_buffer = (unsigned char*)malloc(ui_len * sizeof(char));
							memset(puc_buffer,0,ui_len * sizeof(char));
							ui_DevID = ((unsigned int)uc_detectinfo[2] << 8) | (unsigned int)uc_detectinfo[3];
							ui_err = Comm_Confirm(&ui_DevID, ui_cnt + 1, uc_buffer);
							ComP_Sleep(200);
							if(ui_err == COMM_OK) {
								//Open link led
								uc_link_led &= ~(0x01 << ui_cnt);
								Light_One_Led(LINK_LED, uc_link_led);
								// Upload Device Configuration Database
								ui_subtype = 0xFFFFFF;
								ui_err = Comm_Upload(&puc_buffer, DEVICE_CONFIG_FILE, ui_DevID, &ui_subtype);
								if (ui_err == COMM_OK) {
									puc_ptr = puc_buffer;
									g_pks_EUPara->uc_PortType[ui_cnt] = uc_detectinfo[12];
									switch (g_pks_EUPara->uc_PortType[ui_cnt]) {
									case EU_CFGDEV:
										g_pks_EUPara->kp_pPort[ui_cnt] = malloc(sizeof(EU_DevPara) * sizeof(char));
										g_pks_EUPara->ui_PortSize[ui_cnt] = sizeof(EU_DevPara);
										// Copy EU configuration information
										memcpy(g_pks_EUPara->kp_pPort[ui_cnt], puc_ptr, sizeof(EU_DevPara));
										peu = (EU_DevPara*)(g_pks_EUPara->kp_pPort[ui_cnt]);
										peu->kp_pEuB = &(peu->kp_EuB);
										peu->kp_pExpO = &(peu->kp_ExpO);
										peu->kp_pRemO = &(peu->kp_RemO);
										peu->kp_pPdu = &(peu->kp_Pdu);
										puc_ptr += sizeof(EU_DevPara);
										Comm_EvolveEu(g_pks_EUPara->kp_pPort[ui_cnt], puc_ptr, (ui_len - sizeof(EU_DevPara)), &ui_reallen);

										break;
									case RU_CFGDEV:
										g_pks_EUPara->kp_pPort[ui_cnt] = malloc(sizeof(RU_DevPara) * sizeof(char));
										g_pks_EUPara->ui_PortSize[ui_cnt] = sizeof(RU_DevPara);
										// Copy RU configuration information
										memcpy(g_pks_EUPara->kp_pPort[ui_cnt], puc_ptr, sizeof(RU_DevPara));
										pru = (RU_DevPara*)(g_pks_EUPara->kp_pPort[ui_cnt]);
										pru->kp_pRuB = &(pru->kp_RuB);
										pru->kp_pRemO = &(pru->kp_RemO);
										ui_tfddnum = 0;
										do {
											pru->kp_pFdd[ui_tfddnum] = &(pru->kp_Fdd[ui_tfddnum]);
										} while (++ ui_tfddnum < MAX_FDD);
										ui_tfddnum = 0;
										do {
											pru->kp_pTdd[ui_tfddnum] = &(pru->kp_Tdd[ui_tfddnum]);
										} while (++ ui_tfddnum < MAX_TDD);
										break;
									}

									g_ui_CfgChanged = CFG_CHANGED;
									g_ui_TopoChanged = TOPOL_CHANGED;
								}
								else {
									// Record Error into Error Message Memory 
									switch_OneExpO(ui_cnt,EXPOUL_ONE_OFF);
								}
							} 
							else {
								//Open status led
								uc_status_led &= ~(0x01 << ui_cnt);
								Light_One_Led(STATUS_LED, uc_status_led);
								//++write
								switch_OneExpO(ui_cnt,EXPOUL_ONE_OFF);
							} 
							free(puc_buffer);
						} 
						else {
							//++
							switch_OneExpO(ui_cnt,EXPOUL_ONE_OFF);
						}
					}
				}
			} 
			else {
				//++
				
			}
		} 
		else {
			// Get Device ID no matter it's EU or RU, as the first 4 bytes contain the Device ID only.
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_cnt];
			ui_DevID = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
				((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			ui_suberr = 0;
			ui_exist = 0;
			ui_err = Comm_DevPoll(ui_cnt + 1, ui_DevID, &ui_exist, &ui_suberr);
			printf("port = %d  DevPoll ui_err =  %d  ui_suberr = %d\n", ui_cnt, ui_err, ui_suberr);
			if ((ui_err != COMM_OK) || (ui_suberr != COMM_OK)) {
				g_ui_PortExist[ui_cnt]++;
				if(g_ui_PortExist[ui_cnt] == THREETIMES) {
					printf("---------------------------------------------------------------dev unExist\n");
					// Alarm Here...
					uc_link_led |= (0x01 << ui_cnt);
					uc_status_led |= (0x01 << ui_cnt);
					Light_One_Led(LINK_LED, uc_link_led);
					Light_One_Led(STATUS_LED, uc_status_led);
					
							
					if (g_pks_EUPara->uc_PortType[ui_cnt] == EU_CFGDEV) {
						Comm_FreeEu(g_pks_EUPara->kp_pPort[ui_cnt], CFG_TYPE);
					}
					free(g_pks_EUPara->kp_pPort[ui_cnt]);
					g_pks_EUPara->kp_pPort[ui_cnt] = NULL;
					g_pks_EUPara->ui_PortSize[ui_cnt] = 0;
					g_pks_EUPara->uc_PortType[ui_cnt] = DEVICE_NONE;
					
					if(g_pks_EUState->uc_PortType[ui_cnt] == EU_CFGDEV){
						Comm_FreeEu(g_pks_EUState->kp_pPort[ui_cnt], STATE_TYPE);
					}
					free(g_pks_EUState->kp_pPort[ui_cnt]);
					g_pks_EUState->kp_pPort[ui_cnt] = NULL;
					g_pks_EUState->ui_PortSize[ui_cnt] = 0;
					g_pks_EUState->uc_PortType[ui_cnt] = DEVICE_NONE;

					g_ui_PortExist[ui_cnt] = 0;
					ui_err = switch_OneExpO(ui_cnt,EXPOUL_ONE_OFF);
					//ComP_Sleep(10);
					g_ui_CfgChanged = CFG_CHANGED;
					g_ui_TopoChanged = TOPOL_CHANGED;
				}
			} 
			else {
				g_ui_PortExist[ui_cnt] = 0;
			}
		}
	} while(++ ui_cnt < NUMBER_OF_PORT);


	return ui_err;

}
unsigned int Comm_DevCheckConfig()
{
	unsigned int ui_err, ui_crc, ui_calcrc, ui_subtype, ui_port, ui_DevID, ui_tfddnum;
	unsigned int ui_chgtype, ui_reallen;
	unsigned char uc_buffer[32], uc_payload[528];
	unsigned char* puc_ptr;
	void* pvoid_ptr;
	unsigned int ui_returnptr;
	EU_DevPara* peu;
	RU_DevPara* pru;


	ui_err = COMM_CONFIGCHECKERR;
	ui_port = ui_returnptr = 0;
	do {
		if (g_pks_EUPara->uc_PortType[ui_port] != DEVICE_NONE) {
			memset(uc_buffer, 0 ,sizeof(uc_buffer));
			// Get Device ID no matter it's EU or RU, as the first 4 bytes contain the Device ID only.
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_port];
			ui_DevID = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
						((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			ComD_ConfigCheck1Cmd(ui_DevID, uc_buffer);
			ui_err = Comm_Transfer(uc_buffer, 16, 28, uc_payload);
			if (ui_err == COMM_OK) {
				ui_calcrc = CRC16(&uc_payload[2], 22);
				ui_crc = ((unsigned int)uc_payload[24] << 8 | (unsigned int)uc_payload[25]);
				if (ui_crc == ui_calcrc) {
					ui_chgtype = uc_payload[12];
					switch (ui_chgtype) {
						case NO_CHANGE:
							break;
						case TOPO_CHANGE:
						case TOPO_NEW:
						case TOPO_LOST:
						case PARA_CHANGE:
							//if(ui_chgtype != PARA_CHANGE){
							//	g_ui_TopoChanged = TOPOL_CHANGED;
							//}
							pvoid_ptr = NULL;
							ui_subtype = 0xFFFFFFFF;
							ui_err = Comm_Upload((unsigned char**)&pvoid_ptr, DEVICE_CONFIG_FILE, ui_DevID, &ui_subtype);
							if (ui_err == COMM_OK) {
								if (g_pks_EUPara->uc_PortType[ui_port] == EU_CFGDEV) {						
									Comm_FreeEu(g_pks_EUPara->kp_pPort[ui_port], CFG_TYPE);
									free(g_pks_EUPara->kp_pPort[ui_port]);
								
									g_pks_EUPara->kp_pPort[ui_port] = malloc(sizeof(EU_DevPara) * sizeof(char));
									g_pks_EUPara->ui_PortSize[ui_port] = sizeof(EU_DevPara);
									g_pks_EUPara->uc_PortType[ui_port] = EU_CFGDEV;
									// Copy EU configuration information
									memcpy(g_pks_EUPara->kp_pPort[ui_port], pvoid_ptr, sizeof(EU_DevPara));
									peu = (EU_DevPara*)(g_pks_EUPara->kp_pPort[ui_port]);
									peu->kp_pEuB = &(peu->kp_EuB);
									peu->kp_pExpO = &(peu->kp_ExpO);
									peu->kp_pRemO = &(peu->kp_RemO);
									peu->kp_pPdu = &(peu->kp_Pdu);
									puc_ptr = (unsigned char*)pvoid_ptr + sizeof(EU_DevPara);
									Comm_EvolveEu(g_pks_EUPara->kp_pPort[ui_port], puc_ptr, (ui_subtype - sizeof(EU_DevPara)), &ui_reallen);
								}
								else {
									// Copy RU configuration information
									memcpy(g_pks_EUPara->kp_pPort[ui_port], pvoid_ptr, sizeof(RU_DevPara));
									pru = (RU_DevPara*)(g_pks_EUPara->kp_pPort[ui_port]);
									pru->kp_pRuB = &(pru->kp_RuB);
									pru->kp_pRemO = &(pru->kp_RemO);
									ui_tfddnum = 0;
									do {
										pru->kp_pFdd[ui_tfddnum] = &(pru->kp_Fdd[ui_tfddnum]);
									} while (++ ui_tfddnum < MAX_FDD);
									ui_tfddnum = 0;
									do {
										pru->kp_pTdd[ui_tfddnum] = &(pru->kp_Tdd[ui_tfddnum]);
									} while (++ ui_tfddnum < MAX_TDD);
								}
							}

							free(pvoid_ptr);
							g_ui_CfgChanged = CFG_CHANGED;
							break;
					}
				}
				else {
					printf("********************Comm_DevCheckConfig  CRC error\n");
					// Record Error into Error Message Memory

				}
			}
			else {
				// Record Error into Error MessageMemroy
			}
		}
	} while (++ ui_port < NUMBER_OF_PORT);


	return ui_err;
} 


unsigned int Comm_DevCheckState()
{
	unsigned int ui_err, ui_len, ui_crc, ui_calcrc, ui_reallen;
	unsigned int ui_cnt, ui_off, ui_port, ui_DevID;
	unsigned char* puc_ptr;
	unsigned char* puc_buffer;
	unsigned char uc_buffer[32], uc_payload[528];


	ui_err = COMM_STATECHECKERR;
	ui_port = 0;
	do {
		if (g_pks_EUPara->uc_PortType[ui_port] != DEVICE_NONE) {
			memset(uc_buffer, 0 ,sizeof(uc_buffer));
			memset(uc_payload, 0, sizeof(uc_payload));
			// Get Device ID no matter it's EU or RU, as the first 4 bytes contain the Device ID only.
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_port];
			ui_DevID = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
						((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			ComD_StateCheck1Cmd(ui_DevID, uc_buffer);
			ui_err = Comm_Transfer(uc_buffer, 16, 20, uc_payload);
			if (ui_err == COMM_OK) {
				ui_calcrc = CRC16(&uc_payload[2], 14);
				ui_crc = ((unsigned int)uc_payload[16] << 8 | (unsigned int)uc_payload[17]);
				if (ui_crc == ui_calcrc) {
					ui_len = ((unsigned int)uc_payload[12] << 24) | ((unsigned int)uc_payload[13] << 16) |
								((unsigned int)uc_payload[14] << 8) | (unsigned int)uc_payload[15];
					ui_cnt = ui_len / MAX_PACKETSIZE;
					ui_off = ui_len % MAX_PACKETSIZE;
					puc_buffer = (unsigned char*)malloc((ui_cnt + 1) * MAX_PACKETSIZE);
					ComD_StateCheck2Cmd(ui_DevID, uc_buffer);
					if (ui_len <= MAX_PACKETSIZE) {
						ui_err = Comm_Transfer(uc_buffer, 20, MAX_PACKETSIZE + 16, uc_payload);
						if (ui_err == COMM_OK) {
							ui_calcrc = CRC16(&uc_payload[2], 10 + MAX_PACKETSIZE);
							ui_crc = ((unsigned int)uc_payload[12 + MAX_PACKETSIZE] << 8 | (unsigned int)uc_payload[13 + MAX_PACKETSIZE]);
							if (ui_calcrc != ui_crc) {
								ui_err = CRC_ERROR;
								printf("********************Comm_DevCheckState  CRC error ****1\n");
							}
							else {
								memcpy(puc_buffer, &uc_payload[12], ui_len);
							}
						}
					}
					else {
						// Receiving State Information in loop
						puc_ptr = puc_buffer;
						if (ui_off) {
							ui_cnt += 1;
						}
						
						do {
							ui_err = Comm_Transfer(uc_buffer, 20, MAX_PACKETSIZE + 16, uc_payload);
							ui_calcrc = CRC16(&uc_payload[2], 10 + MAX_PACKETSIZE);
							ui_crc = ((unsigned int)uc_payload[12 + MAX_PACKETSIZE] << 8 | (unsigned int)uc_payload[13 + MAX_PACKETSIZE]);
							if (ui_calcrc != ui_crc) {
								ui_err = CRC_ERROR;
								printf("********************Comm_DevCheckState  CRC error ****2\n");
							}
							else {
								memcpy(puc_ptr, &uc_payload[12], MAX_PACKETSIZE);
								puc_ptr += MAX_PACKETSIZE;
							}
						} while ((-- ui_cnt) && (ui_err == COMM_OK));
					}
					if (ui_err == COMM_OK) {
						ComD_StateCheck3Cmd(ui_DevID, ui_port, uc_buffer);//ui_port
						ui_err = Comm_Transfer(uc_buffer, 20, 20, uc_payload);
						if (ui_err == COMM_OK) {
							ui_calcrc = CRC16(&uc_payload[2], 14);
							ui_crc = ((unsigned int)uc_payload[16] << 8 | (unsigned int)uc_payload[17]);
							if(ui_calcrc == ui_crc) {
								// Update State Information
								puc_ptr = puc_buffer;
								switch(g_pks_EUPara->uc_PortType[ui_port]) {
									case EU_CFGDEV:
										if(g_pks_EUState->kp_pPort[ui_port] != NULL){								
											Comm_FreeEu(g_pks_EUState->kp_pPort[ui_port], STATE_TYPE);
											free(g_pks_EUState->kp_pPort[ui_port]);
										}
										g_pks_EUState->kp_pPort[ui_port] = malloc(sizeof(EU_DevState) * sizeof(char));
										g_pks_EUState->ui_PortSize[ui_port] = sizeof(EU_DevState);
										g_pks_EUState->uc_PortType[ui_port] = EU_CFGDEV;
										// Copy EU configuration information
										memcpy(g_pks_EUState->kp_pPort[ui_port], puc_ptr, sizeof(EU_DevState));
										puc_ptr += sizeof(EU_DevState);
										Comm_EvolveEu(g_pks_EUState->kp_pPort[ui_port], puc_ptr, (ui_len - sizeof(EU_DevState)), &ui_reallen);
										break;
									case RU_CFGDEV:
										if(g_pks_EUState->kp_pPort[ui_port] != NULL){
											free(g_pks_EUState->kp_pPort[ui_port]);
										}
										g_pks_EUState->kp_pPort[ui_port] = malloc(sizeof(RU_DevState) * sizeof(char));
										g_pks_EUState->ui_PortSize[ui_port] = sizeof(RU_DevState);
										g_pks_EUState->uc_PortType[ui_port] = RU_CFGDEV;
										// Copy RU configuration information
										memcpy(g_pks_EUState->kp_pPort[ui_port], puc_ptr, sizeof(RU_DevState));
										break;
								}
							}
							else {
								printf("********************Comm_DevCheckState  CRC error ****3\n");
								// Record Error into Error Message Memory
							}			
						}
						else {
							// Record Error into Error Message Memory
						}
					}
					else {
						// Record Error into Error Message Memory
					}
					free(puc_buffer);
				}
				else {	
					printf("********************Comm_DevCheckState  CRC error ****4\n");		
					// Record Error into Error Message Memory
				}
			}
			else {
				// Record Error into Error Message Memory
			}
		}
	} while (++ ui_port < NUMBER_OF_PORT);


	return ui_err;
}


unsigned int Comm_DevCheckError()
{
	unsigned int ui_err, ui_crc, ui_calcrc;
	unsigned int ui_errnum, ui_port, ui_DevID;
	unsigned char* puc_ptr;
	unsigned char uc_buffer[32], uc_payload[160];


	ui_err = COMM_ERRCHECKERR;
	ui_port = 0;
	do {
		if (g_pks_EUPara->uc_PortType[ui_port] != DEVICE_NONE) {
			memset(uc_buffer, 0 ,sizeof(uc_buffer));
			memset(uc_payload, 0, sizeof(uc_payload));
			// Get Device ID no matter it's EU or RU, as the first 4 bytes contain the Device ID only.
			puc_ptr = (unsigned char*)g_pks_EUPara->kp_pPort[ui_port];
			ui_DevID = ((unsigned int)puc_ptr[0] << 24) | ((unsigned int)puc_ptr[1] << 16) |
						((unsigned int)puc_ptr[2] << 8) | (unsigned int)puc_ptr[3];
			ComD_ErrCheckCmd(ui_DevID, uc_buffer);
			ui_err = Comm_Transfer(uc_buffer, 16, 146, uc_payload);
			if (ui_err == COMM_OK) {
				ui_calcrc = CRC16(&uc_payload[2], 140);
				ui_crc = ((unsigned int)uc_payload[142] << 8 | (unsigned int)uc_payload[143]);
				if (ui_crc == ui_calcrc) {
					if (uc_payload[12] == ERROR_EXISTED) {
						ui_errnum = (unsigned int)uc_payload[13];
						puc_ptr = &uc_payload[14];
						// Enter Critical Section
						ComP_EnterCriticalSection();
						do {
							// Copy Error Message for Network
							memcpy(g_puc_ErrNetworkWptr, puc_ptr, ERRMSG_LEN);
							g_puc_ErrNetworkWptr += ERRMSG_LEN;
							if (g_puc_ErrNetworkWptr == (g_uc_ErrNetworkMemory + ERRMSG_BUFFERLEN)) {
								g_puc_ErrNetworkWptr = g_uc_ErrNetworkMemory;
							}
							// Copy Error Message for Upper-Device
							memcpy(g_puc_ErrUpperWptr, puc_ptr, ERRMSG_LEN);
							g_puc_ErrUpperWptr += ERRMSG_LEN;
							if (g_puc_ErrUpperWptr == (g_uc_ErrUpperMemory + ERRMSG_BUFFERLEN)) {
								g_puc_ErrUpperWptr = g_uc_ErrUpperMemory;
							}
						} while (-- ui_errnum > 0);
						// Leave Critical Section
						ComP_LeaveCriticalSection();
					}
				}
				else {
					printf("********************Comm_DevCheckError  CRC error \n");
					// Record Error into Error Message Memory
				}
			}
			else {
				// Record Error into Error Message Memory
			}
		}
	} while (++ ui_port < NUMBER_OF_PORT);


	return ui_err;
}


static unsigned int Comm_DealwithCmd(unsigned char* puc_cmdbuffer, unsigned int ui_cmdlen)
{
	unsigned int ui_err, ui_desID, ui_EuID, ui_CmdID, ui_lastID, ui_subID;
	unsigned int ui_cmdL, ui_suberr, ui_calcrc;
	unsigned char uc_payload[64];
	unsigned char* puc_ptr, *puc_EUptr;


	puc_ptr = puc_cmdbuffer;
	ui_err = 0;
	// Check header
	if ((puc_ptr[0] == (unsigned char)(DEVICE_HEAD >> 8)) && (puc_ptr[1] == (unsigned char)DEVICE_HEAD)) {
		// Check end
		if ((puc_ptr[ui_cmdlen - 1] == (unsigned char)DEVICE_END) && (puc_ptr[ui_cmdlen - 2] == (unsigned char)(DEVICE_END >> 8))) {
			ui_EuID = ((unsigned int)(g_pks_EUPara->kp_pEuB->uc_DevID[0]) << 24) | ((unsigned int)(g_pks_EUPara->kp_pEuB->uc_DevID[1]) << 16) |
				((unsigned int)(g_pks_EUPara->kp_pEuB->uc_DevID[2]) << 8) | (unsigned int)(g_pks_EUPara->kp_pEuB->uc_DevID[3]);
			ui_cmdL = ((unsigned int)puc_ptr[DEV_CMDLEN_OFFSET] << 8) | (unsigned int)puc_ptr[DEV_CMDLEN_OFFSET + 1];
			if (((puc_ptr[DEV_PAYLOAD_OFFSET] >> 4) == 1) || ((puc_ptr[DEV_PAYLOAD_OFFSET] >> 4) == 2)) { 
				if ((puc_ptr[DEV_PAYLOAD_OFFSET] & 0x0F) == 0){		
					// Get Destination ID
					ui_lastID = ((unsigned int)puc_ptr[DEV_LASTDESID_OFFSET] << 8) | (unsigned int)puc_ptr[DEV_LASTDESID_OFFSET + 1];
					ui_CmdID = ((unsigned int)puc_ptr[DEV_CMDID_OFFSET] << 8) | (unsigned int)puc_ptr[DEV_CMDID_OFFSET + 1];
					ui_subID = ((unsigned int)puc_ptr[DEV_SUBID_OFFSET] << 8) | (unsigned int)puc_ptr[DEV_SUBID_OFFSET + 1];
					if (ui_lastID == ui_EuID) {
						// For EU Device
						if ((ui_CmdID >= CMDEU_START) && (ui_CmdID <= CMDEU_END)) {
							// Configure EU Basic
							g_func_EuBasicFunc[ui_CmdID - CMDEU_START](ui_cmdL, &puc_ptr[DEV_PAYLOAD_OFFSET], &ui_suberr);
						}
						if ((ui_CmdID >= CMDPDU_START) && (ui_CmdID <= CMDPDU_END)) {
							// Configure EU PDU Module
							g_p_CommandPdu[ui_CmdID - CMDPDU_START](ui_cmdL,&puc_ptr[DEV_PAYLOAD_OFFSET],&ui_suberr,uc_payload);
						}
						else if ((ui_CmdID >= CMDEOPT_START) && (ui_CmdID <= CMDEOPT_END)) {
							// Configure EU Expansion Optical Module
							g_p_CommandExOptical[ui_CmdID - CMDEOPT_START](ui_cmdL,&puc_ptr[DEV_PAYLOAD_OFFSET],&ui_suberr,uc_payload);
						}
						else if ((ui_CmdID >= CMDROPT_START) && (ui_CmdID <= CMDROPT_END)) {
							// Configure EU Remote Optical Module
							g_p_CommandReOptical[ui_CmdID - CMDROPT_START](ui_cmdL,&puc_ptr[DEV_PAYLOAD_OFFSET],&ui_suberr,uc_payload);
						}
						else {
							// Special Command 
						}
					}
					else {
						//Forward Command to RU
						puc_ptr[DEV_SRCID_OFFSET] = (unsigned char)(ui_EuID >> 8);
						puc_ptr[DEV_SRCID_OFFSET + 1] = (unsigned char)ui_EuID;
						puc_ptr[DEV_DESID_OFFSET] = (unsigned char)(ui_lastID >> 8);
						puc_ptr[DEV_DESID_OFFSET + 1] = (unsigned char)ui_lastID;
						ui_calcrc = CRC16(&puc_ptr[DEV_SRCID_OFFSET], ui_cmdL + 10);
						puc_ptr[DEV_PAYLOAD_OFFSET + ui_cmdL] = (unsigned char)(ui_calcrc >> 8);
						puc_ptr[DEV_PAYLOAD_OFFSET + ui_cmdL + 1] = (unsigned char)ui_calcrc;
						ui_err = Comm_Transfer(puc_ptr, ui_cmdlen, 18, uc_payload);
					}
				}
				else {
					// Forward Command to EU
					puc_ptr[DEV_PAYLOAD_OFFSET] -= 1;
					puc_EUptr = (unsigned char*)g_pks_EUPara->kp_pPort[(unsigned int)(puc_ptr[DEV_PORTNUM_OFFSET] & 0x0F) - 1];
					ui_desID = ((unsigned int)puc_EUptr[0] << 24) | ((unsigned int)puc_EUptr[1] << 16) |
						((unsigned int)puc_EUptr[2] << 8) | (unsigned int)puc_EUptr[3];
					puc_ptr[DEV_SRCID_OFFSET] = (unsigned char)(ui_EuID >> 8);
					puc_ptr[DEV_SRCID_OFFSET + 1] = (unsigned char)ui_EuID;
					puc_ptr[DEV_DESID_OFFSET] = (unsigned char)(ui_desID >> 8);
					puc_ptr[DEV_DESID_OFFSET + 1] = (unsigned char)ui_desID;
					ui_calcrc = CRC16(&puc_ptr[DEV_SRCID_OFFSET], ui_cmdL + 10);
					puc_ptr[DEV_PAYLOAD_OFFSET + ui_cmdL] = (unsigned char)(ui_calcrc >> 8);
					puc_ptr[DEV_PAYLOAD_OFFSET + ui_cmdL + 1] = (unsigned char)ui_calcrc;
					ui_err = Comm_Transfer(puc_ptr, ui_cmdlen, 18, uc_payload);
				}
			}
		}
	}

	return ui_err;
}


unsigned int Comm_CheckMsg()
{
	unsigned int ui_err, ui_cmdcount, ui_off, ui_cmdlen;
	unsigned char uc_CmdBuffer[MAX_CMDLEN];


	ui_err = COMM_OK;
	// Check Command from Network
	//ComP_CmdEnterCriticalSection();
	ui_cmdcount = (g_puc_CmdNetworkRptr <= g_puc_CmdNetworkWptr) ? (g_puc_CmdNetworkWptr - g_puc_CmdNetworkRptr) : 
		(CMD_BUFFERLEN - (g_puc_CmdNetworkRptr - g_puc_CmdNetworkWptr));
	//ComP_CmdLeaveCriticalSection();
	if (ui_cmdcount > 0) {
		do {
			// Copy Command to a specific buffer which we'll deal with later
			//ComP_CmdEnterCriticalSection();
			if ((g_puc_CmdNetworkRptr + DEV_PAYLOAD_OFFSET) > (g_uc_CmdNetworkBuffer + CMD_BUFFERLEN)) {
				ui_off = (g_uc_CmdNetworkBuffer + CMD_BUFFERLEN) - g_puc_CmdNetworkRptr;
				memcpy(uc_CmdBuffer, g_puc_CmdNetworkRptr, ui_off);
				memcpy((uc_CmdBuffer + ui_off), g_uc_CmdNetworkBuffer, (DEV_PAYLOAD_OFFSET - ui_off));
				// Get Command Length
				ui_cmdlen = ((unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET] << 8) | (unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET + 1];
				memcpy(uc_CmdBuffer + DEV_PAYLOAD_OFFSET, g_uc_CmdNetworkBuffer + (DEV_PAYLOAD_OFFSET - ui_off), ui_cmdlen + DEV_ENDLEN);
				// Clear region in Command Buffer
				memset(g_puc_CmdNetworkRptr, 0, ui_off);
				memset(g_uc_CmdNetworkBuffer, 0, (DEV_PAYLOAD_OFFSET - ui_off + ui_cmdlen + DEV_ENDLEN));
				// Update Read pointer
				g_puc_CmdNetworkRptr = g_uc_CmdNetworkBuffer + (DEV_PAYLOAD_OFFSET - ui_off + ui_cmdlen + DEV_ENDLEN);
			}
			else {
				memcpy(uc_CmdBuffer, g_puc_CmdNetworkRptr, DEV_PAYLOAD_OFFSET);
				ui_cmdlen = ((unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET] << 8) | (unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET + 1];
				if ((g_puc_CmdNetworkRptr + DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN) > (g_uc_CmdNetworkBuffer + CMD_BUFFERLEN)) {
					ui_off = (g_uc_CmdNetworkBuffer + CMD_BUFFERLEN) - (g_puc_CmdNetworkRptr + DEV_PAYLOAD_OFFSET);
					memcpy((uc_CmdBuffer + DEV_PAYLOAD_OFFSET), (g_puc_CmdNetworkRptr + DEV_PAYLOAD_OFFSET), ui_off);
					memcpy((uc_CmdBuffer + DEV_PAYLOAD_OFFSET + ui_off), g_uc_CmdNetworkBuffer, (ui_cmdlen + DEV_ENDLEN - ui_off));
					// Clear region in Command Buffer
					memset(g_puc_CmdNetworkRptr, 0, (DEV_PAYLOAD_OFFSET + ui_off));
					memset(g_uc_CmdNetworkBuffer, 0, (ui_cmdlen + DEV_ENDLEN - ui_off));
					// Update Read pointer
					g_puc_CmdNetworkRptr = g_uc_CmdNetworkBuffer + (ui_cmdlen + DEV_ENDLEN - ui_off);
				}
				else {
					memcpy(uc_CmdBuffer + DEV_PAYLOAD_OFFSET, g_puc_CmdNetworkRptr + DEV_PAYLOAD_OFFSET, ui_cmdlen + DEV_ENDLEN);
					// Clear region in Command Buffer
					memset(g_puc_CmdNetworkRptr, 0, (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN));
					// Update Read pointer
					g_puc_CmdNetworkRptr += (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
				}
				//ComP_CmdLeaveCriticalSection();
				// Deal with comand in specific buffer
				ui_err = Comm_DealwithCmd(uc_CmdBuffer, DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
			}
			ui_cmdcount -= (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
		} while (ui_cmdcount);
	}
	ComP_CmdEnterCriticalSection();
	// Check Command from Upper-Device
	ui_cmdcount = (g_puc_CmdUpperRptr <= g_puc_CmdUpperWptr) ? (g_puc_CmdUpperWptr - g_puc_CmdUpperRptr) : 
		(CMD_BUFFERLEN - (g_puc_CmdUpperRptr - g_puc_CmdUpperWptr));
	ComP_CmdLeaveCriticalSection();
	if (ui_cmdcount > 0) {
		do {
			ComP_CmdEnterCriticalSection();
			// Copy Command to a specific buffer which we'll deal with later
			if ((g_puc_CmdUpperRptr + DEV_PAYLOAD_OFFSET) > (g_uc_CmdUpperBuffer + CMD_BUFFERLEN)) {
				ui_off = (g_uc_CmdUpperBuffer + CMD_BUFFERLEN) - g_puc_CmdUpperRptr;
				memcpy(uc_CmdBuffer, g_puc_CmdUpperRptr, ui_off);
				memcpy((uc_CmdBuffer + ui_off), g_uc_CmdUpperBuffer, (DEV_PAYLOAD_OFFSET - ui_off));
				// Get Command Length
				ui_cmdlen = ((unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET] << 8) | (unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET + 1];
				memcpy(uc_CmdBuffer + DEV_PAYLOAD_OFFSET, g_uc_CmdUpperBuffer + (DEV_PAYLOAD_OFFSET - ui_off), ui_cmdlen + DEV_ENDLEN);
				// Clear region in Command Buffer
				memset(g_puc_CmdUpperRptr, 0, ui_off);
				memset(g_uc_CmdUpperBuffer, 0, (DEV_PAYLOAD_OFFSET - ui_off + ui_cmdlen + DEV_ENDLEN));
				// Update Read pointer
				g_puc_CmdUpperRptr = g_uc_CmdUpperBuffer + (DEV_PAYLOAD_OFFSET - ui_off + ui_cmdlen + DEV_ENDLEN);
			}
			else {
				memcpy(uc_CmdBuffer, g_puc_CmdUpperRptr, DEV_PAYLOAD_OFFSET);
				ui_cmdlen = ((unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET] << 8) | (unsigned int)uc_CmdBuffer[DEV_CMDLEN_OFFSET + 1];
				if ((g_puc_CmdUpperRptr + DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN) > (g_uc_CmdUpperBuffer + CMD_BUFFERLEN)) {
					ui_off = (g_uc_CmdUpperBuffer + CMD_BUFFERLEN) - (g_puc_CmdUpperRptr + DEV_PAYLOAD_OFFSET);
					memcpy((uc_CmdBuffer + DEV_PAYLOAD_OFFSET), (g_puc_CmdUpperRptr + DEV_PAYLOAD_OFFSET), ui_off);
					memcpy((uc_CmdBuffer + DEV_PAYLOAD_OFFSET + ui_off), g_uc_CmdUpperBuffer, (ui_cmdlen + DEV_ENDLEN - ui_off));
					// Clear region in Command Buffer
					memset(g_puc_CmdUpperRptr, 0, (DEV_PAYLOAD_OFFSET + ui_off));
					memset(g_uc_CmdUpperBuffer, 0, (ui_cmdlen + DEV_ENDLEN - ui_off));
					// Update Read pointer
					g_puc_CmdUpperRptr = g_uc_CmdUpperBuffer + (ui_cmdlen + DEV_ENDLEN - ui_off);
				}
				else {
					memcpy(uc_CmdBuffer + DEV_PAYLOAD_OFFSET, g_puc_CmdUpperRptr + DEV_PAYLOAD_OFFSET, ui_cmdlen + DEV_ENDLEN);
					// Clear region in Command Buffer
					memset(g_puc_CmdUpperRptr, 0, (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN));
					// Update Read pointer
					g_puc_CmdUpperRptr += (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
				}
				ComP_CmdLeaveCriticalSection();
				// Deal with comand in specific buffer
				ui_err = Comm_DealwithCmd(uc_CmdBuffer, DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
			}
			ui_cmdcount -= (DEV_PAYLOAD_OFFSET + ui_cmdlen + DEV_ENDLEN);
		} while (ui_cmdcount);
	}


	return ui_err;
}


unsigned int Comm_WholeUpdate()
{
	unsigned int ui_cnt, ui_paratotalsize, ui_stattotalsize, ui_subtotalsize, ui_superID;
	unsigned char* puc_ptr;
	LOAD_HEADER hloader;
	LOAD_HEADER* phloader;

	// Calculate the total size of UDAS Configuration database
	ui_paratotalsize = sizeof(EU_DevPara);
	if (g_ui_CfgChanged == CFG_CHANGED) {
		ui_cnt = 0;
		do {
			if (g_pks_EUPara->uc_PortType[ui_cnt] != DEVICE_NONE) {
				if (g_pks_EUPara->uc_PortType[ui_cnt] == EU_CFGDEV) {
					Comm_CalSizeEu(g_pks_EUPara->kp_pPort[ui_cnt], CFG_TYPE, &ui_subtotalsize);
					ui_paratotalsize += ui_subtotalsize;
					ui_paratotalsize += sizeof(EU_DevPara);
				}
				else {
					ui_paratotalsize += sizeof(RU_DevPara);
				}
				ui_paratotalsize += sizeof(LOAD_HEADER);
			}
		} while (++ ui_cnt < NUMBER_OF_PORT);	
	}
	// Calculate the totalsize of UDAS State Database
	ui_stattotalsize = sizeof(EU_DevState);
	//printf("1111111111**************ui_stattotalsize = %d\n", ui_stattotalsize);
	//if (g_ui_TopoChanged == TOPOL_CHANGED) {
		ui_cnt = 0;
		do {
			if (g_pks_EUState->uc_PortType[ui_cnt] != DEVICE_NONE) {
				if (g_pks_EUState->uc_PortType[ui_cnt] == EU_CFGDEV) {
					Comm_CalSizeEu(g_pks_EUState->kp_pPort[ui_cnt], STATE_TYPE, &ui_subtotalsize);
					ui_stattotalsize += ui_subtotalsize;
					ui_stattotalsize += sizeof(EU_DevState);
				}
				else {
					ui_stattotalsize += sizeof(RU_DevState);
					//printf("22222222222**************ui_stattotalsize = %d\n", ui_stattotalsize);
				}
				ui_stattotalsize += sizeof(LOAD_HEADER);
				//printf("333333333333**************ui_stattotalsize = %d\n", ui_stattotalsize);
			}
		} while (++ ui_cnt < NUMBER_OF_PORT);
	//}
	//printf("**************ui_stattotalsize = %d\n", ui_stattotalsize);


	// Enter Critical Section
	ComP_EnterCriticalSection();

	// Re-malloc the UDAS Configuration Database
	if (g_ui_CfgChanged == CFG_CHANGED) {
		g_ui_paratotalsize = ui_paratotalsize;
		g_ui_CONFIG_CHANGE = CFG_CHANGED;
		if (g_pvoid_bakofEUPara != NULL) {
			free(g_pvoid_bakofEUPara);
		}
		g_pvoid_bakofEUPara = malloc(ui_paratotalsize * sizeof(char));
		memset(g_pvoid_bakofEUPara, 0, g_ui_paratotalsize);
	}
	// Re-malloc the UDAS State Database
	//if (g_ui_TopoChanged == TOPOL_CHANGED) {
		g_ui_stattotalsize = ui_stattotalsize;
		if (g_pvoid_bakofEUState != NULL) {
			free(g_pvoid_bakofEUState);
		}
		g_pvoid_bakofEUState = malloc(ui_stattotalsize * sizeof(char));
		memset(g_pvoid_bakofEUState, 0, g_ui_stattotalsize);
	//}

	// Copy UDAS State database and Configuration database when it is changed
	phloader = &hloader;
	ui_cnt = 0;
	memset(phloader, 0, sizeof(LOAD_HEADER));
	// Copy UDAS State database;
	ui_superID = ((unsigned int)(g_pks_EUState->uc_DevID[0]) << 24) | ((unsigned int)(g_pks_EUState->uc_DevID[1]) << 16) |
					((unsigned int)(g_pks_EUState->uc_DevID[2]) << 8) | (unsigned int)g_pks_EUState->uc_DevID[3];
	puc_ptr = (unsigned char*)g_pvoid_bakofEUState;
	memcpy(puc_ptr, g_pks_EUState, sizeof(EU_DevState));
	puc_ptr += sizeof(EU_DevState);
	Comm_CopyEu(ui_superID, g_pks_EUState, &puc_ptr, STATE_TYPE);
	//test
	//printf("********************g_ui_stattotalsize = %d\n", g_ui_stattotalsize);
	//Write_File(g_pvoid_bakofEUState, g_ui_stattotalsize);
	
	// Copy UDAS Configuration database	
	puc_ptr = (unsigned char*)g_pvoid_bakofEUPara;
	memcpy(puc_ptr, g_pks_EUPara, sizeof(EU_DevPara));
	if (g_ui_CfgChanged == CFG_CHANGED) {
		memset(phloader, 0, sizeof(LOAD_HEADER));
		ui_cnt = 0;
		// Copy UDAS State database;
		//puc_ptr = (unsigned char*)g_pvoid_bakofEUPara;
		//memcpy(puc_ptr, g_pks_EUPara, sizeof(EU_DevPara));
		puc_ptr += sizeof(EU_DevPara);
		Comm_CopyEu(ui_superID, g_pks_EUPara, &puc_ptr, CFG_TYPE);
	}

	// Leave Critical Section
	ComP_LeaveCriticalSection();
	
	
	//printf("________________________________WholeUpdate\n");

	// Reset the sign of the changing of database config and topology state
	g_ui_CfgChanged = CFG_UNCHANGED;
	g_ui_TopoChanged = TOPOL_UNCHANGED;


	return 0;
}

void Comm_MainLoop()
{

	//SYSTEMTIME systime3,systime4;
	// work to do...
	//if (Comm_Init() == COMM_OK) {
		//Test_Exoptical();
	//	do {\A1\A1
			
			// Check Topology in real-time
			Comm_PollDevState();

			// Check Configuration Information Changed in real-time
			Comm_DevCheckConfig();
			// Check State Information Changed in real-time
			Comm_DevCheckState();

			// Check Error or Alarm Message
			Comm_DevCheckError();
			// Check Message which will be sent to 2nd level devices or MU itself
			Comm_CheckMsg();
			 
			// Check if UDAS Configuration database is changed, and then update to backup
			// Also, update UDAS State database to bakcup
			Comm_WholeUpdate();
		
		
			// Sleep for a while
			ComP_Sleep(500);

	//	} while (1);

	//}
}
