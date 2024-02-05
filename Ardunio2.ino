#include <DHT.h>
#include "TFT_eSPI.h"
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#define BLYNK_TEMPLATE_ID "TMPL02lfL1oq"
#define BLYNK_DEVICE_NAME "Testcpe101"
//change WIO_Light to errr something from sensor?

#ifdef REAL

#define DHTPIN D1
//Dust pin
#define DUSTPIN A0
#define LEDDUSTPIN D2

DHT dht(DHTPIN,DHT22);

void readHumidTempDust(int* humidity,int* Temperature,int* dust){
    digitalWrite(LEDDUSTPIN,LOW);// power on the LED
    delayMicroseconds(delayTime);
    *dust = analogRead(DUSTPIN);// read the dust value via pin 5 on the sensor
    delayMicroseconds(delayTime2);
    digitalWrite(LEDDUSTPIN,HIGH);// turn the LED off
    delayMicroseconds(offTime);
    *Temperature = dht.readTemperature();
    *humidity = dht.readHumidity();
}

#else

void readHumidTempDust(int* humidity,int* Temperature,int* dust){
    *humidity = analogRead(WIO_LIGHT);
    *Temperature = analogRead(WIO_LIGHT);
    *dust = analogRead(WIO_LIGHT);
}

#endif
// bluetooth
// setup
// loop

// wifi 
const char *ssid = "true_home2G_Dfg";
const char *password = "XWUF4jtT";

WebServer server(80);
 
void handleRoot() {
  server.send(200, "text/plain", "hello from Wio Terminal!");
}
 
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
// setup
void serverSetup(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.on("/", handleRoot);
 
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
 
  server.onNotFound(handleNotFound);
 
  server.begin();
  Serial.println("HTTP server started");
}
 
// loop
void serverLoop(void) {
  server.handleClient();
  Serial.println(WiFi.localIP());
}

//Page changer
int pageUI = 2;
int booleaner = 1;
int DangerVal = 100;
//Device Manager
//Note you can input online or offline of that device by change 1 to 0 or 0 to 1 your call
int AIRCON = 0;
int AIRPUR = 0;
int pageDM = 1; //Will reset every time when change page

//Device setting
int pageDS = 1;
int LOCKER = 0;
int ADJtemp = 25;

//Dust variable
int delayTime=280;
int delayTime2=40;
float offTime=9680;

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

void _DeviceSetting(){
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.drawString("Device Setting",13,40);
    tft.setTextSize(2);
    tft.drawString("Adjust temperature",30,120);
    tft.drawString("Dust Danger level",30,150);
}
void _DeviceSettingloop(){
    int dust,temperature,humidity;
    readHumidTempDust(&dust,&temperature,&humidity);
    //Setting your device status here
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    //Selectint part
    if (LOCKER == 0){
        if(digitalRead(WIO_5S_UP) == LOW){//DO the white Round stuff isn't finish
            pageDS += 1;
            if (pageDS > 2){
                pageDS = 1;
            }
        }
        else if(digitalRead(WIO_5S_DOWN) == LOW){
            pageDS -= 1;
            if (pageDS < 1){
                pageDS = 2;
            }
        }
    }
    //Confirm Selection part
    if(pageDS == 1){
        tft.fillCircle(15,125,5,TFT_WHITE);
        tft.fillCircle(15,155,5,TFT_BLACK);
        if((pageDS == 1) && (digitalRead(WIO_5S_PRESS) == LOW ) && (LOCKER == 0)){
            LOCKER = 1;
            tft.drawRect(28,118,220,20,TFT_CYAN);
            delay(250);
        }
        else if ((pageDS == 1) && (digitalRead(WIO_5S_PRESS) == LOW ) && (LOCKER == 1)){
            LOCKER = 0;
            tft.drawRect(28,118,220,20,TFT_BLACK);
            delay(250);
        }
    }
    if(pageDS == 2){
        tft.fillCircle(15,125,5,TFT_BLACK);
        tft.fillCircle(15,155,5,TFT_WHITE);
        if((pageDS == 2) && (digitalRead(WIO_5S_PRESS) == LOW ) && (LOCKER == 0)){
            LOCKER = 1;
            tft.drawRect(28,148,220,20,TFT_CYAN);
            delay(250);
        }
        else if ((pageDS == 2) && (digitalRead(WIO_5S_PRESS) == LOW ) && (LOCKER == 1)){
            LOCKER = 0;
            tft.drawRect(28,148,220,20,TFT_BLACK);
            delay(250);
        }
    }
    delay(20);
    //Contorl and Display On or off status
    tft.fillRect(270,123,25,15,TFT_BLACK);
    tft.drawNumber(ADJtemp,270,123);
    tft.fillRect(270,153,40,25,TFT_BLACK);
    tft.drawNumber(DangerVal,270,153);
    if((pageDS == 1) && (LOCKER == 1)){
        if (digitalRead(WIO_5S_RIGHT) == LOW){
            if(ADJtemp != 31){
                ADJtemp += 1;
            }
            delay(250);
        }
        delay(250);
        if (digitalRead(WIO_5S_LEFT) == LOW){
            if(ADJtemp != 16){
                ADJtemp -= 1;
            }
            delay(250);
        }
    }
    if((pageDS == 2) && (LOCKER == 1)){
        if (digitalRead(WIO_5S_RIGHT) == LOW){
            if(DangerVal != 700){
                DangerVal += 1;
            }
            delay(250);
        }
        delay(250);
        if (digitalRead(WIO_5S_LEFT) == LOW){
            if(DangerVal != 30){
                DangerVal -= 1;
            }
            delay(250);
        }
    }
    if(dust >= DangerVal){// Display DUST EXCEED LIMIT
        tft.setTextColor(TFT_RED);
        tft.fillRect(20,80,300,30,TFT_BLACK);  
        delay(500);
        tft.drawString("=DUST LEVEL OVERLIMIT!=",20,80);
        delay(500);
    }
    else{
        tft.fillRect(20,80,300,30,TFT_BLACK);  
    }
}

