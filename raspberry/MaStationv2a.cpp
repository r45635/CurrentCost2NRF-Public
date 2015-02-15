//
// NRF24 Notes on raspberry: http://rlogiacco.wordpress.com/tag/wireless/
// Compilation
// MySql installation / See: http://raspberrypi-easy.blogspot.fr/2013/12/how-to-use-database-mysql-with.html
// Compilation avec MySQL
// g++ -Wall -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -L../librf24/  -lrf24 MaStationv2a.cpp  `mysql_config --cflags` `mysql_config --libs` -o MaStationv2a


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include "../RF24.h"
#include "sensor_payload.h"
#include <mysql/mysql.h>

using namespace std;
RF24 radio("/dev/spidev0.0",8000000 , 25);  //spi device, speed and CSN,only CSN is NEEDED in RPI
const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xE8E8F0F0E1LL };

Payload payload = Payload();

#define DATABASE_NAME  "STATION_DATA"
#define DATABASE_USERNAME "XXXXXXX"
#define DATABASE_PASSWORD "XXXXXXXXX"

MYSQL *mysql1;
char SQLstring[100];
uint8_t pipeNo;
int ack_id = 99;

//*****************************************
//*****************************************
//********** CONNECT TO DATABASE **********
//*****************************************
//*****************************************
void mysql_connect (void)
{
	//initialize MYSQL object for connections
	mysql1 = mysql_init(NULL);
	if(mysql1 == NULL) {	
		fprintf(stderr, "ABB : %s\n", mysql_error(mysql1));
		return;
	}
	//Connect to the database
	if(mysql_real_connect(mysql1, "localhost", DATABASE_USERNAME, DATABASE_PASSWORD, DATABASE_NAME, 0, NULL, 0) == NULL) { 
		fprintf(stderr, "%s\n", mysql_error(mysql1)); 
	}
	else { 
		printf("Database connection successful.\r\n");
	}
}

//**********************************************
//**********************************************
//********** DISCONNECT FROM DATABASE **********
//**********************************************
//**********************************************
void mysql_disconnect (void)
{
	mysql_close(mysql1);
	printf( "Disconnected from database.\r\n");
}


void setup(void){
	//Prepare the radio module
	printf("\nPreparing NRF24L01 interface\n");
	radio.begin();
	radio.setRetries( 15, 15);
	radio.setChannel(120);
	radio.enableAckPayload();
	//radio.disableCRC();
	radio.setAutoAck(true);
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(2,pipes[1]);
	radio.openReadingPipe(3,pipes[2]);
	radio.openReadingPipe(1,pipes[0]);
	radio.printDetails();
	printf("\nPreparing MySQL interface.\n");
	mysql_connect();
	if ((mysql1 != NULL)) {
		sprintf(SQLstring,"CREATE TABLE IF NOT EXISTS CC_SENSOR_DYN (timestamp DATETIME, id INTEGER, temperature FLOAT, value INTEGER);");
		if (!mysql_query(mysql1, SQLstring)) { printf("SQL CC_SENSOR_DYN Table is Ok: %s\n",SQLstring); }  else { printf("SQL CC_SENSOR_DYN NOk: %s\n",SQLstring); printf("%s\n", mysql_error(mysql1)); }

		sprintf(SQLstring,"CREATE TABLE IF NOT EXISTS CC_SENSOR_HIST (timestamp DATETIME, id INTEGER, hist_type VARCHAR(1), hist_period INTEGER, value FLOAT);");
		if (!mysql_query(mysql1, SQLstring)) { printf("SQL CC_SENSOR_HIST Table is Ok: %s\n",SQLstring); }  else { printf("SQL CC_SENSOR_HIST NOk: %s\n",SQLstring); printf("%s\n", mysql_error(mysql1)); }
				
		}
	radio.startListening();
	printf("\nNow Listening...\n");

}

/*
************** LOOP Procedure **************
*/
void loop(void) {
	if (radio.available(&pipeNo)) {
		int len = radio.getDynamicPayloadSize();  // Size of th epayload to read
		radio.read(&payload, len);	// Read the payload
		printf("%d:",pipeNo); 
		sprintf(SQLstring,";");	// Initialise SQLString to a default value "nothing"
		switch(payload.type) {    
		case SENSOR_STATION:
			printf("SENSOR_STATION:");
			printf("packet %d:%d ID:%u PowerVoltage:%u status:%u \n", len, sizeof(payload), payload.data.SENSOR_STATION.Station_id, payload.data.SENSOR_STATION.Power_Voltage, payload.data.SENSOR_STATION.status);
			break;	
		case CC_SENSOR_DYN:
			printf("CC_SENSOR_DYN:");
			printf("packet %d:%d Sensor:%d Temp:%f Watts:%d status:%u \n", len, sizeof(payload), payload.data.CC_SENSOR_DYN.sensor_id, 
			payload.data.CC_SENSOR_DYN.temperature, payload.data.CC_SENSOR_DYN.watts, payload.data.CC_SENSOR_DYN.status);
			sprintf(SQLstring,"INSERT INTO CC_SENSOR_DYN VALUES(NOW(),%d,%5.1f,%d)",payload.data.CC_SENSOR_DYN.sensor_id, payload.data.CC_SENSOR_DYN.temperature, payload.data.CC_SENSOR_DYN.watts);
			break;		
		case CC_SENSOR_HIST:
			printf("CC_SENSOR_HIST:");
			printf("packet %d:%d Sensor:%d Period:%lc:%i Watts:%3.3f status:%u \n", len, sizeof(payload), payload.data.CC_SENSOR_HIST.sensor_id, 
			payload.data.CC_SENSOR_HIST.hist_type, payload.data.CC_SENSOR_HIST.hist_period, payload.data.CC_SENSOR_HIST.watts, payload.data.CC_SENSOR_HIST.status);
			sprintf(SQLstring,"INSERT INTO CC_SENSOR_HIST VALUES(NOW(),%d,'%lc',%i, %3.3f)",payload.data.CC_SENSOR_HIST.sensor_id, payload.data.CC_SENSOR_HIST.hist_type, payload.data.CC_SENSOR_HIST.hist_period, payload.data.CC_SENSOR_HIST.watts);
			break;		

		default:
			printf("Unknown message.\n");
			break;
		}

		// Executing the SQL instruction. There is one payload per loop, therefore one SQL instructions per loop.
		if ((mysql1 != NULL) &&  (strcmp(SQLstring,";") != 0)) { // Check mySql connection availability and Sql Instructions validity
			if (!mysql_query(mysql1, SQLstring)) { 	
				//printf(" >SQL Ok: %s\n",SQLstring); 
			}  
			else { 
				printf("  >SQL NOk: %s\n",SQLstring); 	// Error Occured
				printf("%s\n", mysql_error(mysql1)); 	// Print mysql Error
			}
		}
	}
	delay(20);
}


/*
************** MAIN PROGRAM ENTRY **************
*/
int main(int argc, char** argv){
	setup();
	while(1)
	loop();

	return 0;
}

