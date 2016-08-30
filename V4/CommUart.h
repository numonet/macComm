///////////////////////////////////////////////////
// File name:	CommUart.h
//
//
//  Created on: 05/02/2016
//      Author: Mian Tang
//
//
///////////////////////////////////////////////////






unsigned int Com_Init(const char* dev_path, int baudrate, int data_bits, int stop_bits, char parity);
unsigned int Com_ConfigureTimeout(unsigned char uc_time);
unsigned int Com_Send(char* puc_ptr, int ui_len);
unsigned int Com_Receive(char* puc_ptr, int ui_len);
unsigned int Com_Close();

