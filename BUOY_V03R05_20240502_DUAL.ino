#include <STM32LowPower.h>
#include <low_power.h>

/***********************************************************************************************************************
* DISCLAIMER
* This software is created by RCN Corporation and is only intended for use with BUOY products.
* No other uses are authorized. This software is owned by RCN Corporation and is protected under all
* applicable laws, including copyright laws. 
*
* Copyright (C) 2023 RCN. Co., Ltd. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : BUOY_xxx.ino
* Version      : CodeGenerator for STM32L152RE V0.0.001 [10 Nov 2023]
* Device(s)    : STM32L152RET6
* Tool-Chain   : Arduino
* Description  : UL Message Sequecns Number
* Creation Date: 2024-01-10
***********************************************************************************************************************/

#include "include_app.h"
#include "time.h"

t_BuoyPacket  buoyDat[2];
uint8_t buoyDatCnt = 0;

bool strComplete2 = false;
bool strComplete3 = false;
bool stringComplete5 = false;

#define UART2_INPUT_BUFFER_SIZE   256
#define UART3_INPUT_BUFFER_SIZE   256
#define UART5_INPUT_BUFFER_SIZE   256

char inStr2[UART2_INPUT_BUFFER_SIZE] = {0x00, };
char inStr2Parse[16][16];
uint8_t gpsIndex[32];

char inStr3[UART3_INPUT_BUFFER_SIZE] = {0x00, };

char inStr5[UART5_INPUT_BUFFER_SIZE] = {0x00, };
char inStr5Cp[UART5_INPUT_BUFFER_SIZE] = {0x00, };
uint8_t inputCnt2 = 0, inputCntCp2 = 0, tC1, tC2;
uint8_t inputCnt3 = 0;
uint8_t inputCnt5 = 0, inputCntCp5 = 0;

uint8_t inpParseCnt = 0;

char rdDevEui[16], sndDevEui[8];
uint32_t randomseed=1067859423;//@@adh
uint8_t sktLoraStatus;
t_RunManage cRun;

uint32_t LoRaBaudDat;
uint8_t gpsProcSt;
uint32_t gpsElapsedCount, gpsElapsedtime, gpsInittime, gpsTimeout, gpsBaudDat;
uint16_t gpsDetectCount = 0;

#define WISOL_ANT_SELECT       0
#define RFM95W_ANT_SELECT      1

bool powerOnLora, atCmdMode;

bool buoyInit = true; //@@kdh
// strcpy -> strlcpy @@kdh

/***********************************************************************************************************************
* Function Name: buoyRunInit
* Description  : This function runs after READY.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void buoyRunInit(){
  /******* automatic buoy run @@kdh*******/
  if (buoyInit){
    buoyInit = false;
    // LoRa_AT_Command("buoy run\r\n");
    atCmdMode = false;
    cRun.curRunMode = cmp_1st;
  }
} // end of buoyRunInit

/***********************************************************************************************************************
* Function Name: setup
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void setup()
{
  /****************************************
   * 
   * GPIO initialize
   */

  GPIOInit();
  atCmdMode = true;
  
  // Battery level check
  pinMode(BATTERY_INPUT, INPUT);
  
  /****************************************
   * 
   * initialize serial communication
   */

  Serial3.begin(115200);    // for debug display
  while (!Serial3);

  inputCnt2 = 0;
  memset(inStr2, 0x00, UART2_INPUT_BUFFER_SIZE);
  memset(inStr3, 0x00, UART3_INPUT_BUFFER_SIZE);  
  memset(inStr5, 0x00, UART5_INPUT_BUFFER_SIZE);

  Serial3.println("\r\n=======================");
  Serial3.println(" BUOY Device Ver.0.1.0");
  Serial3.println(" by RCN Co., LTD.");
  Serial3.println(" LoRa Module : SEONGJI LOM202A00 v2.04");

  Serial3.print(" The MCU Core is "); Serial3.println(BOARD_NAME);
  Serial3.print(" EEPROM Size is "); Serial3.println(EEPROM.length());
  Serial3.println("=======================");
  Serial3.print(" ");
  Serial3.print(__DATE__);
  Serial3.print(" ");
  Serial3.println(__TIME__);
  Serial3.println("_______________________");

  /* For Battery value */
  analogReadResolution(12);

  delay(100);

  /****************************************
   * 
   * Lowpower initialize
   */
  // Configure low power
  #ifdef _LOWPOWER_ON_
    LowPower.begin();
  #endif

  /****************************************
   * 
   * Global variables and functions
   */
  memset(rcvLoRa, 0x00, sizeof(rcvLoRa));

  inpParseCnt = 0;

  retryCnt = 0;

  currentMillis = 0;
  previousMillis = 0;
  previousMillis2 = 0;

  LoRa_DataInit();
  buoyDat[0].detail.buoyId.sequenceNo         = 0;

  delay(2000);
  if (batteryPowLevel() > 1) { }
  else {Serial3.println("Low battery!");}

  cRun.runSt.b.st_gps_rcv = false;
  gpsProcSt = 0;
  memset(gpsIndex, 0x00, sizeof(gpsIndex));
  memset(gpsGGA.msgId, 0x00, sizeof(t_GPS_GGA_DataFormat));
  memset(gpsRMC.msgId, 0x00, sizeof(t_GPS_RMC_DataFormat));

  LoRaApp[0] = (t_DebugMsg_TYPE *)LoRaDebugMsg;
  LoRaAppReal = LoRaApp[0];

  LoRaCID[0] = (t_SKTLoRaCID_TYPE *)CID;
  LoRaCIDReal = LoRaCID[0];

  /* for SKT LoRa Baudrate */
  LoRaBaudDat =	 (int)(EEPROM.read(ADDR_LORA_BAUD  )<<24);
  LoRaBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+1)<<16);
  LoRaBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+2)<<8);
  LoRaBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+3));
  
  if ( 
    (LoRaBaudDat == 9600) ||
    (LoRaBaudDat == 19200) ||
    (LoRaBaudDat == 38400) ||
    (LoRaBaudDat == 57600) ||
    (LoRaBaudDat == 115200) ||
    (LoRaBaudDat == 230400)
    )
  {
    Serial3.printf("LoRa Buadrate: %d bps", LoRaBaudDat);
    Serial3.println("");
  }
  else
  {
    Serial3.printf("[EEPROM: %d] LoRa default Buadrate: %d bps", LoRaBaudDat, _UART5_BAUDRATE_);
    Serial3.println("");
    LoRaBaudDat = _UART5_BAUDRATE_;
  }
  Serial5.begin(LoRaBaudDat);     // for SKT LoRa
  while (!Serial5);

  /* for GPS Baudrate */
  gpsBaudDat =	(int)(EEPROM.read(ADDR_GPS_BAUD  )<<24);
  gpsBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+1)<<16);
  gpsBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+2)<<8);
  gpsBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+3));
  
  if ( 
    (gpsBaudDat == 9600) ||
    (gpsBaudDat == 19200) ||
    (gpsBaudDat == 38400) ||
    (gpsBaudDat == 57600) ||
    (gpsBaudDat == 115200) ||
    (gpsBaudDat == 230400)
    )
  {
    Serial3.printf("GPS Buadrate: %d bps", gpsBaudDat);
    Serial3.println("");
  }
  else
  {
    Serial3.printf("[EEPROM: %d bps] GPS default Buadrate: %d bps", gpsBaudDat, _USART2_BAUDRATE_);
    Serial3.println("");
    gpsBaudDat = _USART2_BAUDRATE_;
  }

  /* for GPS TIMEOUT */
  gpsTimeout =	(int)(EEPROM.read(ADDR_GPS_TIMEOUT	)<<24);
  gpsTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+1)<<16);
  gpsTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+2)<<8);
  gpsTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+3));

  if (gpsTimeout < TIME_30SEC) 
  {
    Serial3.printf("[EEPROM: %d ms] GPS default Timeout: %d ms", gpsTimeout, TIME_30SEC);
    Serial3.println("");

    gpsTimeout = TIME_30SEC;
  }
  else 
  {
	  Serial3.printf("GPS Timeout: %d ms", gpsTimeout);
    Serial3.println("");
  }

  sktLoraStatus = 0;

  powerOnLora  = true;

  ANT_Control(WISOL_ANT_SELECT);
  LoRa_PowerControl(true);
  LoRa_Reset();

  
  // LoRa_WakeUp();
  #if 0
    randomseed^= random(10000); //@@adh
    randomseed+= 5789;
    Serial3.printf("\r\n@@ Random delay before network Join : %d msec. ", randomseed%10000); //@@adh
    delay(randomseed%10000);  //@@adh
  #else
    Serial3.printf("\r\n@@ Random delay before network Join : %d msec. ", 6903); //@@adh
    delay(6903);  //@@adh
  #endif
  
  LoRa_SetActivationMode("abp"); 

  
} // end of setup()

