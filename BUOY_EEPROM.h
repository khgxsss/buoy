#ifndef _BUOY_EEPROM_H_
#define _BUOY_EEPROM_H_

#define ADDR_EEPROM_BASE        0x4000
#define ADDR_SIZE_INT           4
#define ADDR_SIZE_SHORT         2

#define ADDR_GPS_BAUD           (ADDR_EEPROM_BASE-ADDR_SIZE_INT)
#define ADDR_LORA_BAUD          (ADDR_EEPROM_BASE-ADDR_SIZE_INT*2)
#define ADDR_GPS_TIMEOUT        (ADDR_EEPROM_BASE-ADDR_SIZE_INT*3)


#endif

