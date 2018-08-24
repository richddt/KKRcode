#include <OSCMessage.h>
#include <OSCBundle.h>
#include <DhcpV2_0.h>
#include <DnsV2_0.h>
#include <EthernetClientV2_0.h>
#include <EthernetServerV2_0.h>
#include <EthernetUdpV2_0.h>
#include <EthernetV2_0.h>
#include <utilV2_0.h>
#include <SPI.h>

/****************************[ NETWORK SETUP ]*********************************/

#define W5200_CS  10
#define SDCARD_CS 4

// Specify local settings
byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 10,0,0,5 }; // only use if DHCP doesn't work
IPAddress myIP(10,0,0,5); // only use if DHCP doesn't work

// Specify remote settings
int destPort=1234;

IPAddress testIp2(192,168,31,10); //Live destination IP - Chris's iPad

// Set the port for web diagnostics
EthernetServer wserver = EthernetServer(80);

// Set the testing port
EthernetServer testdata = EthernetServer(4949);

// Setup a UDP object
EthernetUDP Udp;

/****************************[ PERFORMANCE SETUP ]*****************************/



/****************************[ SENSORS SETUP ]*********************************/

//Set Channels & Selector Pins (Digital pins)
const byte sensor_channels=8;
const byte Selector_pin[3] = {5,6,7};

// Set the pins for the FruitBats (Analog Read pins)
const byte FruitBat[15]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14};

// Set Trigger Pin (Digital Pin)
const byte Trigger=9;

// Set Inhibit Pin (Digital Pin)
const byte Inhibit=8;

// Create output array of bytes
uint8_t EchoLocation[32]={0};
uint8_t FollowSensors[30]={0};

// Toggle logging to serial. 0=no, 1=yes
byte debug=1;

unsigned long tinit=0;
unsigned long tsetup=0;
unsigned long tosc=0;
unsigned long tcycle=0;
unsigned long tsprint=0;
unsigned long treads=0;
unsigned long ttprint;
unsigned long ttotal=0;

byte sensor_start_delay=250;
byte sensor_read_delay=50;
byte setup_delay=0;
byte add_delay=0;

uint8_t tcount=0;

int S0;
int S1;
int S2;



int analog_delay=10;

/****************************[ INITIALIZATION ]*********************************/
void setup(){ 
  
  // Start Serial
  if (debug==1) {Serial.begin(9600);}

  //Deselect the SD card
  if (debug==1) {Serial.print("SD Card:\t\t");}
  pinMode(SDCARD_CS,OUTPUT);
  digitalWrite(SDCARD_CS,HIGH);
  if (debug==1) {Serial.println("Disabled.");}

  //Start Ethernet
  if (debug==1) {Serial.print("Ethernet:\t\t");}
  Ethernet.begin(myMac);
  if (debug==1) {Serial.println(Ethernet.localIP());}

  //Open a TCP Port
  if (debug==1) {Serial.print("Opening Server Port:\t");}
  wserver.begin();
  if (debug==1) {Serial.println("Server Port Open");}

  //Open a TCP Port
  if (debug==1) {Serial.print("Opening testdata:\t");}
  testdata.begin();
  if (debug==1) {Serial.println("testdata Open");}

  //Open a UDP Port
  if (debug==1) {Serial.print("Opening UDP Port:\t");}
  Udp.begin(1234);
  if (debug==1) {Serial.println("1234");}
  
  if (debug==1) {Serial.println();}

  tsetup=millis()-tsetup;
  setup_delay=max(sensor_start_delay-tsetup,1);
  if (debug==1) {
    Serial.print("Setup took ");Serial.print(tsetup);Serial.println("ms.");
    Serial.print("Waiting an additional ");Serial.print(setup_delay);Serial.println("...");
  }
  delay(setup_delay); // The sensors have a 250ms startup time

  if (debug==0){Serial.println("FruitBat, Awaken!");}
  
  // Send First Trigger 
  digitalWrite(Trigger,HIGH);
  delayMicroseconds(20); //Bring high for 20uS or more to start a range reading
  digitalWrite(Trigger,LOW);  

  delay(50); // The first reading takes 50ms extra
}


