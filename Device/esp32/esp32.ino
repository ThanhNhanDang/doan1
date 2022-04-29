HardwareSerial swSer(2);
#include <ArduinoJson.h>
#include "RSAAuth.h"
#include "DHT.h"
#define DHTTYPE DHT11 
#define DHTPin 15 
#define BUTTON1  26
#define BUTTON2  25
//#define Buzzer 23
#define ledRed 18
#define ledYellow 19

DHT dht(DHTPin, DHTTYPE);     // Initialize DHT sensor.
#define BAUD_RATE 115200
static unsigned long t_demo = 0;
                     /* current counting time for LCD Display */
unsigned long max_demo = 5*1000;

float tempCompareMax = 35;
float tempCompareMin = 30;
float humidCompareMax = 60;
float humidCompareMin = 50;
DynamicJsonDocument doc(1024);
float temp = 1;
float humid = 1;      
char cstr[255];
#define output1 13 
#define output2 12
#define output3 14 // Động cơ như quạt máy phun sương
#define output4 27  
bool checkSend1 = false;
bool checkSend2 = false;
bool checkSend12 = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 20;

unsigned long startMillisTempLed = 0;
unsigned long startMillisHumidLed = 0;
int btnState;
int lastBtnState = HIGH;
unsigned char onoff = 0;

unsigned long lastDebounceTime2 = 0;

int btnState2;
int lastBtnState2 = HIGH;
unsigned char onoff2 = 0;
byte en[255];
byte pac[255];
bool state1 = false;
bool state2 = false;
bool state3 = false;
bool state4 = false;

void guiDHT () {
  for(int i=0;i<100;i++)
  cstr[i]=0;
  sprintf(cstr,"{\"idgw\":1,\"t\":%.2f,\"h\":%.2f,\"iddv\":1,\"st1\":\"%s\",\"st2\":\"%s\",\"st3\":\"%s\",\"st4\":\"%s\"}", 
                                      temp,humid, state1 ? "OFF":"ON", state2 ? "OFF":"ON", state3 ? "OFF":"ON", state4 ? "OFF":"ON");
  rsaSend();
}

void guiStateRelay (int idrl) {
   readStateRelay();
   for(int i=0;i<100;i++)
   cstr[i]=0;
   sprintf(cstr,"{\"idgw\":1,\"t\":%.2f,\"h\":%.2f,\"iddv\":1,\"st1\":\"%s\",\"st2\":\"%s\",\"st3\":\"%s\",\"st4\":\"%s\",\"idrl\":%d,\"r\":1}", 
                                      temp,humid, state1 ? "OFF":"ON", state2 ? "OFF":"ON", state3 ? "OFF":"ON", state4 ? "OFF":"ON", idrl);
   rsaSend();
}

void guiStateWarning (int idrl3, int idrl4) {
   readStateRelay();
   for(int i=0;i<100;i++)
   cstr[i]=0;
   sprintf(cstr,"{\"idgw\":1,\"t\":%.2f,\"h\":%.2f,\"iddv\":1,\"st1\":\"%s\",\"st2\":\"%s\",\"st3\":\"%s\",\"st4\":\"%s\",\"idrl\":[%d, %d],\"w\":1}", 
                                      temp,humid, state1 ? "OFF":"ON", state2 ? "OFF":"ON", state3 ? "OFF":"ON", state4 ? "OFF":"ON", idrl3, idrl4);
   rsaSend();
}

void guiWarningX2 (int idrl3, int idrl4) {
   readStateRelay();
   for(int i=0;i<100;i++)
   cstr[i]=0;
   sprintf(cstr,"{\"idgw\":1,\"t\":%.2f,\"h\":%.2f,\"iddv\":1,\"st1\":\"%s\",\"st2\":\"%s\",\"st3\":\"%s\",\"st4\":\"%s\",\"idrl\":[%d, %d],\"wx2\":1}", 
                                      temp,humid, state1 ? "OFF":"ON", state2 ? "OFF":"ON", state3 ? "OFF":"ON", state4 ? "OFF":"ON", idrl3, idrl4);
   rsaSend();
}

void rsaSend(){
  Serial.println("-Gui:");
  Serial.println((char*)cstr);
  yield();
  rsa.Send(cstr,swSer);
}

void readStateRelay(){
    state1 = digitalRead(output1);
    state2 = digitalRead(output2);
    state3 = digitalRead(output3);
    state4 = digitalRead(output4);
}

void demo_TIMER(void){
  if ((unsigned long) millis() - t_demo >= max_demo) {
    readDHT();
    t_demo = millis();
    guiDHT();
  }
}

