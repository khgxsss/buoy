
/*
******************************************************
Mode           OTAA               ABP
------------   ---------------    -----------------
Display Mode   OTAA               ABP
Data           DevAddr:<value>    NetID:<value>
               J_A_Delay1:5000    DevAddr:<value>
               J_A_Delay2:6000    NwkSKey:<value>
                                  AppSKey:<value>
******************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SktLoRa.h"

/*
 * Set Activation mode
 * Select LoRaWAN activation mode
 * (default : over the air activation mode)
 *
 * �� The module is reset after the execution is completed to apply the set value.
 *
 * Option 1 - Message
 *  (1) otaa : over the air activation mode
 *      ? execution Message : "Set over the air activation"
 *  (2) abp : activation by personalization
 *      ? execution Message : "Set activation by personalization"
 *
 *   - Ex) LRW 30 otaa
 */
void LoRa_SetActivationMode(char *opt)
{
  /*
   * Execution Return Message
   * "Ack received" / "Ack not received"
   * "READY"
   */
#if 0
  Serial5.print("LRW 30 ");
  Serial5.print((char *)opt);		                  /* "otaa" or "abp"  */
  Serial5.print("\r\n");
#else
  Serial5.printf("LRW 30 %s\r\n", opt);    /* "otaa" or "abp"  */
#endif
}

/*
 * Tx confirm/unconfirm msg
 * (@@@ Please note, Tx confirm/unconfirm binary data @@@)
 * Uplink confirmed data or Uplink unconfirmed data
 *
 * Option 1 - Message
 *  (1) Maximum size of message differs according to DataRate(DR).
 *      - See the document "LoRaWAN Regional Parameters" provided by Lora alliance.

 *  (2) When the transmitted message is 1 byte , 
 *      That is, the entire transmitted data length is 10 bytes.
 *  (3) When the mess age to be transmitted is 242byte s , i.e total data length = 252
 *
 * Option 2 - Confirmed or Unconfirmed
 *  (1) cnf   : confirmed data
 *  (2) uncnf : unconfirmed data
 *
 * Option 3 - Fport
 *   - Fport : 1 ~ 221
 *
 *   - Ex) LRW 31 adcd 1234 cnf 1
 */
void LoRa_TXMsg(char *opt1, uint8_t opt2, uint8_t opt3)
{
  /*
   * Execution Return Message
   * "Ack received" / "Ack not received"
   * "READY"
   */
  Serial5.printf("LRW 31 %s", opt1);  // message data
  if (opt2)					                  // confirmed or unconfirmed
  {
	  Serial5.print(" cnf ");
  }
  else
  {
	  Serial5.print(" uncnf ");
  }
  Serial5.print(opt3);		// Fport number
  Serial5.print("\r\n");
}

/*
 * Set Report Time
 * Set Report Time (uint : sec)
 * (default : 60 sec)
 * Option 1
 *   - Report time : 1 ~ 2592000 (2,592,000 = 3-days, Set max 30days)
 */
void LoRa_SetReportTime(uint32_t opt)
{
  /*
   * Execution Return Message
   * "Set Report Time : <Report time value>"
   */
  Serial5.printf("LRW 32 %d\r\n", opt);
}

/*
 * Set App Eui
 * Application EUI setting
 *
 * Option 1
 *   - 8byte Hexadecimal AppEUI
 *
 *   - Ex) LRW 33 1234567812345678
 */
void LoRa_SetAppEui(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "AppEui : xxxxxxxxxxxxxxxx"
   */
  Serial5.printf("LRW 33 %s\r\n", opt);
}

/*
 * Get Report Time
 * Get Report Time (uint : sec)
 */
void LoRa_GetReportTime(void)
{
  /*
   * Execution Return Message
   * "Report Time : <Report time value>"
   */
  Serial5.print("LRW 34\r\n");
}

/*
 * Set Data rate
 * Data rate setting
 *
 * Option 1
 *   - 0 ~ 5(SKT/KR), 0 ~ 6(AU915), 2 ~ 6(Brazil)
 *  (1) 0 : SF12/125KHz
 *  (2) 1 : SF11/125KHz
 *  (3) 2 : SF10/125KHz
 *  (4) 3 : SF9 /125KHz
 *  (5) 4 : SF8 /125KHz
 *  (6) 5 : SF7 /125KHz
 *  (7) 6 : SF8 /500KHz
 *
 *   - Ex) LRW 35 2
 */
void LoRa_SetDataRate(uint8_t opt)
{
  /*
   * Execution Return Message
   * "Set DR : <data rate value"
   *  - See the document "LoRaWAN Regional Parameters" provided by Lora alliance.
   */
  Serial5.printf("LRW 35 %d\r\n", opt);
}

