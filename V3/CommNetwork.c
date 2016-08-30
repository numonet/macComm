//******************************************************************
//
// File Name:				CommNetwork.c
// File Description:		It supports the configuration of network module
//							 
// Author:					Tang Mian
// Date:					2012/10/09
// Company Name:			Telestone
//
//******************************************************************

//#include "UdasDatabase.h"
//#include "CommMem.h"
#include "CommUart.h"
#include "CommNetwork.h"
#include "Include.h"
//#include "Configuration.h"




// Definition of ERROR code
#define COMM_OK								0
#define COMM_DEVERR							1
#define COMM_TRANSERR						2
#define COMM_PARAERR						3
#define CFG_DNS_ERR							4
#define CFG_IP_ERR							5
#define CFG_ERR								6
////////////////////////////////////////////////////////////////////////
// the struct IP
////////////////////////////////////////////////////////////////////////
typedef struct _ip {  
	unsigned char ipaddr[15];
	unsigned char netmask[15];  
	unsigned char gateway[15]; 
	unsigned char Dnspri[15];
	unsigned char Dnssec[15];
} IP; 
////////////////////////////////////////////////////////////////////////
// Function Name: 			Ip_config
// Function Description:  configured ip information
// Function Parameters:
//	INPUT:					ip_para
//	OUTPUT:	
// Return Value: 			0: success  
//							1: failure
////////////////////////////////////////////////////////////////////////
int Ip_config_system(const IP *ip, unsigned int ui_ipaddrlen, unsigned int ui_netmasklen, unsigned int ui_gatewaylen)  
{  
	int  ret = 0;  
	unsigned char uc_cmdipnet[55];  
	unsigned char uc_cmdgate[36];

	// ifconfig  
	strcpy(uc_cmdipnet, "ifconfig eth0 "); 	 
	strncat(uc_cmdipnet, ip->ipaddr, ui_ipaddrlen); 	 
	strcat(uc_cmdipnet, " netmask ");  	 
	strncat(uc_cmdipnet, ip->netmask, ui_netmasklen); 	// ifconfig eth0 xxx.xxx.xxx.xxx netmask xxx.xxx.xxx.xxx  
	ret = system(uc_cmdipnet);  
	if (ret < 0) {  
		perror("ifconfig error");    
	}  
	else { 

	    // route  
	    strcpy(uc_cmdgate, "route add default gw "); 	  
	    strncat(uc_cmdgate, ip->gateway, ui_gatewaylen);         // route add default gw xxx.xxx.xxx.xxx 
	    ret = system(uc_cmdgate);  
	    if (ret < 0) {  
		perror("route error");    
	    }  
	}    

	return ret;  
}

unsigned int Dns_config(IP *ip_para, unsigned int ui_dnsprilen, unsigned int ui_dnsseclen)
{
	int fd_dns;
	unsigned int ui_err;
	unsigned char uc_cmdDNSsec[11 + ui_dnsprilen];
	unsigned char uc_cmdDNSpri[11 + ui_dnsseclen];
	
	
	ui_err = 0;
	strcpy(uc_cmdDNSpri, "nameserver ");
	strncat(uc_cmdDNSpri, ip_para->Dnspri, ui_dnsprilen);
	printf("%s\n", uc_cmdDNSpri);

	strcpy(uc_cmdDNSsec, "nameserver ");
	strncat(uc_cmdDNSsec, ip_para->Dnssec, ui_dnsseclen);
	printf("%s\n", uc_cmdDNSsec);
	
	remove("/etc/resolv.conf");
	fd_dns = open("/etc/resolv.conf", O_CREAT | O_EXCL | O_WRONLY, 00770);
	if (fd_dns < 0) {
		perror("fd_dns error: ");
		ui_err = 1;
	}
	else {
		if (write(fd_dns, uc_cmdDNSpri, 11 + ui_dnsprilen) < 0) {
			perror("DNS_1 config");
			ui_err = 1;
		}
		else {
			if (write(fd_dns, "\n", 1) < 0) {
				ui_err = 1;
			}else {
				if (write(fd_dns, uc_cmdDNSsec, 11 + ui_dnsseclen) < 0) {
					perror("DNS_2 config");
					ui_err = 1;
				}
			}
		}
	}
	close(fd_dns);
	
	return ui_err;
}


static unsigned int Compose(unsigned char *uc_ipdata, unsigned char* puc_upload, unsigned int *ui_leng)
{
	unsigned char *puc_ptr;
	unsigned int ui_cnt, ui_j;	
	unsigned char uc_buff[3];


	puc_ptr = puc_upload;
	ui_cnt = 0;
	ui_j = 0;
	do { 
		uc_buff[0] = uc_ipdata[ui_cnt] / 100 + '0';
		uc_buff[1] = (uc_ipdata[ui_cnt] % 100) / 10 + '0';	
		uc_buff[2] = uc_ipdata [ui_cnt] % 10 + '0';	
		if (uc_buff[0] != '0') {
			puc_ptr[ui_j++] = uc_buff[0];	
			puc_ptr[ui_j++] = uc_buff[1];
			puc_ptr[ui_j++] = uc_buff[2];
		}
		if ((uc_buff[0] == '0') && (uc_buff[1] != '0')) {	
			puc_ptr[ui_j++] = uc_buff[1];
			puc_ptr[ui_j++] = uc_buff[2];
		}
		if ((uc_buff[0] == '0') && (uc_buff[1] == '0')) {	
			puc_ptr[ui_j++] = uc_buff[2];
		}
		
		if (ui_cnt < 3) {
			puc_ptr[ui_j++] = '.';
		}
	}while((++ui_cnt) < 4);
	*ui_leng = ui_j;

	return 0;
}



