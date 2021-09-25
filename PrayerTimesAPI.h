WiFiClientSecure client;
float floatOnline[] = {0,0,0,0, 0,0,0,0};
uint8_t intOnline[8][3]={
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
};
#define HOST "api.myquran.com"

bool online = false;



void setupAPI()
{
  client.setInsecure();
}

void updateWaktuSholatOnline()
{
  if (!client.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }
  yield();
  
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
//  client.print("/v1/sholat/jadwal/1221/"+String(rTah)+"/"+String(rBul)+"/"+String(rTgl)); // kab bekasi
  client.print("/v1/sholat/jadwal/1203/"+String(rTah)+"/"+String(rBul)+"/"+String(rTgl)); // kab bekasi
//  client.print("/v1/sholat/jadwal/1203/2021/09/20"); // kota bekasi
  
  client.println(F(" HTTP/1.0"));

  //Headers
  client.print(F("Host: "));
  client.println(HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));

  // Check if it responded "OK" with either HTTP 1.0 or 1.1
  if (strcmp(status, "HTTP/1.0 200 OK") == 0 || strcmp(status, "HTTP/1.1 200 OK") == 0)
  {
    {
      Serial.print(F("Unexpected response : "));
      Serial.println(status);
//      return;
    }
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }
  
  String data = "";
  while (client.available() && client.peek() != '{' && client.peek() != '[')
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
    data += c;
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  while (client.available())
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    data +=c;
  }
  Serial.println();
  Serial.println("data adalah : ");
  Serial.println(data);
  DynamicJsonDocument doc(1024); //For ESP32/ESP8266 you'll mainly use dynamic.

  DeserializationError error = deserializeJson(doc, data);

  if (!error) {
    String imsak = doc["data"]["jadwal"]["imsak"];
    String subuh = doc["data"]["jadwal"]["subuh"];
    String terbit = doc["data"]["jadwal"]["terbit"];
    String dhuha = doc["data"]["jadwal"]["dhuha"];
    
    String dzuhur = doc["data"]["jadwal"]["dzuhur"];
    String ashar = doc["data"]["jadwal"]["ashar"];
    String maghrib = doc["data"]["jadwal"]["maghrib"];
    String isya = doc["data"]["jadwal"]["isya"];
    
    intOnline[0][0] = imsak.substring(0,2).toInt();
    intOnline[0][1] = imsak.substring(3,5).toInt();

    intOnline[1][0] = subuh.substring(0,2).toInt();
    intOnline[1][1] = subuh.substring(3,5).toInt();

    intOnline[2][0] = terbit.substring(0,2).toInt();
    intOnline[2][1] = terbit.substring(3,5).toInt();

    intOnline[3][0] = dhuha.substring(0,2).toInt();
    intOnline[3][1] = dhuha.substring(3,5).toInt();

    intOnline[4][0] = dzuhur.substring(0,2).toInt();
    intOnline[4][1] = dzuhur.substring(3,5).toInt();

    intOnline[5][0] = ashar.substring(0,2).toInt();
    intOnline[5][1] = ashar.substring(3,5).toInt();

    intOnline[6][0] = maghrib.substring(0,2).toInt();
    intOnline[6][1] = maghrib.substring(3,5).toInt();

    intOnline[7][0] = isya.substring(0,2).toInt();
    intOnline[7][1] = isya.substring(3,5).toInt();
    Serial.println("waktu dzuhur adalah");
    Serial.printf("%02d : %02d",intOnline[4][0],intOnline[4][1]);
    Serial.println();

    for(int i = 0; i<=7; i++){
      floatOnline[i] = (float)intOnline[i][0] + (float)intOnline[i][1]/60;
    }
    Serial.println(floatOnline[4]);
    
  } else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
}