/*
 * Set ADR
 * Adaptive data rate function on/off
 * (default : ON)
 *
 * Option 1
 *  (1) on  : ADR on
 *  (2) off : ADR off
 *
 *   - Ex) LRW 36 off
 */
void LoRa_SetADR(uint8_t* opt)
{
  /*
   * Execution Return Message
   * "Set ADR ON"
   * "Set ADR OFF"
   */
  Serial5.printf("LRW 36 %s\r\n", opt);
}

/*
 * Set ReTx
 * Re-transmission number setting
 * (default : 8)
 *
 * Option 1
 *  (1) ReTx : Re-transmission number 1~8
 *
 *   - Ex) LRW 37 8
 */
void LoRa_SetReTX(uint8_t opt)
{
  /*
   * Execution Return Message
   * "Set cnf_retx_nb : <ReTx number>"
   */
  Serial5.printf("LRW 36 %d\r\n", opt);
}


/*
 * Send a message for Link Check Request
 */
void LoRa_SendLinkCheckRequest(void)
{
  /*
   * Execution Return Message
   * ""
   */
  Serial5.printf("LRW 38\r\n");
}

/*
 * Check device EUI
 */
void LoRa_GetDeviceEUI(void)
{
  /*
   * Execution Return Message
   * "DevEui : xxxxxxxxxxxxxxxx"
   */
  Serial5.printf("LRW 3F\r\n");
}

/*
 * Check App EUI
 */
void LoRa_GetAppEUI(void)
{
  /*
   * Execution Return Message
   * "AppEui : xxxxxxxxxxxxxxxx"
   */
  Serial5.printf("LRW 40\r\n");
}

/*
 * Check Tx data rate
 */
void LoRa_GetTxDataRate(void)
{
  /*
   * Execution Return Message
   * "DR : <data rate value>"
   */
  Serial5.printf("LRW 42\r\n");
}

/*
 * Join request ( Initialze the variable ) Untested
 */
void LoRa_Join_request(void)
{
  /*
   * Execution Return Message
   * "Join Request"
   */
  Serial5.printf("LRW 43\r\n");
}

/*
 * Get ADR
 * Check if Adaptive data rate function on/off
 */
void LoRa_GetADR(void)
{
  /*
   * Execution Return Message
   * "ADR status : <ON or OFF>"
   */
  Serial5.print("LRW 44\r\n");
}

/*
 * Check Uplink Re-transmission number of Confirmed Message
 */
void LoRa_GetReTx(void)
{
  /*
   * Execution Return Message
   * "Cnf_retx_nb : <ReTx number>"
   * "CLASS C"
   */
  Serial5.printf("LRW 45\r\n");
}

/*
 * Set Repeater Support
 * Repeater Support setting
 * (default : 0)
 * Option 1
 *   (1) 0 : Repeater Support off
 *   (2) 1 : Repeater Support on
 *   - Ex) LRW 47 0
 */
void LoRa_SetRepeaterSupport(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set Repeater Support OFF"
   * "Set Repeater Support ON"
   */
  Serial5.printf("LRW 47 %s\r\n", opt);
}

/*
 * Get Repeater Support
 * Check Repeater Support
 *   - Ex) LRW 48
 */
void LoRa_GetRepeaterSupport(void)
{
  /*
   * Execution Return Message
   * "Repeater Support" or "Repeater Support ON"
   */
  Serial5.printf("LRW 48\r\n");
}

/*
 * Check connection
 * Check status of Serial connection.
 *   - Ex) LRW 49
 */
void LoRa_CheckConnection(void)
{
  /*
   * Execution Return Message
   * Return "OK" when inputting command
   */
  Serial5.print("LRW 49\r\n");
}

/*
 * Get the Last RSSI & SNR
 */
void LoRa_GetLastRssiSnr(void)
{
  /*
   * Execution Return Message
   * "RSSI : <RSSI value>"
   * "SNR : <SNR value>"
   */
  Serial5.print("LRW 4A\r\n");
}

/*
 * Set Class type
 * LoRaWAN Class setting
 * (default : CLASS A)
 * �� The module is reset after the execution is
 *   completed to apply the set value
 *
 * Option 1
 *  (1) 0 : Class A
 *  (2) 1 : Class B
 *  (3) 2 : Class C
 *
 *   - Ex) LRW 4B 0
 */
void LoRa_SetClassType(uint8_t opt)
{
  /*
   * Execution Return Message
   * (1) "Set CLASS A"
   *     "Reset for config"
   * (2) "Not Support CLASS B"
   * (3) "Set CLASS C"
   *     "Reset for config"
   */
  Serial5.printf("LRW 4B %d\r\n", opt);
}

/*
 * Check LoRaWAN Class settings
 */
