#ifndef SKTLORA_H
#define SKTLORA_H

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
void LoRa_SetActivationMode(uint8_t *opt1);

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
void LoRa_TXMsg(uint8_t *opt1, uint8_t opt2, uint8_t opt3);

/*
 * Send a message for Link Check Request
 */
void LoRa_SendLinkCheckRequest(void);

/*
 * Check device EUI
 */
void LoRa_GetDeviceEUI(void);

/*
 * Check App EUI
 */
void LoRa_GetAppEUI(void);

/*
 * Check Tx data rate
 */
void LoRa_GetTxDataRate(void);

/*
 * Join request ( Initialze the variable ) Untested
 */
void LoRa_Join_request(void);

/*
 * Check Uplink Re-transmission number of Confirmed Message
 */
void LoRa_GetReTx(void);

/*
 * Set Repeater Support
 * Repeater Support setting
 * (default : 0)
 * Option 1
 *   (1) 0 : Repeater Support off
 *   (2) 1 : Repeater Support on
 *   - Ex) LRW 47 0
 */
void LoRa_SetRepeaterSupport(uint8_t *opt);


/*
 * Get Repeater Support
 * Check Repeater Support
 *   - Ex) LRW 48
 */
void LoRa_GetRepeaterSupport(void);


/*
 * Get the Last RSSI & SNR
 */
void LoRa_GetLastRssiSnr(void);


/*
 * Check LoRaWAN Class settings
 */
void LoRa_GetClassType(void);

void LoRa_TXbinary(uint8_t opt1, uint8_t opt2, uint8_t opt3, uint8_t *opt4);

void LoRa_GetFirmwareVer(void);


void LoRa_SetDebugMsg(uint8_t opt);

/*
 * Get Data Information
 * Check Data Information
 */
void LoRa_GetDataInformation(void);

/*
 * Set TX Power
 * TX Power setting
 * Option 1
 *   - See the document LoRaWAN Regional Parameters provided by Lora alliance.
 *   - 0: 14dBm, 14: 0dBm(SKT/KR), 0:30dBm, 15: 0dBm(AU, Brazil)
 *   - LOM202A : Max 14dBm, LOM204A : Max 20dBm
 *   - Ex) LRW 6C 5
 */
void LoRa_SetTXPower(uint8_t *opt);

/*
 * Get TX Power & Channel Info
 * Check TX Power & Channel Info
 */
void LoRa_GetTXPower_ChInfo(void);

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
void LoRa_SetChannelTxPower(uint8_t opt1, uint8_t opt2);

/*
 * ��ü ä�� Tx power ���� �� Ȯ�� 
 * (Only for SKT KR version)
 */
void LoRa_GetChannelTxPower(void);

void LoRa_GetDebugMsg(void);

/*
 * Set Network ID
 * Network ID setting
 * Option 1
 *   - Net ID: 3 byte Hexadecimal
 *   - Ex) LRW 65 123456
 */
void LoRa_SetNetworkID(uint8_t *opt);

/*
 * Set Device Address
 * Device Address setting
 * Option 1
 *   - Devaddr: 4 byte Hexadecimal
 *   - Ex) LRW 66 12345678
 */
void LoRa_SetDeviceAddress(uint8_t *opt);

/*
 * Set Network Session Key
 * Network Session Key setting
 * Option 1
 *   - NwkSKey: 16 byte Hexadecimal
 *   - Ex) LRW 67 12345678901234567890123456789012
 */
void LoRa_SetNetworkSessionKey(uint8_t *opt);

/*
 * Set Application Session Key
 * Application Session Key setting
 * Option 1
 *   - AppSKey: 16 byte Hexadecimal
 *   - Ex) LRW 68 12345678901234567890123456789012
 */
void LoRa_SetApplicationSessionKey(uint8_t *opt);

/*
 * Get Device Address
 * Check Device Address
 */
void LoRa_GetDeviceAddress(void);

/*
 * Set Channel Mask
 * Option 1, ~ Option 6
 *   (1) Channel mask table 0 ~ 4
 *   (2) 4 digits Hexa format
 *   - Ex) "LRW 6D ff00 0000 0000 0000 0002 0000" (for LA915) <= OK
 *         "LRW 6D 0007, 0000, 0000, 0000, 0000, 0000" <= OK
 */
void LoRa_SetChannelMask(uint8_t *opt1, uint8_t *opt2, uint8_t *opt3, uint8_t *opt4, uint8_t *opt5, uint8_t *opt6);

/*
 * Get Device Address
 * Check Device Address
 */
void LoRa_GetChannelMask(void);

void LoRa_SystemSoftwareReset(void);

#endif	// SKTLORA_H
