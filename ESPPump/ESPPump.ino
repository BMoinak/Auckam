#include <SoftwareSerial.h>
//#include <UnixTime.h>
int wk=0;
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/kentaylor/WiFiManager
#include <DoubleResetDetector.h>  //https://github.com/datacute/DoubleResetDetector
#include <NTPtimeESP.h>
#define DRD_TIMEOUT 10
int cu;
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

NTPtime NTPch("ch.pool.ntp.org");
strDateTime dateTime;
String StartTime, EndTime,Time;
const char* ssid     = "Auckam";

const char* password = "1234qwer()*&";

const char* host = "auckam.reifiersoft.com";

bool initialConfig = false;
const int httpPort = 80;
//const char* privateKey = "....................";
unsigned long previousMillis = 0;
uint8_t entry = HIGH;
int indata[230] = {0};
char gsm_format[150] = {'P','A','-','0','0','0','.','0','0','-','0','0','.','0','0','0','-','0','0','.','0','0','0','-','0','0','0','.','0','0','!',
'P','B','-','0','0','0','.','0','0','-','0','0','.','0','0','0','-','0','0','.','0','0','0','-','0','0','0','.','0','0','!',
'P','C','-','0','0','0','.','0','0','-','0','0','.','0','0','0','-','0','0','.','0','0','0','-','0','0','0','.','0','0','!',
'A','E','-','0','0','0','0','0','-','0','0','0','0','0','-','0','0','0','0','0','!',  
'R','E','-','0','0','0','0','0','-','0','0','0','0','0','-','0','0','0','0','0'}; 
String activeenergy, avoltage, bvoltage, cvoltage;
char max_data[40] = {'0'}; 

int j = 0;
int k = 0;
int a = 0;
int b = 1;
int c = 0;
SoftwareSerial swSer(14, 12);

uint8_t mac[6];
String mac_id;
int stateLED = LOW;
int stateButton;
int previous = LOW;
long time1 = 0;
long debounce = 250;
//uint8_t rst = 0;
uint8_t rst_count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
swSer.begin(9600);
  Serial.print("PUMP: ");
  Serial.println(digitalRead(4));
  espconnect();
 ESP.wdtEnable(999999);
  delay(100);
  while(1)
  {
   get_pump_status();
//dateTime = NTPch.getNTPtime(5.5,0);
send_pumpdata();
if(entry == HIGH)
  {
       wifi_postdata();
  }
  my_delay(10000);
  ESP.wdtFeed();
  }
}
void loop()
{
}
void get_pump_status()
{
  Serial.println("Get pump status");
   WiFiClient client;
//yield();
  while (!client.connect(host, httpPort)) {
    Serial.println("connection failed");espconnect();
  }
  String url = "/pumpdata.php?opfunc=PUMPSTATUS&macId=";
  url += mac_id;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\nAccept: */*\r\n\r\n");
    String line;delay(500);
  while(client.available()){
    line = client.readStringUntil('\r');
  }  
  
  if(line.length() == 3)
    digitalWrite(4, HIGH);
    else if(line.length() == 4)
     digitalWrite(4, LOW);
     Serial.print("PUMP : ");
  Serial.println(digitalRead(4));
  ESP.wdtFeed();
}
void send_pumpdata()
{
Serial.print("wk= ");Serial.println(wk);

dateTime = NTPch.getNTPtime(5.5,0);
    NTPch.printDateTime(dateTime);
    Time = String(dateTime.year) + '-' + String(dateTime.month) + '-' + String(dateTime.day) + 'T' + String(dateTime.hour) + ':' + String(dateTime.minute) + ':' + String(dateTime.second); 
    if(digitalRead(4) == HIGH)
 { if(wk==0 && Time!="1073644968-0-0T9:96:32")
  { 
 wk=1;
    //send start time
    
    StartTime = Time;
   Serial.print("connecting to  at send_pumpdata ");
  Serial.println(host);
 WiFiClient client;
//yield();
  while (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    espconnect();
  }
String url = "/pumpdata.php?opfunc=PUMPLOG&macId=";
  url += mac_id;
  url += "&pumpStatus=ON";
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\nAccept: */*\r\n\r\n");
    delay(500);
    // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r'); 
  }  

  }}