/***********************************************************************************************************************
* Function Name: loop
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void loop()
{
  UART5proc();
  USART3proc();
  USART2proc();

  switch(cRun.curRunMode)
  {
    case cmp_idle:
      break;

    case cmp_1st:
      cRun.curRunMode = cmp_2nd;

	  break;

    case cmp_2nd:
      batteryPowLevel();
      #if _Include_Serial3_
        Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.print("[Batt: ");
        Serial3.print(calBattery);
        Serial3.print(": ");
        Serial3.print(calculationV);
        Serial3.println(" V]");
      #endif
      if (!cRun.runSt.b.st_activation_mode){ // abp mode
        cRun.curRunMode = cmp_run;
        break;
      }
      cRun.curRunMode = cmp_3rd;
      gpsDetectCount = 0;

      memset(gpsGGA.msgId, 0x00, sizeof(t_GPS_GGA_DataFormat));
      memset(gpsRMC.msgId, 0x00, sizeof(t_GPS_RMC_DataFormat));
      break;

    case cmp_3rd:
      Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.println("GPS Process.");
      inputCnt2 = 0;

      Serial2.begin(gpsBaudDat);
      while (!Serial2);

      Serial2.println("$");

      // cRun.runSt.b.st_gps_mon = true; //@@kdh
      gpsProcSt = 1;
      gpsElapsedtime = 0;
      gpsInittime = 0;

      switch (gpsProcess())
      {
        case 1:
          gpsDetectCount++;
          break;

        case 2:
          Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.println("Received message: $PMTK010,001*2E");
          break;

        default:
          break;
      }

      cRun.gpsProcMillis = millis();
      cRun.curRunMode = cmp_run;
      break;

    case cmp_4th:

      // Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.print("LoRa Reset: "); Serial3.println(cRun.runSt.b.st_lora_reset);
      

      // Serial5.begin(LoRaBaudDat);	  // for SKT LoRa
      // while (!Serial5);
	  
      // ANT_Control(WISOL_ANT_SELECT);
      // LoRa_PowerControl(true);
      // LoRa_Reset();
      // LoRa_WakeUp();
      cRun.curRunMode = cmp_5th;
	    break;


    case cmp_5th:
      int sameRssiIndex[8], minrssi, mincount, selectIndex;
      minrssi = 0;
      mincount = 0;
      memset(sameRssiIndex, 0xFF, 8);

      LoRa_DataProcess();
      cRun.curRunMode = cmp_6th;

      break;


    case cmp_6th:
      ANT_Control(WISOL_ANT_SELECT);
      LoRa_WakeUp();
      delay(1);
      LoRa_TXbinary(0x01, 0x46, 31, sndData[0]);
      buoyDat[0].detail.buoyId.sequenceNo++;
      if (!cRun.runSt.b.st_activation_mode){ // abp mode
        LoRa_SetActivationMode("otaa");
      }else { // otaa mode
        LoRa_SetActivationMode("abp");
      }
      cRun.curRunMode = cmp_run;
      break;

    case cmp_run:
      break;

    default:
      break;
  }

  if ( (gpsProcSt == 1) )
  {
    if (cRun.runSt.b.st_gps_rcv == true)
    {
      cRun.runSt.b.st_gps_rcv = false;
      if (cRun.runSt.b.st_gps_mon == false) {Serial3.write('.');}

      switch (gpsProcess())
      {
        case 1:
          gpsDetectCount++;
          if (gpsDetectCount > 3)
          {
            gpsProcSt = 0;

            Serial3.println("");
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.day);
            Serial3.print('/');
            Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.month);
            Serial3.print('/');
            Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.year);
            Serial3.print(' ');
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.hour);
            Serial3.print(':');
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.minute);
            Serial3.print(':');
            Serial3.println(buoyDat[0].detail.timeInfo.second.b.sec);

            Serial3.print("[");Serial3.print(__LINE__);Serial3.print(":");Serial3.print(gpsProcSt);Serial3.println("]GPS Found!");
            #if _GPS_PMTK_CMD_Sleep_
              GPS_PMTK_CMD_Sleep();
            #endif
            #if _GPS_PMTK_CMD_Stop_
              GPS_PMTK_CMD_Stop();
            #endif

            cRun.runSt.b.st_gps_mon = false;
            Serial2.end();
            gpsElapsedtime = 0xFFFFF000;
            cRun.curRunMode = cmp_4th;
          }
          break;
    
        case 2:
          gpsInittime++;
          break;
    
        default:
          if ( ((millis() - cRun.gpsProcMillis) >= gpsTimeout) )
          {
            gpsProcSt = 0;

            Serial3.println("");
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.day);
            Serial3.print('/');
            Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.month);
            Serial3.print('/');
            Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.year);
            Serial3.print(' ');
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.hour);
            Serial3.print(':');
            Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.minute);
            Serial3.print(':');
            Serial3.println(buoyDat[0].detail.timeInfo.second.b.sec);  

            Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.printf("[Not found the GPS: %d ms]\r\n", gpsTimeout);Serial3.println("");

            #if _GPS_PMTK_CMD_Sleep_
              GPS_PMTK_CMD_Sleep();
            #endif
            #if _GPS_PMTK_CMD_Stop_
              GPS_PMTK_CMD_Stop();
            #endif
            cRun.runSt.b.st_gps_mon = false;
            Serial2.end();
            gpsElapsedtime = 0xFFFFF000;
            cRun.curRunMode = cmp_4th;
          }
          break;
      }// end of switch(gpsProcess()) ~ case
    }
    else
    {
      if ( ((millis() - cRun.gpsProcMillis) >= gpsTimeout) )
      {
        gpsProcSt = 0;

        Serial3.println("");
        Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.day);
        Serial3.print('/');
        Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.month);
        Serial3.print('/');
        Serial3.print(buoyDat[0].detail.timeInfo.yearMon.b.year);
        Serial3.print(' ');
        Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.hour);
        Serial3.print(':');
        Serial3.print(buoyDat[0].detail.timeInfo.dateHourMin.b.minute);
        Serial3.print(':');
        Serial3.println(buoyDat[0].detail.timeInfo.second.b.sec);	

        Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.printf("[Not found the GPS: %d ms]\r\n", gpsTimeout);Serial3.println("");

        #if _GPS_PMTK_CMD_Sleep_
          GPS_PMTK_CMD_Sleep();
        #endif
        #if _GPS_PMTK_CMD_Stop_
          GPS_PMTK_CMD_Stop();
        #endif
        cRun.runSt.b.st_gps_mon = false;
        Serial2.end();
        gpsElapsedtime = 0xFFFFF000;
        cRun.curRunMode = cmp_4th;
      }
    }

    gpsElapsedtime++;
  }// end of if ((gpsProcSt == true) && (gpsElapsedtime < GPS_ELAPSED_COUNT) )

  if ( (millis() - cRun.joinProcMillis >= TIME_2MIN) && (cRun.runSt.b.st_powerOn == true)&& (cRun.runSt.b.st_Join_Complete == false) && (cRun.runSt.b.st_ready == false) )
  {
    cRun.joinProcMillis = millis();
    #if 1
        LoRa_Reset(); //@@kdh
        // LoRa_WakeUp();
        // cRun.curRunMode=cmp_5th;
    #else
        LoRa_SystemSoftwareReset();
    #endif
    Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");Serial3.println("LoRa Reset");
  }

  if( cRun.runSt.b.st_lora_reset == true ) 
  {
    if (millis() - cRun.resetProcMillis >= TIME_3SEC)
    {
      cRun.runSt.b.st_lora_reset = false;
      digitalWrite(PA5, LOW);
      delay(5);
      LoRa_WakeUp();
    }
  }// if( cRun.runSt.b.st_lora_reset == true )

  if ( (cRun.runSt.b.st_busy == true) && (cRun.curRunMode == cmp_run) )
  {
    if ( (millis() - cRun.busyProcMillis ) > TIME_30SEC )
    {
      cRun.runSt.b.st_busy = false;
    }
  }

  if (cRun.runSt.b.st_finish == true)
  {
    if ( (millis() - cRun.finishMillis) > 30000 ) // TIME_2MIN
    {
      cRun.runSt.b.st_finish = false;

      // Serial5.begin(LoRaBaudDat);	  // for SKT LoRa
      // while (!Serial5);

      if (cRun.curRunMode != cmp_stop)
      {
        Serial3.println("");

        LoRa_DataInit();

        cRun.curRunMode = cmp_1st;
      }
      else
      {
      }
    }
  }
}// End of loop()


/*********************************************
 * Serial Event
 *********************************************/
#if _Include_Serial2_
void serialEvent2() {
  while (Serial2.available()) { 
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    inStr2[inputCnt2++] = inChar;

    currentMillis = millis();
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      strComplete2 = true;
    }
  }
}
#endif

#if _Include_Serial3_
void serialEvent3() {
  while (Serial3.available()) {

    // get the new byte:
    char inChar = (char)Serial3.read();
    Serial3.print(inChar);

    // add it to the inputString:
    if (inChar == 0x08)
    {
      if (inputCnt3 > 0)
      {
        Serial3.print(" \b");
        Serial3.print(inChar);
        inStr3[inputCnt3--] = 0x00;
      }
    }
    else
    {
      inStr3[inputCnt3++] = inChar;
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      strComplete3 = true;
    }
  }
}
#endif

#if _Include_Serial5_
void serialEvent5()
{
  while (Serial5.available()) {
    // get the new byte:
    char inChar = (char)Serial5.read();
    Serial3.print(inChar);

    // add it to the inputString:
    inStr5[inputCnt5++] = inChar;
    // if the incoming character is a newline
    if (inChar == '\n') {
      inStr5[inputCnt5] = '\0'; // null-terminate the string
      // Check if the string contains the time sync message
      if (strstr(inStr5, "ABP") != NULL) {
        cRun.runSt.b.st_activation_mode = 0;
      }else if (strstr(inStr5, "OTAA") != NULL) {
        cRun.runSt.b.st_activation_mode = 1;
      }
      stringComplete5 = true;
    }
  }
}
#endif

/*********************************************
 * Serial Baudrate Set
 *********************************************/
void USART2_Baud_Set(void)
{
  int S2baudrate;

  S2baudrate =  EEPROM.read(ADDR_GPS_BAUD)   << 24;
  S2baudrate |= EEPROM.read(ADDR_GPS_BAUD+1) << 16;
  S2baudrate |= EEPROM.read(ADDR_GPS_BAUD+2) << 8;
  S2baudrate |= EEPROM.read(ADDR_GPS_BAUD+3);

  #if __SERIAL3_DEBUG__
  Serial3.printf("RD EEPROM for GPS BAUD: %d", S2baudrate);
  Serial3.println("");
  #endif

  if ( 
      (S2baudrate == 9600) ||
      (S2baudrate == 19200) ||
      (S2baudrate == 38400) ||
      (S2baudrate == 57600) ||
      (S2baudrate == 115200) ||
      (S2baudrate == 230400)
      )
  {
    gpsBaudDat = S2baudrate;
  }
  else
  {
    gpsBaudDat = _USART2_BAUDRATE_;
  }
  Serial2.begin(gpsBaudDat);   // for GPS
  while (!Serial2);
}

void UART5_Baud_Set(void)
{
  int S5baudrate;

  S5baudrate =  EEPROM.read(ADDR_LORA_BAUD)   << 24;
  S5baudrate |= EEPROM.read(ADDR_LORA_BAUD+1) << 16;
  S5baudrate |= EEPROM.read(ADDR_LORA_BAUD+2) << 8;
  S5baudrate |= EEPROM.read(ADDR_LORA_BAUD+3);

  #if __SERIAL3_DEBUG__
  Serial3.printf("RD EEPROM for LoRa BAUD: %d", S5baudrate);
  Serial3.println("");
  #endif

  if ( 
      (S5baudrate == 9600) ||
      (S5baudrate == 19200) ||
      (S5baudrate == 38400) ||
      (S5baudrate == 57600) ||
      (S5baudrate == 115200) ||
      (S5baudrate == 230400)
      )
  {
    LoRaBaudDat = S5baudrate;
  }
  else
  {
    LoRaBaudDat = _UART5_BAUDRATE_;
  }
  // Serial5.begin(LoRaBaudDat);   // for LoRa
  // while (!Serial5);
}

/*********************************************
 * Serial Precess
 *********************************************/