void _DeviceManager(){
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.drawString("Device Manager",13,40);
    tft.setTextSize(2);
    tft.drawString("Air Conditioner",30,120);
    tft.drawString("Air Purifier",30,150);
}
void _DeviceManagerloop(){//==============================Still have a problem
    int dust,temperature,humidity;
    readHumidTempDust(&dust,&temperature,&humidity);
    //Setting your device status here
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    //Selectint part
    if(digitalRead(WIO_5S_UP) == LOW){//DO the white Round stuff isn't finish
        pageDM += 1;
        if (pageDM > 2){
            pageDM = 1;
        }
    }
    else if(digitalRead(WIO_5S_DOWN) == LOW){
        pageDM -= 1;
        if (pageDM < 1){
            pageDM = 2;
        }
    }
    //setting device to turn on or off
    if(pageDM == 1){
        tft.fillCircle(15,125,5,TFT_WHITE);
        tft.fillCircle(15,155,5,TFT_BLACK);
        if((pageDM == 1) && (digitalRead(WIO_5S_PRESS) == LOW )){
            if (AIRCON == 0){
                AIRCON = 1;
                delay(250);
                //form off to on
            }
            else if (AIRCON == 1){
                AIRCON = 0;
                delay(250);
                //form on to off
            }
        }
    }
    if(pageDM == 2){
        tft.fillCircle(15,125,5,TFT_BLACK);
        tft.fillCircle(15,155,5,TFT_WHITE);
        if((pageDM == 2) && (digitalRead(WIO_5S_PRESS) == LOW )){
            if (AIRPUR == 0){
                AIRPUR = 1;
                delay(250);
                //form off to on
            }
            else if (AIRPUR == 1){
                AIRPUR = 0;
                delay(250);
                //form on to off
            }
        }
    }
    delay(20);
    //Display On or off status device
    if(AIRCON == 0){
        tft.fillRect(250,120,40,20,TFT_BLACK);
        tft.drawRect(250,120,40,20,TFT_RED);
        tft.drawString("OFF",253,123);
    }
    else if(AIRCON == 1){
        tft.fillRect(250,120,40,20,TFT_BLACK);
        tft.drawRect(250,120,40,20,TFT_GREEN);
        tft.drawString("ON",259,123);
    }
    if(AIRPUR == 0){
        tft.fillRect(250,150,40,20,TFT_BLACK);
        tft.drawRect(250,150,40,20,TFT_RED);
        tft.drawString("OFF",253,153);
    }
    else if(AIRPUR == 1){
        tft.fillRect(250,150,40,20,TFT_BLACK);
        tft.drawRect(250,150,40,20,TFT_GREEN);
        tft.drawString("ON",259,153);
    }
    if(dust >= DangerVal){// Display DUST EXCEED LIMIT
        tft.setTextColor(TFT_RED);
        tft.fillRect(20,80,300,30,TFT_BLACK);  
        delay(500);
        tft.drawString("=DUST LEVEL OVERLIMIT!=",20,80);
        delay(500);
    }
    else{
        tft.fillRect(20,80,300,30,TFT_BLACK);  
    }
}