else if(digitalRead(4) == LOW && wk==1  && !Time.equals("1073644968-0-0T9:96:32"))
  {
    wk=0;
    /*if((post_data == 1) && (EEPROM.read(2) == 0))
    {*/
      energy_meter1();
      energy_meter2();
    //send start time
    
    EndTime =Time;
    //send end time and load data
    String max_volt_a = String(max_data[1]) + String(max_data[2]) + String(max_data[3]) + '.' + String(max_data[4]) + String(max_data[5]);
    String max_curr_a = String(max_data[6]) + String(max_data[7]) + '.' + String(max_data[8]) + String(max_data[9]) + String(max_data[10]);
    String max_volt_b = String(max_data[12]) + String(max_data[13]) + String(max_data[14]) + '.' + String(max_data[15]) + String(max_data[16]);
    String max_curr_b = String(max_data[17]) + String(max_data[18]) + '.' + String(max_data[19]) + String(max_data[20]) + String(max_data[21]);
    String max_volt_c = String(max_data[23]) + String(max_data[24]) + String(max_data[25]) + '.' + String(max_data[26]) + String(max_data[27]);
    String max_curr_c = String(max_data[28]) + String(max_data[29]) + '.' + String(max_data[30]) + String(max_data[31]) + String(max_data[32]);
   WiFiClient client;

  while(!client.connect(host, httpPort)) {
    Serial.println("connection failed");espconnect();
      }

  // We now create a URI for the request
  String url = "/pumpdata.php?opfunc=PUMPLOG&macId=";
  url += mac_id;
  url += "&pumpStatus=OFF&endTime=";
  url += EndTime;
  url += "&maxVoltage=A:";
  url += max_volt_a;
  url += "ANDB:";
  url += max_volt_b;
  url += "ANDC:";
  url += max_volt_c;
  url += "&maxCurrent=A:";
  url += max_curr_a;
  url += "ANDB:";
  url += max_curr_b;
  url += "ANDC:";
  url += max_curr_c;
  url += "&unit=";
  url += activeenergy;
  url += "&startTime=";
  url += StartTime;
  Serial.println(url);
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\nAccept: */*\r\n\r\n");
    
    // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    delay(100);
  }}
  ESP.wdtFeed();
}
void wifi_postdata()
{
  
  energy_meter1();
  energy_meter2();
  a=0, c=0;
  avoltage = String(gsm_format[3]) + String(gsm_format[4]) + String(gsm_format[5]) + String(gsm_format[6]) + String(gsm_format[7]) + String(gsm_format[8]);
  String acurrent = String(gsm_format[10]) + String(gsm_format[11]) + String(gsm_format[12]) + String(gsm_format[13]) + String(gsm_format[14]) + String(gsm_format[15]);
  String apowerfactor = String(gsm_format[17]) + String(gsm_format[18]) + String(gsm_format[19]) + String(gsm_format[20]) + String(gsm_format[21]) + String(gsm_format[22]);
  String apower = String(gsm_format[24]) + String(gsm_format[25]) + String(gsm_format[26]) + String(gsm_format[27]) + String(gsm_format[28]) + String(gsm_format[29]);
  
  bvoltage = String(gsm_format[34]) + String(gsm_format[35]) + String(gsm_format[36]) + String(gsm_format[37]) + String(gsm_format[38]) + String(gsm_format[39]);
  String bcurrent = String(gsm_format[41]) + String(gsm_format[42]) + String(gsm_format[43]) + String(gsm_format[44]) + String(gsm_format[45]) + String(gsm_format[46]);
  String bpowerfactor = String(gsm_format[48]) + String(gsm_format[49]) + String(gsm_format[50]) + String(gsm_format[51]) + String(gsm_format[52]) + String(gsm_format[53]);
  String bpower = String(gsm_format[55]) + String(gsm_format[56]) + String(gsm_format[57]) + String(gsm_format[58]) + String(gsm_format[59]) + String(gsm_format[60]);
  
  cvoltage = String(gsm_format[65]) + String(gsm_format[66]) + String(gsm_format[67]) + String(gsm_format[68]) + String(gsm_format[69]) + String(gsm_format[70]);
  String ccurrent = String(gsm_format[72]) + String(gsm_format[73]) + String(gsm_format[74]) + String(gsm_format[75]) + String(gsm_format[76]) + String(gsm_format[77]);
  String cpowerfactor = String(gsm_format[79]) + String(gsm_format[80]) + String(gsm_format[81]) + String(gsm_format[82]) + String(gsm_format[83]) + String(gsm_format[84]);
  String cpower = String(gsm_format[86]) + String(gsm_format[87]) + String(gsm_format[88]) + String(gsm_format[89]) + String(gsm_format[90]) + String(gsm_format[91]);
  
  activeenergy = String(gsm_format[96]) + String(gsm_format[97]) + String(gsm_format[98]) + String(gsm_format[99]) + String(gsm_format[100]);
  String reactiveenergy = String(gsm_format[117]) + String(gsm_format[118]) + String(gsm_format[119]) + String(gsm_format[120]) + String(gsm_format[121]);
 // delay(1000);
  //++value;

  Serial.print("connecting to at WiFi Post Data");
  Serial.println(host);
 WiFiClient client;
  const int httpPort = 80;
  while(!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    espconnect();
  }
   String url = "/pumpdata.php?opfunc=UPDATEREAL&macId=";
  url += mac_id;
  url += "&realValue=R:";
  url += avoltage;
  url += ':';
  url += acurrent;
  url += ':';
  url += apowerfactor;
  url += ':';
  url += apower;
  url += "ANDY:";
  url += bvoltage;
  url += ':';
  url += bcurrent;
  url += ':';
  url += bpowerfactor;
  url += ':';
  url += bpower;
  url += "ANDB:";
  url += cvoltage;
  url += ':';
  url += ccurrent;
  url += ':';
  url += cpowerfactor;
  url += ':';
  url += cpower;
  url += "ANDAE:";
  url += activeenergy;
  url += "ANDRE:";
  url += reactiveenergy;
 client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\nAccept: */*\r\n\r\n");

while(client.available()){
    String line = client.readStringUntil('\r');
   // Serial.print(line);
  }
  ESP.wdtFeed();
}
void energy_meter1()
{
      while(a < 220)
      {
       if(swSer.available() > 0)
        {
           indata[a] = swSer.read();
          // Serial.print(indata[a]);
          // Serial.println(a);
           a++;
        }
      }
     ESP.wdtFeed();
}
void energy_meter2()
{
      while(c < 220)
      {         
        //PHASE A DATA
            if(indata[c] == 250)
            {
              //Serial.println(a);
              if(c-32 >= 0)
              {
                if(indata[c-32] == 65)
                {
                   // PHASE A VOLTAGE
                    gsm_format[3] = (char)(indata[c-31] + 48);
                    gsm_format[4] = (char)(indata[c-30] + 48);
                    gsm_format[5] = (char)(indata[c-29] + 48);
                    gsm_format[7] = (char)(indata[c-28] + 48);
                    gsm_format[8] = (char)(indata[c-27] + 48);
                   // Serial.println("VA");
      
                    // PHASE A CURRENT
                    gsm_format[10] = (char)(indata[c-26] + 48);
                    gsm_format[11] = (char)(indata[c-25] + 48);
                    gsm_format[13] = (char)(indata[c-24] + 48);
                    gsm_format[14] = (char)(indata[c-23] + 48);
                    gsm_format[15] = (char)(indata[c-22] + 48);
                   // Serial.println("CA");
      
                    // PHASE A POWER FACTOR
                    gsm_format[17] = (char)(indata[c-21] + 48);
                    if(gsm_format[17] == '1')
                    gsm_format[17] = '-';
                    else
                    gsm_format[17] = '+';
                    gsm_format[18] = (char)(indata[c-20] + 48);
                    gsm_format[20] = (char)(indata[c-19] + 48);
                    gsm_format[21] = (char)(indata[c-18] + 48);
                    gsm_format[22] = (char)(indata[c-17] + 48);
//                    Serial.println("PFA");
      
                    // PHASE A ACTIVE POWER
                    gsm_format[24] = (char)(indata[c-16] + 48);
                    gsm_format[25] = (char)(indata[c-15] + 48);
                    gsm_format[26] = (char)(indata[c-14] + 48);
                    gsm_format[28] = (char)(indata[c-13] + 48);
                    gsm_format[29] = (char)(indata[c-12] + 48);
//                    Serial.println("APA");
      
                    // PHASE A CONSUMED ACTIVE ENERGY
                    gsm_format[96] = (char)(indata[c-11] + 48);
                    gsm_format[97] = (char)(indata[c-10] + 48);
                    gsm_format[98] = (char)(indata[c-9] + 48);
                    gsm_format[99] = (char)(indata[c-8] + 48);
                    gsm_format[100] = (char)(indata[c-7] + 48);
//                    Serial.println("AEA");
      
                    // PHASE A CONSUMED REACTIVE ENERGY
                    gsm_format[117] = (char)(indata[c-6] + 48);
                    gsm_format[118] = (char)(indata[c-5] + 48);
                    gsm_format[119] = (char)(indata[c-4] + 48);
                    gsm_format[120] = (char)(indata[c-3] + 48);
                    gsm_format[121] = (char)(indata[c-2] + 48);
//                    Serial.println("REA"); 
                    b++;
              
                 }
                }
              }

              if(indata[c] == 251)
              {
                //Serial.println(a);
                if(c-32 >= 0)
                {
                if(indata[c-32] == 66)
                {
                   // gsm_format[c-32] = 'B';
      
                    // PHASE B VOLTAGE
                    gsm_format[34] = (char)(indata[c-31] + 48);
                    gsm_format[35] = (char)(indata[c-30] + 48);
                    gsm_format[36] = (char)(indata[c-29] + 48);
                    gsm_format[38] = (char)(indata[c-28] + 48);
                    gsm_format[39] = (char)(indata[c-27] + 48);
//                    Serial.println("VB");
                    
                    // PHASE B CURRENT
                    gsm_format[41] = (char)(indata[c-26] + 48);
                    gsm_format[42] = (char)(indata[c-25] + 48);
                    gsm_format[44] = (char)(indata[c-24] + 48);
                    gsm_format[45] = (char)(indata[c-23] + 48);
                    gsm_format[46] = (char)(indata[c-22] + 48);
//                    Serial.println("CB");
      
                    // PHASE B POWER FACTOR
                    gsm_format[48] = (char)(indata[c-21] + 48);
                    if(gsm_format[48] == '1')
                    gsm_format[48] = '-';
                    else
                    gsm_format[48] = '+';
                    gsm_format[49] = (char)(indata[c-20] + 48);
                    gsm_format[51] = (char)(indata[c-19] + 48);
                    gsm_format[52] = (char)(indata[c-18] + 48);
                    gsm_format[53] = (char)(indata[c-17] + 48);
//                    Serial.println("PFB");
      
                    // PHASE B ACTIVE POWER
                    gsm_format[55] = (char)(indata[c-16] + 48);
                    gsm_format[56] = (char)(indata[c-15] + 48);
                    gsm_format[57] = (char)(indata[c-14] + 48);
                    gsm_format[59] = (char)(indata[c-13] + 48);
                    gsm_format[60] = (char)(indata[c-12] + 48);
//                    Serial.println("APB");
      
                    // PHASE B CONSUMED ACTIVE ENERGY
                    gsm_format[102] = (char)(indata[c-11] + 48);
                    gsm_format[103] = (char)(indata[c-10] + 48);
                    gsm_format[104] = (char)(indata[c-9] + 48);
                    gsm_format[105] = (char)(indata[c-8] + 48);
                    gsm_format[106] = (char)(indata[c-7] + 48);
//                    Serial.println("AEB");
      
                    // PHASE B CONSUMED REACTIVE ENERGY
                    gsm_format[123] = (char)(indata[c-6] + 48);
                    gsm_format[124] = (char)(indata[c-5] + 48);
                    gsm_format[125] = (char)(indata[c-4] + 48);
                    gsm_format[126] = (char)(indata[c-3] + 48);
                    gsm_format[127] = (char)(indata[c-2] + 48); 
//                    Serial.println("REB");
                    b++;     
                } 
              }
            }
            
            if(indata[c] == 252)
              {
                //Serial.println(a);
                if(c-32 >= 0)
                {
                  if(indata[c-32] == 67)
                  {
      
                    // PHASE C VOLTAGE
                    gsm_format[65] = (char)(indata[c-31] + 48);
                    gsm_format[66] = (char)(indata[c-30] + 48);
                    gsm_format[67] = (char)(indata[c-29] + 48);
                    gsm_format[69] = (char)(indata[c-28] + 48);
                    gsm_format[70] = (char)(indata[c-27] + 48);
//                    Serial.println("VC");
                    
                    // PHASE C CURRENT
                    gsm_format[72] = (char)(indata[c-26] + 48);
                    gsm_format[73] = (char)(indata[c-25] + 48);
                    gsm_format[75] = (char)(indata[c-24] + 48);
                    gsm_format[76] = (char)(indata[c-23] + 48);
                    gsm_format[77] = (char)(indata[c-22] + 48);
//                    Serial.println("CC");
      
                    // PHASE C POWER FACTOR
                    gsm_format[79] = (char)(indata[c-21] + 48);
                    if(gsm_format[79] == '1')
                    gsm_format[79] = '-';
                    else
                    gsm_format[79] = '+';
                    gsm_format[80] = (char)(indata[c-20] + 48);
                    gsm_format[82] = (char)(indata[c-19] + 48);
                    gsm_format[83] = (char)(indata[c-18] + 48);
                    gsm_format[84] = (char)(indata[c-17] + 48);
//                    Serial.println("PFC");
      
                    // PHASE C ACTIVE POWER
                    gsm_format[86] = (char)(indata[c-16] + 48);
                    gsm_format[87] = (char)(indata[c-15] + 48);
                    gsm_format[88] = (char)(indata[c-14] + 48);
                    gsm_format[90] = (char)(indata[c-13] + 48);
                    gsm_format[91] = (char)(indata[c-12] + 48);
//                    Serial.println("APC");
      
                    // PHASE C CONSUMED ACTIVE ENERGY
                    gsm_format[108] = (char)(indata[c-11] + 48);
                    gsm_format[109] = (char)(indata[c-10] + 48);
                    gsm_format[110] = (char)(indata[c-9] + 48);
                    gsm_format[111] = (char)(indata[c-8] + 48);
                    gsm_format[112] = (char)(indata[c-7] + 48);
//                    Serial.println("AEC");
      
                    // PHASE C CONSUMED REACTIVE ENERGY
                    gsm_format[129] = (char)(indata[c-6] + 48);
                    gsm_format[130] = (char)(indata[c-5] + 48);
                    gsm_format[131] = (char)(indata[c-4] + 48);
                    gsm_format[132] = (char)(indata[c-3] + 48);
                    gsm_format[133] = (char)(indata[c-2] + 48); 
//                   Serial.println("REC");
                    b++;
                  }
                } 
              }

              if(indata[c] == 77)
              {
                
                max_data[0] = (char)(indata[c] + 48);
                
                //PHASE A MAX-VOLTAGE
                max_data[1] = (char)(indata[c+1] + 48);
                max_data[2] = (char)(indata[c+2] + 48);
                max_data[3] = (char)(indata[c+3] + 48);
                max_data[4] = (char)(indata[c+4] + 48);
                max_data[5] = (char)(indata[c+5] + 48);

                //PHASE A MAX-CURRENT
                max_data[6] = (char)(indata[c+6] + 48);
                max_data[7] = (char)(indata[c+7] + 48);
                max_data[8] = (char)(indata[c+8] + 48);
                max_data[9] = (char)(indata[c+9] + 48);
                max_data[10] = (char)(indata[c+10] + 48);
                
                max_data[11] = (char)(indata[c+11] + 48); 

                //PHASE B MAX-VOLTAGE
                max_data[12] = (char)(indata[c+12] + 48);
                max_data[13] = (char)(indata[c+13] + 48);
                max_data[14] = (char)(indata[c+14] + 48);
                max_data[15] = (char)(indata[c+15] + 48);
                max_data[16] = (char)(indata[c+16] + 48);

                //PHASE B MAX-CURRENT
                max_data[17] = (char)(indata[c+17] + 48);
                max_data[18] = (char)(indata[c+18] + 48);
                max_data[19] = (char)(indata[c+19] + 48);
                max_data[20] = (char)(indata[c+20] + 48);
                max_data[21] = (char)(indata[c+21] + 48);
                
                max_data[22] = (char)(indata[c+22] + 48);

                //PHASE C MAX-VOLTAGE
                max_data[23] = (char)(indata[c+23] + 48);
                max_data[24] = (char)(indata[c+24] + 48);
                max_data[25] = (char)(indata[c+25] + 48);
                max_data[26] = (char)(indata[c+26] + 48);
                max_data[27] = (char)(indata[c+27] + 48);

                //PHASE C MAX-CURRENT
                max_data[28] = (char)(indata[c+28] + 48);  
                max_data[29] = (char)(indata[c+29] + 48);
                max_data[30] = (char)(indata[c+30] + 48);
                max_data[31] = (char)(indata[c+31] + 48);
                max_data[32] = (char)(indata[c+32] + 48);
                c+=32;
              }
              
            
              c++;
          }         
                                  
} 

void my_delay(int interval)
{
 entry = LOW; 
   unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;  

     entry = HIGH;
     //Serial.println(interval);
     //Serial.println(pulse_delay);
     //Serial.println();
  }
}
void espconnect(){
 Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");cu++;
    if(cu==40)
    {cu=0;break;}
  }
  if(cu==0)
  {espconnect();
  }else 
  {cu=0;
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  WiFi.macAddress(mac);
  String mac_5 = String(mac[5], HEX);
  String mac_4 = String(mac[4], HEX);
  String mac_3 = String(mac[3], HEX);
  String mac_2 = String(mac[2], HEX);
  String mac_1 = String(mac[1], HEX);
  String mac_0 = String(mac[0], HEX);

  mac_id = mac_5 + ':' + mac_4 + ':' + mac_3 + ':' + mac_2 + ':' + mac_1 + ':' + mac_0;

  Serial.print("MAC : ");
  Serial.println(mac_id);
}}












