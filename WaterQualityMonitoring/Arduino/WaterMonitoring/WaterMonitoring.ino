#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5

RTC_DS3231 rtc;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

extern float waterDepth; 
extern float phValue;
int getMedian(int array[], int len);

unsigned long waktuLama = 0; 

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  sensor.begin();
  
  initRTC();
  initSD(); 
  Serial.println(F("Sistem Water Monitoring Siap!"));
}

void loop() {
  updatePressure();
  updateTDS();
  updatePH();

  float temperature = readTemperature();
  float tds = readTDS(temperature);
  
  DateTime now = rtc.now();

  if (millis() - waktuLama >= 10000) { 
    waktuLama = millis();
    
    if(now.day() < 10) Serial.print('0'); Serial.print(now.day()); Serial.print('-');
    if(now.month() < 10) Serial.print('0'); Serial.print(now.month()); Serial.print('-');
    Serial.print(now.year()); Serial.print(" ");
    if(now.hour() < 10) Serial.print('0'); Serial.print(now.hour()); Serial.print(':');
    if(now.minute() < 10) Serial.print('0'); Serial.print(now.minute()); Serial.print(':');
    if(now.second() < 10) Serial.print('0'); Serial.print(now.second());
    
    Serial.print(F(" | Temp: "));  Serial.print(temperature, 1); Serial.print(F(" C"));
    Serial.print(F(" | Depth: ")); Serial.print(waterDepth, 2);  Serial.print(F(" m"));
    Serial.print(F(" | TDS: "));    Serial.print(tds, 0);         Serial.print(F(" ppm"));
    Serial.print(F(" | pH: "));     Serial.print(phValue, 2);

    File dataFile = SD.open("DATA.CSV", FILE_WRITE);
    if (dataFile) {
      if(now.day() < 10) dataFile.print('0'); dataFile.print(now.day()); dataFile.print('-');
      if(now.month() < 10) dataFile.print('0'); dataFile.print(now.month()); dataFile.print('-');
      dataFile.print(now.year()); dataFile.print(',');
      
      if(now.hour() < 10) dataFile.print('0'); dataFile.print(now.hour()); dataFile.print(':');
      if(now.minute() < 10) dataFile.print('0'); dataFile.print(now.minute()); dataFile.print(':');
      if(now.second() < 10) dataFile.print('0'); dataFile.print(now.second()); dataFile.print(',');

      dataFile.print(temperature, 1); dataFile.print(',');
      dataFile.print(waterDepth, 2);  dataFile.print(',');
      dataFile.print(tds, 0);         dataFile.print(',');
      dataFile.print(phValue, 2);     
      dataFile.println();            
      
      dataFile.flush(); 
      dataFile.close(); 
      Serial.println(F(" -> [SUKSES MASUK CSV]"));
    } else {
      Serial.println(F(" -> [GAGAL AKSES SD CARD]"));
    }
  }
}