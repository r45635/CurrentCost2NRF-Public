/*
 Copyright (C) 2014 Vincent Cruvellier <vincent(at)cruvellier(dot)eu>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#include <inttypes.h>
#define PAYLOAD_VERSION 2.3;
// Version 2.1 - Add of Payload Type for WS2355
// Version 2.2 - Add of Payload for Water Counter
// Version 2.2 - Add of Payload for CurrentCost Envir
// Version 2.3 - Add of Payload for CurrentCost Envir Historic data

#define PACK __attribute__((packed))  // Important declaration to make sure cross-platform compilation of structure are equivalent

#define SET_ADMIN_CODE 120         // Arbitrary Code for Cautious Function
#define STATION_WATER_COUNTER_ID 5 // Id For the Water Counter Arduino Station

// PayloadType: Enumaration of Payload Type to be managed
enum PayloadType {
  SENSOR_DHT,
  SENSOR_DUST,
  SENSOR_MQ2,
  SENSOR_AQS,
  SENSOR_LIGHT,
  SENSOR_SOUND,
  SENSOR_STATION,
  SENSOR_UV,
  SENSOR_MQ131,
  SENSOR_BMP085,
  WS2355_TEMP,
  WS2355_HUM,
  WS2355_RAIN,
  WS2355_WIND,
  RESET_HARD,
  WATER_COUNT,
  WATER_SET_COUNT,
  CC_SENSOR_DYN,
  CC_SENSOR_HIST
}
PACK;

const char strWindDirection[16][4] =
{
  "N  ", "NNE", "NE ", "ENE",
  "E  ", "ESE", "SE ", "SSE",
  "S  ", "SSW", "SW ", "WSW",
  "W  ", "WNW", "NW ", "NNW"
};

typedef uint8_t vn_payload_type;    // Type of Payload see PayloadType
typedef uint8_t vn_payload_version; // Version of Payload, Not used.
typedef uint16_t vn_payload_CRC;    // 16bits CRC for the payload, Not Used as Nrf24 CRC & ACK options is used during tranmission

//CC_SENSOR_HIST
typedef struct {
  uint8_t   sensor_id;     // Id of the sensor 
  uint8_t   hist_type;   //  reported
  uint16_t  hist_period;   //  reported
  float  watts;         // Watts Energy for the sensor
  uint8_t   status;
}
PACK vn_cc_sensor_hist_t;

//CC_SENSOR_DYN
typedef struct {
  uint8_t   sensor_id;     // Id of the sensor 
  float     temperature;   // temperature reported
  uint32_t  watts;         // Watts Energy for the sensor
  uint8_t   status;
}
PACK vn_cc_sensor_dyn_t;

//WATER_SET_COUNT 
typedef struct {
  uint32_t  value;        // Total Value to set in Liter (L) in the eEprom
  uint8_t   COUNTER_ID;   // Counter ID
  uint8_t   ADMIN_CODE;   // Necessary ADMIN CODE related to SET_ADMIN_CODE define
  uint8_t   status;
}
PACK vn_water_set_count_t;

//  WATER_COUNT
typedef struct {
  uint32_t  value;        // Total Value in Liter (L) 
  uint8_t   COUNTER_ID;   // Counter ID
  uint8_t   status;
}
PACK vn_water_count_t;

//  WS2355_TEMP
typedef struct {
  float     temperature;        // Temperature in C°
  uint8_t   WS2355_ID;			// Station ID from WS2355
  uint8_t   status;
}
PACK vn_ws2355_temp_t;

//  WS2355_HUM
typedef struct {
  float     humidity;        // Humidity in %
  uint8_t	WS2355_ID;       // Station ID from WS2355
  uint8_t   status;
}
PACK vn_ws2355_hum_t;


//  WS2355_RAIN
typedef struct {
  float     totalrain;        // Total rain in mm
  uint8_t   status;
}
PACK vn_ws2355_rain_t;

//  WS2355_WIND
typedef struct {
  float     speed;        		// Total rain in mm
  uint8_t	direction;			// Index code for Wind direction, see table
  uint8_t   status;
}
PACK vn_ws2355_wind_t;


// SENSOR_DHT
typedef struct {
  float     humidity;           // Humidity in Percentage
  float     temperature;        // Temperature in CÂ°
  float     dewpoint;           // Dewpoint in CÂ°
  uint8_t   status;
}
PACK vn_sensor_dht_t;

// SENSOR_DUST
typedef struct {
  uint16_t   LowPpulseOccupancyDUST;   // Low Pulse Occupancy value from Dust Sensor
  float      RatioDUST;  			          // Ratio Value from Dust Sensor
  float      ConcentrationDUST;    		      // PPM Concentration Value from Dust Sensor
  uint8_t    status;
}
PACK vn_sensor_dust_t;

// SENSOR_MQ2
typedef struct {
  float     LPG_Concentration;   			// LPG (liquefied petroleum gas) Concentration (PPM) from MQ2 Sensor
  float     CO_Concentration;  			    // CO Concentration (PPM) from MQ2 Sensor
  float     SMOKE_Concentration;    		// SMOKE PPM Concentration Value from MQ2 Sensor
  uint16_t  AnalogValue;					// Raw ANALOG Value, no interpretation
  uint8_t   status;
}
PACK vn_sensor_mq2_t;

// SENSOR_AQS
typedef struct {
  uint16_t  AnalogValue;					// Raw ANALOG Value, no interpretation
  uint8_t   status;
}
PACK vn_sensor_aqs_t;

// SENSOR_LIGHT
typedef struct {
  uint16_t  AnalogValue;					// Raw ANALOG Value, no interpretation - it is not Lux Value
  uint8_t   status;
}
PACK vn_sensor_light_t;

// SENSOR_SOUND
typedef struct {
  uint16_t  AnalogValue;					// Raw ANALOG Value, no interpretation
  uint16_t  MaxValue;             // Max Value within timeframe
  uint8_t   status;
}
PACK vn_sensor_sound_t;

// SENSOR_STATION
typedef struct {
  uint16_t    Station_id;             // Sensor Node
  uint16_t    Power_Voltage;				// AnalogValue from Power Supply
  uint8_t     status;
}
PACK vn_sensor_station_t;

// SENSOR_UV
typedef struct {
  uint16_t   AnalogValue;					// Raw ANALOG Value, no interpretation
  uint8_t    IndexUV;              // UV index conversion
  uint8_t    status;
}
PACK vn_sensor_uv_t;

// SENSOR_MQ131
typedef struct {
  uint16_t    AnalogValue;					// Raw ANALOG Value, no interpretation
  float       OzonePPM;						//
  float       ChlorinePPM;
  uint8_t     status;
}
PACK vn_sensor_mq131_t;

// SENSOR_BMP085
typedef struct {
  float     temperature;              // Temperature in CÂ°
  uint32_t  Pressure;                 // Pressure in Pa
  float     Altitude;                 // Altitude in meters
  uint8_t   status;
}
PACK vn_sensor_bmp085_t;

// Payload Structure
struct Payload {
  vn_payload_type      type;
  vn_payload_version   Version;
  uint8_t              Station_id;				
  uint8_t              Node;
  union {
    vn_sensor_dht_t 	  SENSOR_DHT;
    vn_sensor_dust_t 	  SENSOR_DUST;
    vn_sensor_mq2_t 	  SENSOR_MQ2;
    vn_sensor_aqs_t	  SENSOR_AQS;
    vn_sensor_light_t	  SENSOR_LIGHT;
    vn_sensor_sound_t     SENSOR_SOUND;
    vn_sensor_station_t	  SENSOR_STATION;
    vn_sensor_uv_t        SENSOR_UV;
    vn_sensor_mq131_t     SENSOR_MQ131;
    vn_sensor_bmp085_t    SENSOR_BMP085;
    vn_ws2355_temp_t	  WS2355_TEMP;
    vn_ws2355_hum_t       WS2355_HUM;
    vn_ws2355_rain_t	  WS2355_RAIN;
    vn_ws2355_wind_t	  WS2355_WIND;
    vn_water_count_t      WATER_COUNT;
    vn_water_set_count_t  WATER_SET_COUNT;
    vn_cc_sensor_dyn_t    CC_SENSOR_DYN;
    vn_cc_sensor_hist_t   CC_SENSOR_HIST;
  }
  PACK data;
#ifdef CRC_PAYLOAD
  vn_payload_CRC payloadCRC;
#endif
}
PACK;
