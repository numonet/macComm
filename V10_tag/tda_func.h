


#ifndef TDA_FUNC_H_
#define TDA_FUNC_H_




unsigned int tda_clock(float clock_dif, char* clock_type, char* clock_in_char, int* clock_in_num);


unsigned int tda_modemInit(void);
unsigned int tda_getLocalAddr(char xid, char* local_addr);
unsigned int tda_getSnifferMode(char xid, char* sniffer_mode);
unsigned int tda_transmitInit(char xid1, char xid2, char ack, char retrans);
unsigned int tda_asyncNotify(struct modemPacket* pmPkt);
unsigned int tda_remSendData(char xid, char remoteAddr, char* msgdata, int msglen);
unsigned int tda_ping(char xid, char* msgdata, int msglen);











#endif