#if _Include_Serial2_
void USART2proc(void)
{
  if (strComplete2) {
    strComplete2 = false;
    if (cRun.runSt.b.st_gps_mon == true){Serial3.print(inStr2);}
    tC2 = gpsParse(inStr2);

    // clear the string:
    inputCnt2 = 0;
    memset(inStr2, 0x00, UART2_INPUT_BUFFER_SIZE);

    if ((tC2 == 1) || (tC2 == 2))
    {
      cRun.runSt.b.st_gps_rcv=true;
    }
  }
}
#endif
  
#if _Include_Serial3_
void USART3proc(void)
{
  serialEvent3();
  
  if (strComplete3) {
    strComplete3 = false;
    Serial3.print(">> ");

    if (LoRa_AT_Command(inStr3)){}
    else {}
    // clear the string:
    inputCnt3 = 0;
    memset (inStr3, 0x00, UART3_INPUT_BUFFER_SIZE);
  }
}
#endif

#if _Include_Serial5_
void UART5proc(void)
{
  serialEvent5();
  if (stringComplete5) {
    stringComplete5 = false;

    inputCntCp5 = inputCnt5;
    strcpy (inStr5Cp, inStr5);
    inStr5Cp[inputCntCp5] = '\0';

    if ( LoRa_retDataCompare(inStr5Cp) ){}
    else{}

    // clear the string:
    inputCnt5 = 0;
    memset (inStr5, 0x00, UART5_INPUT_BUFFER_SIZE);
  } // if (stringComplete5)
}
#endif

