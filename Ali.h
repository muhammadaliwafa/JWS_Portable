bool kdp;
int8_t SholatNow = -1;
bool azzan = false;
bool tarhim = false;
bool mur = false;
uint8_t alarm_pagi = 0;

uint8_t stateMinus =0;
uint8_t indexSurah = 0;
bool interrupt = false;

uint8_t folder = 2;




void jamNormal(uint8_t x, uint8_t y) {
  char buff[8];
  char cip[15];
  if (kdp) {
    sprintf(buff, "%02d:%02d:%02d", rJam, rMen, rDet);
  }
  else {
    sprintf(buff, "%02d %02d %02d", rJam, rMen, rDet);
  }

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(x, y, buff);
  display.setFont(ArialMT_Plain_10);
  if(online){
    display.drawString(20, 5, "online");
  }
  else{
    display.drawString(20, 5, "offline");
  }
//  ip.toCharArray(cip, 15);
  display.drawString(64, 53, ip);
//  Disp.drawText(x, y, buff);

}

void kedip() {
  static uint32_t   timerSave, lastSecond;
  uint32_t          nowmilis = millis();
  if (lastSecond != rDet) {
    lastSecond = rDet;
    kdp = true;
    timerSave = nowmilis;
  }
  if ((nowmilis - timerSave) > 500) {
    kdp = false;
  }
}

void waktuSholatNow() {
  SholatNow = 0;
  for (int i = 7; i >= 0; i--) {
    
    if (floatnow > stimeFloat[i]) {
      Serial.println("update wakyu");
      Serial.print(floatnow);
      Serial.print("\t");
      Serial.println(stimeFloat[i]);
      SholatNow = i;// kalo 0 itu adalah waktu imsyak cuy, sedangkan waktu imsyak itu adalah sebelum 
      break;
    }
  }
  Serial.printf("waktu sekarang adalah %d", SholatNow);
  Serial.println();

}

void alertBefore(uint8_t x){
  static uint8_t alarm=x;
  static uint32_t lsRn;
  static uint8_t lsShltNow;
  uint32_t Tmr=millis();
  
//  uint32_t Tmr;
  interrupt = true;
  if(alarm>(x+3))alarm=x; 
  if((alarm==7 && lsShltNow != SholatNow) ){
    Serial.println("reset alarm dipanggil");
    waktuSholatNow();
    lsShltNow = SholatNow;
    alarm = x;
  }
  
  if(alarm==x+0){
    
    myDFPlayer.pause();
    myDFPlayer.volume(configdisp.hadist);
    Serial.print("volume tarhim");
    Serial.println(configdisp.hadist);
    switch(SholatNow){
      case 0:
//        myDFPlayer.volume(vol_priority);
        Serial.printf("play sebelum (%02)\n", x+6);
        myDFPlayer.playMp3Folder(x + 6);
        break;
      case 3:
//        myDFPlayer.volume(vol_priority);
        
        Serial.printf("play sebelum (%02)\n", x+7);
        myDFPlayer.playMp3Folder(x + 7);
        break;
      case 4:
//        myDFPlayer.volume(vol_priority);
        Serial.printf("play sebelum (%02)\n", x+8);
        myDFPlayer.playMp3Folder(x + 8);
        break;
      case 5:
//        myDFPlayer.volume(vol_priority);
        Serial.printf("play sebelum (%02)\n", x+9);
        myDFPlayer.playMp3Folder(x + 9);
        break;
      case 6:
//        myDFPlayer.volume(vol_priority);
        Serial.printf("play sebelum (%02)\n", x+10);
        myDFPlayer.playMp3Folder(x + 10);
        break;
    }
    alarm = x+1;
    Serial.printf("alarm : %02d dan %02d menit sebelum adzan\n", alarm, x);
    lsRn = millis();
//    delay(100);
  }
  else if(alarm == x+1 && digitalRead(D7)&&(Tmr-lsRn)>500){
    Serial.println("tarhim");
    if(x==5){
      myDFPlayer.volume(configdisp.vol_adzan);
    }
    else{
      myDFPlayer.volume(0.8*configdisp.vol_adzan);
    }
    myDFPlayer.playMp3Folder(3);
    alarm = x+2;
    lsShltNow = SholatNow;
    Serial.printf("alarm : %02d dan %02d menit sebelum adzan\n", alarm, x);
  }
  
}

uint16_t findIndex(){
  uint16_t idk;
  if(random(0,2)==1){
    uint16_t daftar[] = {200, 201, 202, 203, 204, 205,
                         210, 211, 212, 213, 214,
                         231, 232, 233, 234, 235,
                         241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251,
                         261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 279, 280};
    idk = daftar[random(0,49)];
  }else{
    idk = random(301, 365);
  }
  Serial.println(idk);
  return idk;
}

