#include <SPI.h>             // For NRF24L01 SPI interface
#include "RF24.h"            // NRF24L01 Lib
#include "sensor_payload.h"  // Payload Definition, enclosed in MaStation Git Repository
#include <SoftwareSerial.h>

//Entree:
//<msg><src>CC128-v1.48</src><dsb>00427</dsb><time>07:07:20</time><hist><dsw>00429</dsw><type>1</type><units>kwhr</units><data><sensor>0</sensor><h448>5.990</h448><h446>5.827</h446><h444>4.836</h444><h442>8.513</h442></data><data><sensor>1</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>2</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>3</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>4</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>5</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>6</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>7</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>8</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data><data><sensor>9</sensor><h448>0.000</h448><h446>0.000</h446><h444>0.000</h444><h442>0.000</h442></data></hist></msg>
// Sortie
//<<0,h,448,5.990><0,h,446,5.827><0,h,444,4.836><0,h,442,8.513><<1,h,448,0.000><1,h,446,0.000><1,h,444,0.000><1,h,442,0.000><<2,h,448,0.000><2,h,446,0.000><2,h,444,0.000><2,h,442,0.000><<3,h,448,0.000><3,h,446,0.000><3,h,444,0.000><3,h,442,0.000><<4,h,448,0.000><4,h,446,0.000><4,h,444,0.000><4,h,442,0.000><<5,h,448,0.000><5,h,446,0.000><5,h,444,0.000><5,h,442,0.000><<6,h,448,0.000><6,h,446,0.000><6,h,444,0.000><6,h,442,0.000><<7,h,448,0.000><7,h,446,0.000><7,h,444,0.000><7,h,442,0.000><<8,h,448,0.000><8,h,446,0.000><8,h,444,0.000><8,h,442,0.000><<9,h,448,0.000><9,h,446,0.000><9,h,444,0.000><9,h,442,0.000>

#define SERIAL_RX_BUFFER_SIZE 1024 // latest Arduino libaries > 1.5
#define SERIAL_TX_BUFFER_SIZE 64
#define ID_OF_MAX_SENSOR_ID 1 // 2 sensors management (0 & 1)


#define WAIT_FOR_HIST       1
#define WAIT_FOR_TYPE       2
#define WAIT_FOR_DATA       3
#define WAIT_FOR_SENSOR     4
#define WAIT_FOR_BEG_DATA   5

byte waitForStatus = WAIT_FOR_HIST;

SoftwareSerial dbgSerial(3, 4); // RX, TX

String inputString = "";         // a string to hold incoming data
String debugString = "";         // a string to hold incoming data
String inString, inString2, inString3 ;
boolean stringComplete = false;  // whether the string is complete
boolean stringDataComplete = false;  // whether presence of string data type
boolean verbose = false;
byte sensor_id;


// NRF24 L01
// Hardware configuration
//
/*
radio 7 (miso) to uno d12 (pin 18) or mega d50
radio 6 (mosi) to uno d11 (pin 17) or mega d51
radio 5 (sck)  to uno d13 (pin 19) or mega d52
radio 4 (csn)  to uno d10 (pin 16) or mega d48
radio 3 (ce)   to uno d9  (pin 15) or mega d49
radio 2 (3.3v) to uno 3.3v
radio 1 (gnd)  to uno gnd
*/
// **********************************************************************
// * SPI
// **********************************************************************
// * NRF24L01+
// Module NRF24L01+ (version + => I/O 5V Tolerant)
// !!!WARNING!!! powered at 3.3V, a capacitor is mandatory to be added in order to have module working properly.
// WIKI: http://www.dx.com/fr/p/nrf24l01-rf-board-b-296420
// DATASHEET: http://www.wvshare.com/downloads/accBoard/NRF24L01-RF-Board-B.7z
#define NRF24L01_CE_PIN	 9  	// nRF24L01 SPI CE Pin 
#define NRF24L01_CSN_PIN  10 	// nRF24L01 SPI CSN Pin 
//*****************************
// * NRF24L01 CONFIGURATION
#define NRF24L01_USE_ACK true	// Set auto ACK configuration Options.
#define NRF24L01_STATION_ID 10  // Id of the station
#define NRF24L01_CHANNEL 120  // Radio Channel

//#define DEBUG_INTERFACE 1