void _mainframe(){
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.drawString("Air Quality Meter",13,40);          //Header
    tft.setTextSize(0.5);
    //the power part
    tft.drawString("3",305,115);
    tft.drawString("o",280,145);
    tft.setTextSize(2);
    tft.drawString("C",290,150);
    tft.drawString("-",123,187);
    //Note 000 behind just to make sure that the number will not go rough and get the fuck out of the frame
    tft.drawString("Dust density  = 000 ug/m",15,120);
    tft.drawString("|",251,124);//Micro part
    tft.drawString("Room temperature = 000 ",15,150);
    tft.drawString("Humidity +5%  = 000 %RH",15,180);
}

void _mainframeloop(){
    int dust,temperature,humidity;
    readHumidTempDust(&dust,&temperature,&humidity);
    //dust part
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        //clear the previous number vvv
        tft.fillRect(207,120,34,16,TFT_BLACK);//dust clear
        tft.fillRect(243,150,34,16,TFT_BLACK);//temp clear
        tft.fillRect(207,180,34,16,TFT_BLACK);//humidity clear
        //print new number
        if(dust < DangerVal-30){
            tft.setTextColor(TFT_WHITE);
            tft.drawNumber(dust,207,120);
        }
        else if((dust >= DangerVal-30) && (dust < DangerVal)){
            tft.setTextColor(TFT_ORANGE);
            tft.drawNumber(dust,207,120);
            tft.setTextColor(TFT_WHITE);
        }
        else if((dust >= DangerVal) && (dust <= 999)){
            tft.setTextColor(TFT_RED);
            tft.drawNumber(dust,207,120);
            tft.setTextColor(TFT_WHITE);
        }
        else{
            tft.setTextColor(TFT_RED);
            tft.drawNumber(999,207,120);
            tft.setTextColor(TFT_WHITE);
        }
        tft.drawNumber(temperature,243,150);
        tft.drawNumber(humidity,207,180);
        delay(200);
        if(dust >= DangerVal){// Display DUST EXCEED LIMIT
            tft.setTextColor(TFT_RED);
            tft.fillRect(20,80,300,30,TFT_BLACK);  
            delay(500);
            tft.drawString("=DUST LEVEL OVERLIMIT!=",20,80);
            delay(500);
        }
        else{
            tft.fillRect(20,80,300,30,TFT_BLACK);  
        }
}

void Uisetup(){
    //Uisetup;
    tft.begin();
    tft.setRotation(3);

    pinMode(WIO_5S_UP, INPUT);
    pinMode(WIO_5S_DOWN, INPUT);
    pinMode(WIO_5S_LEFT, INPUT);
    pinMode(WIO_5S_RIGHT, INPUT);
    pinMode(WIO_5S_PRESS, INPUT);
    pinMode(WIO_LIGHT, INPUT);

    //Use WIO_5S_blah to recieve and input from typical human using hand
}
void Uiloop(){
    if(LOCKER == 0){
        if (digitalRead(WIO_5S_RIGHT) == LOW){
            pageUI += 1;
            booleaner = 1;
            if (pageUI > 3){
                pageUI = 1;
            }
            
        }
        delay(200);
        if (digitalRead(WIO_5S_LEFT) == LOW){
            pageUI -= 1;
            booleaner = 1;
            if (pageUI < 1){
                pageUI = 3;
            }
        }
    }
    if(pageUI == 1){
        if(booleaner){
            _DeviceManager();
            pageDM = 1;// 
        }
        _DeviceManagerloop();
        booleaner = 0;
    }
    else if(pageUI == 2){
        if(booleaner){
            _mainframe();
        }
        _mainframeloop();
        booleaner = 0;
    }
    else if(pageUI == 3){
        if(booleaner){
            _DeviceSetting();
            pageDS = 1;
        }
        _DeviceSettingloop();
        booleaner = 0;
    }
}


//Don't touch this part
void setup(){
    Serial.begin(9600);
#ifdef REAL
    pinMode(LEDDUSTPIN,OUTPUT);
    dht.begin();
#endif
    Uisetup();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.drawString("Connecting",80,120);
    tft.setTextSize(2);
    tft.drawString("Please Wait",105,150);
    serverSetup();
}

void loop(){
    Uiloop();
    serverLoop();
}
