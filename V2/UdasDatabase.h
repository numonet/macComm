/*
 * UdasDatabase.h
 *
 *  Created on: 2013-3-1
 *      Author: Zhang Qiang
 *		Company: Telestone
 */

#ifndef UDASDATABASE_H_
#define UDASDATABASE_H_




#define CHANNEL_8								8




#define MAX_FDD									4
#define MAX_TDD									4







#define USERNAME_LEN						16
#define PASSWORD_LEN						16
#define GEOLOGY_LEN							16
#define IP_LEN								4

#define OPTICAL_8CH							8

#define ONEBYTE								1
#define TWOBYTE								2
#define THREEBYTE							3
#define FOURBYTE							4


// Definition of Device Type
#define DEVICE_NONE							0
#define DEVICE_EU							1
#define DEVICE_RU							2
#define DEVICE_DIU							3


// Basic Configuration for RU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned char uc_SerialNo[FOURBYTE];				// Serial No.
	unsigned char uc_Dbaudrate[FOURBYTE];				// Debug Baudrate
	unsigned char uc_FirmwareVer[FOURBYTE];				// Firmware Version
} RU_Para;


// Basic Configuration for EU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned char uc_SerialNo[FOURBYTE];				// Serial No.
	unsigned char uc_FirmwareVer[FOURBYTE];				// Firmware Version
	unsigned char uc_TempThr;							// Threshold of Temperature
	unsigned char uc_AlarmC;							// Alarm Configuration
	unsigned char us_Reserved1[TWOBYTE];				// Reserved
	unsigned char uc_UserName[USERNAME_LEN];			// User Name
	unsigned char uc_Pwd[PASSWORD_LEN];					// Password
	unsigned char uc_SPwd[PASSWORD_LEN];				// Super Password
} EU_Para;


// Basic Configuration for MU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned char uc_SerialNo[FOURBYTE];				// Serial No.
	unsigned char uc_FirmwareVer[FOURBYTE];				// Firmware Version
	unsigned char uc_TempThr;							// Threshold of Temperature
	unsigned char uc_AlarmC;							// Alarm Configuration
	unsigned char uc_Reserved1[TWOBYTE];				// Reserved
	unsigned char uc_AlarmL1[TWOBYTE];					// Alarm Link 1
	unsigned char uc_AlarmL2[TWOBYTE];					// Alarm Link 2
	unsigned char uc_Longtitude[GEOLOGY_LEN];			// Longtitude
	unsigned char uc_Latitude[GEOLOGY_LEN];				// Latitude
	unsigned char uc_UserName[USERNAME_LEN];			// User Name
	unsigned char uc_Pwd[PASSWORD_LEN];					// Password
	unsigned char uc_SPwd[PASSWORD_LEN];				// Super Password
} MU_Para;


// Basic Configuration for DIU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned char uc_SerialNo[FOURBYTE];				// Serial No.
	unsigned char uc_TempThr;							// Threshold of Temperature
	unsigned char uc_AlarmC;							// Alarm Configuration
	unsigned char uc_Reserved[TWOBYTE];					// Reserved
} DIU_Para;


// Configuration for Remote Optical Module
typedef struct {
	unsigned char uc_RecvThr;							// Receiving Threshold
	unsigned char uc_SendThr;							// Sending Threshold
	unsigned char uc_AttThr;							// ATT Threshold
	unsigned char uc_OpticalSwitch;						// Optical Switch
} RemOptical_Para;


// Configuration for Expand Optical Module
typedef struct {
	unsigned char uc_UrupThr[OPTICAL_8CH];				// UL Receiving Optical Under Power Threshold for 8 Channels
	unsigned char uc_AttThr[OPTICAL_8CH];				// UL Receiving Optical ATT Threshold for 8 Channels
	unsigned char uc_DsupThr;							// DL Sending Optical Under Power Threshold
	unsigned char uc_UlSwitch;							// UL Optical Switch
	unsigned char uc_DlSwitch;							// DL Optical Switch
	unsigned char uc_Reserved;							// Reserved
} ExpOptical_Para;


// Configuration for PDU Module
typedef struct {
	unsigned char uc_PowerSwitch;						// Power Switch for 8 Channels
	unsigned char uc_AlarmC;							// Alarm Enable/Disable
	unsigned char uc_Reserved[TWOBYTE];					// Reserved
} PDU_Para;



