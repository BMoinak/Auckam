#include <ESP8266WiFi.h>
#include "ESP8266WiFiMesh2.h"
#include <ESP8266WebServer.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
//#include <String.h>
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(14, 12); // RX, TX

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

const char serveri[] = "api.thingspeak.com";
const char apn[]  = "imis";
const char user[] = "";
const char pass[] = "";
unsigned int request_i = 0;
unsigned int response_i = 0;
int connected_flag=1;
const int port = 80;
   char reqv[60];
   char prevmsg[60]="";char prevmsgi[60]="";
   int count=1;
String manageRequest(String request);
/* Create the mesh node object */
String mesh_id="101";
ESP8266WiFiMesh2 mesh_node = ESP8266WiFiMesh2(ESP.getChipId(), manageRequest);
ESP8266WebServer server(80);

String manageRequest(String request)
{
  String m;int i, id=0;
  /* Print out received message */
  
  Serial.print("request: ");
  Serial.println(request);
   char req[60];
    strcpy(req, mesh_id.c_str());
  strcat(req,"FF0000000");
   if(request == req)
  {
     strcpy(prevmsgi, request.c_str());
    connected_flag=1;
    return("");
  }
  else if(/*(request.substring(0,3)).equals(mesh_id)&&*/(request.substring(7)).equals("FFFFFFFFF") || !((request.substring(5,7)).equals("01")))
  {
    char retur[60];
   strcpy(retur, (request.substring(0,5)).c_str());
   int cout=(request.substring(5,7)).toInt();
   String co=String(cout-1);
    if(co.length()==1)
    co="0"+co;
   strcat(retur, co.c_str());
  strcat(retur, (request.substring(7)).c_str());
  return retur;
  }
  /*if(request == reqv)
  {
    if(prevmsgi == req)
    connected_flag=1;

    else
    connected_flag=0;
     strcpy(prevmsgi, request.c_str());
    return("");
  }*/
 
  else{
     if((request.substring(0,3)).equals(mesh_id) && (request.substring(3,5)).equals("00"))
  {
   String requ=request.substring(request.length()-2);
    count++;
    if( prevmsg==requ.c_str() )
    {count--;
    //Serial.println(count);
    }
     //strcpy(prevmsgi, request.c_str());
     /*Serial.print("Request from: ");
     Serial.println(requ);
     Serial.print("Message: ");
     Serial.println(request.substring(7,request.length()-2));*/
     else
     {
      Serial.print(F("Connecting to "));
  Serial.print(serveri);
  if (!client.connect(serveri, port)) {
    Serial.println(" fail");
    delay(10000);
    //return;
  }
  Serial.println(" OK");
       client.print(String("GET ")+ "https://api.thingspeak.com/update?api_key=EG0D5995OJ43YH0L&field1=");
       client.print(request.substring(7,request.length()-2));
       client.print("&field2=");client.print(requ);
        client.print(String("Host: ")); 
        client.print(serveri); 
        client.print("\r\n");
       client.print("Connection: close\r\n\r\n");
     }

      strcpy(prevmsg,requ.c_str());
       return req;
  }
  }
  /*char retur[60];
   strcpy(retur, (request.substring(0,5)).c_str());
   int cout=(request.substring(5,7)).toInt();
   String co=String(cout-1);
    if(co.length()==1)
    co="0"+co;
   strcat(retur, co.c_str());
  strcat(retur, (request.substring(7)).c_str());
  return retur;*/
}
void setup()
{
 
Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(15,INPUT);
  pinMode(13,INPUT);
    strcpy(prevmsgi, "");
    SerialAT.begin(115200);
  delay(3000);
/*
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
   Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

 /* Serial.print(F("Connecting to "));
  Serial.print(serveri);
  if (!client.connect(serveri, port)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");
*/
  Serial.println("Setting up mesh node...");
  /* Initialise the mesh node */
  mesh_node.begin();
  Serial.println("Mesh is set");
}

void loop()
{
  /*Serial.print(F("Connecting to "));
  Serial.print(serveri);
  if (!client.connect(serveri, port)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");*/
  if(digitalRead(15)==HIGH || connected_flag == 0)
  {
    if(count>2)
   { count=1;
  //Serial.println("change");
   }
    strcpy(reqv, mesh_id.c_str());
    String co=String(count);
    if(co.length()==1)
    co="0"+co;
    strcat(reqv,co.c_str());
    
  strcat(reqv,"00FFFFFFFFF");
    connected_flag=0;
    Serial.println("try");
  mesh_node.attemptScan(reqv);
  }
 
  else
  { Serial.println("Read");
    mesh_node.acceptRequest();
delay(1000);
  }
}
