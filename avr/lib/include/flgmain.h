
void flg_set_addr(void *data, uint8_t addr);
void flg_queue_pkt(void *data, uint8_t *pkt_data, int len);

/* structs defined in board */
extern struct proto flg_proto;

/* callbacks */
void flg_hw_setup(void);
void flg_set_txen(uint8_t en);
void flg_recv(uint8_t c);
void flg_ping(void);
void flg_work(void);