// Configuration for RU TDD Module
typedef struct {
	unsigned char uc_RfId[TWOBYTE];						// RF ID for RF board
	unsigned char uc_Alarm1C;							// Alarm 1 series Enable/Disable
	unsigned char uc_Alarm2C;							// Alarm 2 series Enable/Disable

	unsigned char uc_UoopThr;							// UL Output Over Power Threshold
	unsigned char uc_DoopThr;							// DL Output Over Power Threshold
	unsigned char uc_DoupThr;							// DL Output Under Power Threshold
	unsigned char uc_Vswr;								// VSWR
	unsigned char uc_TempThr;							// Temperature Threshold
	unsigned char uc_DiopThr;							// DL Input Over Power Threshold
	unsigned char uc_DiupThr;							// DL Input Under Power Threshold
	unsigned char uc_DodopThr;							// DL Output Pilot Over Power Threshold
	unsigned char uc_DodupThr;							// DL Output Pilot Under Power Threshold
	unsigned char uc_DidopThr;							// DL Input Pilot Over Power Threshold
	unsigned char uc_DidupThr;							// DL Input Pilot Under Power Threshold
	unsigned char uc_TdscdmaLThr;						// TD-SCDMA Lost Threshold

	unsigned char uc_Time1st;							// Time for the 1st switch
	unsigned char uc_Time2nd;							// Time for the 2nd switch
	unsigned char uc_TimeSwitch;						// Proportion of Timing
	unsigned char uc_UopcBias;							// UL Output Power Bias
	unsigned char uc_DopcBias;							// DL Output Power Bias
	unsigned char uc_DrpcBias;							// DL Reflect Power Bias
	unsigned char uc_DipcBias;							// DL Input Power Bias
	unsigned char uc_DodpcBias;							// DL Output Pilot Power Bias
	unsigned char uc_DidpcBias;							// DL Input Pilot Power Bias
	unsigned char uc_Ts0Att;							// TS0 ATT
	unsigned char uc_Ts1Att;							// TS1 ATT
	unsigned char uc_Ts2Att;							// TS2 ATT
	unsigned char uc_Ts3Att;							// TS3 ATT
	unsigned char uc_Ts4Att;							// TS4 ATT
	unsigned char uc_Ts5Att;							// TS5 ATT
	unsigned char uc_Ts6Att;							// TS6 ATT

	unsigned char uc_UlGain;							// UL Gain
	unsigned char uc_DlGain;							// DL Gain
	unsigned char uc_UlAtt;								// UL ATT
	unsigned char uc_DlAtt;								// DL ATT
	unsigned char uc_UlAlc;								// UL ALC
	unsigned char uc_DlAlc;								// DL ALC
	unsigned char uc_DlCtrig;							// DL Close-Trig Threshold
	unsigned char uc_DlOtrig;							// DL Open-Trig Threshold

	unsigned char uc_RfSwitch;							// RF Swtich
	unsigned char uc_Reserved[THREEBYTE];				// Reserved
} RUTDD_Para;


// Configuration for RU FDD Module
typedef struct {
	unsigned char uc_RfId[TWOBYTE];						// RF ID for RF Board
	unsigned char uc_AlarmC;							// Alarm Enable/Disable
	unsigned char uc_UoopThr;							// UL Output Over Power Threshold
	unsigned char uc_DoopThr;							// DL Output Over Power Threshold
	unsigned char uc_DoupThr;							// DL Output Under Power Threshold
	unsigned char uc_Vswr;								// VSWR
	unsigned char uc_TempThr;							// Temperature Threshold

	unsigned char uc_UopcBias;							// UL Output Power Bias
	unsigned char uc_DopcBias;							// DL Output Power Bias
	unsigned char uc_DrpcBias;							// DL Reflect Power Bias
	unsigned char uc_UlGain;							// UL Gain
	unsigned char uc_DlGain;							// DL Gain
	unsigned char uc_UlAtt;								// UL ATT
	unsigned char uc_DlAtt;								// DL ATT
	unsigned char uc_UlAlc;								// UL ALC
	unsigned char uc_DlAlc;								// DL ALC
	unsigned char uc_RfSwitch;							// RF Swtich
	unsigned char uc_Reserved[TWOBYTE];					// Reserved
} RUFDD_Para;


// Configuration for DIU FDD/TDD Module
typedef struct {
	unsigned char uc_RfId[TWOBYTE];						// RF ID for RF Board
	unsigned char uc_AlarmC;							// Alarm Enable/Disable
	unsigned char uc_UoopThr;							// UL Output Over Power Threshold
	unsigned char uc_DoopThr;							// DL Output Over Power Threshold
	unsigned char uc_DoupThr;							// DL Output Under Power Threshold
	unsigned char uc_Reserved[TWOBYTE];					// Reserved
} DIUFTDD_Para;