/***********************************************************************************************************************
* Function Name: GPIOInit
* Description  : This function implements GPIO initialize function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void GPIOInit(void)
{
#if _LED_PC2_DISPLAY_
  pinMode(LED_PC2, OUTPUT);   // BAT_LVL_LED
  digitalWrite(LED_PC2, HIGH);
#endif

#if _SKT_LORA_EXECUTE_
  pinMode(PA5, OUTPUT);       // LoRa_NRST          
  pinMode(PA7, OUTPUT);       // LoRa_WKUP     
//  pinMode(PA6, OUTPUT);     // LoRa_Sleep_EN
  pinMode(PC13, OUTPUT);      // LoRa_PWR_CTRL

#else   // _SKT_LORA_EXECUTE_
  pinMode(PC13, OUTPUT);      // LoRa_PWR_CTRL
  digitalWrite(PC13, HIGH);   // LoRa_PWR_CTRL --> OFF
#endif  // _SKT_LORA_EXECUTE_

  pinMode(RF_SWITCH_CTRL, OUTPUT);
}

void ANT_Control(uint8_t inSignal)
{
//  Serial3.print("ANT_Control ");
  if (inSignal == true)
  {
    digitalWrite(RF_SWITCH_CTRL, LOW);	// LoRa_PWR_CTRL --> ON
//    Serial3.println("On");
  }
  else
  {
    digitalWrite(RF_SWITCH_CTRL, HIGH);	// LoRa_PWR_CTRL --> OFF
//    Serial3.println("Off");
  }
}

/***********************************************************************************************************************
 * Part : GPS
 * Function Name: 
 * Description  : 
 * Arguments    : None
 * Return Value : None
***********************************************************************************************************************/
uint8_t gpsParse(char *inData)
{
  uint8_t index;
  int strLen;

  index = 0;
  tC1 = 0;
  memset(gpsIndex, 0x00, sizeof(gpsIndex));
  memset(inStr2Parse, 0x00, sizeof(inStr2Parse));

  strLen = strlen(inData);
  for(int ii=0; ii<strLen; ii++)
  {
    if(inData[ii] == ',')
    {
      gpsIndex[index++] = ii;
    }
  }

  if (memcmp(inData+3, "GGA", 3) == 0) 
  {
    tC1 = 1;
    memcpy (gpsGGA.msgId         , inData            , gpsIndex[0]);
    #if _DEBUG_GPS_
      Serial3.printf("[0]%s, ",gpsGGA.msgId);
    #endif
    if ((gpsIndex[1]-gpsIndex[0]) > 1) {
      memcpy (gpsGGA.utcTime       , inData+gpsIndex[0]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[1]%s, ",gpsGGA.utcTime);
      #endif
    }
    if ((gpsIndex[2]-gpsIndex[1]) > 1) {
      memcpy (gpsGGA.latitude      , inData+gpsIndex[1]+1, gpsIndex[2]-(gpsIndex[1]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[2]%s, ",gpsGGA.latitude);
      #endif
    }
    if ((gpsIndex[3]-gpsIndex[2]) > 1) {
      gpsGGA.N_S_indicator = inData[gpsIndex[2]+1]; 
      #if _DEBUG_GPS_
        Serial3.printf("[3]%c, ",gpsGGA.N_S_indicator);
      #endif
    }
    if ((gpsIndex[4]-gpsIndex[3]) > 1) {
      memcpy (gpsGGA.longitude     , inData+gpsIndex[3]+1, gpsIndex[4]-(gpsIndex[3]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[4]%s, ",gpsGGA.longitude);
      #endif
    }
    if ((gpsIndex[5]-gpsIndex[4]) > 1) {
      gpsGGA.E_W_indicator = inData[gpsIndex[4]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[5]%c, ",gpsGGA.E_W_indicator);
      #endif
    }
    if ((gpsIndex[6]-gpsIndex[5]) > 1) {
      gpsGGA.positionFix = inData[gpsIndex[5]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[6]%c, ",gpsGGA.positionFix);
      #endif
    }
    if ((gpsIndex[7]-gpsIndex[6]) > 1) {
      memcpy (gpsGGA.usedSatellites, inData+gpsIndex[6]+1, gpsIndex[7]-(gpsIndex[6]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[7]%s, ",gpsGGA.usedSatellites);
      #endif
    }
    if ((gpsIndex[8]-gpsIndex[7]) > 1) {
       memcpy (gpsGGA.HDOP          , inData+gpsIndex[7]+1, gpsIndex[8]-(gpsIndex[7]+1));
        #if _DEBUG_GPS_
          Serial3.printf("[8]%s, ",gpsGGA.HDOP);
        #endif
     }
    if ((gpsIndex[9]-gpsIndex[8]) > 1) {
      memcpy (gpsGGA.mslAltitude   , inData+gpsIndex[8]+1, gpsIndex[9]-(gpsIndex[8]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[9]%s, ",gpsGGA.mslAltitude);
      #endif
    }
    if ((gpsIndex[10]-gpsIndex[9]) > 1) {
      gpsGGA.uints1 = inData[gpsIndex[9]+1]; }
      #if _DEBUG_GPS_
        Serial3.printf("[10]%c, ",gpsGGA.uints1);
      #endif
    if ((gpsIndex[11]-gpsIndex[10]) > 1) {
      memcpy (gpsGGA.geoidalSeparation, inData+gpsIndex[10]+1, gpsIndex[11]-(gpsIndex[10]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[11]%s, ",gpsGGA.geoidalSeparation);
      #endif
    }
    if ((gpsIndex[12]-gpsIndex[11]) > 1) {
      gpsGGA.uints2 = inData[gpsIndex[11]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[12]%c, ",gpsGGA.uints2);
      #endif
    }
    if ((gpsIndex[13]-gpsIndex[12]) > 1) {
      gpsGGA.AgeOfDiff = inData[gpsIndex[12]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[13]%c, ",gpsGGA.AgeOfDiff);
      #endif
    }
    memcpy (gpsGGA.checksum    , inData+gpsIndex[13]+1, 3);
    #if _DEBUG_GPS_
      Serial3.printf("[14]%s\r\n",gpsGGA.checksum);
    #endif
  }

  if (memcmp(inData+3, "RMC", 3) == 0) 
  {
    tC1 = 2;
    memcpy (gpsRMC.msgId, inData, gpsIndex[0]);
    #if _DEBUG_GPS_
      Serial3.printf("[0]%s, ",gpsRMC.msgId);
    #endif
    if ((gpsIndex[1]-gpsIndex[0]) > 1) {
      memcpy (gpsRMC.utcTime, inData+gpsIndex[0]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[1]%s, ",gpsRMC.utcTime);
      #endif
    }
    if ((gpsIndex[2]-gpsIndex[1]) > 1) {
      gpsRMC.status = inData[gpsIndex[1]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[2]%c, ",gpsRMC.status);
      #endif
    }
    if ((gpsIndex[3]-gpsIndex[2]) > 1) {
      memcpy (gpsRMC.latitude, inData+gpsIndex[2]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[3]%s, ",gpsRMC.latitude);
      #endif
    }
    if ((gpsIndex[4]-gpsIndex[3]) > 1) {
      gpsRMC.N_S_indicator = inData[gpsIndex[3]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[4]%c, ",gpsRMC.N_S_indicator);
      #endif
    }
    if ((gpsIndex[5]-gpsIndex[4]) > 1) {
      memcpy (gpsRMC.longitude, inData+gpsIndex[4]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[5]%s, ",gpsRMC.longitude);
      #endif
    }
    if ((gpsIndex[6]-gpsIndex[5]) > 1) {
      gpsRMC.E_W_indicator = inData[gpsIndex[5]+1];
      #if _DEBUG_GPS_
        Serial3.printf("[6]%c, ",gpsRMC.E_W_indicator);
      #endif
    }
    if ((gpsIndex[7]-gpsIndex[6]) > 1) {
      memcpy (gpsRMC.speed, inData+gpsIndex[6]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[7]%s, ",gpsRMC.speed);
      #endif
    }
    if ((gpsIndex[8]-gpsIndex[7]) > 1) {
      memcpy (gpsRMC.course, inData+gpsIndex[7]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[8]%s, ",gpsRMC.course);
      #endif
    }
    if ((gpsIndex[9]-gpsIndex[8]) > 1) {
      memcpy (gpsRMC.date, inData+gpsIndex[8]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[9]%s, ",gpsRMC.date);
      #endif
    }
    if ((gpsIndex[10]-gpsIndex[9]) > 1) {
      memcpy (gpsRMC.magnetic, inData+gpsIndex[9]+1, gpsIndex[1]-(gpsIndex[0]+1));
      #if _DEBUG_GPS_
        Serial3.printf("[10]%s, ",gpsRMC.magnetic);
      #endif
      if ((gpsIndex[11]-gpsIndex[10]) > 1) {
        gpsRMC.E_W_indicator2 = inData[gpsIndex[10]+1];
        #if _DEBUG_GPS_
          Serial3.printf("[11]%c, ",gpsRMC.E_W_indicator2);
        #endif
      }
      if ((gpsIndex[12]-gpsIndex[11]) > 1) {
        gpsRMC.mode = inData[gpsIndex[11]+1];
        #if _DEBUG_GPS_
          Serial3.printf("[12]%c, ",gpsRMC.mode);
        #endif
      }    
      memcpy (gpsRMC.checksum, inData+gpsIndex[12]+1, 3);
      #if _DEBUG_GPS_
        Serial3.printf("[13]%s\r\n",gpsRMC.checksum);
      #endif
    }
    else {
      if ((gpsIndex[11]-gpsIndex[10]) > 1) {
        gpsRMC.mode = inData[gpsIndex[10]+1];
        #if _DEBUG_GPS_
          Serial3.printf("[11]%c, ",gpsRMC.mode);
        #endif
      }	 
      memcpy (gpsRMC.checksum, inData+gpsIndex[11]+1, 3);
      #if _DEBUG_GPS_
        Serial3.printf("[12]%s\r\n",gpsRMC.checksum);
      #endif
    }
  }
  else 
  {

  }

  return tC1;
}

uint8_t gpsProcess(void) // run over and over again
{
  char str[4][16];
  uint8_t ret = 0;

  buoyDat[0].detail.timeInfo.yearMon.b.year = 2000+((gpsRMC.date[4]&0x0F)*10)+(gpsRMC.date[5]&0x0F);
  buoyDat[0].detail.timeInfo.yearMon.b.month = ((gpsRMC.date[2]&0x01)*10)+(gpsRMC.date[3]&0x0F);
  buoyDat[0].detail.timeInfo.dateHourMin.b.day = ((gpsRMC.date[0]&0x03)*10)+(gpsRMC.date[1]&0x0F);;
  buoyDat[0].detail.timeInfo.dateHourMin.b.hour = ((gpsRMC.utcTime[0]&0x03)*10)+(gpsRMC.utcTime[1]&0x0F);
  buoyDat[0].detail.timeInfo.dateHourMin.b.minute = ((gpsRMC.utcTime[2]&0x0F)*10)+(gpsRMC.utcTime[3]&0x0F);
  buoyDat[0].detail.timeInfo.second.b.sec = ((gpsRMC.utcTime[4]&0x0F)*10)+(gpsRMC.utcTime[5]&0x0F);
  buoyDat[0].detail.timeInfo.second.b.rsvd = 0;

  buoyDat[0].detail.gpsSt.b.decimalPoint = 4;

  if ( (gpsGGA.positionFix > '0') || (gpsRMC.status == 'A') )
  {
    ret = 1;
    buoyDat[0].detail.buoySt.b.gnssModule = 0;
    Serial3.print("Location: ");
    buoyDat[0].detail.gpsSt.b.lockSt = 1;
    if(gpsRMC.N_S_indicator == 'N') {buoyDat[0].detail.gpsSt.b.latitudePos = 0;} else {buoyDat[0].detail.gpsSt.b.latitudePos = 1;}
    if(gpsRMC.E_W_indicator == 'E') {buoyDat[0].detail.gpsSt.b.longitudePos = 0;} else {buoyDat[0].detail.gpsSt.b.longitudePos = 1;}
    buoyDat[0].detail.gpsSt.b.decimalPoint = 4;  // Ascen Korea's GPS module
    buoyDat[0].detail.gpsSt.b.rsvd = 0;
    switch (buoyDat[0].detail.gpsSt.b.decimalPoint)
    {
      case 4:
        memset (str, 0x00, sizeof (str));
        memcpy (str[0], gpsRMC.latitude, 4);
        memcpy (str[1], gpsRMC.latitude+5, 4);
        memcpy (str[2], gpsRMC.longitude, 5);
        memcpy (str[3], gpsRMC.longitude+6, 4);
        buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal = atoi(str[0]);
        buoyDat[0].detail.gpsInfo.latitudeInfo.b.point   = atoi(str[1]);
        buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal = atoi(str[2]);
        buoyDat[0].detail.gpsInfo.longitueInfo.b.point   = atoi(str[3]);
        break;

      case 5:
        memset (str, 0x00, sizeof (str));
        memcpy (str[0], gpsRMC.latitude, 4);
        memcpy (str[1], gpsRMC.latitude+5, 5);
        memcpy (str[2], gpsRMC.longitude, 5);
        memcpy (str[3], gpsRMC.longitude+6, 5);
        buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal = atoi(str[0]);
        buoyDat[0].detail.gpsInfo.latitudeInfo.b.point   = atoi(str[1]);
        buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal = atoi(str[2]);
        buoyDat[0].detail.gpsInfo.longitueInfo.b.point   = atoi(str[3]);
        break;
	    
      default:
        break;
    }
    Serial3.print(buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal);Serial3.print(".");Serial3.print(buoyDat[0].detail.gpsInfo.latitudeInfo.b.point);
    Serial3.printf(", %c, ", gpsRMC.N_S_indicator);

    Serial3.print(buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal);Serial3.print(".");Serial3.print(buoyDat[0].detail.gpsInfo.longitueInfo.b.point);
    Serial3.printf(", %c\r\n", gpsRMC.E_W_indicator);
    Serial3.println("");
  }
  else
  {
    buoyDat[0].detail.buoySt.b.gnssModule = 1;
  }

  return ret;
}


/***********************************************************************************************************************
 * Part : SKT-LoRa
 * Function Name: LoRa_PowerControl
 * Description	: This function implements for Power control of SKT LoRa.
 * Arguments	: Power Off -> 0 , Power On -> 1
 * Return Value : None
 ***********************************************************************************************************************/
void LoRa_PowerControl(uint8_t inSignal)
{
  if (inSignal == true)
  {
    digitalWrite(PC13, LOW);	// LoRa_PWR_CTRL --> ON
    cRun.runSt.b.st_powerOn = true;
  }
  else
  {
    digitalWrite(PC13, HIGH);	// LoRa_PWR_CTRL --> OFF
    cRun.runSt.b.st_powerOn = false;
  }
}

void LoRa_Reset(void)
{
  digitalWrite(PA5, LOW);
  delay(5);
  digitalWrite(PA5, HIGH);
  cRun.runSt.b.st_lora_reset = true;
  cRun.resetProcMillis = millis();
}

void LoRa_WakeUp(void)
{
  digitalWrite(PA7, LOW); 	// LoRa_WKUP 
  delay(5);
  digitalWrite(PA7, HIGH);
  delay(5);
  digitalWrite(PA7, LOW);
}

void LoRa_DataInit(void)
{
  buoyDat[0].detail.sof                       = 0xaa;

  buoyDat[0].detail.buoyId.msgId              = 0x31;
//  buoyDat[0].detail.buoyId.sequenceNo         = 0;
  buoyDat[0].detail.buoyId.len                = 0x18;
  memset(buoyDat[0].detail.buoyId.buoyId, 0x00, 8);
  
  buoyDat[0].detail.buoySt.b.activeComm       = 1;
  buoyDat[0].detail.buoySt.b.mainCommModule   = 1;
  buoyDat[0].detail.buoySt.b.pLoRaModule      = 1;
  buoyDat[0].detail.buoySt.b.gnssModule       = 1;
  buoyDat[0].detail.buoySt.b.BuoyPwr          = 0;
  buoyDat[0].detail.buoySt.b.sktLoRaRSSI      = 0;
  buoyDat[0].detail.buoySt.b.pLoRaRSSI        = 0;
  buoyDat[0].detail.buoySt.b.fishingBoatRSSI  = 0;

  buoyDat[0].detail.timeInfo.yearMon.data     = 0;
  buoyDat[0].detail.timeInfo.dateHourMin.data = 0;
  buoyDat[0].detail.timeInfo.second.data      = 0;

  buoyDat[0].detail.gpsSt.data                = 0;
  buoyDat[0].detail.gpsInfo.latitudeInfo.data = 0;
  buoyDat[0].detail.gpsInfo.longitueInfo.data = 0;

  buoyDat[0].detail.checkSum                  = LoRa_CheckSum(buoyDat[0].data);
  buoyDat[0].detail.eof                       = 0xfe;

  cRun.runSt.data = 0;
  cRun.curRunMode = cmp_idle;
  cRun.runSt.b.st_finish = false;
  cRun.runSt.b.st_gps_mon = false;
  cRun.resetProcMillis = millis();
  cRun.joinProcMillis = millis();
}

uint8_t LoRa_CheckSum(uint8_t *inData)
{
  uint8_t cnt, checkSum;

  checkSum=0;
  for (cnt=0; cnt < 30; cnt++)
  {
    checkSum +=inData[cnt];
    if(cnt == 15) Serial3.print("\r\n");
  }
  
  return checkSum;
}

bool LoRa_AT_Command(char *cmd)
{
  uint8_t cmpSt = false;
  bool ret = false;
  char *cpCmd, sndToSKT[256];
  char *ptr, str[64];
  int Index;
  int cnt, len;

  if ( (memcmp (cmd, "at+", 3)==0) || (memcmp (cmd, "AT+", 3)==0) || (memcmp (cmd, "At+", 3)==0) || (memcmp (cmd, "aT+", 3)==0) )
  {
    cpCmd = cmd+3;
    memset (sndToSKT, 0x00, sizeof(sndToSKT));
    strcpy (sndToSKT, cpCmd);

    cnt = 0;
    ptr = strtok(cpCmd, " ");
    if ( (memcmp (ptr, "LRW", 3)==0) || (memcmp (ptr, "lrw", 3)==0) )
    {
      cnt = 1;
      if ( (ptr = strtok(NULL, " ")) != NULL )
      {
        cnt = 2;
        cmpSt = false;
        for (Index=0; LoRaCIDReal[Index].cid != NULL; Index++)
        {
          if (memcmp (LoRaCIDReal[Index].cid, ptr, 2) == 0) {cmpSt = true; break;}
        }

        if(cmpSt)
        {
          switch(Index)
          {
            case 33: // 4D
              if ( (ptr = strtok(NULL, " \r\n")) != NULL )         // check the message type
              {
                txBin.confirm = atoh(ptr);
                if ( (ptr = strtok(NULL, " \r\n")) != NULL )       // check the Fport
                {
                  txBin.Fport = atoh(ptr);
                  if ( (ptr = strtok(NULL, " \r\n")) != NULL )     // check the length
                  {
                    txBin.len = atoh(ptr);
                    if ( (ptr = strtok(NULL, " \r\n")) != NULL )   // Check whether there is input message
                    {
                      bool msgStatus = true;

                      len = 0;
                      while (ptr != NULL)
                      {
                        if (atoh(ptr) < 0x100 )
                        {
                          txBin.msg[len] = atoh(ptr);
                          len++;
                          if(len > sizeof(txBin.msg)){len = 0; Serial3.println("error:input message size over."); msgStatus = false;}
                          ptr = strtok(NULL, " \r\n");
                        }
                        else
                        {
                          ptr = NULL;
                          msgStatus = false;
                          Serial3.println("error:Input value is greater than 0xFF.");
                        }

                      }
                      if (msgStatus == true)
                      {
                        uint8_t *snd;

                        snd = (uint8_t *) txBin.cmd;
                        for (int j=0; j < len+10; j++)
                        {
                          Serial3.printf("%02X ", snd[j]);
                        }
                        Serial3.print("\r\n>> ");

                        #if __SERIAL5_PRINT__
                          // Serial5.begin(LoRaBaudDat); 	// for SKT LoRa
                          // while (!Serial5);

                          ANT_Control(WISOL_ANT_SELECT);
                          LoRa_WakeUp();
                          Serial5.write(snd, len+10);
                          Serial5.print("\r\n");
                        #endif
                        ret = true;
                      }
                    }
                    else{Serial3.println("error:There is no message."); Serial3.print(">> ");}
                  }
                  else{Serial3.println("error:There is no length."); Serial3.print(">> ");}
                }
                else{Serial3.println("error:There is no Fport."); Serial3.print(">> ");}
              }
              else{Serial3.println("error:There is no message type.(Mtype)"); Serial3.print(">> ");}

              cnt = 2;
              break;

            default:
              if (Index == 31) // class 변경 금지(at+lrw 4B)
              {
                Serial3.println("Class type should never be changed");
                break;
              }
              Serial3.print(sndToSKT);
              Serial3.print(">> ");
              #if __SERIAL5_PRINT__
                Serial5.begin(LoRaBaudDat);	  // for SKT LoRa
                while (!Serial5);

                ANT_Control(WISOL_ANT_SELECT);
                LoRa_WakeUp();
                Serial5.print(sndToSKT);
              #endif
              ret = true;
              break;
          }
        } // end of if(cmpSt)
        else{Serial3.println("error:There is no CID."); Serial3.print(">> ");}
      }
      else{Serial3.println("error:CID NULL"); Serial3.print(">> ");}
    }
    else{Serial3.println("error:Input AT command error!"); Serial3.print(">> ");}
  }

  if ( (memcmp (cmd, "buoy", 4)==0) || (memcmp (cmd, "BUOY", 4)==0) || (memcmp (cmd, "Buoy", 4)==0) )
  {
    ptr = strtok(cmd, " ");
    while (ptr != NULL)
    {
      strcpy(str, ptr);
	  ptr = strtok(NULL, " ");
    }
    if ( (memcmp (str, "run", 3)==0) || (memcmp (str, "RUN", 3)==0) || (memcmp (str, "Run", 3)==0) )
    {
      Serial3.println("[Req CMD: BUOY RUNNING!]");
      Serial3.println("");
      atCmdMode = false;
      cRun.curRunMode = cmp_1st;
      ret = true;
    }

    if ( (memcmp (str, "stop", 4)==0) || (memcmp (str, "STOP", 4)==0) || (memcmp (str, "Stop", 4)==0) )
    {
      Serial3.println("[Req CMD: BUOY STOP!]");

	  atCmdMode = true;
      cRun.curRunMode = cmp_stop;
      ret = true;
    }

    if ( (memcmp (str, "pwrsave", 7)==0) || (memcmp (str, "PWRSAVE", 7)==0) || (memcmp (str, "Pwrsave", 7)==0) )
    {
      Serial3.println("[Req CMD: BUOY POWER SAVE!]");
      Serial2.begin(gpsBaudDat);
      while (!Serial2);

      #if _GPS_PMTK_CMD_Sleep_
        GPS_PMTK_CMD_Sleep();
      #endif
      #if _GPS_PMTK_CMD_Stop_
        GPS_PMTK_CMD_Stop();
      #endif
      Serial5.end();
      Serial3.end();
      Serial2.end();

      #if __IDLE__
        digitalWrite(LED_BUILTIN, HIGH);
        LowPower.idle(1000);
        digitalWrite(LED_BUILTIN, LOW);
        LowPower.idle(1000);
      #endif
        
      #if __SLEEP__
        digitalWrite(LED_BUILTIN, HIGH);
        LowPower.sleep(1000);
        digitalWrite(LED_BUILTIN, LOW);
        LowPower.sleep(1000);
      #endif
        
      #if __DEEP_SLEEP__
        digitalWrite(LED_BUILTIN, HIGH);
        LowPower.deepSleep(1000);
        digitalWrite(LED_BUILTIN, LOW);
        LowPower.deepSleep(1000);
      #endif
        
      #if __SHUTDOWN__
        digitalWrite(LED_BUILTIN, HIGH);
        LowPower.shutdown(1000);
        digitalWrite(LED_BUILTIN, LOW);
        LowPower.shutdown(1000);
      #endif
  
      atCmdMode = true;
      cRun.curRunMode = cmp_stop;
      ret = true;
    }

  }

  if ( (memcmp (cmd, "gps", 3)==0) || (memcmp (cmd, "GPS", 3)==0) || (memcmp (cmd, "Gps", 3)==0) )
  {
    if ( (ptr = strtok(cmd, " \r\n")) != NULL )		 // check the message type
    {
      strcpy(str, ptr);
      #if __SERIAL3_DEBUG__
      Serial3.println(str);
      #endif

      if ( (ptr = strtok(NULL, " ")) != NULL )
      {
        strcpy(str, ptr);
        #if __SERIAL3_DEBUG__
        Serial3.println(str);
        #endif

        if ( (memcmp (str, "on", 2)==0) || (memcmp (str, "ON", 3)==0) || (memcmp (str, "On", 3)==0) )
        {
          Serial3.println("[Req CMD: GPS Monitoring On!]");
          inputCnt2 = 0;

          Serial2.begin(gpsBaudDat);
          while (!Serial2);

          Serial2.println("$");
          cRun.runSt.b.st_gps_mon = true;      
          ret = true;
        }

        if ( (memcmp (str, "off", 3)==0) || (memcmp (str, "OFF", 3)==0) || (memcmp (str, "Off", 3)==0) )
        {
          Serial3.println("[Req CMD: GPS Monitoring Off!]");
          Serial2.end();   // for GPS
          cRun.runSt.b.st_gps_mon = false;
          ret = true;
        }

        if ( (memcmp (str, "set", 3)==0) || (memcmp (str, "SET", 3)==0) || (memcmp (str, "Set", 3)==0) )
        {
          if ( (ptr = strtok(NULL, " ")) != NULL )
          {
            strcpy(str, ptr);
            #if __SERIAL3_DEBUG__
            Serial3.println(str);
            #endif

            if ( (memcmp (str, "baud", 4)==0) || (memcmp (str, "BAUD", 4)==0) || (memcmp (str, "Baud", 4)==0) )
            {
              if ( (ptr = strtok(NULL, " ")) != NULL )
              {
                int setBaudDat;
                strcpy(str, ptr);

                setBaudDat = atoi(str);
                EEPROM.update(ADDR_GPS_BAUD,   setBaudDat>>24);
                EEPROM.update(ADDR_GPS_BAUD+1, setBaudDat>>16);
                EEPROM.update(ADDR_GPS_BAUD+2, setBaudDat>>8);
                EEPROM.update(ADDR_GPS_BAUD+3, setBaudDat);

                Serial3.printf("GPS Badurate Set OK: %d bps", setBaudDat);
                Serial3.println("");
                Serial3.print(">> ");

                USART2_Baud_Set();

                ret = true;
              }
            }
            else if ( (memcmp (str, "timeout", 7)==0) || (memcmp (str, "TIMEOUT", 7)==0) || (memcmp (str, "Timeout", 7)==0) )
            {
              if ( (ptr = strtok(NULL, " ")) != NULL )
              {
                int setTimeout;
                strcpy(str, ptr);
                #if __SERIAL3_DEBUG__
                Serial3.println(str);
                #endif

                setTimeout = atoi(str);
                EEPROM.update(ADDR_GPS_TIMEOUT,	  setTimeout>>24);
                EEPROM.update(ADDR_GPS_TIMEOUT+1, setTimeout>>16);
                EEPROM.update(ADDR_GPS_TIMEOUT+2, setTimeout>>8);
                EEPROM.update(ADDR_GPS_TIMEOUT+3, setTimeout);

                if (setTimeout < TIME_30SEC) 
                {
                  gpsTimeout = TIME_30SEC; 
                  Serial3.printf("[EEPROM: %d ms] GPS default Timeout Set: %d ms", setTimeout, gpsTimeout);
                }
                else 
                { 
                  gpsTimeout = setTimeout; 
                  Serial3.printf("GPS Timeout Set OK: %d ms", setTimeout);
                }
                Serial3.println("");
                Serial3.print(">> ");
              }
              else {Serial3.println("error: There is no timeout value."); Serial3.print(">> ");}
            }
            else {Serial3.println("error: Wrong 2nd command of Set!"); Serial3.print(">> ");}
          }
          else {Serial3.println("error: There is no 2nd parameter of Set."); Serial3.print(">> ");}
        }

        if ( (memcmp (str, "get", 3)==0) || (memcmp (str, "GET", 3)==0) || (memcmp (str, "Get", 3)==0) )
        {
          if ( (ptr = strtok(NULL, " ")) != NULL )
          {
            strcpy(str, ptr);
            #if __SERIAL3_DEBUG__
            Serial3.println(str);
            #endif

            if ( (memcmp (str, "baud", 4)==0) || (memcmp (str, "BAUD", 4)==0) || (memcmp (str, "Baud", 4)==0) )
            {
              int getBaudDat;

              getBaudDat =  (int)(EEPROM.read(ADDR_GPS_BAUD  )<<24);
              getBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+1)<<16);
              getBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+2)<<8);
              getBaudDat |= (int)(EEPROM.read(ADDR_GPS_BAUD+3));

              if ( 
                (getBaudDat == 9600) ||
                (getBaudDat == 19200) ||
                (getBaudDat == 38400) ||
                (getBaudDat == 57600) ||
                (getBaudDat == 115200) ||
                (getBaudDat == 230400)
                )
              {
                gpsBaudDat = getBaudDat;
                Serial3.printf("Get the GPS Buadrate: %d bps", getBaudDat);
                Serial3.println("");
                Serial3.print(">> ");
              }
              else
              {
                gpsBaudDat = _USART2_BAUDRATE_;
                Serial3.printf("[EEPROM: %d] Get the GPS default Buadrate: %d bps", getBaudDat, _USART2_BAUDRATE_);
                Serial3.println("");
                Serial3.print(">> ");
              }
            }
            else if ( (memcmp (str, "timeout", 7)==0) || (memcmp (str, "TIMEOUT", 7)==0) || (memcmp (str, "Timeout", 7)==0) )
            {
              int getTimeout;

              getTimeout =  (int)(EEPROM.read(ADDR_GPS_TIMEOUT  )<<24);
              getTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+1)<<16);
              getTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+2)<<8);
              getTimeout |= (int)(EEPROM.read(ADDR_GPS_TIMEOUT+3));

              if (getTimeout < TIME_30SEC) 
              {
                gpsTimeout = TIME_30SEC; 
                Serial3.printf("[EEPROM: %d ms] Get the GPS default Timeout: %d ms", getTimeout, gpsTimeout);
              }
              else 
              { 
                gpsTimeout = getTimeout; 
                Serial3.printf("Get the GPS Timeout: %d ms", getTimeout);
              }

              Serial3.println("");
              Serial3.print(">> ");
            }
            else {Serial3.println("error: Wrong 2nd command of Get!"); Serial3.print(">> ");}
          }
          else {Serial3.println("error: There is no 2nd parameter of Get."); Serial3.print(">> ");}
        }
      }
      else{Serial3.println("error:There is no parameter!"); Serial3.print(">> ");}
    }
    else{}
  }

	if ( (memcmp (cmd, "LoRa", 4)==0) || (memcmp (cmd, "lora", 4)==0) || (memcmp (cmd, "LORA", 4)==0) )
	{
	  if ( (ptr = strtok(cmd, " \r\n")) != NULL )	   // check the message type
	  {
      strcpy(str, ptr);
      #if __SERIAL3_DEBUG__
      Serial3.println(str);
      #endif
  
      if ( (ptr = strtok(NULL, " ")) != NULL )
      {
        strcpy(str, ptr);
        #if __SERIAL3_DEBUG__
        Serial3.println(str);
        #endif
  
        if ( (memcmp (str, "set", 3)==0) || (memcmp (str, "SET", 3)==0) || (memcmp (str, "Set", 3)==0) )
        {
          if ( (ptr = strtok(NULL, " ")) != NULL )
          {
            strcpy(str, ptr);
            #if __SERIAL3_DEBUG__
            Serial3.println(str);
            #endif
      
            if ( (memcmp (str, "baud", 4)==0) || (memcmp (str, "BAUD", 4)==0) || (memcmp (str, "Baud", 4)==0) )
            {
              if ( (ptr = strtok(NULL, " ")) != NULL )
              {
                int setBaudDat;
                strcpy(str, ptr);

                setBaudDat = atoi(str);
                EEPROM.update(ADDR_LORA_BAUD,	  setBaudDat>>24);
                EEPROM.update(ADDR_LORA_BAUD+1, setBaudDat>>16);
                EEPROM.update(ADDR_LORA_BAUD+2, setBaudDat>>8);
                EEPROM.update(ADDR_LORA_BAUD+3, setBaudDat);
  
                Serial3.printf("LoRa Badurate Set OK: %d bps", setBaudDat);
                Serial3.println("");
                Serial3.print(">> ");
  
                UART5_Baud_Set();
                ret = true;
              }
            }
          }
          else {Serial3.println("error: There is no 2nd parameter of Set."); Serial3.print(">> ");}
        }
    
        if ( (memcmp (str, "get", 3)==0) || (memcmp (str, "GET", 3)==0) || (memcmp (str, "Get", 3)==0) )
        {
          if ( (ptr = strtok(NULL, " ")) != NULL )
          {
              strcpy(str, ptr);
              #if __SERIAL3_DEBUG__
              Serial3.println(str);
              #endif
    
              if ( (memcmp (str, "baud", 4)==0) || (memcmp (str, "BAUD", 4)==0) || (memcmp (str, "Baud", 4)==0) )
              {
              int getBaudDat;
    
              getBaudDat =  (int)(EEPROM.read(ADDR_LORA_BAUD  )<<24);
              getBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+1)<<16);
              getBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+2)<<8);
              getBaudDat |= (int)(EEPROM.read(ADDR_LORA_BAUD+3));
    
              if ( 
                (getBaudDat == 9600) ||
                (getBaudDat == 19200) ||
                (getBaudDat == 38400) ||
                (getBaudDat == 57600) ||
                (getBaudDat == 115200) ||
                (getBaudDat == 230400)
                )
              {
                LoRaBaudDat = getBaudDat;
                Serial3.printf("Get the LoRa Buadrate: %d bps", getBaudDat);
                Serial3.println("");
                Serial3.print(">> ");
              }
              else
              {
                gpsBaudDat = _UART5_BAUDRATE_;
                Serial3.printf("[EEPROM: %d] Get the LoRa default Buadrate: %d bps", getBaudDat, _UART5_BAUDRATE_);
                Serial3.println("");
                Serial3.print(">> ");
              }
            }
          }
          else {Serial3.println("error: There is no 2nd parameter of Get."); Serial3.print(">> ");}
        }
      }
      else{Serial3.println("error:There is no parameter!"); Serial3.print(">> ");}
	  }
	  else{}
	}

  if ( memcmp (cmd, "$", 1)==0 )
  {
    Serial3.print("[");Serial3.print(__LINE__);Serial3.print("]");  Serial3.print(cmd);
    Serial2.print(cmd);
  }

  return ret;
}// End of bool LoRa_AT_Command(char *cmd)

uint8_t LoRa_retDataCompare(char *inData)
{
  
  int rot;
  uint8_t cmpSt = 0;
  uint8_t ret = 0;
  char *ptr, str[64];
  int iii;

  for (rot=0; LoRaAppReal[rot].cmd != NULL; rot++)
  {
    if (memcmp (LoRaAppReal[rot].cmd, inStr5Cp, LoRaAppReal[rot].len) == 0) {cmpSt = 1; break;}
  }
  if(cmpSt)
  {
    // Serial3.println("ROT");
    // Serial3.println(rot);
    switch(rot)
    {
      case 0:         // "OK"
        break;

      case 1:         // "ERROR"
        cRun.runSt.b.st_error = true;
        break;

      case 2:         // "BUSY"
        cRun.runSt.b.st_busy = true;
        cRun.busyProcMillis = millis();
        break;


      case 3:         // "READY"
        cRun.runSt.b.st_ready = true;
        if ((cRun.runSt.b.st_ack_rcv) || (cRun.runSt.b.st_ack_not_rcv))
        {
          cRun.runSt.b.st_ack_rcv=false;
          cRun.runSt.b.st_ack_not_rcv=false;

          cRun.runSt.b.st_gps_mon = false;
          Serial2.end();
          // Serial5.end();

          Serial3.printf("\r\n\n");

          //Serial3.printf("[%d.%d.%d]\r\n", buoyDat[0].detail.timeInfo.yearMon.b.year, buoyDat[0].detail.timeInfo.yearMon.b.month, buoyDat[0].detail.timeInfo.dateHourMin.b.day);
          //Serial3.printf("[%d.%d.%d]\r\n", buoyDat[0].detail.timeInfo.dateHourMin.b.hour, buoyDat[0].detail.timeInfo.dateHourMin.b.minute, buoyDat[0].detail.timeInfo.second.b.sec);
          Serial3.printf("[UL MSG SEQ number Fcnt:: %d]\r\n", buoyDat[0].detail.buoyId.sequenceNo); // ** Sequenc Number **//
          //Serial3.printf("Done!\r\n");

          Serial3.printf("\r\n");
          //Serial3.print(__DATE__);
          //Serial3.print(" ");
          //Serial3.printf("Time:: ");
          //Serial3.println(__TIME__);

          cRun.runSt.b.st_finish = true;
          cRun.finishMillis = millis();
        }
        else
        {
          if (powerOnLora == true)
          {
            powerOnLora = false;
            cRun.curRunMode = cmp_1st;
          }
          else
          {
            cRun.runSt.b.st_sleep = true;
            sktLoraStatus |= 0x04;
            if (sktLoraStatus == 0x07)
            {
              buoyDat[0].detail.buoySt.b.activeComm = 0;
              buoyDat[0].detail.buoySt.b.activeComm = 0;
              LoRa_WakeUp();
              LoRa_GetLastRssiSnr();
            }
          }
        }
        buoyRunInit(); //@@kdh
        break;

      case 4:         // "Wisol LoRa"

        break;

      case 5:         // "DevEui"
        iii = 0;
        ptr = strtok(inData, " ");
        while (ptr != NULL)
        {
          strcpy(rdDevEui, ptr);			
          ptr = strtok(NULL, " ");
          iii++;
        }
        ascii_to_hex(rdDevEui, 16, buoyDat[0].detail.buoyId.buoyId);
        cRun.joinProcMillis = millis();
        break;

      case 7:         // "JOIN_COMPLETE"
        sktLoraStatus |= 0x02;
        cRun.runSt.b.st_Join_Complete = true;

        break;

      case 10:        // "JOIN_ACCEPT"
        cRun.runSt.b.st_Join_Accept = true;
        sktLoraStatus = 0x01;
        break;

      case 11:         // "RSSI"
        iii = 0;
        memset (str, 0x00, sizeof(str));
        ptr = strtok(inData, " ");
        while (ptr != NULL)
        {
          strcpy(str, ptr);
          ptr = strtok(NULL, " ");
          iii++;
        }
        rot = atoi(str);
        if (rot >= -90)
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 5;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 0;
        }
        else if(rot >= -100)
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 4;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 0;
        }
        else if(rot >= -110)
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 3;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 0;
        }
        else if(rot >= -120)
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 2;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 0;
        }
        else if(rot < -120)
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 1;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 1;
        }
        else
        {
          buoyDat[0].detail.buoySt.b.sktLoRaRSSI = 0;
          buoyDat[0].detail.buoySt.b.mainCommModule = 0;
          buoyDat[0].detail.buoySt.b.pLoRaModule    = 1;
        }
        
        break;

      case 12:        // "SNR"
        iii = 0;
        memset (str, 0x00, sizeof(str));
        ptr = strtok(inData, " ");
        while (ptr != NULL)
        {
          strcpy(str, ptr);
          ptr = strtok(NULL, " ");
          iii++;
        }
        rot = atoi(str);

        if (atCmdMode == true){}
        else
        {
          cRun.curRunMode = cmp_5th;
        }
        break;

      case 13:        // RX1CH_OPEN

	    break;


      case 14:        // RX2CH_OPEN

        break;

      case 15:        // Ack received
        cRun.runSt.b.st_ack_rcv = true;
        break;

      case 16:        // Ack not received
        cRun.runSt.b.st_ack_not_rcv = true;
        break;        

      case 18:        // Fcnt

        break;        

      case 19:        // DevReset
      case 20:        // DEVRESET
        LoRa_Reset();
        break;		


      case 28:        // SKT
        LoRa_WakeUp();
        LoRa_GetDebugMsg();
        break;

      case 29:        // Freq

        break;

      case 30:        // SEND

        break;

      case 31:        // CON_UP

        break;

      case 48:		  // CLASS
        iii = 0;
        memset (str, 0x00, sizeof(str));
        ptr = strtok(inData, " ");
        while (ptr != NULL)
        {
          strcpy(str, ptr);
          ptr = strtok(NULL, " ");
          iii++;
        }

        LoRa_WakeUp();
        LoRa_GetDeviceEUI();
        break;

      case 51:			// Debug MSG
        iii = 0;
        memset (str, 0x00, sizeof(str));
        ptr = strtok(inData, " ");
        while (ptr != NULL)
        {
          strcpy(str, ptr);
          ptr = strtok(NULL, " ");
          iii++;
        }

        LoRa_WakeUp();
        LoRa_GetClassType();
        break;

      default:

        break;
    }
    ret = 1;
  }
  return ret;
} // End of uint8_t LoRa_retDataCompare(char *inData)

void LoRa_DataProcess(void)
{
  uint16_t buoyStChgtemp = 0;
  uint8_t  gpsTimeChgtemp[5];
  uint8_t  gpsPositionChgtemp[8];
  uint32_t tt = 0;

  /*
   * sort data order 
   */

  Serial3.printf("%d ", buoyDat[0].detail.buoySt.b.activeComm);
  Serial3.printf("%d ", buoyDat[0].detail.buoySt.b.mainCommModule);
  Serial3.printf("%d ", buoyDat[0].detail.buoySt.b.pLoRaModule);
  Serial3.printf("%d\r\n", buoyDat[0].detail.buoySt.b.gnssModule);

  Serial3.printf("Power Status: %d\r\n", buoyDat[0].detail.buoySt.b.BuoyPwr);
  Serial3.printf("LoRa rssi Status: %d\r\n", buoyDat[0].detail.buoySt.b.sktLoRaRSSI);
  Serial3.printf("pLoRa rssi Status: %d\r\n", buoyDat[0].detail.buoySt.b.pLoRaRSSI);
  Serial3.printf("pLoRa of boat rssi Status: %d\r\n", buoyDat[0].detail.buoySt.b.fishingBoatRSSI);

  buoyStChgtemp = (uint16_t)(buoyDat[0].detail.buoySt.b.activeComm      << 15);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.mainCommModule  << 14);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.pLoRaModule     << 13);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.gnssModule      << 12);
  
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.BuoyPwr         << 9);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.sktLoRaRSSI     << 6);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.pLoRaRSSI       << 3);
  buoyStChgtemp |= (uint16_t)(buoyDat[0].detail.buoySt.b.fishingBoatRSSI);

  Serial3.println("");
  Serial3.printf("buoyStChgtemp: 0x%04x\r\n", buoyStChgtemp);

  buoyDat[0].data[13] = (buoyStChgtemp & 0xFF00) >> 8;
  buoyDat[0].data[14] = (buoyStChgtemp & 0x00FF);


  Serial3.printf("GNSS LOCK: %d\r\n", buoyDat[0].detail.gpsSt.b.lockSt);
  Serial3.printf("Lat Direct: %d\r\n", buoyDat[0].detail.gpsSt.b.latitudePos);
  Serial3.printf("Lon Direct: %d\r\n", buoyDat[0].detail.gpsSt.b.longitudePos);
  Serial3.printf("GPS Point size: %d\r\n", buoyDat[0].detail.gpsSt.b.decimalPoint);

  buoyDat[0].detail.gpsSt.data = 0x10;

  Serial3.printf("Y: %d(%04xh)\r\n", buoyDat[0].detail.timeInfo.yearMon.b.year, buoyDat[0].detail.timeInfo.yearMon.b.year);
  Serial3.printf("Mo: %d(%02xh)\r\n", buoyDat[0].detail.timeInfo.yearMon.b.month, buoyDat[0].detail.timeInfo.yearMon.b.month);
  Serial3.printf("D: %d(%02xh)\r\n", buoyDat[0].detail.timeInfo.dateHourMin.b.day, buoyDat[0].detail.timeInfo.dateHourMin.b.day);
  Serial3.printf("H: %d(%02xh)\r\n", buoyDat[0].detail.timeInfo.dateHourMin.b.hour, buoyDat[0].detail.timeInfo.dateHourMin.b.hour);
  Serial3.printf("Mi: %d(%02xh)\r\n", buoyDat[0].detail.timeInfo.dateHourMin.b.minute, buoyDat[0].detail.timeInfo.dateHourMin.b.minute);
  Serial3.printf("S: %d(%02xh)\r\n", buoyDat[0].detail.timeInfo.second.b.sec, buoyDat[0].detail.timeInfo.second.b.sec);
  
  Serial3.printf("GPS Status: 0x%02x\r\n", buoyDat[0].detail.gpsSt.data);

  Serial3.printf("Lat D: %d(%04xh)\r\n", buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal, buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal);
  Serial3.printf("Lat P: %d(%05xh)\r\n", buoyDat[0].detail.gpsInfo.latitudeInfo.b.point, buoyDat[0].detail.gpsInfo.latitudeInfo.b.point);
  Serial3.printf("Lon D: %d(%04xh)\r\n", buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal, buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal);
  Serial3.printf("Lon P: %d(%05xh)\r\n", buoyDat[0].detail.gpsInfo.longitueInfo.b.point, buoyDat[0].detail.gpsInfo.longitueInfo.b.point);

  gpsTimeChgtemp[0] = (uint8_t) ( ((buoyDat[0].detail.timeInfo.yearMon.b.year) & 0x0FF0) >> 4) ;
  gpsTimeChgtemp[1] = (uint8_t) ( ((buoyDat[0].detail.timeInfo.yearMon.b.year & 0x000F) << 4 ) | buoyDat[0].detail.timeInfo.yearMon.b.month );
  gpsTimeChgtemp[2] = (uint8_t) ( (buoyDat[0].detail.timeInfo.dateHourMin.b.day << 3) | ((buoyDat[0].detail.timeInfo.dateHourMin.b.hour & 0x1C) >> 2) );
  gpsTimeChgtemp[3] = (uint8_t) ( ((buoyDat[0].detail.timeInfo.dateHourMin.b.hour & 0x03) << 6) | buoyDat[0].detail.timeInfo.dateHourMin.b.minute );
  gpsTimeChgtemp[4] = (uint8_t) ( buoyDat[0].detail.timeInfo.second.b.sec << 2 );

  tt = (buoyDat[0].detail.gpsInfo.latitudeInfo.b.decimal << 17) | buoyDat[0].detail.gpsInfo.latitudeInfo.b.point;
  gpsPositionChgtemp[0] = (tt & 0xFF000000) >> 24;
  gpsPositionChgtemp[1] = (tt & 0x00FF0000) >> 16;
  gpsPositionChgtemp[2] = (tt & 0x0000FF00) >> 8;
  gpsPositionChgtemp[3] = (tt & 0x000000FF);

  tt = (buoyDat[0].detail.gpsInfo.longitueInfo.b.decimal << 17) | buoyDat[0].detail.gpsInfo.longitueInfo.b.point;
  gpsPositionChgtemp[4] = (tt & 0xFF000000) >> 24;
  gpsPositionChgtemp[5] = (tt & 0x00FF0000) >> 16;
  gpsPositionChgtemp[6] = (tt & 0x0000FF00) >> 8;
  gpsPositionChgtemp[7] = (tt & 0x000000FF);

  memcpy(&buoyDat[0].data[15], gpsTimeChgtemp, 5);
  memcpy(&buoyDat[0].data[21], gpsPositionChgtemp, 8);

  buoyDat[0].detail.checkSum                  = LoRa_CheckSum(buoyDat[0].data);

  memcpy (buoyDat[1].data, buoyDat[0].data, 31);
  buoyDat[1].detail.buoyId.msgId = 0x82;
  buoyDat[1].detail.checkSum                  = LoRa_CheckSum(buoyDat[1].data);


  Serial3.printf("Original the data array for SKT LoRa\r\n");
  for(int p=0; p<31; p++)
  {
	Serial3.printf("%02X ",buoyDat[0].data[p]);
	if ( (p == 15) ) Serial3.print("\r\n");
  }
  Serial3.println("\r\n");

  LoRa_sndDataChange(buoyDat[0].data);

  #if 1
    /* @@adh, collision avoidence using random delay from 0~10 seconds */
    randomseed= (randomseed+123)^((randomseed<<9)|(randomseed>>23)); //@@adh
    Serial3.printf("\r\n@@ Random delay before transmission (0~10sec): %d msec.\r\n \r\n", randomseed%10000); //@@adh
    delay(randomseed%10000);  //@@adh
  #endif

  Serial3.printf("Send to SKT LoRa!\r\n");
  for(int p=0; p<31; p++)
  {
	Serial3.printf("%02X ",sndData[0][p]);
	if ( (p == 15) ) Serial3.print("\r\n");
  }
  Serial3.println("");

  gpsProcSt = 0;

} // End of void LoRa_DataProcess(void)

