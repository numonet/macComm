/*
 * config.h
 *
 *  Created on: 2012-11-27
 *      Author: zhang
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#define LEN					128
#define PATH				"/dev/ttySAC3"
#define UPPATH				"/dev/ttySAC0"
#define FIFO				"fork_fifo"
#define BUFFSIZE			(2 * 1024)
#define	CHILD_A				"CommMain_A"
#define CHILD_B				"CommMain_B"
#define CHILD_UPDATE		"CommMain_Update"
#define UPDATE_SORCE		"/mnt/usb-1/CommMain_Update"
#define SORCE_DIR			"/mnt/usb-1"
#define USB					"/dev/sda1"
#define KILL				"001"
#define DNS_PATH			"/etc/resolv.conf"
#define SYSDEFAULT_CFGFILE	"/home/default.ucfg"
#define USER_CFGFILE		"/home/user.ucfg"

#define SERVER_PORT			8888
#define BACKLOG				2
#define RD_SIZE				(64 * 1024)
#define WR_SIZE				(8 * 1024)
#define SEND_SIZE			1472

#define PACKAGE_SIZE 		20
#define SRCID_WEB			0xFFFE
#define SRCID_CNET			0xFFFF
#define SRCID_ADDR_H		0
#define SRCID_ADDR_S		1
#define SRCTYPE_ADDR_H		2
#define SRCTYPE_ADDR_S		3
#define DESID_ADDR_H		4
#define DESID_ADDR_S		5
#define DESTYPE_ADDR_H		6
#define DESTYPE_ADDR_S		7
#define PROTO_VER			0x01
#define SYS_ID				0xFFFF
#define RESERVE				0xFFFF
#define MSGTYPE_ADDR_H		14
#define MSGTYPE_ADDR_S		15
#define MSGID_ADDR_H		16
#define	MSGID_ADDR_S		17
#define MSGLEN_ADDR_H		18
#define MSGLEN_ADDR_S		19
#define FILESIZE_ADDR_H16	20
#define FILESIZE_ADDR_H8	21
#define FILESIZE_ADDR_S16	22
#define FILESIZE_ADDR_S8	23
#define FILETYPE_ADDR_H		24
#define FILETYPE_ADDR_S		25
#define UP_FILETYPE_ADDR_H	20
#define UP_FILETYPE_ADDR_S	21
#define CRC_ADDR_H16		24
#define CRC_ADDR_H8			25
#define CRC_ADDR_S16		26
#define CRC_ADDR_S8			27
#define FILENUM_ADDR_H		20
#define FILENUM_ADDR_S		21
#define SRC_TYPE			0x0006
#define DEST_ID_H			0
#define BROAD_DESTID		0xFFFF
#define DESTYPE_MU			0x0001
#define DESTYPE_EU			0x0002
#define SINGLE_MSG			0x0001
#define BROAD_MSG			0x0002
#define MSG_TYPEERR			0x0002
#define FILE_TYPEERR		0x0007
#define GET_DESTID			0x0212
#define MSGID_UPFILE		0x020F
#define MSGID_UPSTART 		0x0210
#define MSGID_UPOVER		0x0211
#define MSGID_DOWNFILE		0x020C
#define MSGID_DOWNOVER		0x020E
#define MSGID_DOWNSTART		0x020D
#define MSG_LEN				0x0026
#define MSG_CONTENT			2048
#define MSG_CONTENT2		1024
#define RESPON_UPFILE		0x820F
#define RESPON_UPSTART 		0x8210
#define RESPON_UPOVER		0x8211
#define RESPON_GET_DESTID   0x8212
#define RESPON_DOWNFILE		0x820C
#define RESPON_DOWNSTART	0x820D
#define RESPON_DOWNOVER		0x820E
#define CRC_SIZE 			4
#define RESPON_LEN1			0x0004
#define RESPON_LEN2			0x0002
#define RESPON_LEN3			0x0008
#define RESPON_LEN4			0x0000
#define RESPON_LEN5			0x0000
#define SUCCESS				0x00
#define ERROR				0x01
#define COMMAND_LEN			20
#define FILE_NUM			2
#define COMMAND_ERR			0x8204
#define NOT_SUPPORTCMD		0x0006

#define FILE_SIZE			(2 * 1024 * 1024)
#define FILE_NAME 			32

#define NORMAL_COMMAND		0
#define DOWNFILE_COMMAND	1
#define UPFILE_COMMAND		2

#define COMMAND 			0
#define DATA 				1


//应用程序
#define APP_FILE			0x0001
#define APP_SOURCE			"/home/zhang/u-boot.bin"
#define ELF					0x7F454C46
//配置文件
#define CONFIG_FILE			0x0002
#define CONFIG_SOURCE		"eu_config.ucfg"
#define TELE				0x54454C45
//数据库文件
#define DATA_FILE			0x0003
#define	LIB_SOURCE 			"/home/zhang/u-boot.bin"
#define LIB					0x1
//LOG文件
#define LOG_FILE			0x0004
#define LOG_SOURCE			"/home/zhang/u-boot.bin"
#define LOG					0x2
//其他文件
#define OTHER_FILE			0x0005
#define OTHER_SOURCE		"/home/zhang/u-boot.bin"
#define OTHER				0x3

#define ONE_SECONDS			1
#define ZERO_MS				0

#define BAUD_RATE			115200
#define DATA_BITS 			8
#define STOP_BITS			1
#define PARITY				'N'

#define KILL_NUM	1
#define DES_ID		0x000A

#define DEBUG

#ifdef DEBUG
	#define PRI_DEBUG(fmt...)	 printf(fmt)
#else
	#define PRI_DEBUG(fmt...)
#endif

#define PRIORITY	99//线程最大优先级 1-99

#endif /* CONFIG_H_ */
