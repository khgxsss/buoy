
typedef struct	DebugMsg_TYPE {
	const char 		  *cmd ;
	uint8_t         len;
	unsigned long   waitTime;
} t_DebugMsg_TYPE;


const t_DebugMsg_TYPE LoRaDebugMsg[] = {
  {"OK",                  2,          1},
  {"ERROR",               5,          1},
  {"BUSY",                4,      40000},
  {"READY",               5,          1},
  {"Wisol LoRa",         10,          1},
  {"DevEui : ",           9,          1},
  {"Join is completed",  17,        500},
  {"JOIN_COMPLETE",      13,          1},
  {"JOIN_REQ_1ST",       12,          1},
  {"JOIN_REQ_2ND",       12,          1},
  {"JOIN_ACCEPT",        11,          1},
  {"RSSI",                4,          1},
  {"SNR",                 3,          1},
  {"RX1CH_OPEN",         10,          1},
  {"RX2CH_OPEN",         10,          1},
  {"Ack received",       12,          1},
  {"Ack not received",   16,          1},
  {"ABP Connection OK",  17,          1},
  {"FCnt",                4,          1},
  {"DevReset",            8,          1},
  {"DEVRESET",            8,          1},
  {"RepPerChange",       12,          1}, 
  {"INTERVAL",            8,          1},
  {"RepImmediate",       12,          1},
  {"REPORT",              6,          1},
  {"extDevMgmt",         10,          1},
  {"EXTDEVMGMT",         10,          1},
  {"OTAA",                4,          1},
  {"SKT",                 3,          1},
  {"Freq",                4,          1},
  {"SEND",                4,          1},
  {"CON_UP",              6,       7000},
  {"UNCON_UP",            8,          1},
  {"CON_UP_RETX_OK",     14,          1},
  {"UNCON_UP_RETX_OK",   16,          1},
  {"RX_MSG_FP",           9,          1},
  {"CON_DOWN",            8,          1},
  {"UNCON_DOWN",         10,          1},
  {"LINK_CHECK_REQ",     14,          1},
  {"LINK_CHECK_ANS",     14,          1},
  {"LBT_DETECT",         10,          1},
  {"SLEEP",               5,          1},
  {"WAKE_UP",             7,          1},
  {"FW_VER",              6,          1},
  {"PROV_ENABLE",        11,          1},
  {"PROV_DISABLE",       12,          1},
  {"PROV_STATE_NONE",    15,          1},
  {"PROV_STATE_DONE",    15,          1},
  {"CLASS",               5,          1},
  {"CYCLE_ON",            8,          1},
  {"CYCLE_OFF",           9,          1},
  {"Debug MSG",           9,          1},
  {"AppEui",              6,          1},
  { NULL,              NULL,       NULL}
};

t_DebugMsg_TYPE *LoRaApp[2];
t_DebugMsg_TYPE	*LoRaAppReal;

typedef struct SKTLoRaCID_TYPE {
  const char *cid;
}t_SKTLoRaCID_TYPE;

const t_SKTLoRaCID_TYPE CID[] = {
  /*{"20"},*/ {"21"}, {"22"}, /*{"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"}, {"2A"},*/ {"2B"}, {"2C"}, {"2D"}, {"2E"}, {"2F"},
  {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"}, /*{"3A"},*/ {"3B"}, {"3C"}, {"3D"}, /*{"3E"},*/ {"3F"},
  {"40"}, /*{"41"},*/ {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"}, {"4A"}, {"4B"}, {"4C"}, {"4D"}, {"4E"}, {"4F"},
  {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, /*{"58"}, {"59"}, {"5A"}, {"5B"},*/ {"5C"}, {"5D"}, {"5E"}, {"5F"},
  {"60"}, {"61"}, {"62"}, {"63"}, {"64"}, {"65"}, {"66"}, {"67"}, {"68"}, /*{"69"},*/ {"6A"}, /*{"6B"}, {"6C"},*/ {"6D"}, {"6E"}, {"6F"},
  {"70"}, {"71"}, {"72"}, {"73"}, {"74"}, /*{"75"}, {"76"}, {"77"}, {"78"},*/ {"79"}, {"7A"}, /*{"7B"}, {"7C"}, {"7D"}, {"7E"},*/ {"7F"},
  {"80"}, {"81"}, {"82"}, {"83"}, /*{"84"}, {"85"}, {"86"}, {"87"}, {"88"}, {"89"}, {"8A"}, {"8B"}, {"8C"}, {"8D"}, {"8E"},*/ NULL
};

t_SKTLoRaCID_TYPE *LoRaCID[1];
t_SKTLoRaCID_TYPE	*LoRaCIDReal;


typedef struct  __attribute__ ((packed)) TxBinaryMsg_TYPE {
	char            cmd[4] = {'L', 'R', 'W', ' '};
	char            cid[3] = {'4', 'D', ' '};
	uint8_t         confirm;
	uint8_t         Fport;
	uint8_t         len;
	char            msg[128];
} t_TxBinaryMsg_TYPE;

t_TxBinaryMsg_TYPE txBin;



