bool kdp;
int8_t SholatNow = -1;
bool azzan = false;
bool tarhim = false;
bool mur = false;

uint8_t stateMinus =0;
uint8_t indexSurah = 0;
bool interrupt = false;




void jamNormal(uint8_t x, uint8_t y) {
  char buff[8];
  if (kdp) {
    sprintf(buff, "%02d:%02d:%02d", rJam, rMen, rDet);
  }
  else {
    sprintf(buff, "%02d %02d %02d", rJam, rMen, rDet);
  }

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(x, y, buff);
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
  for (int i = 7; i >= 0; i--) {
    Serial.println(stimeFloat[i] );
    if (floatnow > stimeFloat[i]) {
      SholatNow = i;
      break;
    }
  }
  Serial.printf("waktu sekarang adalah %d", SholatNow);
  Serial.println();

}

void alertBefore(uint8_t x){
  static uint8_t alarm=x;
  static uint32_t lsRn;
  uint32_t Tmr=millis();
//  uint32_t Tmr;
  interrupt = true;
  if(alarm>(x+3))alarm=x; 
  if((alarm==2 && azzan) ){
    alarm = x;
  }
  
  if(alarm==x+0){
    myDFPlayer.pause();
    switch(SholatNow){
      case 0:
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(x + 6);
        break;
      case 3:
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(x + 7);
        break;
      case 4:
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(x + 8);
        break;
      case 5:
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(x + 9);
        break;
      case 6:
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(x + 0);
        break;
    }
    alarm = x+1;
    lsRn = millis();
//    delay(100);
  }
  else if(alarm == x+1 && digitalRead(D7)&&(Tmr-lsRn)>100){
    Serial.println("tarhim");
    myDFPlayer.volume(0.9*vol_priority);
    myDFPlayer.playMp3Folder(3);
    alarm = x+2;
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
     if(floatnow>7.3 && floatnow < 18 && (Tmr-lsRnHadist)>60000){
      if(play<1){
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(6);
        play=1;
        lsRn = Tmr;
      }else if(play==1 && (Tmr-lsRn)>1000){
        myDFPlayer.playMp3Folder(8);
        myDFPlayer.advertise(findIndex());
        delay(500);
        play=0;
        lsRn=Tmr;
        lsRnHadist = Tmr;
      }      
      
     }else if((Tmr-lsRn)>50){
//      lsRn = Tmr;
      myDFPlayer.volume(vol_ordinary);
      Serial.print("play surah ke-");
      Serial.println(indexSurah);
      myDFPlayer.playFolder(2, indexSurah);
      delay(80);
      lsRn = Tmr;
      indexSurah++;
      if(indexSurah>114) indexSurah=0;
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
        myDFPlayer.volume(vol_priority);
        myDFPlayer.playMp3Folder(6);
        setelah_adzan = 1;
        lsRn = millis();
      }else
      if(setelah_adzan==1 && (Tmr-lsRn)>200){
        myDFPlayer.volume(vol_priority);
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
            myDFPlayer.advertise(43);
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
      else if((Tmr-lsRn)>200 && setelah_adzan==2){
        azzan = false;
        setelah_adzan = 0;
  //      stateMinus = 0;
  //      tarhim = false;
        Serial.println("adzan false");
        interrupt = false;
      }
      
     
   }
}




void checkAdzan(){
  if(!azzan){
    
    for(int i=0; i<=7; i++){
      if(i==2){
        i+=2;
      }
      float selisih = stimeFloat[i] - floatnow;
//      Serial.println(selisih, 6);
//      if(i==0){
//        Serial.println("if satu");
//        if(selisih<1 && selisih>0.9898 && digitalRead(D7)){
//          myDFPlayer.volume(vol_priority);
//          myDFPlayer.playMp3Folder(4);
//        }
//      }
//      else 
      if(selisih < 0
        and (selisih > -0.03)
        ){
          Serial.println("if adzan");
          azzan = true;
          Serial.println("adzan 1");
          myDFPlayer.pause();
          myDFPlayer.volume(0.9*vol_priority);
          if(i==1){
            myDFPlayer.playMp3Folder(2);
          }else{
            myDFPlayer.playMp3Folder(1);
          }
          
          delay(500);
          waktuSholatNow();
          tarhim = false;
          stateMinus=0;
          return;
      }
      else if(selisih > 0){
        Serial.println("if dua");
//        if(selisih<0.08194 and !tarhim){
//          Serial.println("tarhim");
//          myDFPlayer.volume(vol_priority);
//          myDFPlayer.playMp3Folder(3);
//          delay(500);
//          tarhim = true;
//          return;
//        }
//        else 
        if(selisih<0.08333){
          alertBefore(5);//mainkan sebelum 5 menit
          
        }
        else if(selisih<0.167){
          alertBefore(10);
          
        }
        else if(!interrupt){
            stanbyState();
        }
          
      }
      else if(!interrupt){
          stanbyState();
      }
    }

  }
  else{
    setelahAdzan();
  }
} 
