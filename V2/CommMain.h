//******************************************************************
//
// File Name:				CommMain.h
// File Description:		Header file of the database of UDAS system
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************

#ifndef __COMMMAIN_H_
#define __COMMMAIN_H_





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
#define COMM_ERRFORWARDERR					13

// Definition of Communication Coniguration
// Number of times to try when timeout occurs during communication
#define NUMBER_OF_TRY							3
// Number of ports for device connection over Optical Module
#define NUMBER_OF_PORT							8
// Unknown ID used to detect new device
#define UNKNOWN_ID								0x0000
// Optical Threshold for Device detect. In general, it should be -15dB
#define OPTICAL_THR								-15


#define DEVICE_HEAD							0xFFF0
#define DEVICE_END							0x0FFF
#define DEV_DESID_OFFSET					4
#define DEV_PAYLOAD_OFFSET					12
#define DEV_CMDLEN_OFFSET					10
#define DEV_CMDID_OFFSET					8
#define DEV_ENDLEN							4



#define NO_CHANGE			0
#define TOPO_CHANGE			1
#define TOPO_NEW			2
#define TOPO_LOST			3
#define PARA_CHANGE			4 

#define CMDMU_START							0x00B1
#define CMDMU_END							0x00CF
#define CMDEU_START							0x0081
#define CMDEU_END							0x008F
#define CMDPDU_START						0x0020
#define CMDPDU_END							0x002F
#define CMDEOPT_START						0x0010
#define CMDEOPT_END							0x001F
#define CMDROPT_START						0x0001
#define CMDROPT_END							0x001F




#define FOUND								0
#define NOTFOUND							1
#define STATUS_LED							0
#define LINK_LED							1


typedef struct {
	unsigned int ui_header;
	unsigned int ui_supervisorID;
	unsigned char uc_portnum;
	unsigned char uc_devtype;
	unsigned char uc_numofdev;
	unsigned char uc_reserved;
} LOAD_HEADER;




unsigned int Comm_Init();
unsigned int Comm_DeInit(void);
unsigned int Comm_Transfer(unsigned char* puc_Cmd, unsigned int ui_CmdLen, unsigned int ui_ReturnLen, unsigned char* puc_Payload);
unsigned int Write_File(unsigned char *uc_data, unsigned int ui_lenth);

void Comm_MainLoop();









#endif  /* __COMMMAIN_H_ */
