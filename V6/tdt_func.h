


#ifndef TDT_FUNC_H_
#define TDT_FUNC_H_





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












#endif


