#ifndef BUOYMSGFRAME_H
#define BUOYMSGFRAME_H

/**
 * BUOY Terminal message frame format
**/
typedef struct __attribute__ ((packed)) BuoyId
{
	uint8_t  msgId;
	uint16_t sequenceNo;
	uint8_t  len;
	uint8_t  buoyId[8];
}t_BuoyId;

typedef union BuoyStatus
{
#if 1
	uint16_t data;
#else
	uint8_t data[2];
#endif
	struct
	{
		uint16_t activeComm       :1;
		uint16_t mainCommModule   :1;
		uint16_t pLoRaModule      :1;
		uint16_t gnssModule       :1;
		uint16_t BuoyPwr          :3;
		uint16_t sktLoRaRSSI      :3;
		uint16_t pLoRaRSSI        :3;
		uint16_t fishingBoatRSSI  :3;
	}b;
}t_BuoyStatus;

typedef union YearMon
{
  uint16_t data;
  struct
  {
    uint16_t year             :12;
    uint16_t month            :4;
  }b;
}t_YearMon;

typedef union DateHourMin
{
  uint16_t data;
  struct
  {
    uint16_t day              :5;
    uint16_t hour             :5;
    uint16_t minute           :6;
  }b;
}t_DateTimeMin;

typedef union Second
{
  uint8_t data;
  struct
  {
    uint8_t sec               :6;
    uint8_t rsvd              :2;
  }b;
}t_Second;

typedef struct __attribute__ ((packed)) UtcInfo
{
  t_YearMon     yearMon;
  DateHourMin   dateHourMin;
  t_Second      second;
}t_UtcInfo;

typedef union GPS_Status
{
  uint8_t data;
  struct
  {
    uint8_t lockSt            :1;
    uint8_t latitudePos       :1;
    uint8_t longitudePos      :1;
    uint8_t decimalPoint      :3;
    uint8_t rsvd              :2;
  }b;
}t_GPS_Status;

typedef union LatitudeInfo
{
  uint32_t data;
  struct
  {
    uint32_t decimal          :15;
    uint32_t point            :17;
  }b;
}t_LatitudeInfo;

typedef union LongitudeInfo
{
  uint32_t data;
  struct
  {
    uint32_t decimal          :15;
    uint32_t point            :17;
  }b;
}t_LongitudeInfo;

typedef struct __attribute__ ((packed)) GPSInfo
{
  t_LatitudeInfo    latitudeInfo;
  t_LongitudeInfo   longitueInfo;
}t_GPSInfo;

typedef union BuoyPacket
{
  uint8_t data[31];
  struct __attribute__ ((packed)){
	  uint8_t       sof;        // 0xAA
	  t_BuoyId      buoyId;
	  t_BuoyStatus  buoySt;
	  t_UtcInfo     timeInfo;
	  t_GPS_Status  gpsSt;
	  t_GPSInfo     gpsInfo;
	  uint8_t       checkSum;
	  uint8_t       eof;        // 0xFE
  }detail;
}t_BuoyPacket;

uint8_t sndData[2][31];

#endif	// BUOYMSGFRAME_H
