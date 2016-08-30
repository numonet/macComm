/*
 * CommPlatform.h
 *
 *  Created on: 2013-3-1
 *      Author: Zhang Qiang
 *		Company: Telestone
 */

#ifndef COMMPLATFORM_H_
#define COMMPLATFORM_H_





//unsigned int uart_Init(void);
unsigned int ComP_DownDeInit(void);
unsigned int ComP_UpDeInit(void);
unsigned int ComP_UpWrite(unsigned char* puc_Cmd, unsigned int ui_CmdLen);
unsigned int ComP_UpRead(unsigned char* puc_Payload, unsigned int ui_ReturnLen);
unsigned int ComP_UpReadClear();
unsigned int ComP_ReadClear();
unsigned int ComP_Transfer(unsigned char* puc_Cmd, unsigned int ui_CmdLen, unsigned int ui_ReturnLen, unsigned char* puc_Payload);
unsigned int ComP_HostTransfer(unsigned char* puc_Cmd, unsigned int ui_CmdLen, unsigned int ui_ReturnLen, unsigned char* puc_Payload);
unsigned int ComP_Sleep(unsigned int nMs);

unsigned int ComP_EnterCriticalSection();
unsigned int ComP_LeaveCriticalSection();
unsigned int ComP_CmdEnterCriticalSection();
unsigned int ComP_CmdEnterCriticalSection();


#endif /* COMMPLATFORM_H_ */