uint8_t *LoRa_sndDataChange(uint8_t *inData)
{
  memcpy(sndData[0], inData, 31);

  // Buoy ID
  sndData[0][5]  = inData[12];
  sndData[0][6]  = inData[11];
  sndData[0][7]  = inData[10];
  sndData[0][8]  = inData[9];
  sndData[0][9]  = inData[8];
  sndData[0][10] = inData[7];
  sndData[0][11] = inData[6];
  sndData[0][12] = inData[5];

  // Buoy Status Info
  sndData[0][13] = inData[14];
  sndData[0][14] = inData[13];

  // Year, Month, Date, Time
  sndData[0][15] = inData[16];
  sndData[0][16] = inData[15];
  sndData[0][17] = inData[18];
  sndData[0][18] = inData[17];
  sndData[0][19] = inData[19];

  // GPS Data
  sndData[0][21] = inData[24];
  sndData[0][22] = inData[23];
  sndData[0][23] = inData[22];
  sndData[0][24] = inData[21];
  sndData[0][25] = inData[28];
  sndData[0][26] = inData[27];
  sndData[0][27] = inData[26];
  sndData[0][28] = inData[25];

  return sndData[0];
} // End of uint8_t *LoRa_sndDataChange(uint8_t *inData)

/***********************************************************************************************************************
 * Part : Sensing of the Battery voltage and Temperature
 * Function Name: readVref(), readTempSensor(), readVoltage() and batteryPowLevel()
 * Description  : 
 * Arguments    : None
 * Return Value : None
***********************************************************************************************************************/
static int32_t readVref()
{
  #ifdef __LL_ADC_CALC_VREFANALOG_VOLTAGE
  #ifdef STM32U5xx
    return (__LL_ADC_CALC_VREFANALOG_VOLTAGE(ADC1, analogRead(AVREF), LL_ADC_RESOLUTION));
  #else
    return (__LL_ADC_CALC_VREFANALOG_VOLTAGE(analogRead(AVREF), LL_ADC_RESOLUTION));
  #endif
  #else
    return (VREFINT * ADC_RANGE / analogRead(AVREF)); // ADC sample to mV
  #endif
} // End of static int32_t readVref()

