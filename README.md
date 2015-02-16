CurrentCost2NRF-Public
# DIY - Current Cost Home Energy Monitoring

### Project context:
I wish remotely monitoring my home electrical energy consumption. I currently have installed a Curret Cost with two sensor:
- Sensor 0: Monitoring whole home appliance,
- Sensor 1: Monitoring my whole HVAC installation.

I wish to collect both history and actual data from the current cost in order to have nice charts display directly in my phone or anywhere i get internet access. Basically category of data i want to monitor are: 
- dynamic data (each sensor is supposed to refresh instant data every 3 seconds).
- history of dynamic data (last 12hrs for example, in add set specific timing category: low & standard prices).
- history of 2 hours rolling data (standard CurrentCost raw data).
- history of daily rolling data (standard CurrentCost raw data).
- history of Monthly rolling data (standard CurrentCost raw data).

I wish to send data grabbed from the CurrentCost with an Arduino (nano) and send this data through 2.4ghz NRF24L01 payload. Arduino shall be power from 5V and Current Cost shall be powered from this power (usin step down Volatge). 

I wish to receive 2.4Ghz Payloads with my raspberry Pi and store this data in mySql Database.

I wish to exploit data stored in mySql database from a very nice html interface using:
- php for accessing database,
- mysql event scheduler in order to let mysql prepare specific table for reporting (needed for accelerating data loading to html).
- google charts in order to leverage my investment uderstanding them,
- Jquery mobile in order to try this. Seems very nice, does means easy? in conclusion I should say yes.

### Project Challenge:
#### Grabbing Serial Data on Arduino:
For sure the big challenge was the arduino program to developed. CurrentCost (CC128) send data though serial port. The CC128 Display Unit outputs ASCII text over its serial port at 57600 baud. Well, two issues: 
- this Arduino (nano) has only one Serial Hardware, usually used for debug and programming. Impossible to use properly Serial Software as the bauds rate was not accurate above 9600bds. Instead, Serial Software will be used at 9600bds debug (versus an incoming speed at 56000bds CC data).
- the Arduino RAM is very limited: the code has to very optimized to handle data. Storage of large Strings was not possible but the incoming Serial buffer has to be large enough to retrieve the incoming data. He code has been optimized managing immediately the trame input from the CC, this necessary management allowed to finally get a smallest trame to be decoded after. Indeed, historic data are huge amount of ASCII data, the dynamic data trame do not have such issue.
- the Execution speed is somehow low, I mean spending too much time processing the trame affect the incoming Serial accuracy. I tried to fasten as possible the data management needed to send the payloads through NRF24.
A this point, I obviously have to recognized that this challenge wouldn’t been if the primary choice was to use a raspberry directly connected to the CC128.  I assume this, my whole Home monitoring is based on this principle.

#### Sending Data from Arduino to Raspberry:
Pretty easy things in fact, as I’m using always the same concept based on NRF24L01 devices and specific payload type definition. The benefits to use payload definition is that the code to handle payload is simple. In other hand, the fact that payload is limited to 32bytes (NRF24 limitation) urge you to define small and efficient structure. When Arduino sends a payload, the raspberry get it and process it accordingly to his type.
I’ve defined only two payloads for this project:
- Dynamic data packet
  ```
//CC_SENSOR_DYN
typedef struct {
  uint8_t   sensor_id;     // Id of the sensor 
  float     temperature;   // temperature reported
  uint32_t  watts;         // Watts Energy for the sensor
  uint8_t   status;
}
PACK vn_cc_sensor_dyn_t;
  ```

- Historic data packet
  ```
//CC_SENSOR_HIST
typedef struct {
  uint8_t   sensor_id;     // Id of the sensor 
  uint8_t   hist_type;   //  reported
  uint16_t  hist_period;   //  reported
  float  watts;         // Watts Energy for the sensor
  uint8_t   status;
}
PACK vn_cc_sensor_hist_t;
```
  For further details, have a look at payload.h file. This file is used in Arduino and Raspberry code environment.

#### Sending Data Raspberry to MySQL Database:
Two tables have to be used, they are based on payload structure. A table for dynamic data and another one for the history trame.
```
CREATE TABLE IF NOT EXISTS CC_SENSOR_DYN (timestamp DATETIME, id INTEGER, temperature FLOAT, value INTEGER);
CREATE TABLE IF NOT EXISTS CC_SENSOR_HIST (timestamp DATETIME, id INTEGER, hist_type VARCHAR(1), hist_period INTEGER, value FLOAT);
```

  For further details, have a look at MaStationv2a.cpp example file.