void LoRa_GetClassType(void)
{
  /*
   * Execution Return Message
   * "CLASS A"
   * "CLASS C"
   */
//  Serial5.print("LRW 4C\r\n");
  Serial5.printf("LRW 4C\r\n");
}

/*
 * Tx confirm/unconfirm binary data
 * Uplink confirmed binary dat a or Uplink unconfirmed binary data
 *
 * Tx confirm/unconfirm binary data commands follow a different format 
 * from the CLI Commands introduced so far following the specifications 
 * of Array [0] ~ Array[n+2])
 *
 * Option 1 - Confirmed or Unconfirmed
 *  (1) 0x01 : confirmed data
 *  (2) 0x00 : unconfirmed data
 *
 * Option 2 - Fport
 *   - Fport : 0x01 ~ 0xDD ( 1 ~ 221 )
 *
 * Option 3 - Length
 *  (1) Length range : 0x01 ~ 0xF2
 *  (2) The maximum transmission size of Tx data differs for each DataRate (DR) of each
 *      country according to the LoRaWAN standard.
 *      (For details, refer to the LoRaWAN specification document)
 *
 * Option 4 - Message
 *  (1) Variable according to the length of the Message (Array [10] ~ [252])
 *  (2) When the transmitted message is 1 byte , 
 *      That is, the entire transmitted data length is 10 bytes.
 *  (3) When the mess age to be transmitted is 242byte s , i.e total data length = 252
 *   - Ex) 4C 52 57 20 34 44 20 01 01 0A 01 02 03 04 05 06 07 08 09 0A 0D 0A
 *         L  R  W ' ' 4  D ' ' 01 01 0A 01 02 03 04 05 06 07 08 09 0A 0D 0A
 */
void LoRa_TXbinary(uint8_t opt1, uint8_t opt2, uint8_t opt3, uint8_t *opt4)
{
  /*
   * Execution Return Message
   * "Ack received" / "Ack not received"
   * "READY"
   */
  Serial5.print("LRW 4D ");
  Serial5.write(opt1);
  Serial5.write(opt2);
  Serial5.write(opt3);
  for(int cnt=0; cnt < opt3; cnt++)
  {
  	Serial5.write(opt4[cnt]);
  }
  Serial5.print("\r\n");
}

void LoRa_GetFirmwareVer(void)
{
  /*
   * Execution Return Message
   * "Firmware version : <version>"
   */
  Serial5.printf("LRW 4F\r\n");
}

void LoRa_SetDebugMsg(uint8_t opt)
{
  /*
   * Execution Return Message
   * opt 0 : "Set Debug MSG OFF"
   * opt 1 : "Set Debug MSG ON"
   * opt 2 : "Set Debug MSG NONE"
   */
  Serial5.printf("LRW 50 %d\r\n", opt);     // opt 0 : Off , opt 1 : on , opt 2 : none
}

/*
 * Set Application key
 * Application key setting
 *
 * Option 1
 *  - 16 byte Hexadecimal AppKey
 *
 *  - Ex) LRW 51 12345678901234567890123456789012
 */
void LoRa_SetApplicationKey(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "AppKey : <AppKey value>"
   */
  Serial5.printf("LRW 51 %s\r\n", opt);
}

/*
 * Get Application key
 * Check Application key
 *
 *  - Ex) LRW 52
 */
void LoRa_GetApplicationKey(void)
{
  /*
   * Execution Return Message
   * "AppKey : <AppKey value>"
   */
  Serial5.print("LRW 52\r\n");
}

/*
 * Get Data Information
 * Check Data Information
 */
void LoRa_GetDataInformation(void)
{
  /*
   * Execution Return Message
   * LRW 57   
   * OTAA
   * DevAddr : 00000000
   * J_A_Delay1 : 5000
   * J_A_Delay2 : 6000
   */
  Serial5.printf("LRW 57\r\n");
}

/*
 * Set TX Power
 * TX Power setting
 * Option 1
 *   - See the document LoRaWAN Regional Parameters provided by Lora alliance.
 *   - 0: 14dBm, 14: 0dBm(SKT/KR), 0:30dBm, 15: 0dBm(AU, Brazil)
 *   - LOM202A : Max 14dBm, LOM204A : Max 20dBm
 *   - Ex) LRW 6C 5
 */
void LoRa_SetTXPower(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set TXPower : <value>"
   */
  Serial5.printf("LRW 5C %s\r\n", opt);
}

/*
 * Get TX Power & Channel Info
 * Check TX Power & Channel Info
 */
void LoRa_GetTXPower_ChInfo(void)
{
  /*
   * Execution Return Message
   * "TXPower : <value>"
   * "CH<value> Freq <value> DR <value>"
   */
  Serial5.printf("LRW 5D\r\n");
}