// Configuration for Network
typedef struct {
	unsigned char uc_IpAddr[IP_LEN];					// IP Address
	unsigned char uc_NetMask[IP_LEN];					// Net Mask
	unsigned char uc_Gateway[IP_LEN];					// IP Gateway
	unsigned char uc_PriDNS[IP_LEN];					// Primary DNS
	unsigned char uc_SecDNS[IP_LEN];					// Secondary DNS
} Network_Para;



// Configuration for MU Device
typedef struct {
	MU_Para				kp_MuB;							// MU Basic Module
	MU_Para*			kp_pMuB;
	ExpOptical_Para		kp_ExpO;						// Expansion Optical Module
	ExpOptical_Para*	kp_pExpO;
	PDU_Para			kp_Pdu;							// PDU Module
	PDU_Para*			kp_pPdu;
	Network_Para		kp_Net;							// Network Module
	Network_Para*		kp_pNet;
	unsigned int		ui_basicstate;					// Describe the working status itself
	unsigned int		ui_PortSize[CHANNEL_8];			// Size in Bytes of each EU/RU configuraiton informaiton which is connected to MU's port
	unsigned char		uc_PortType[CHANNEL_8];			// Type of EU/RU which is connected to MU's port
	void*				kp_pPort[CHANNEL_8];			// Information of EU/RU which is connected to MU's port
} MU_DevPara;



// Configuration for EU Device
typedef struct {
	EU_Para				kp_EuB;							// EU Basic Module
	EU_Para*			kp_pEuB;
	ExpOptical_Para		kp_ExpO;						// Expansion Optical Module
	ExpOptical_Para*	kp_pExpO;
	RemOptical_Para		kp_RemO;						// Remote Optical Module
	RemOptical_Para*	kp_pRemO;
	PDU_Para			kp_Pdu;							// PDU Module
	PDU_Para*			kp_pPdu;
	Network_Para		kp_Net;							// Network Module
	Network_Para*		kp_pNet;
	unsigned int		ui_basicstate;					// Describe the working status itself
	unsigned char		uc_PortType[CHANNEL_8];			// Type of EU/RU which is connected to EU's port
	unsigned int		ui_PortSize[CHANNEL_8];			// Size in Bytes of each EU/RU configuraiton informaiton which is connected to MU's port
	void*				kp_pPort[CHANNEL_8];			// Information of EU/RU which is connected to EU's port
	unsigned int		ui_superiorDevID;				// Superior's Device ID
	unsigned int		ui_superiorPort;				// Superior's port
} EU_DevPara;



// Configuration for RU Device
typedef struct {
	RU_Para				kp_RuB;							// RU Basic Module
	RU_Para*			kp_pRuB;
	RemOptical_Para		kp_RemO;						// Remote Optical Module
	RemOptical_Para*	kp_pRemO;
	RUFDD_Para			kp_Fdd[MAX_FDD];				// RF FDD Modules
	RUFDD_Para*			kp_pFdd[MAX_FDD];
	RUTDD_Para			kp_Tdd[MAX_TDD];				// RF TDD Modules
	RUTDD_Para*			kp_pTdd[MAX_TDD];
	unsigned int		ui_fddnum;						// Number of RF FDD
	unsigned int		ui_tddnum;						// Number of RF TDD
	unsigned int		ui_superiorDevID;				// Superior's Device ID
	unsigned int		ui_superiorPort;				// Superior's port
	unsigned int		ui_basicstate;					// Describe the working status itself
} RU_DevPara;










// State for Remote Optical Module
typedef struct {
	unsigned char uc_RecvP;								// Power Consumption of Receiving
	unsigned char uc_SendP;								// Power Consumption of Sending
	unsigned char uc_AlarmState;						// State of Alarm
	unsigned char uc_Reserved;
} RemOptical_State;


// State for Expand Optical Module
typedef struct {
	unsigned char uc_DlRecvP[OPTICAL_8CH];				// Power Consumption of 1-8 Channel DL Receiving
	unsigned char uc_AlarmState[TWOBYTE];				// State of Alarm
	unsigned char uc_UlRecvP;							// Power Consumption of UL Receiving
	unsigned char uc_Reserved;							// Reserved
} ExpOptical_State;


// State for PDU Module
typedef struct {
	unsigned char uc_AlarmState[FOURBYTE];				// State of Alarm
} PDU_State;


