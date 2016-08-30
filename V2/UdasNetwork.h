//******************************************************************
//
// File Name:				UdasNetwork.h
// File Description:		Header file of Network Protocol of UDAS system
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************

#ifndef __UDASNETWORK_H_
#define __UDASNETWORK_H_




#define CFGUPLOAD_HEADER					0xFFFFFFF0
#define STATUPLOAD_HEADER					0xFFFFFFF1



// Definition of ERROR code for application
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


// The Definition of Error CODE for communication
#define UNKNOWN_ERR							1
#define HOST_ERR							2
#define SLAVE_ERR							3
#define ETHERNET_ERR						4
#define CRC_ERR								5
#define UNSUPPORTED_ERR						6
#define PARA_ERR							7
#define PARARANGE_ERR						8
#define RIGHT_ERR							9
#define UNSUCCESS_ERR						10
#define ONLY_ERR							11
#define HEADER_ERR							12
#define DEVICEID_ERR						13





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



#define DEVHEADER_LEN						2
#define DEVICE_HEAD							0xFFF0
#define DEVICE_END							0x0FFF
#define DEV_SRCID_OFFSET					2
#define DEV_DESID_OFFSET					4
#define DEV_PAYLOAD_OFFSET					12
#define DEV_CMDLEN_OFFSET					10
#define DEV_CMDID_OFFSET					8
#define DEV_CMDTYPE_OFFSET					6
#define DEV_ENDLEN							4
#define HEADERLEN							12
#define ENDLEN								4
#define DEV_PORTNUM_OFFSET					13
#define DEV_LASTDESID_OFFSET				14
#define DEV_SUBID_OFFSET					18


#define UPLAOD_ENDFLAG						0xAC1529C0



#define CONFIG_CHANGED							1
#define TOPO_CHANGED							2
#define TOPO_NEWDEVICE							1

#define CFG_UNCHANGED							0
#define CFG_CHANGED								1
#define TOPOL_CHANGED							0
#define TOPOL_UNCHANGED							1
#define THREETIMES								3


// Definition of Communication Coniguration
// Number of times to try when timeout occurs during communication
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



// Length of Command Buffer
#define CMD_BUFFERLEN							10240

// Max Length of Command
#define MAX_CMDLEN								256

// Broadcast Device ID
#define BROADCAST_ID							0xFFFF

// Self-Device ID Flag
#define SELF_ID									0xFFFF

// Alarm or Error Message length
#define ALARMERR_LEN							8
// Maximum message for each payload
#define NUMBER_OF_ALARMERR						16


// Command List
#define PROBE_NEW_CMD								0x100
#define CONFIRM_NEW_CMD								0x10D
#define POLL_EXIST_CMD								0x10B
#define UPLOAD_STAT_CMD								0x10C
#define UPLOAD1_FILE_CMD							0x103
#define UPLOAD2_FILE_CMD							0x104
#define UPLOAD3_FILE_CMD							0x105
#define UPLOAD_ALARMERR_CMD							0x109



// Length of Payload for Command List
#define PROBE_CMDLEN								0x0C
#define CONFIRM_CMDLEN								0x04
#define POLL_CMDLEN									0x04
#define UPLOADSTAT1_CMDLEN							0x04
#define UPLOADSTAT3_CMDLEN							0x04
#define UPLOADFILE1_CMDLEN							0x04
#define UPLOADFILE3_CMDLEN							0x00
#define UPLOADALARMERR_CMDLEN						0x82













#endif   /*__UDASNETWORK_H_ */
