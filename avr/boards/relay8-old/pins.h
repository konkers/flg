/* Port B */
#define   RELAY6   PB0
#define   RELAY7   PB1
#define   DATALED  PB2
#define   SWITCH6  PB3
#define   SWITCH7  PB4
#define   SWITCH8  PB5

/* Port C */
/* Relay indices are 0-based */
/* Outlet numbers in the external world are 1-based; see parse_packet() in flg-p
acket.c */
#define   RELAY0     PC0
#define   RELAY1     PC1
#define   RELAY2     PC2
#define   RELAY3     PC3
#define   RELAY4     PC4
#define   RELAY5     PC5

/* Port D */
/* Note switch indices are 1-based cause the switch is labeled that way on the P
C board! */
/* RS485RECEIVEDATA       PD0 */
/* RS485TRANSMITDATA      PD1 */
#define   RS485TRANSMIT   PD2
#define   SWITCH1         PD3
#define   SWITCH2         PD4
#define   SWITCH3         PD5
#define   SWITCH4         PD6
#define   SWITCH5         PD7


/* For address read from DIP switch */
#define   SWITCH_MASK     0x1f
#define   SWITCH_SHIFT    3