// State for RU TDD Module
typedef struct {
	unsigned char uc_AlarmState[TWOBYTE];				// State of Alarm
	unsigned char uc_pa5v;								// Power Consumption of PA 5V
	unsigned char uc_lna5v;								// Power Consumption of LNA 5V
	unsigned char uc_mcu5v;								// Power Consumption of MCU 5V
	unsigned char uc_work24v;							// Power Consumption of 24V
	unsigned char uc_uInputP;							// UL Input Power Consumption
	unsigned char uc_dInputP;							// DL Input Power Consumption
	unsigned char uc_uOutputP;							// UL Output Power Consumption
	unsigned char uc_dOutputP;							// DL Output Power Consumption
	unsigned char uc_dOutputfP;							// DL Output Reflect Power Consumption
	unsigned char uc_dOutputdP;							// DL Output Pilot Power Consumption
	unsigned char uc_dInputdP;							// DL Input Pilot Power Consumption
	unsigned char uc_vswr;								// VSWR
	unsigned char uc_temperature;						// Device temperature
	unsigned char uc_Reserved;
} RUTDD_State;


// State for RU FDD Module
typedef struct {
	unsigned char uc_AlarmState;						// State of Alarm
	unsigned char uc_pa5v;								// Power Consumption of PA 5V
	unsigned char uc_lna5v;								// Power Consumption of LNA 5V
	unsigned char uc_mcu5v;								// Power Consumption of MCU 5V
	unsigned char uc_work24v;							// Power Consumption of 24V
	unsigned char uc_uInputP;							// UL Input Power Consumption
	unsigned char uc_dInputP;							// DL Input Power Consumption
	unsigned char uc_uOutputP;							// UL Output Power Consumption
	unsigned char uc_dOutputP;							// DL Output Power Consumption
	unsigned char uc_vswr;								// VSWR
	unsigned char uc_dOutputfP;							// DL Output Reflect Power Consumption
	unsigned char uc_temperature;						// Device Temperature
} RUFDD_State;


// State for MU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned int  ui_runtime;							// time of running
	unsigned char uc_temperature;						// Device Temperature
	unsigned char uc_AlarmState;						// State of Alarm
	unsigned char uc_Reserved[TWOBYTE];					// Reserved

	ExpOptical_State	kp_ExpO;						// Expansion Optical Module
	ExpOptical_State*	kp_pExpO;
	PDU_State			kp_Pdu;							// PDU Module
	PDU_State*			kp_pPdu;

	unsigned int		ui_PortSize[CHANNEL_8];			// Size in Bytes of each EU/RU configuraiton informaiton which is connected to MU's port
	unsigned char		uc_PortType[CHANNEL_8];			// Type of EU/RU which is connected to MU's port
	void*				kp_pPort[CHANNEL_8];			// Information of EU/RU which is connected to MU's port
} MU_DevState;


// State for EU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned int  ui_runtime;							// time of running
	unsigned char uc_temperature;						// Device Temperature
	unsigned char uc_AlarmState;						// State of Alarm
	unsigned char uc_Reserved[TWOBYTE];					// Reserved

	ExpOptical_State	kp_ExpO;						// Expansion Optical Module
	ExpOptical_State*	kp_pExpO;
	RemOptical_State	kp_RemO;						// Remote Optical Module
	RemOptical_State*	kp_pRemO;
	PDU_State			kp_Pdu;							// PDU Module
	PDU_State*			kp_pPdu;

	unsigned int		ui_PortSize[CHANNEL_8];			// Size in Bytes of each EU/RU configuraiton informaiton which is connected to MU's port
	unsigned char		uc_PortType[CHANNEL_8];			// Type of EU/RU which is connected to EU's port
	void*				kp_pPort[CHANNEL_8];			// Information of EU/RU which is connected to EU's port
} EU_DevState;


// State for RU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned int  ui_runtime;							// time of running

	RemOptical_State	kp_RemO;						// Remote Optical Module
	RemOptical_State*	kp_pRemO;
	RUFDD_State			kp_Fdd[MAX_FDD];				// RF FDD Modules
	RUFDD_State*		kp_pFdd[MAX_FDD];
	RUTDD_State			kp_Tdd[MAX_TDD];				// RF TDD Modules
	RUTDD_State*		kp_pTdd[MAX_FDD];

	unsigned int		ui_fddnum;						// Number of RF FDD
	unsigned int		ui_tddnum;						// Number of RF TDD
	unsigned int		ui_superiorDevID;				// Superior's Device ID
	unsigned int		ui_superiorPort;				// Superior's port
	unsigned int		ui_basicstate;					// Describe the working status itself
} RU_DevState;


// State for DIU Device
typedef struct {
	unsigned char uc_DevID[FOURBYTE];					// Device ID
	unsigned int  ui_runtime;							// time of running
} DIU_DevState;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>




#endif /* UDASDATABASE_H_ */