/*
 * Set channel TX power
 * (Only for SKT KR version)
 *
 * default value
 * -----------------------------------
 * Ch ID 1 : Tx Power Index: 0 (14dBm)
 * Ch ID 2 : Tx Power Index: 0 (14dBm)
 * Ch ID 3 : Tx Power Index: 0 (14dBm)
 * Ch ID 4 : Tx Power Index: 4 (10dBm)
 * Ch ID 5 : Tx Power Index: 0 (14dBm)
 * Ch ID 6 : Tx Power Index: 0 (14dBm)
 * Ch ID 7 : Tx Power Index: 0 (14dBm)
 * Ch ID 8 : Tx Power Index: 0 (14dBm)
 * -----------------------------------
 */
void LoRa_SetChannelTxPower(uint8_t opt1, uint8_t opt2)
{
  /*
   * Execution Return Message
   * "Set CH<Channel ID> PWR : <Tx Power ID>"
   */
  Serial5.printf("LRW 5E %d %d\r\n", opt1, opt2);     // opt1 : Channel ID Index (1~8), opt2 : Tx Power ID Index (0:14dBm~14:0dBm)
}

/*
 * ��ü ä�� Tx power ���� �� Ȯ�� 
 * (Only for SKT KR version)
 */
void LoRa_GetChannelTxPower(void)
{
  /*
   * Execution Return Message
   * CH<channel ID> Freq <Frequency value>Hz PWR <Tx Power ID>
   */
  Serial5.printf("LRW 5F\r\n");
}

void LoRa_GetDebugMsg(void)
{
  /*
   * Execution Return Message
   * "Debug MSG OFF"
   * "Debug MSG ON"
   */
  Serial5.printf("LRW 64\r\n");
}

/*
 * Set Network ID
 * Network ID setting
 * Option 1
 *   - Net ID: 3 byte Hexadecimal
 *   - Ex) LRW 65 123456
 */
void LoRa_SetNetworkID(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set NetID : <value>"
   */
  Serial5.printf("LRW 65 %s\r\n", opt);
}

/*
 * Set Device Address
 * Device Address setting
 * Option 1
 *   - Devaddr: 4 byte Hexadecimal
 *   - Ex) LRW 66 12345678
 */
void LoRa_SetDeviceAddress(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set DevAddr : <value>"
   */
  Serial5.printf("LRW 66 %s\r\n", opt);
}

/*
 * Set Network Session Key
 * Network Session Key setting
 * Option 1
 *   - NwkSKey: 16 byte Hexadecimal
 *   - Ex) LRW 67 12345678901234567890123456789012
 */
void LoRa_SetNetworkSessionKey(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set NwkSKey : <value>"
   */
  Serial5.printf("LRW 67 %s\r\n", opt);
}

/*
 * Set Application Session Key
 * Application Session Key setting
 * Option 1
 *   - AppSKey: 16 byte Hexadecimal
 *   - Ex) LRW 68 12345678901234567890123456789012
 */
void LoRa_SetApplicationSessionKey(uint8_t *opt)
{
  /*
   * Execution Return Message
   * "Set AppSKey : <value>"
   */
  Serial5.printf("LRW 68 %s\r\n", opt);
}

/*
 * Get Device Address
 * Check Device Address
 */
void LoRa_GetDeviceAddress(void)
{
  /*
   * Execution Return Message
   * "DevAddr : <value>"
   */
  Serial5.printf("LRW 6A\r\n");
}

/*
 * Set Channel Mask
 * Option 1, ~ Option 6
 *   (1) Channel mask table 0 ~ 4
 *   (2) 4 digits Hexa format
 *   - Ex) "LRW 6D ff00 0000 0000 0000 0002 0000" (for LA915) <= OK
 *         "LRW 6D 0007, 0000, 0000, 0000, 0000, 0000" <= OK
 */
void LoRa_SetChannelMask(uint8_t *opt1, uint8_t *opt2, uint8_t *opt3, uint8_t *opt4, uint8_t *opt5, uint8_t *opt6)
{
  /*
   * Execution Return Message
   * "OK"
   */
  Serial5.printf("LRW 6D %s %s %s %s %s %s\r\n", opt1, opt2, opt3, opt4, opt5, opt6);
}


/*
 * Get Device Address
 * Check Device Address
 */
void LoRa_GetChannelMask(void)
{
  /*
   * Execution Return Message
   * "Channel Mask = 0xffff, 0xffff, 0xffff, 0x00ff, 0x0000"
   */
  Serial5.printf("LRW 6E\r\n");
}

void LoRa_SystemSoftwareReset(void)
{
  /*
   * Execution Return Message
   * "System reset!"
   */
  Serial5.printf("LRW 70\r\n");
}

