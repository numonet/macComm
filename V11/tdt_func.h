


#ifndef TDT_FUNC_H_
#define TDT_FUNC_H_




#define TIMETABLE_LENGTH		128
#define DELAYBUFFER_LENGTH		32
#define RXPACKET_LENGTH			32



struct def_state {
    char Ping;
    char Tx;
    char reserved1;
    char reserved2;
};


struct def_queue_length {
    char Ping;
    char Tx;
    char Rx;
    char Tx_Notify;
    char Rx_Notify;
    char addr_to_add;
    char reserved1;
    char reserved2;
};


struct def_Ping_queue {
    char reserved1;
    char xid;
    char type;
    char numParam;
    char subsys;
    char field;
    char length[2];
    char message[32];
    char signature[32];
};


struct def_Addr_book {
    char absent;
    char lastPkt;
    char reserved1;
    char reserved2;
    int address;
    int distance;
    char timeLastPkt[32];
};


struct def_Tx_queue {
    char length;
    char counter;
    char address;
    char Buffer_len;
    char roffset;
    char woffset;
    char payload[32][32];
    char Buffer_payload[32][32];
    int Buffer_Retries[32];
    int Buffer_time[32];
    int Buffer_flag[32];
};


struct def_Time_Table {
    int Gen[TIMETABLE_LENGTH];
    int Sent[TIMETABLE_LENGTH];
    int Tx_count[TIMETABLE_LENGTH];
    float Tx_prob[TIMETABLE_LENGTH];
};


struct def_Q_empty {
    char address;
    float in;
    float out;
    float I;
};


struct def_Rx_queue {
    char counter;
    char length;
    char PacketList[RXPACKET_LENGTH];
    int PacketTime[RXPACKET_LENGTH];
};


struct def_delays {
    int total[DELAYBUFFER_LENGTH];
    int propagation[DELAYBUFFER_LENGTH];
    int GenTime[DELAYBUFFER_LENGTH];
    int Received[DELAYBUFFER_LENGTH];
    int num;
};


struct def_TimerCounter {
    int DataProd;
    int Tx;
    int GUI;
};



unsigned int tdt_modemInit(int macProtocol, char txEnable, char pingEnable, char xid__, float packetRate);
unsigned int tdt_txTask(void);
unsigned int tdt_pingTask(void);
unsigned int tdt_notifyTask(void);
unsigned int tdt_qmonitorTask(void);
unsigned int tdt_dataprodTask(void);
unsigned int tdt_doSync(void);


unsigned int tdt_txEnable(char enable);
unsigned int tdt_pingEnable(char enable);
unsigned int tdt_packetRate(float rate);
unsigned int tdt_retransTimeout(int retrans);
unsigned int tdt_protocol(int macProtocol);
unsigned int tdt_xidIs(char xid__);
unsigned int tdt_autoPing(char ping);
unsigned int tdt_autoSync(char sync);






#endif


