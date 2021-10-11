


/*

minus alarm
peringatan dhuha
syuruq
peringatan tahajud
dzuhur masih belum digabung

*/

#include <Wire.h>
#include<SH1106.h>
#include <JadwalSholat.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>


#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(D7, D6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


SH1106 display(0x3C,D1,D2);

#include <Wire.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
#include <F1kM_Hisab.h>
#include <TimeLib.h>//menghitung hari berikutnya untuk perhitungan hijriah

uint8_t rTgl;
uint8_t rBul;
uint16_t rTah;
int vol_priority = 15;
uint8_t vol_ordinary = 10, vol_midnight = 5;
bool disable_update = true;
String ip; 
bool updt = false;
//int a = 30;

//#include "PrayerTimes.h"
//#include "Display.h"
//#include "Buzzer.h"
#include "PrayerTimesAPI.h"
#include "RTC.h"
#include "CalculateTime.h"
#include "FSConfig.h"



#include "WebPage.h"
#include "Ali.h"
//#include "JWS.h"
//#include "Puasa.h"
//#include "Button.h"
#include "WebServer.h"


bool enableSettingMode = true;






void setup() {
  
  Serial.begin(115200);
//  pinMode(D7, INPUT);
  initDF();
  delay(400);
  mulaiRTC();
  LittleFS.begin();
  loadJwsConfig(fileconfigjws, configjws);
  loadAdminConfig(fileconfigadmin, configadmin);
  loadDispConfig(fileconfigdisp, configdisp);
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  char msg[20];
  sprintf(msg, "%s", "Ali Wafa");
  int max_x = display.getStringWidth(msg);
//  Serial.println();
//  Serial.println();
//  Serial.println(max_x);
  int tbl = analogRead(A0);
  if(tbl<30){
    updt = true;
    Serial.print("UPDATE.....");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_24);
    display.drawString(64, 18, "UPDATE..");
    display.display(); 
  }

  if (enableSettingMode) {
    Serial.println("WIFI STARTED");
    wifiConnect();

    server.on("/", []() {
      server.send_P(200, "text/html", setwaktu);

      // Kalau ada perubahan tanggal
      if (server.hasArg("date")) {

        uint16_t jam;
        uint8_t menit;
        uint8_t detik;
        String sd = server.arg("date");
        String lastSd;

        jam = ((sd[0] - '0') * 1000) + ((sd[1] - '0') * 100) + ((sd[2] - '0') * 10) + (sd[3] - '0');
        menit = ((sd[5] - '0') * 10) + (sd[6] - '0');
        detik = ((sd[8] - '0') * 10) + (sd[9] - '0');

        if (sd != lastSd) {
          RtcDateTime now = Rtc.GetDateTime();
          uint8_t hour = now.Hour();
          uint8_t minute = now.Minute();
          Rtc.SetDateTime(RtcDateTime(jam, menit, detik, hour, minute, 0));
          lastSd = sd;
        }

        BacaRTC();
        waktuSholatNow();

        server.send ( 404 , "text", message );

      }

      // Kalau ada perubahaan jam
      if (server.hasArg("time")) {

        String st = server.arg("time");
        String lastSt;
        uint8_t jam = ((st[0] - '0') * 10) + (st[1] - '0');
        uint8_t menit = ((st[3] - '0') * 10) + (st[4] - '0');

        if (st != lastSt) {
          RtcDateTime now = Rtc.GetDateTime();
          uint16_t year = now.Year();
          uint8_t month = now.Month();
          uint8_t day = now.Day();
          Rtc.SetDateTime(RtcDateTime(year, month, day, jam, menit, 0));
          lastSt = st;
        }

        BacaRTC();
        updateJWS();
        TanggalHijriah();
        waktuSholatNow();
        //         bacaParameter();

        server.send ( 404 , "text", message );


      }
    });


    server.on("/admin", []() {
      server.send_P(200, "text/html", halamanadmin);
      if (server.hasArg("en")){
        String en = server.arg("en");
        disable_update = en != "1";
        Serial.print("nilai en adalah ");
        Serial.println(disable_update);
      }
    });

    server.on("/selaraskan", HTTP_POST, handleSelaraskan);
    server.on("/kecerahan", HTTP_POST, handleKecerahan);
    server.on("/simpandatajws", HTTP_POST, handleSettingJwsUpdate);
    server.on("/simpandataadmin", HTTP_POST, handleSettingAdminUpdate);

    server.on("/xmlwaktu", handleXMLWaktu); // http://192.168.4.1/xmlwaktu
    server.on("/xmldataadmin", handleXMLAdmin); // http://192.168.4.1/xmlwaktu
    server.on("/xmldebug", handleXMLDebug); // http://192.168.4.1/xmldebug
    server.on("/xmldatajws", handleXMLDataJWS); // http://192.168.4.1/xmldatajws
    server.on("/reset", reset_Par);

    httpUpdater.setup(&server);
    server.begin();
    Serial.println("HTTP server started");

  }
  BacaRTC();
  updateJWS();
  TanggalHijriah();
  waktuSholatNow();
  if(SholatNow==6)interrupt = true;
//  reset_Par();
//  vol_priority = configdisp.cerah;
//  Serial.println(vol_priority);
//  vol_ordinary = 0.3*vol_priority;
//  vol_midnight = 0.15*vol_priority;
}

void loop() {
  server.handleClient();
  
  BacaRTC();
  if(Serial.available()){
    uint8_t tgl = Serial.parseInt();
    uint8_t jam = Serial.parseInt();
    uint8_t menit = Serial.parseInt();
    uint8_t detik = Serial.parseInt();
    uint8_t state = Serial.parseInt();
    
    Serial.printf("%02d : %02d : %02d \n", jam, menit, detik);
    if(state){
      
      Rtc.SetDateTime(RtcDateTime(2021, 10, tgl, jam, menit, detik));
//      BacaRTC();
//      updateJWS();
//      TanggalHijriah();
//      waktuSholatNow();
    }
    
  }
  checkAdzan();
  UpdateWaktu();
  display.clear();
  jamNormal(64, 18);
  display.display(); 
  kedip();
}


void initDF(){
  mySoftwareSerial.begin(9600);
  
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
//  myDFPlayer.setTimeOut(500);
  
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.volume(5);
//  myDFPlayer.playFolder(2, 2);//play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
//  delay(4000);
//  myDFPlayer.pause();
  myDFPlayer.playMp3Folder(100);
//  myDFPlayer.playMp3Folder(10 + 7);
//  delay(1000);
//  myDFPlayer.advertise(44);


//  myDFPlayer.volume(10);
//  myDFPlayer.volume(15);
//  myDFPlayer.advertise(43);
  
//  delay(100);
//  while(!digitalRead(D7)){delay(100);}
//  myDFPlayer.volume(5);
//  myDFPlayer.advertise(43);
//  delay(10000);


//  myDFPlayer.volume(5);
//  myDFPlayer.start();
//  delay(2000);
}
