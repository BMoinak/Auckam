// Core library for code-sense - IDE-based
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(ROBOTIS) // Robotis specific
#include "libpandora_types.h"
#include "pandora.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#include "LRF.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#include "Arduino.h"
#elif defined(RFDUINO) // RFduino specific
#include "Arduino.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#include "application.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#   error Platform not defined
#endif // end IDE

// Include application, user and local libraries
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

//Flow Sensor Variables
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
//unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
int count=0;float data=0.0;

// The time library provides all the tools.
// No need for reinventing them!
//#include "time.h"
#include "RTC_Library.h"
#include "NTP_WiFi.h"
char ssid[]="Auckam ACT";
char password[]="1234qwer()*&";
int m=0;
// Define variables and constants
DateTime myRTC;
time_t myEpochNTP, myEpochRTC;
tm myTimeNTP, myTimeRTC;
uint32_t counter = 0;

// First time update of RTC
bool flagRTC = true;

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
//IPAddress hostIp(184,106,153,149);
// ThingSpeak API key
String writeAPIKey = "PKA2QR3R02NAFL2I";

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize WiFi Client
WiFiClient client;
// Prototypes
void printWifiStatus();
void updts(String);
uint32_t sendNTPpacket(IPAddress& address);

// Add setup code
// Add loop code
void flow () // Interrupt function
{
   flow_frequency++;
}

void setup()
{
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    delay(100);
    Serial.println();

    myRTC.begin();
    myRTC.setTimeZone(tz_IST);

    // Set communication pins for CC3000
   pinMode(29, INPUT_PULLUP);
   attachInterrupt(29, flow, FALLING); // Interrupt is fired whenever button is pressed
  // Start WiFi
    // Connect to Wifi network:
    Serial.print("Connecting to network ");
    Serial.print(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println(" done");

    Serial.print("Waiting for IP address...");

    while (WiFi.localIP() == INADDR_NONE)
    {
        Serial.print(".");
        delay(300);
    }

    IPAddress myIP = WiFi.localIP();
    Serial.print(myIP);
    Serial.println(" done");
    ntppr();
}

// Add loop code
String ntppr()
{
    bool flagNTP =0;
    while(1)
    {
      flagNTP =getTimeNTP(myEpochNTP);
      if(flagNTP==1)
      break;
    }
    Serial.print("NTP ? ");
    Serial.println(flagNTP, DEC);

    if (flagNTP)
    {
        // Set time to RTC, only once
        if (flagRTC)
        {
            myRTC.setTime(myEpochNTP);
            flagRTC = false;
            Serial.println("*** CC3200 RTC updated!");
        }

        myEpochRTC = myRTC.getTime();
        convertEpoch2Structure(myEpochNTP, myTimeNTP);
        convertEpoch2Structure(myEpochRTC, myTimeRTC);
        String w=formatDateTime2String("%I%M%S", myRTC.getLocalTime());
        /*while(m!=0)
        {
          updts(w);
        }*/
        return w;
        }
    
    }

void loop()
{

if(flow_frequency!=0)
{ 
  String start=ntppr();
 String fl= flowing();
  String endi=ntppr();
  Serial.println(start+" "+fl+" "+endi);
}




  
  
  /*while(1){
  if(m==1)
 { ntppr();
 }
 if(m==1)
 continue;
 delay(1000);
  if(Serial.available()){
    m=Serial.parseInt();
  } 
  
 delay(1000);
}
*/



}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void updateThingSpeak(String tsData)
{
  Serial.println("Updating ThingSpeak");
  if (client.connect(thingSpeakAddress, 80)) {
    Serial.println("Connected to ThingSpeak!");
    Serial.println();
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    m=0;
    Serial.println("Updated!");
    failedCounter = 0;
  } else {
    failedCounter++;
    client.stop();
 
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
  }
  
  lastConnectionTime = millis();
}
void updts(String k)
{

 if (client.available())
  { 
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected() && lastConnected)
  {
    
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
    if(client.connected()==0 ||(client.connected()==1 && failedCounter==0))
  {
    updateThingSpeak("field1="+k);Serial.println("Update= "+k);
   
  }
  // Check if WiFi needs to be restarted
if (failedCounter > 3 ) {failedCounter = 0; setup();}


 
  lastConnected = client.connected();

}
String flowing()
{
while(1)
{

currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 500))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
   }
   if(l_hour!=0)
   {
    data=data+l_hour;
    count++;
    continue;
   }
   if(data!=0.0 && l_hour==0.0)
   {
     data=data/count;
     String mio="";
     count=0;
     String dat = String(data,DEC);
     int i;
     for(int i=0;i<dat.length();i++)
     {
        if(dat[i]!='.')
        mio=mio+dat[i];
      else
     { mio=mio+dat[i]+dat[i+1]+dat[i+2];break;}
     }
     return mio;
   }
}
}

