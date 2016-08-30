/*
 * log.h
 *
 *  Created on: Jan 17, 2013
 *      Author: zhang
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <syslog.h>
#include <string.h>

#define BUFF_SIZE	512
#define SYNC		1    //同步写
#define N_SYNC		0	//非同步,512字节写一次

void Write_MuLog(const char *p_data, unsigned int ui_size,int flag);
void Write_EuLog(const char *p_data, unsigned int ui_size,int flag);



#endif /* LOG_H_ */