/****************************[ MAIN LOOP ]*********************************/
void loop(){
  // Initial system timestamp starts at trigger
  tinit=millis(); //timestamp
  
  // Send out the last measurements as OSC
  osc_sendmsg();
  tosc=millis()-tinit; // OSC timing
  
  // Write out the last measurements
  Serial.print("|\t");
  for (byte n=0;n<30;n++){
  Serial.print(FollowSensors[n]);Serial.print(" ");
  }
  Serial.println();

  tsprint=millis()-tinit-tosc; // serial write timing
  
  // Enbable Sensor data flow (Disable Inhibit)
  digitalWrite(Inhibit, LOW);

 /* 
  // Iterate through each Sensor Channel to read measurements
    // Reading 0 
    // Set the digital selector pins
    digitalWrite(Selector_pin[0], LOW);
    digitalWrite(Selector_pin[1], LOW);
    digitalWrite(Selector_pin[2], LOW);
 */
    
    //delay(analog_delay);
    // Get Sensor Data
    for (byte i=0; i<15; i++){
      FollowSensors[i]=analogRead(FruitBat[i])/2;
    }
    
/*  EchoLocation[0]=analogRead(FruitBat[0])/2;
    EchoLocation[8]=analogRead(FruitBat[1])/2;
    EchoLocation[16]=analogRead(FruitBat[2])/2;
    EchoLocation[24]=analogRead(FruitBat[3])/2;

    

    // Reading 1
    digitalWrite(Selector_pin[0], HIGH);
    digitalWrite(Selector_pin[1], LOW);
    digitalWrite(Selector_pin[2], LOW);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[1]=analogRead(FruitBat[0])/2;
    EchoLocation[9]=analogRead(FruitBat[1])/2;
    EchoLocation[17]=analogRead(FruitBat[2])/2;
    EchoLocation[25]=analogRead(FruitBat[3])/2
    
    // Reading 2
    digitalWrite(Selector_pin[0], LOW);
    digitalWrite(Selector_pin[1], HIGH);
    digitalWrite(Selector_pin[2], LOW);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[2]=analogRead(FruitBat[0])/2;
    EchoLocation[10]=analogRead(FruitBat[1])/2;
    EchoLocation[18]=analogRead(FruitBat[2])/2;
    EchoLocation[26]=analogRead(FruitBat[3])/2;  
  
    // Reading 3
    digitalWrite(Selector_pin[0], HIGH);
    digitalWrite(Selector_pin[1], HIGH);
    digitalWrite(Selector_pin[2], LOW);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[3]=analogRead(FruitBat[0])/2;
    EchoLocation[11]=analogRead(FruitBat[1])/2;
    EchoLocation[19]=analogRead(FruitBat[2])/2;
    EchoLocation[27]=analogRead(FruitBat[3])/2;

    // Reading 4  
    // Set the digital selector pins
    digitalWrite(Selector_pin[0], LOW);
    digitalWrite(Selector_pin[1], LOW);
    digitalWrite(Selector_pin[2], HIGH);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[4]=analogRead(FruitBat[0])/2;
    EchoLocation[12]=analogRead(FruitBat[1])/2;
    EchoLocation[20]=analogRead(FruitBat[2])/2;
    EchoLocation[28]=analogRead(FruitBat[3])/2;

    // Reading 5
    digitalWrite(Selector_pin[0], HIGH);
    digitalWrite(Selector_pin[1], LOW);
    digitalWrite(Selector_pin[2], HIGH);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[5]=analogRead(FruitBat[0])/2;
    EchoLocation[13]=analogRead(FruitBat[1])/2;
    EchoLocation[21]=analogRead(FruitBat[2])/2;
    EchoLocation[29]=analogRead(FruitBat[3])/2;

    // Reading 6
    digitalWrite(Selector_pin[0], LOW);
    digitalWrite(Selector_pin[1], HIGH);
    digitalWrite(Selector_pin[2], HIGH);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[6]=analogRead(FruitBat[0])/2;
    EchoLocation[14]=analogRead(FruitBat[1])/2;
    EchoLocation[22]=analogRead(FruitBat[2])/2;
    EchoLocation[30]=analogRead(FruitBat[3])/2;  

    // Reading 7
    digitalWrite(Selector_pin[0], LOW);
    digitalWrite(Selector_pin[1], LOW);
    digitalWrite(Selector_pin[2], HIGH);

    delay(analog_delay);
    // Get Sensor Data
    EchoLocation[7]=analogRead(FruitBat[0])/2;
    EchoLocation[15]=analogRead(FruitBat[1])/2;
    EchoLocation[23]=analogRead(FruitBat[2])/2;
    EchoLocation[31]=analogRead(FruitBat[3])/2;
  */

  treads=millis()-tinit-tosc-tsprint; //sensor read timing

  // Disable Sensors: Inhibit data flow until the next read cycle
  digitalWrite(Inhibit, HIGH);
  tcount++;
  if (tcount==100){
    Serial.println("---------------------------------------------------------------------------------------------------------------------------------------------------------");
    Serial.println("Init\tOSC\tPrint\tData\tTiming\tCycle\tFreq\t\tSensor Readings");
    Serial.println("---------------------------------------------------------------------------------------------------------------------------------------------------------");
    tcount=0;
  }
  Serial.print(tinit);          Serial.print("\t");
  Serial.print(tosc);           Serial.print("\t");
  Serial.print(tsprint);        Serial.print("\t");
  Serial.print(treads);         Serial.print("\t");
  
  ttprint=millis()-tinit-tosc-tsprint-treads; // time output timing
  Serial.print(ttprint);        Serial.print("\t");
  
  tcycle=millis()-tinit;  // total time per cycle, before delay
  Serial.print(tcycle);         Serial.print("\t");

  // If ttotal is less than sensor_read_delay, we need to add some delay:
  add_delay=max(sensor_read_delay-byte(tcycle),1);
  delay(add_delay);
  
  ttotal=1000/(millis()-tinit);  // frequency achieved this cycle
  Serial.print(ttotal);         Serial.print("\t");
  
  // Send Trigger for next cycle
  digitalWrite(Trigger,HIGH);
  delay(2); //Bring high for 20uS or more to start a range reading
  digitalWrite(Trigger,LOW);  
}

/****************************[ FUNCTIONS ]*********************************/

void osc_sendmsg(){
  //for (int n=0;n<15;n++){
      //FollowSensors[n]=EchoLocation[n+1];
  //}
  OSCMessage msg("/Follow/Sensors");
  msg.add(FollowSensors,30);
  Udp.beginPacket(testIp2, destPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

}
