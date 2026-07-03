#include <Wire.h>
#include "RTClib.h"
#include <WiFi.h>
#include <SPI.h>
#include <SD.h>

// ISI DENGAN WIFI KAMU
const char* ssid     = "UnpadWiFiGuest";
const char* password = "";

// Pin CS untuk MicroSD
const int chipSelect = 5;

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Inisialisasi I2C

  // 1. Inisialisasi RTC & WiFi
  initRTCandWiFi(ssid, password);

  // 2. Inisialisasi MicroSD (Fungsi dari tab baru)
  initSD(chipSelect);
}

void loop() {
  // Tampilkan data di Serial Monitor
  tampilkanDataRTC();
  
  // Contoh jika kamu ingin sekalian simpan data ke MicroSD setiap 5 detik
  // simpanKeSD(); 

  delay(1000);
}