RF24 radio(NRF24L01_CE_PIN, NRF24L01_CSN_PIN); // Set up nRF24L01 radio on SPI bus plus pins CE, CSN
// NRF24 Radio pipe addresses for the 2 nodes to communicate, only one used actually
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E3LL }; //
// Receiver Station @NRF24 => 0xF0F0F0F0E3LL
// Emitter  Station @NRF24: 0xF0F0F0F0E1LL


Payload payloadr = (Payload) {
  SENSOR_STATION
}; // Payload use for Emission
Payload payload_hist  = (Payload) {
  CC_SENSOR_HIST
}; // Payload use for Reception
Payload payload_dyn = (Payload) {
  CC_SENSOR_DYN
}; // Payload use for Counter Emission

/*********************************************************************************
 * serial_putc()
 *  
********************************************************************************/
int serial_putc( char c, FILE * )
{
  dbgSerial.write( c );
  return c;
}

/*********************************************************************************
 * printf_begin()
 *  
********************************************************************************/
void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

/*********************************************************************************
 * doSendMsg()
 *  Sent Msg To Station
********************************************************************************/
bool doSendMsg()
{
  radio.stopListening();
  radio.openWritingPipe(pipes[0]);
  bool done = (radio.write(&payloadr, sizeof(payloadr)), true);
#ifdef DEBUG_INTERFACE
  if (done) {
    printf("OK %d ", payloadr.type);
  }
  else {
    printf("NOK ");
  }
  printf("Tr %d bytes / max %u -> [%u/%u/%u]\n", sizeof(payloadr), radio.getPayloadSize(), done, true, radio.isAckPayloadAvailable());
#endif
  radio.startListening();
}

/*********************************************************************************
 * serialEvent()
 *  called every time a char is received from the Hardware Serial port
********************************************************************************/
void serialEvent() {

  while (Serial.available()) {
    char inChar = (char)Serial.read();     // get the new byte:
    if ((inChar != '\n') || (inChar != '\r')) {
      inputString += inChar;     // add it to the inputString:
    }

    if (inChar == '>') { // end of a balise
      switch (waitForStatus) {

        case WAIT_FOR_HIST:
          if (inputString.endsWith(F("<hist>"))) {
            stringDataComplete = false; // find begin of a data trame
            stringComplete = false;
            inputString = F("<hist>"); // discard of all previous data;
            waitForStatus = WAIT_FOR_TYPE;
          }
          break;

        case WAIT_FOR_TYPE:
          if (inputString.endsWith(F("</type>"))) {
            if (inputString.endsWith(F("1</type>"))) {
              waitForStatus = WAIT_FOR_DATA;
            } else {
              // Managing only of Type 1 data (only Watts NRJ)
              waitForStatus = WAIT_FOR_DATA;
              inputString = "";
              debugString = "";
            }
          }
          break;

        case WAIT_FOR_DATA:
          if (inputString.endsWith(F("</hist>"))) {
            stringDataComplete = true; // find begin of a data trame
            waitForStatus = WAIT_FOR_HIST;
          }
          if (inputString.endsWith(F("<data>"))) {
            waitForStatus = WAIT_FOR_SENSOR;
          }
          break;

        case WAIT_FOR_SENSOR:
          if (inputString.endsWith(F("</sensor>"))) {
            int t0 = inputString.lastIndexOf(F("<sensor>"));
            int t1 = inputString.lastIndexOf(F("</sensor>"));
            if ((t0 == -1) || (t1 == -1)) { // if one of previous fields was not found then stop the handling
              waitForStatus = WAIT_FOR_HIST;
            }
            inString3 = inputString.substring(t0 + 8, t1); // get Sensor id String
            //sensor_id = inString3.toInt(); // Store and convert to byte the sensor Id
            debugString += "<";
            waitForStatus = WAIT_FOR_BEG_DATA;
          }
          break;

        case WAIT_FOR_BEG_DATA:
          if (inputString.endsWith(F("</data>"))) {
            waitForStatus = WAIT_FOR_DATA;

          } else 
          {
            int t0 = inputString.lastIndexOf("<");
            inString2 = inputString.substring(0, t0);
            inString = inputString.substring(t0);
            t0 = inString.indexOf("/");
            if (t0 > 0) {
              char outstr[15];
              inString2.toCharArray(outstr, inString2.length() + 1);
              //float val = atof(outstr);
              //dtostrf(val,7, 3, outstr);
              debugString += '<' + inString3 + "," + inString.substring(t0 + 1, t0 + 2) + ',' + inString.substring(t0 + 2, inString.length() - 1) + ',' + outstr + '>';
            }
            inputString = "";
            waitForStatus = WAIT_FOR_BEG_DATA;
          }
      }
      break;

    } // endof switch

    if (inChar == '\n') {
      stringComplete = true;
      waitForStatus = WAIT_FOR_HIST;
      break;
    }
  }
}

