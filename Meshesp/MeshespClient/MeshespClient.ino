#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "ESP8266WiFiMesh3.h"
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(14, 12);
//#include <String.h>
//unsigned int request_i = 0;
//unsigned int response_i = 0;
int reqpin=4;
int input_flag=0;
int request_flag=0;
char requesti[60];
char prevmsg[60];
String manageRequest(String request);
//String mess="";
String mesh_id;
String id,nextid,previd;
unsigned long signal_time=0;
ESP8266WiFiMesh3 mesh_node = ESP8266WiFiMesh3(ESP.getChipId(), manageRequest);
ESP8266WebServer server(80);
/*int pin = 4;
unsigned long duration;
unsigned long previousMillis = 0; 
const long interval = 1000;
uint8_t entry = 0;
int count = 0;
int sec = 0;
*/
String manageRequest(String request)
{
  String m;int i;
	/* Print out received message */
  Serial.print("Request: ");
  Serial.println(request);
    char relay[60]; 
    char req[60];
    strcpy(req, mesh_id.c_str());
  strcat(req,id.c_str());strcat(req,previd.c_str());
  strcat(req,"FFFFFFFFF");
   strcpy(relay, mesh_id.c_str());
  strcat(relay,"FF0000000");
  if( request==relay )
  {
     Serial.println("IN RELAY");
    strcpy(requesti,"");
       return "";
  }
  else if( request==req )
  {
    request_flag=1;
    Serial.println("In Request");
    //strcpy(requesti, request.c_str());
    signal_time= millis();
    return relay;
  }
 
  else{
   if((request!=prevmsg) && (request.substring(0,3)).equals(mesh_id) && ((request.substring(5,7)).equals(previd) || (request.substring(5,7)).equals(nextid)))
  { 
  strcpy(prevmsg, request.c_str());
  strcpy(requesti, (request.substring(0,5)).c_str());
   strcat(requesti, id.c_str());
  strcat(requesti, (request.substring(7)).c_str());
	  return relay;
	  }
   if(request == prevmsg)
   {
    strcpy(requesti,"");
       return relay;
   }
	  }
   return request;
}

void setup()
{
 
Serial.begin(115200);
delay(10);
SerialAT.begin(115200);
pinMode(reqpin,OUTPUT);
 // pinMode(pin, INPUT);
  mesh_id="101";
  int ide=1;
  id=String(ide);
    if(id.length()==1)
    id="0"+id;
     previd=String(ide-1);
    if(previd.length()==1)
    previd="0"+previd;
    nextid=String(ide+1);
    if(nextid.length()==1)
    nextid="0"+nextid;
   delay(10);
	Serial.println("\n\nSetting up mesh node...");
	/* Initialise the mesh node */
	mesh_node.begin();
  Serial.println("Mesh is set");
}

void loop()
{
 /* duration = pulseIn(pin, HIGH);

  unsigned long currentMillis = millis();
//  Serial.print("entry : ");
//  Serial.println(entry);
  
  if(duration > 0)
     count++;

    
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    Serial.print("count : ");
    Serial.println(count);
    Serial.print("sec : ");
    Serial.println(sec++);
  }*/
  if(request_flag==1)
  {
   // Serial.println("Read");
    digitalWrite(reqpin,HIGH);
    if(SerialAT.available())
  {
    strcpy(requesti,mesh_id.c_str());
    strcat(requesti, "00");
    strcat(requesti, id.c_str());
    strcat(requesti,SerialAT.readString().c_str());
    strcat(requesti, id.c_str());
    request_flag=0;
    Serial.print(requesti);
    strcpy(prevmsg,requesti);
  }
  digitalWrite(reqpin,LOW);
  }
  else{
   if(String(requesti).equals("")) 
   { Serial.println("accept");
	mesh_node.acceptRequest();
	delay(1000);
	}}
 if(!String(requesti).equals("")){
  Serial.println(requesti);
	mesh_node.attemptScan(requesti);}

}
