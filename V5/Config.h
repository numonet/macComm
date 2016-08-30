/*
 *  Config.h
 *
 *  Created on: 05/10/2016
 *      Author: Mian Tang
 */

#ifndef CONFIG_H_
#define CONFIG_H_



#define PRIORITY 		99
#define SERVER_PORT		8888
#define NORMAL_COMMAND		0
#define COMMAND			0
#define INVALID_ADDRESS		0













#define DEBUG



#ifdef DEBUG
	#define PRI_DEBUG(fmt...)	 printf(fmt)
#else
	#define PRI_DEBUG(fmt...)
#endif









 

#endif /* CONFIG_H_ */