/*********************************************************************************
 * setup()
 *
********************************************************************************/
void setup() {
  dbgSerial.begin(9600); // for debuging Maximum allowed by Software Emulation
  dbgSerial.println(F("Hello debug Serial"));
  printf_begin(); // Print NRF24 config
  // NRF24 Initialization
  dbgSerial.print(F("\nPreparing NRF24L01 interface\n"));
  radio.begin();
  radio.setRetries( 15, 15);
  radio.setChannel(NRF24L01_CHANNEL);
  radio.openReadingPipe(1, pipes[0]);
  radio.printDetails();
  radio.startListening();
  delay(200);
  // NRF24L01 Hello World Send
  radio.powerUp();
  dbgSerial.print(F("//NRF24 Module Sensor Enabled.\n"));
  payloadr.Version = PAYLOAD_VERSION; // initialize data for all payload -- PAYLOAD_VERSION; from sensor_payload.h
  payload_dyn.Version = PAYLOAD_VERSION;
  payload_hist.Version = PAYLOAD_VERSION;;
  payloadr.Station_id = NRF24L01_STATION_ID; // initialize data for all payload
  payload_dyn.Station_id = NRF24L01_STATION_ID; // initialize data for all payload
  payload_hist.Station_id = NRF24L01_STATION_ID; // initialize data for all payload
  payloadr.Node = 0; // Master primary
  payload_dyn.Node = 0; // Master primary
  payload_hist.Node = 0; // Master primary
  payloadr.type =  SENSOR_STATION;
  payloadr.data.SENSOR_STATION.Station_id = 11; // Id defined in payload
  payloadr.data.SENSOR_STATION.Power_Voltage = 11 ;
  payloadr.data.SENSOR_STATION.status = 11;
  doSendMsg();
  // put your setup code here, to run once:
  Serial.begin(57600);  // Since last version the Baud rate is 9600
  dbgSerial.print(F("CurrentCost Envir Com data pipe opened."));
  dbgSerial.print(F("Enter \n to siwtch verbose. ."));
}


