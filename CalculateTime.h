void nextDay(uint8_t jam, uint8_t tgl, uint8_t bulan, uint16_t tahun, uint8_t& nextTgl, uint8_t& nextBulan, uint16_t& nextTahun, uint8_t& Day2H){
//  Serial.println(Hjr_Day);
  tmElements_t tanggalPertama = {0,0,0,Day2H+1,tgl,bulan,CalendarYrToTm(tahun)}; // hari + 1
//  tmElements_t tanggalKedua =   {0,0,0,4,10,12,CalendarYrToTm(2020)};
  tmElements_t next;
  breakTime(makeTime(tanggalPertama)+86400, next);
  nextTgl = next.Day;
  nextBulan = next.Month;
  nextTahun = tmYearToCalendar(next.Year);
  Day2H = next.Wday;
//  Serial.println(makeTime(tanggalPertama));
//  Serial.println(Day2H);
//  Serial.print("Tanggal pertama = ");
//  Serial.print(tanggalPertama.Wday);
//  Serial.print(", ");
//  Serial.print(tanggalPertama.Day);
//  Serial.print("/");
//  Serial.print(tanggalPertama.Month);
//  Serial.print("/");
//  Serial.println(tmYearToCalendar(tanggalPertama.Year));
//  Serial.print("Tanggal kedua = ");
//  Serial.print(next.Wday);
//  Serial.print(", ");
//  Serial.print(next.Day);
//  Serial.print("/");
//  Serial.print(next.Month);
//  Serial.print("/");
//  Serial.println(tmYearToCalendar(next.Year));
}
