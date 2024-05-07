#ifndef _INCLUDE_APP_H_
#define _INCLUDE_APP_H_

/****************************************************************************
*       Include header
*****************************************************************************/

#include <stm32yyxx_ll_adc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <STM32LowPower.h>
#include <STM32RTC.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include <math.h>
#include "LoRa_msg.h"
#include "BuoyMsgFrame.h"
#include "DataStructure.h"
#include "BUOY_EEPROM.h"

/****************************************************************************
*       Macro Definitions
*****************************************************************************/

HardwareSerial Serial3(USART3);
HardwareSerial Serial5(UART5);

/* Values available in datasheet */
#if defined(STM32C0xx)
#define CALX_TEMP 30
#else
#define CALX_TEMP 25
#endif

#if defined(STM32C0xx)
#define VTEMP      760
#define AVG_SLOPE 2530
#define VREFINT   1212
#elif defined(STM32F1xx)
#define VTEMP     1430
#define AVG_SLOPE 4300
#define VREFINT   1200
#elif defined(STM32F2xx) || defined(STM32F4xx)
#define VTEMP      760
#define AVG_SLOPE 2500
#define VREFINT   1210
#endif

/* Analog read resolution */
#define LL_ADC_RESOLUTION LL_ADC_RESOLUTION_12B
#define ADC_RANGE 4096

#define _USART2_BAUDRATE_           115200
#define _UART5_BAUDRATE_            115200

#define PIN_UART2_IN              PA3

#define _Include_Serial2_		1
#define _Include_Serial3_		1
#define _Include_Serial5_		1
#define _SKT_LORA_EXECUTE_      1
#define __SERIAL5_PRINT__       1
#define __SERIAL3_DEBUG__       0
#define _DEBUG_GPS_             0
#define _GPS_PMTK_CMD_Sleep_    0
#define _GPS_PMTK_CMD_Stop_     0

//#define __To_LoRa_Send_1st__
//#define __debug__
#define __debug_onReceive__

#define _STANDARD_DEVIATION_

#define BATT_LED                    PC2
#define GPIO2_GPS_RST               PB9
#define GPIO6_GPS_PWR_SW            PB13
#define GPIO7_GPS_PPS               PB14

#define RF_SWITCH_CTRL              PA4

#define BATTERY_INPUT               PC1   // The RCU ADC is PA1 port.

#define _LOWPOWER_ON_

#ifdef _LOWPOWER_ON_
#define __PWR_SAVING__
#endif

#ifdef __PWR_SAVING__
#define __IDLE__           0
#define __SLEEP__          0
#define __DEEP_SLEEP__     1
#define __SHUTDOWN__       0
#endif


#define WDT_TEST_1                    0
#define WDT_TEST_2                    0
#define WDT_TEST_3                    0

// Define Software version
#define OP_VALUE_NO_INFORMATION       0x00    // op = 0
#define OP_VALUE_UPLOAD_INTERVAL      0x40    // op = 1

#define TIME_UNIT_FOR_UPLINK_5MIN     0x00    // unit = 0
#define TIME_UNIT_FOR_UPLINK_HOUR     0x20    // unit = 1


#define RETRY_SEND_COUNT              2
#define LORA_DNLINK                   0
#if 0
#define LORA_UPLINK                   1
#else
#define LORA_UPLINK                   0
#endif

#define SW_VER                        "0.1"
#define SVC_CODE                      0x0A

/****************************************************************************
*       Global Variable Definitions
*****************************************************************************/

