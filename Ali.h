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

void checkAdzan(){
  if(!azzan){
    
    for(int i=0; i<=7; i++){
      if(i==2){
        i+=2;
      }
      float selisih = stimeFloat[i] - floatnow;
      if(i==0){
        if(selisih<1 && selisih>0.9898 && digitalRead(D7)){
          myDFPlayer.volume(vol_priority);
          myDFPlayer.playMp3Folder(4);
        }
      }
      else if(selisih < 0
        and (selisih > -0.03)
        ){
          azzan = true;
          Serial.println("adzan 1");
          myDFPlayer.volume(15);
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
        if(selisih<0.08194 and !tarhim){
          Serial.println("tarhim");
          myDFPlayer.volume(vol_priority);
          myDFPlayer.playMp3Folder(3);
          delay(500);
          tarhim = true;
          return;
        }
        else if(selisih<0.08333 && stateMinus<2){
          Serial.println("5 menit sebelum sholat");
          myDFPlayer.pause();
          switch(SholatNow){
            case 0:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(11);
              stateMinus++;
              break;
            case 3:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(12);
              stateMinus++;
              break;
            case 4:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(13);
              stateMinus++;
              break;
            case 5:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(14);
              stateMinus++;
              break;
            case 6:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(15);
              stateMinus++;
              break;
          }
          interrupt=true;
        }
        else if(selisih<0.167 && stateMinus<1){
          Serial.println("10 menit sebelum sholat");
          myDFPlayer.pause();
          switch(SholatNow){
            case 0:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(16);
              stateMinus++;
              break;
            case 3:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(17);
              stateMinus++;
              break;
            case 4:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(18);
              stateMinus++;
              break;
            case 5:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(19);
              stateMinus++;
              break;
            case 6:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.playMp3Folder(20);
              stateMinus++;
              break;
          }
          interrupt = true;
          delay(5000);
          myDFPlayer.playMp3Folder(3);
        }
        else if(!interrupt){
          static uint32_t lsRn;
          uint32_t Tmr = millis();
          if(digitalRead(D7)){
//            if((Tmr-lsRn)>1000){
              lsRn = Tmr;
              myDFPlayer.volume(5);
              Serial.print("play surah ke-");
              Serial.println(indexSurah);
              myDFPlayer.playFolder(2, indexSurah);
              delay(90);
              indexSurah++;
              if(indexSurah>114) indexSurah=0;
//            }
            
          }
        }
          
      }
      if(rHar==6){
        if(selisih<0.333 and selisih > 0 and !mur){
          myDFPlayer.volume(vol_priority);
          myDFPlayer.playMp3Folder(4);
        }        
      }
    }

  }
  else{
    if(digitalRead(D7)){//ketika lagu diputar kondisinya LOW
      static uint8_t setelah_adzan;
      if(setelah_adzan<1){
        
      }
      azzan = false;
      stateMinus = 0;
      tarhim = false;
      Serial.println("adzan false");
      interrupt = false;
      switch(SholatNow){
            case 1:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.advertise(10);
              stateMinus++;
              break;
            case 4:
              myDFPlayer.volume(vol_priority);
//              myDFPlayer.advertise(41);
//              myDFPlayer.advertise(43);
              myDFPlayer.advertise(42);
              stateMinus++;
              break;
            case 5:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.advertise(51);
              stateMinus++;
              break;
            case 6:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.advertise(61);
              stateMinus++;
              break;
            case 7:
              myDFPlayer.volume(vol_priority);
              myDFPlayer.advertise(71);
              stateMinus++;
              break;
          }
    }
  }
} 

void stanbyState(){
  if(azzan && tarhim) return;

  
  if(digitalRead(D7)){
    if(SholatNow>7 && SholatNow<2){
      myDFPlayer.volume(10);
    }
    else{
      myDFPlayer.volume(20);
    }
    myDFPlayer.playMp3Folder(4);
  }
}