void request(String payload) {
  deserializeJson(doc, payload);
  JsonObject obj = doc.as<JsonObject>();
  int IDRL = obj["idrl"];  
  int ID = obj["iddv"];
  int IDGW = obj["idgw"];
  String con = obj["state"].as<String>();
  
  if(IDGW == 1 && ID ==1)
  {
   switch(IDRL){
   case output1:{
    if(con=="on"){
      digitalWrite(output1,0);
      delay(100);
    }
    else{
      digitalWrite(output1,1);
      delay(100);  
    }
    break;
   }
   case output2:{
    if(con=="on"){
      digitalWrite(output2,0);
      delay(100);
    }
    else{
      digitalWrite(output2,1);
      delay(100);   
    }
    break;
   }
   case output3:{
    if(con=="on"){
      digitalWrite(output3,0);
      delay(100);
    }
    else{
      digitalWrite(output3,1);
      delay(100);
    }
    break;
   }
   case output4:{
   if(con=="on"){
      digitalWrite(output4,0);
      delay(100);
    }
    else{
      digitalWrite(output4,1);
      delay(100);
    }
    break;
   }
   }
  guiStateRelay(IDRL);
  }
}
void setup() {

  Serial.begin (BAUD_RATE); 
  dht.begin();
  //----------
  swSer.begin(BAUD_RATE);
  
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
//  pinMode(Buzzer, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);

  digitalWrite(output1,1);
  digitalWrite(output2,1);
  digitalWrite(output3,1);
  digitalWrite(output4,1);
  readStateRelay();
  Serial.println("\nRSA serial test started");
}
byte buff[255];
int BEGIN=0;
int END=0;
int DesLen=0;
//byte unpac[255];
unsigned long startMillisTemp = 0;
unsigned long startMillisHumid = 0;
unsigned long warningTime = 10000;  
bool checkWarning1 = false;
bool checkWarning2 = false;
int a = 1;
byte m[255];
void loop() {
  DesLen=0;
  if(rsa.Receive(buff,&BEGIN,&END,m,&DesLen, swSer))
  {
    if(DesLen>0)
    {
      Serial.println("-Nhan tu getway:");
      Serial.println((char*)m);
      request((char*)m);  
    }
  }
  else Serial.println("Nhan error:");
  button1();
  button2(); 
  demo_TIMER();
//  if (digitalRead(output3)) {
//    startMillisTemp = millis();
//    checkWarning1 = false;
//  } 
//  else{
//    if((unsigned long)millis() - startMillisTemp >= warningTime){ // Nếu trạng thái của relay1 luôn bật trong khoảng thời gian 10s
//                                                                  // mà nhiệt độ vẫn trên mức báo động thì bật led đỏ thông báo đến người dùng 
//          if((unsigned long)millis() - startMillisTempLed >= 500){
//             if(digitalRead(ledRed) == 1)
//              digitalWrite(ledRed, 0);
//             else digitalWrite(ledRed, 1);
//             guiWarningX2(output3, 0);
//             checkWarning1 = true; 
//             startMillisTempLed = millis();
//          }
//      }
//  }
//  if(digitalRead(output4)) {
//    startMillisHumid = millis();
//    checkWarning2 = false;
//  }
//  else{
//    if((unsigned long)millis() - startMillisHumid >= warningTime){// Nếu trạng thái của relay2 luôn bật trong khoảng thời gian 10s
//                                                                 // mà nhiệt độ vẫn trên mức báo động thì bật led xanh thông báo đến người dùng 
//         if((unsigned long)millis() - startMillisHumidLed >= 500){
//            if(digitalRead(ledYellow) == 1)
//            digitalWrite(ledYellow, 0); 
//            else digitalWrite(ledYellow, 1);
//            
//            checkWarning2 = true;
//            startMillisHumidLed = millis();
//        }
//    } 
//  }
}

void button1(){
  int reading = digitalRead(BUTTON1);

  if (reading != lastBtnState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != btnState) {
      btnState = reading;

      if (btnState == LOW) {
        if (onoff == 0) {
          onoff = 1;
          digitalWrite(output1, 0);

        } else {
          onoff = 0;
          digitalWrite(output1, 1);
        }
        guiStateRelay(output1);
      }
      
    }
  }

  lastBtnState = reading;
}

void button2(){
  int reading = digitalRead(BUTTON2);

  if (reading != lastBtnState2) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading != btnState2) {
      btnState2 = reading;

      if (btnState2 == LOW) {
        if (onoff2 == 0) {
          onoff2 = 1;
          digitalWrite(output2, 0);

        } else {
          onoff2 = 0;
          digitalWrite(output2, 1);
        }
         guiStateRelay(output2);
      }
     
    }
  }

  lastBtnState2 = reading;
}

void readDHT(){
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    t = random(27,40); 
    h = random(55,70);
  }
  humid = h;
  temp = t;

  // Nếu nhiệt độ lớn hơn 35 thì
  if(temp > tempCompareMax && humid > humidCompareMax){
    digitalWrite(output3, 0);
    digitalWrite(output4, 0);
   
      if(!checkSend12){
        digitalWrite(ledRed, 1);
        digitalWrite(ledYellow, 1);
        guiStateWarning(output3, output4);
      }
      checkSend12 = true;
    
  }
  else{
    if(temp > tempCompareMax){ 
      digitalWrite(output3, 0);
      
        if(!checkSend1){
          digitalWrite(ledRed, 1);
          guiStateWarning(output3, 0);
        }
        checkSend1 = true;
      }
    
    else{
      digitalWrite(output3, 1);
      
        if(checkSend1){
          digitalWrite(ledRed, 0);
          guiStateWarning(output3, 0);
        }
        checkSend1 = false;
      
      
    }
    if (humid > humidCompareMax){
      digitalWrite(output4, 0);
      
        if(!checkSend2){
          digitalWrite(ledYellow, 1);
          guiStateWarning(0, output4);
        }
        checkSend2 = true;
      
    }
    else{
      digitalWrite(output4, 1);
      
        if(checkSend2){
          digitalWrite(ledYellow, 0);
          guiStateWarning(0, output4);
        }
        checkSend2 = false;
      
    }
  }