uint8_t networkSessionKey[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t applicationSessionKey[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t deviceAddressABP[4] = {0x11, 0x11, 0x00, 0x0f};

uint8_t otaa[] = {'o', 't', 'a', 'a'};
uint8_t abp[] = {'a', 'b', 'p'};

typedef enum
{
    TIME_1MS          = 1,
    TIME_2MS          = 2,
    TIME_3MS          = 3,
    TIME_4MS          = 4,
    TIME_5MS          = 5,
    TIME_10MS         = 10,
    TIME_20MS         = 20,
    TIME_30MS         = 30,
    TIME_40MS         = 40,
    TIME_50MS         = 50,
    TIME_100MS        = 100,
    TIME_200MS        = 200,
    TIME_300MS        = 300,
    TIME_500MS        = 500,
    TIME_600MS        = 600,
    TIME_700MS        = 700,
    TIME_800MS        = 800,
    TIME_900MS        = 900,
    TIME_1SEC         = 1000,
    TIME_1P5SEC       = 1500,
    TIME_2SEC         = 2000,
    TIME_3SEC         = 3000,
    TIME_4SEC         = 4000,
    TIME_5SEC         = 5000,
    TIME_6SEC         = 6000,
    TIME_7SEC         = 7000,
    TIME_8SEC         = 8000,
    TIME_9SEC         = 9000,
    TIME_10SEC        = 10000,
    TIME_11SEC        = 11000,
    TIME_12SEC        = 12000,
    TIME_13SEC        = 13000,
    TIME_15SEC        = 15000,
    TIME_17SEC	  	  = 17000,
    TIME_20SEC        = 20000,
    TIME_30SEC        = 30000,
    TIME_40SEC        = 40000,
    TIME_1MIN         = 60000,
    TIME_1P5MIN       = 90000,
    TIME_2MIN         = 120000,
    TIME_3MIN         = 180000,
    TIME_5MIN         = 300000,
    TIME_10MIN        = 600000,
    TIME_20MIN        = 1200000,
    TIME_30MIN        = 1800000,
    TIME_1HOUR        = 3600000,
    TIME_2HOUR        = 7200000,
    TIME_4HOUR        = 14400000,
    TIME_8HOUR        = 28800000,
    TIME_12HOUR       = 43200000,
    TIME_18HOUR       = 64800000,
    TIME_24HOUR       = 86400000,
}mTimeInterval;

unsigned char rcvLoRa[64], rcvLoRaProc[64];
unsigned char rcvCnt;
unsigned char txflag;

typedef struct
{
  unsigned char rxLen;
  unsigned char ackCnt;
  unsigned char ackPosIndex[16];
  unsigned char myPosition;
}_RX_ACK_;

typedef union{
  struct{
	  unsigned char r:2;
    unsigned char q:3;
	  unsigned char p:3;
  }Bit;
  unsigned char p_q_r;
}GateWay3rd;

typedef struct
{
  unsigned char nodeID;
  unsigned char hh1;
  unsigned char hh2;
  GateWay3rd    D3;
  unsigned char nodeInfo[32];
}_DOWNLINK_STRUCT_;

typedef struct
{
  unsigned char ndID;
  union{
  	struct {
		  unsigned char value:6;
		  unsigned char op:2;
  	}Bit;
	  unsigned char Byte;
  }D2;
}_ACK_PACKET_;

typedef union {
  struct {
  	unsigned char value:4;
	  unsigned char ctrl:2;
	  unsigned char op:2;
  }Bit;
  unsigned char Byte;
}_OP_FORM1_;

typedef union {
  struct {
  	unsigned char interval:5;
	  unsigned char unit:1;
	  unsigned char op:2;
  }Bit;
  unsigned char Byte;
}_OP_FORM2_;

typedef union {
  struct {
  	unsigned char value:6;
	  unsigned char op:2;
  }Bit;
  unsigned char Byte;
}_OP_FORM3_;


typedef struct
{
  unsigned char nodeID;
  union{
    struct{
	    unsigned char MagneticInd:3;
      unsigned char Batt:2;
      unsigned char Ver:2;
      unsigned char CommDirection:1;
    }Bit;
    unsigned char Byte_magnetic;
  }Dev2nd;

  union{
    struct{
      unsigned char msgId:4;
      unsigned char svcCode:4;
    }Bit;
    unsigned char Byte_svcMsgid;
  }Dev3rd;

  unsigned char Byte_rsvd2;

  union{
    struct{
      unsigned char ChkBit:4;
      unsigned char rsvd3:4;
    }Bit;
    unsigned char Byte_chkBit;
  }Dev5th;
}_UPLINK_STRUCT_;

typedef enum
{
    step1             = 1,
    step2             = 2,
    step3             = 3,
}mStep;

unsigned long currentMillis = 0, previousMillis = 0, previousMillis2 = 0;

unsigned char pStep;
int packetSizel;
int rssiValue;

unsigned int calBattery;
float calculationV;

uint8_t retryCnt = 0;

typedef enum {
	cmp_idle     = 0,
	cmp_run      = 1,
	cmp_sleep    = 2,
	cmp_set      = 3,
	cmp_reset    = 4,
	cmp_stop,
	cmp_1st,
	cmp_2nd,
	cmp_3rd,
	cmp_4th,
	cmp_5th,
	cmp_6th
}mStateValue;

typedef union RunningStatus
{
    uint16_t	data;
	struct
    {
      uint16_t st_powerOn        :1;
      uint16_t st_Join_Accept    :1;
      uint16_t st_Join_Complete  :1;
      uint16_t st_ready          :1;
      uint16_t st_sleep          :1;
      uint16_t st_lora_reset     :1;
      uint16_t st_ack_rcv        :1;
      uint16_t st_ack_not_rcv    :1;

      uint16_t st_idle           :1;
      uint16_t st_busy           :1;
      uint16_t st_error          :1;
      uint16_t st_send           :1;
      uint16_t st_gps_rcv        :1;
      uint16_t st_finish         :1;
      uint16_t st_gps_mon        :1;
      uint16_t st_rsvd1          :1;

      uint16_t st_activation_mode:1; // true : otaa, false : abp
    }b;
}t_RunningStatus;

typedef struct RunManage
{
  t_RunningStatus runSt;
  uint8_t         curRunMode;
  unsigned long   busyProcMillis;
  unsigned long   resetProcMillis;
  unsigned long   joinProcMillis;
  unsigned long   gpsProcMillis;
  unsigned long   finishMillis;
}t_RunManage;

typedef struct __attribute__ ((packed)) GPS_GGA_DataFormat
{
  uint8_t msgId[8];
  uint8_t utcTime[12];
  uint8_t latitude[12];
  uint8_t N_S_indicator;
  uint8_t longitude[12];
  uint8_t E_W_indicator;
  uint8_t positionFix;
  uint8_t usedSatellites[4];
  uint8_t HDOP[8];
  uint8_t mslAltitude[8];
  uint8_t uints1;
  uint8_t geoidalSeparation[8];
  uint8_t uints2;
  uint8_t AgeOfDiff;
  uint8_t *checksum[4];
} t_GPS_GGA_DataFormat;

typedef struct __attribute__ ((packed)) GPS_RMC_DataFormat
{
  uint8_t msgId[8];
  uint8_t utcTime[12];
  uint8_t status;
  uint8_t latitude[12];
  uint8_t N_S_indicator;
  uint8_t longitude[12];
  uint8_t E_W_indicator;
  uint8_t speed[8];
  uint8_t course[8];
  uint8_t date[8];
  uint8_t magnetic[8];
  uint8_t E_W_indicator2;
  uint8_t mode;
  uint8_t checksum[4];
} t_GPS_RMC_DataFormat;

t_GPS_GGA_DataFormat gpsGGA;
t_GPS_RMC_DataFormat gpsRMC;


/****************************************************************************
*       Global Function Definitions
*****************************************************************************/

void serialEvent2(void);
void serialEvent3(void);
void serialEvent5(void);
void USART2_Baud_Set(void);
void UART5_Baud_Set(void);
void USART2proc(void);
void USART3proc(void);
void UART5proc(void);

void GPIOInit(void);
void ANT_Control(uint8_t inSignal);

uint8_t gpsParse(char *inData);
uint8_t gpsProcess(void);

void LoRa_PowerControl(uint8_t inSignal);
void LoRa_Reset(void);
void LoRa_WakeUp(void);
void LoRa_DataInit(void);
uint8_t LoRa_CheckSum(uint8_t *inData);
bool LoRa_AT_Command(char *cmd);
uint8_t LoRa_retDataCompare(char *inData);
void LoRa_DataProcess(void);
uint8_t *LoRa_sndDataChange(uint8_t *inData);

uint8_t batteryPowLevel(void);

//void onReceive(int packetSize);

void GPS_PowerControl(uint8_t inSignal);
void GPS_Reset(void);
void GPS_PMTK_CMD_HOT_START(void);
void GPS_PMTK_CMD_Stop(void);
void GPS_PMTK_CMD_Sleep(void);
void GPS_PMTK_TEST_FINISH(void);
void GPS_PMTK_TEST(void);
void GPS_PMTK_DT_DATA_PORT(void);
void GPS_PMTK_SET_NMEA_BAUDRATE(void);

void WDT_Clear(void);

unsigned int ascii_to_hex(const char* str, size_t size, uint8_t* hex);
bool hexCheck(char *str);
uint8_t atoh(char *str);

#endif // _INCLUDE_APP_H_