/*********************************************************************************
 * loop()
 *
********************************************************************************/
void loop() {

  if (dbgSerial.available()) {
    char inChar = (char)dbgSerial.read();
    if (inChar == '\n') {
      verbose = !verbose;
      if (verbose) {
        dbgSerial.println("");
        dbgSerial.print(F("Verbose Debug enabled."));
      }
      else  {
        dbgSerial.println("");
        dbgSerial.print(F("Verbose Debug disabled."));
      }
    }
  }

  if (stringComplete) { // a string hreceived from serial interface has to be decoded
    if ((inputString.indexOf("<msg>") == 0) && (!stringDataComplete)) {
      // Simple Check trame start by msg and is not a Hist Trame
      if (verbose) {
        dbgSerial.println("");
        dbgSerial.print(F("<"));
      }

      // Retrieve the sensor ID
      int t0 = inputString.indexOf(F("<sensor>"));
      int t1 = inputString.indexOf(F("</sensor>"));
      if ((t0 == -1) || (t1 == -1)) { // if one of previous was not found then stop the handling
        goto something_was_wrong;
      }
      inString = inputString.substring(t0 + 8, t1); // Retrieve id string
      payload_dyn.data.CC_SENSOR_DYN.sensor_id = (byte) inString.toInt(); // store id  in byte
      if (verbose) {
        dbgSerial.print("S=");
        dbgSerial.print(inString.toInt());
      }

      // Retrieve the watts value
      t0 = inputString.indexOf(F("<watts>"));
      t1 = inputString.indexOf(F("</watts>"));
      if ((t0 == -1) || (t1 == -1)) { // if one of previous was not found then stop the handling
        goto something_was_wrong;
      }
      inString = inputString.substring(t0 + 7, t1); // get watts value String
      payload_dyn.data.CC_SENSOR_DYN.watts = inString.toInt(); // store watts in Int
      if (verbose) {
        dbgSerial.print(F(".W="));
        dbgSerial.print(inString.toInt()); // display the instant watts value
      }

      // Retrieve the temperature value
      t0 = inputString.indexOf(F("<tmpr>"));
      t1 = inputString.indexOf(F("</tmpr>"));
      if ((t0 == -1) || (t1 == -1)) { // if one of previous was not found then stop the handling
        goto something_was_wrong;
      }
      inString = inputString.substring(t0 + 6, t1); // Get the temperature value
      payload_dyn.data.CC_SENSOR_DYN.temperature = inString.toFloat(); // store temperature in Float
      if (verbose) {
        // display the temperature period value
        dbgSerial.print(F(".T=")); dbgSerial.print(inString);  dbgSerial.print(F(">"));
      }
      // PAYLOAD Emission Section
      radio.stopListening();
      radio.openWritingPipe(pipes[0]);
      payload_dyn.data.CC_SENSOR_DYN.status = 22;
      bool done = (radio.write(&payload_dyn, sizeof(payload_dyn)));
      radio.startListening();

      inputString = "";
      stringComplete = false;
    } // endif ((inputString.indexOf("<msg>") == 0) && (!stringDataComplete))

    /************************* History TRAME Management */
    else if (stringDataComplete)
    {
      if (verbose) { //display the HIST debug trame to process
        dbgSerial.println();
        // dbgSerial.println("HIST>: "); dbgSerial.println(debugString);
      }
      int t0 = debugString.indexOf("<<");
      // mini block scheme is <<$Sensor_id,$hist_type,$hist_period,$hist_value>
      while (debugString.indexOf(",", t0) > -1) {
        int t1 = debugString.indexOf(",", t0); // just before $sensor_id
        if ((t0 == -1) || (t1 == -1)) { // if one of previous was not found then stop the handling
          goto something_was_wrong;
        }
        sensor_id = debugString.substring(t0 + 2, t1 - 1).toInt();
        if (sensor_id > ID_OF_MAX_SENSOR_ID) {
          if (verbose) {
            dbgSerial.println();
            dbgSerial.println(F("Reached Max Sensor Id."));
          }
          break;
        }
        payload_hist.data.CC_SENSOR_HIST.sensor_id = (byte) sensor_id;
        if (verbose) {
          dbgSerial.print(F("S="));
          dbgSerial.print(sensor_id);
        }
        t0 = t1 + 1;
        char hist_type = debugString.charAt(t1 + 1); // $hist_type
        payload_hist.data.CC_SENSOR_HIST.hist_type = hist_type;
        if (verbose) {
          dbgSerial.print(F(".T="));
          dbgSerial.print(hist_type);
        }
        t0 = debugString.indexOf(",", t1 + 3); // Normally t1+2 is a "," look for next just before $hist_val
        int hist_period = debugString.substring(t1 + 3, t0).toInt(); // $hist__period
        payload_hist.data.CC_SENSOR_HIST.hist_period = hist_period;
        if (verbose) {
          dbgSerial.print(F(".P="));
          dbgSerial.print(hist_period);
        }
        t1 = debugString.indexOf(">", t0); // end of the mini block scheme
        float val = debugString.substring(t0 + 1, t1).toFloat();
        payload_hist.data.CC_SENSOR_HIST.watts = val;
        if (verbose) {
          dbgSerial.print(".V=");
          dbgSerial.println(val);
        }
        t0 = t1 + 2;
        // PAYLOAD Emission Section
        radio.stopListening();
        radio.openWritingPipe(pipes[0]);
        payload_hist.data.CC_SENSOR_HIST.status = 22;
        bool done = (radio.write(&payload_hist, sizeof(payload_hist)));
        radio.startListening();
      } //end of while
      goto cleanup_for_new_trame;
    } // end if (stringDataComplete)
    else { // don't why we have been called or an error coocured during handling
something_was_wrong:
      if (verbose) { 
        // Display the inputString Trame received
        dbgSerial.println(); dbgSerial.print("??: "); dbgSerial.println(inputString);
      }
cleanup_for_new_trame:      
      debugString = "";
      inputString = "";
      stringDataComplete = false;
      stringComplete = false;
      //doSendMsg();
    } // end else
  }
}

// (c) 2015 vincent(at)cruvellier.eu
