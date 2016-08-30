/*
 * log.c
 *
 *  Created on: Jan 17, 2013
 *      Author: zhang
 */

#include "Log.h"


int g_mu_head;  //the head of the g_mu_buff
int g_mu_end;    //the end of the g_mu_buff
int g_eu_head;  //the head of the g_eu_buff
int g_eu_end;    //the end of the g_eu_buff
char g_mu_buff[BUFF_SIZE];
char g_eu_buff[BUFF_SIZE];


/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			Write_MuLog
// Function Description:	write the MU_DATA to user.log
// Function Parameters:
//			INPUT:			p_data:		the base address of data
//							ui_size:	the size of data
//							flag:		SYNC/N_SYNC
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
void Write_MuLog(const char *p_data, unsigned int ui_size,int flag)
{
	unsigned int ui_cnt;
	unsigned int ui_count;
	unsigned int ui_remain;
	unsigned int ui_tmp;


	if(flag == SYNC) {
		//SYNC 直接写入
		syslog(LOG_INFO,"%s",p_data);
	} else {
		//N_SYNC 512字节写一次
		ui_count = 0;
		ui_cnt = ui_size / BUFF_SIZE;
		ui_remain = ui_size % BUFF_SIZE;
		//the ui_size is less than BUFF_SIZE
		if(ui_cnt == 0) {
			g_mu_head += ui_size;
			if(g_mu_head >= BUFF_SIZE) {
				g_mu_head %= BUFF_SIZE;
				ui_tmp = BUFF_SIZE - g_mu_end;
				memcpy(&g_mu_buff[g_mu_end],p_data,ui_tmp);
				//over the mu_buff
				if(g_mu_head > 0) {
					syslog(LOG_INFO,"%s",g_mu_buff);
					g_mu_end = 0;
					memcpy(&g_mu_buff[g_mu_end],p_data + ui_tmp,g_mu_head);
				} else {
					//g_mu_head == 0
					syslog(LOG_INFO,"%s",g_mu_buff);
				}
			} else {
				memcpy(&g_mu_buff[g_mu_end],p_data,ui_size);
			}
		//the ui_size is larger than BUFF_SIZE
		} else {
			ui_tmp = BUFF_SIZE - g_mu_end;
			do {
				memcpy(&g_mu_buff[g_mu_end],p_data + ui_count * BUFF_SIZE,ui_tmp);
				syslog(LOG_INFO,"%s",g_mu_buff);
				printf("%s\n",g_mu_buff);
				g_mu_end = 0;
			}while(++ui_count < ui_cnt);
			memcpy(&g_mu_buff[g_mu_end],p_data + ui_cnt * ui_tmp,(BUFF_SIZE - ui_tmp) + ui_remain);
			//mu_buff is full
			if(((BUFF_SIZE - ui_tmp) + ui_remain) == BUFF_SIZE) {
				syslog(LOG_INFO,"%s",g_mu_buff);
				/*printf("%s\n",g_mu_buff);*/
			}
			g_mu_head += ui_remain;
			g_mu_head %= BUFF_SIZE;
		}
		g_mu_end = g_mu_head;
	}
}



/////////////////////////////////////////////////////////////////////////////////////
// Function Name:			Write_EuLog
// Function Description:	write the EU_DATA to user.log
// Function Parameters:
//			INPUT:			p_data:		the base address of data
//							ui_size:	the size of data
//							flag:		SYNC/N_SYNC
//			OUTPUT:			None
// Return Value:
//							None
/////////////////////////////////////////////////////////////////////////////////////
void Write_EuLog(const char *p_data, unsigned int ui_size,int flag)
{
	unsigned int ui_cnt;
	unsigned int ui_count;
	unsigned int ui_remain;
	unsigned int ui_tmp;


	if(flag == SYNC) {
		//SYNC 直接写入
		syslog(LOG_INFO,"%s",p_data);
	} else {
		//N_SYNC 512字节写一次
		ui_count = 0;
		ui_cnt = ui_size / BUFF_SIZE;
		ui_remain = ui_size % BUFF_SIZE;
		//the ui_size is less than BUFF_SIZE
		if(ui_cnt == 0) {
			g_eu_head += ui_size;
			if(g_eu_head >= BUFF_SIZE) {
				g_eu_head %= BUFF_SIZE;
				ui_tmp = BUFF_SIZE - g_eu_end;
				memcpy(&g_eu_buff[g_eu_end],p_data,ui_tmp);
				//over the mu_buff
				if(g_eu_head > 0) {
					syslog(LOG_INFO,"%s",g_eu_buff);
					g_eu_end = 0;
					memcpy(&g_eu_buff[g_eu_end],p_data + ui_tmp,g_eu_head);
				} else {
					//g_eu_head == 0
					syslog(LOG_INFO,"%s",g_eu_buff);
				}
			} else {
				memcpy(&g_eu_buff[g_eu_end],p_data,ui_size);
			}
		//the ui_size is larger than BUFF_SIZE
		} else {
			ui_tmp = BUFF_SIZE - g_eu_end;
			do {
				memcpy(&g_eu_buff[g_eu_end],p_data + ui_count * BUFF_SIZE,ui_tmp);
				syslog(LOG_INFO,"%s",g_eu_buff);
				printf("%s\n",g_eu_buff);
				g_eu_end = 0;
			}while(++ui_count < ui_cnt);
			memcpy(&g_eu_buff[g_eu_end],p_data + ui_cnt * ui_tmp,(BUFF_SIZE - ui_tmp) + ui_remain);
			//mu_buff is full
			if(((BUFF_SIZE - ui_tmp) + ui_remain) == BUFF_SIZE) {
				syslog(LOG_INFO,"%s",g_eu_buff);
				/*printf("%s\n",g_eu_buff);*/
			}
			g_eu_head += ui_remain;
			g_eu_head %= BUFF_SIZE;
		}
		g_eu_end = g_eu_head;
	}
}