#ifdef ATEMP
static int32_t readTempSensor(int32_t VRef)
{
  #ifdef __LL_ADC_CALC_TEMPERATURE
  #ifdef STM32U5xx
    return (__LL_ADC_CALC_TEMPERATURE(ADC1, VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
  #else
    return (__LL_ADC_CALC_TEMPERATURE(VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
  #endif
  #elif defined(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS)
    return (__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(AVG_SLOPE, VTEMP, CALX_TEMP, VRef, analogRead(ATEMP), LL_ADC_RESOLUTION));
  #else
    return 0;
  #endif
} // End of static int32_t readTempSensor(int32_t VRef)
#endif

static int32_t readVoltage(int32_t VRef, uint32_t pin)
{
  #ifdef STM32U5xx
    return (__LL_ADC_CALC_DATA_TO_VOLTAGE(ADC1, VRef, analogRead(pin), LL_ADC_RESOLUTION));
  #else
    return (__LL_ADC_CALC_DATA_TO_VOLTAGE(VRef, analogRead(pin), LL_ADC_RESOLUTION));
  #endif
} // End of static int32_t readVoltage(int32_t VRef, uint32_t pin)

uint8_t batteryPowLevel(void)
{
  uint32_t rcvBattery;
  
  int32_t VRef = readVref();
  {
    uint32_t rot = 0, bD = 0;
    float tempBatt;

    bD = 0;
    for(rot=0; rot < 50; rot++)
    {
      uint32_t rdBatt;
	  VRef = readVref();
      rdBatt = readVoltage(VRef, PC1);
      bD += rdBatt;
      delay(1);
    }
    rcvBattery = bD / rot + 10;
  }

  calBattery = rcvBattery + rcvBattery/2;
  calculationV = (float)(calBattery/1000.0);
  Serial3.println("");
  Serial3.print("Battery Volt:: ");
  Serial3.print(calBattery);
  Serial3.println(" mV");

  if (calBattery >= 4500)
  {
    buoyDat[0].detail.buoySt.b.BuoyPwr = 5;
  }
  else if(calBattery >= 4000)
  {
    buoyDat[0].detail.buoySt.b.BuoyPwr = 4;
  }
  else if(calBattery >= 3500)
  {
    buoyDat[0].detail.buoySt.b.BuoyPwr = 3;
  }
  else if(calBattery >= 3000)
  {
    buoyDat[0].detail.buoySt.b.BuoyPwr = 2;
  }
  else
  {
    buoyDat[0].detail.buoySt.b.BuoyPwr = 1;
  }

  return buoyDat[0].detail.buoySt.b.BuoyPwr;
} // End of uint8_t batteryPowLevel(void)


/***********************************************************************************************************************
* Function Name: GPS_PowerControl
* Description  : This function implements for Power control of SKT LoRa.
* Arguments    : Power Off -> 0 , Power On -> 1
* Return Value : None
***********************************************************************************************************************/
void GPS_PowerControl(uint8_t inSignal)
{
  if (inSignal == true)
  {
    digitalWrite(GPIO6_GPS_PWR_SW, HIGH);	// GPIO6_GPS_PWR_SW --> ON
  }
  else
  {
    digitalWrite(GPIO6_GPS_PWR_SW, LOW);	// GPIO6_GPS_PWR_SW --> OFF
  }
} // End of void GPS_PowerControl(uint8_t inSignal)

void GPS_Reset(void)
{
  digitalWrite(GPIO2_GPS_RST, LOW);
  delay(5);
  digitalWrite(GPIO2_GPS_RST, HIGH);
  delay(3000);
  digitalWrite(GPIO2_GPS_RST, LOW);
} // End of void GPS_Reset(void)

void GPS_PMTK_CMD_HOT_START(void)
{
  Serial2.println("$PMTK101*32");
} // End of void GPS_PMTK_CMD_HOT_START(void)

void GPS_PMTK_CMD_Stop(void)
{
  Serial2.println("$PMTK161,0*28");
} // End of void GPS_PMTK_CMD_Stop(void)

void GPS_PMTK_CMD_Sleep(void)
{
  Serial2.println("$PMTK161,1*29");
} // End of void GPS_PMTK_CMD_Sleep(void)

void GPS_PMTK_TEST(void)
{
  Serial2.println("$PMTK000*32");
} // End of void GPS_PMTK_TEST(void)

void GPS_PMTK_TEST_FINISH(void)
{
  Serial2.println("$PMTK812*39");
} // End of void GPS_PMTK_TEST_FINISH(void)

/*
 * DataField: $PMTK702,InType,OutType,Baud*CS<CR><LF>
 */
void GPS_PMTK_DT_DATA_PORT(void)
{
  Serial2.println("$PMTK702,1,1,9600*14");
} // End of void GPS_PMTK_DT_DATA_PORT(void)

/*
 * DataField: $PMTK251,Baudrate*CS<CR><LF>
 */
void GPS_PMTK_SET_NMEA_BAUDRATE(void)
{
  Serial2.println("$PMTK251,38400*27");
} // End of void GPS_PMTK_SET_NMEA_BAUDRATE(void)


// --------------------------------------------------------
// Watch dog Clear
//
void WDT_Clear(void)
{
#if WDT_TEST_1
  if (!WDT->SYNCBUSY.bit.CLEAR)                 // Check if the WDT registers are synchronized
  {
    REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;        // Clear the watchdog timer
  }
#endif


#if WDT_TEST_2
  resetWDT ();
#endif

#if WDT_TEST_3
  // Uncomment to change timeout value to 6 seconds
  //IWatchdog.set(6000000);

  // Reload the watchdog only when the button is pressed
    IWatchdog.reload();
#endif
} // End of void WDT_Clear(void)


// --------------------------------------------------------
// Utility Function
//
unsigned int ascii_to_hex(const char* str, size_t size, uint8_t* hex)
{
    unsigned int i, h, high, low;
    for (h = 0, i = 0; i < size; i += 2, ++h) {
        high = (str[i] > '9') ? (str[i] - 'A' + 10)&0x0F : str[i]&0x0F;
        low = (str[i + 1] > '9') ? (str[i + 1] - 'A' + 10)&0x0F : str[i + 1]&0x0F;
        hex[h] = (high << 4) | low;
    }
    return h;
} // End of unsigned int ascii_to_hex(const char* str, size_t size, uint8_t* hex)

bool hexCheck(char *str)
{
  bool ret;
  
  if (strlen(str) < 2) 
  {
    if (
       ( ((str[0] > 0x29) && (str[0] < 0x3A)) || ((str[0] > 0x40) && (str[0] < 0x47)) || ((str[0] > 0x60) && (str[0] < 0x67)) )
       )
       {
         ret = true;
       }
    else {ret = false;}
  }
  else if (strlen(str) < 3) 
  {
    if (
       ( ((str[0] > 0x29) && (str[0] < 0x3A)) || ((str[0] > 0x40) && (str[0] < 0x47)) || ((str[0] > 0x60) && (str[0] < 0x67)) ) &&
       ( ((str[1] > 0x29) && (str[1] < 0x3A)) || ((str[1] > 0x40) && (str[1] < 0x47)) || ((str[1] > 0x60) && (str[1] < 0x67)) )
       )
       {
         ret = true;
       }
    else {ret = false;}
  }
  else if ((strlen(str) < 4) && ((str[2]=='\r') || (str[2]=='\n'))) 
  {
    if (
       ( ((str[0] > 0x29) && (str[0] < 0x3A)) || ((str[0] > 0x40) && (str[0] < 0x47)) || ((str[0] > 0x60) && (str[0] < 0x67)) ) &&
       ( ((str[1] > 0x29) && (str[1] < 0x3A)) || ((str[1] > 0x40) && (str[1] < 0x47)) || ((str[1] > 0x60) && (str[1] < 0x67)) )
       )
       {
         ret = true;
       }
    else {ret = false;}
  }
  else if ((strlen(str) < 5) && ((str[2]=='\r') && (str[3]=='\n')) || ((str[2]=='\n') && (str[3]=='\r'))) 
  {
    if (
       ( ((str[0] > 0x29) && (str[0] < 0x3A)) || ((str[0] > 0x40) && (str[0] < 0x47)) || ((str[0] > 0x60) && (str[0] < 0x67)) ) &&
       ( ((str[1] > 0x29) && (str[1] < 0x3A)) || ((str[1] > 0x40) && (str[1] < 0x47)) || ((str[1] > 0x60) && (str[1] < 0x67)) )
       )
       {
         ret = true;
       }
    else {ret = false;}
  }
  else {ret = false;}

  return ret;
} // End of bool hexCheck(char *str)

uint8_t atoh(char *str)
{
  uint8_t ret;

  if( 
    (strlen(str)==1) || 
    ( (strlen(str)==3) && (((str[2]=='\r') || (str[3]=='\n')) || ((str[2]=='\n') || (str[3]=='\r'))) ) 
    )
  {
    if ((str[0] > 0x29) && (str[0] < 0x3A)) { ret = (str[0]&0x0F); }
    else if ((str[0] > 0x40) && (str[0] < 0x47)) { ret = (str[0]-0x37); }
    else if ((str[0] > 0x60) && (str[0] < 0x67)) { ret = (str[0]-0x57); }
    else {}
  }
  else
  {
    if ((str[0] > 0x29) && (str[0] < 0x3A)) { ret = (str[0]&0x0F)<<4; }
    else if ((str[0] > 0x40) && (str[0] < 0x47)) { ret = (str[0]-0x37)<<4; }
    else if ((str[0] > 0x60) && (str[0] < 0x67)) { ret = (str[0]-0x57)<<4; }
    else {}

    if ((str[1] > 0x29) && (str[1] < 0x3A)) { ret |= (str[1]&0x0F); }
    else if ((str[1] > 0x40) && (str[1] < 0x47)) { ret |= (str[1]-0x37); }
    else if ((str[1] > 0x60) && (str[1] < 0x67)) { ret |= (str[1]-0x57); }
    else {}
  }

  return ret;
} // End of uint8_t atoh(char *str)