void stanbyState(){
  static uint32_t lsRn;
  static uint32_t lsRnHadist;
  uint32_t Tmr = millis();
  static uint8_t play;
//  Serial.println(Tmr-lsRn);
  if(digitalRead(D7)){
     if(floatnow>6 && floatnow < 18 && (Tmr-lsRnHadist)>3600000){
      if(play<1){
        myDFPlayer.volume(configdisp.hadist);
        myDFPlayer.playMp3Folder(6);
        play=1;
        lsRn = Tmr;
      }else if(play==1 && (Tmr-lsRn)>2000){
        myDFPlayer.playMp3Folder(8);
        myDFPlayer.advertise(findIndex());
        delay(500);
        play=0;
        lsRn=Tmr;
        lsRnHadist = Tmr;
      }      
      
     }else if((Tmr-lsRn)>50){
//      lsRn = Tmr;
      if(floatnow>6 && floatnow<18) myDFPlayer.volume(configdisp.murrotal_siang);
      else myDFPlayer.volume(configdisp.murrotal_malam);
      Serial.print("play surah ke-");
      Serial.println(indexSurah);
      myDFPlayer.playFolder(folder, indexSurah);
      delay(80);
      lsRn = Tmr;
      indexSurah++;
      if(indexSurah>114) {
        folder ++;
        if(folder>2)folder=1;
        indexSurah=0;
      }
     }
    
  }
}

void pauseAfter(uint8_t& setelah_adzan){
  float selisih = floatnow - stimeFloat[SholatNow];
  
  if(SholatNow==1 || SholatNow==5){
    if(selisih>0.24167){
      azzan = false;
      Serial.println("adzan false");
      interrupt = false;
      setelah_adzan = 0;
    }
  }else {
    if(selisih>0.4){
      azzan = false;
      Serial.println("adzan false");
      if(SholatNow!=6)interrupt = false;
      setelah_adzan = 0;
//      interrupt = false;
    }
  }
}

void setelahAdzan(){
  static uint32_t lsRn;
  uint32_t Tmr;
  static uint8_t setelah_adzan;
  if(digitalRead(D7)){//ketika lagu diputar kondisinya LOW
      Tmr = millis();
      if(setelah_adzan<1){
        myDFPlayer.volume(configdisp.hadist);
        myDFPlayer.playMp3Folder(6);
        setelah_adzan = 1;
        lsRn = millis();
      }else
      if(setelah_adzan==1 && (Tmr-lsRn)>2000){
        myDFPlayer.volume(configdisp.hadist);
        myDFPlayer.playMp3Folder(8);
        switch(SholatNow){
          case 1:
//            myDFPlayer.volume(vol_priority);
            myDFPlayer.advertise(10);
            break;
          case 4:
//            myDFPlayer.volume(vol_priority);
            Serial.println("diputar");
  //              myDFPlayer.advertise(41);
  //              myDFPlayer.advertise(43);
            myDFPlayer.advertise(44);
            break;
          case 5:
//            myDFPlayer.volume(vol_priority);
            myDFPlayer.advertise(51);
            break;
          case 6:
//            myDFPlayer.volume(vol_priority);
            myDFPlayer.advertise(61);
            break;
          case 7:
//            myDFPlayer.volume(vol_priority);
            myDFPlayer.advertise(71);
            break;
        }
        setelah_adzan = 2;
        lsRn = millis();
      }
      else if(setelah_adzan==2){
        pauseAfter(setelah_adzan);
      }
      
     
   }
}

void alert_alarm(){ // putar alarm berulang-ulang,
  
  static uint8_t lastTgl;
  if(analogRead(A0)<30){
    Serial.println("tombol dipencet");
    myDFPlayer.pause();
    alarm_pagi=3;
  }
  if(rTgl!=lastTgl){
    lastTgl=rTgl;
    alarm_pagi=0;
  }
  if(alarm_pagi==0){
    myDFPlayer.volume(configdisp.vol_adzan);
    myDFPlayer.playMp3Folder(9);
    delay(200);
    alarm_pagi++;
  }
  else if(digitalRead(D7)){
    myDFPlayer.volume(configdisp.vol_adzan);
    myDFPlayer.playMp3Folder(9);
    delay(200);
  }
    
 
  
}






void checkAdzan(){
  if(!azzan){
    
    for(int i=0; i<=7; i++){
      if(i==2){
        i+=2;
      }else if(i==0){
        i++;
      }
      float selisih = stimeFloat[i] - floatnow;
      
      if(selisih < 0
        and (selisih > -0.03)
        ){
//          Serial.println("if adzan");
          azzan = true;
          Serial.println("adzan 1");
          myDFPlayer.pause();
          myDFPlayer.volume(configdisp.vol_adzan);
          if(i==1){
            myDFPlayer.playMp3Folder(2);
            alarm_pagi = true;
          }else{
            myDFPlayer.playMp3Folder(1);
          }
          
          delay(500);
          waktuSholatNow();
          tarhim = false;
//          stateMinus=0;
          return;
      }
      else if(selisih > 0){
        if(selisih<0.08333){
//          Serial.println("5 menit sebelum adzan");
          alertBefore(5);//mainkan sebelum 5 menit
          
        }
        else if(selisih<0.167){//10 menit sebelum sholat
          alertBefore(10);
          
        }
        else if(floatnow > 3.50 && (alarm_pagi<2) && floatnow<3.5167){
          alert_alarm();
        }
        else if(!interrupt){
            stanbyState();
        }
          
      }
      
      else if(!interrupt){//waktu antara isya dan shubuh
          stanbyState();
      }
//      Serial.println(floatnow);
    }

  }
  else{
    setelahAdzan();
  }
} 
