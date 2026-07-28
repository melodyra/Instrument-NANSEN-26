#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5          // Pin OneWire untuk sensor suhu DS18B20

RTC_DS3231 rtc;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

// Variabel global sistem
float waterDepth = 0.0; 
float phValue = 7.0;            
unsigned long samplingInterval = 10000; // Default jeda waktu 10 detik (ms)
float targetDepth = 0.0;                // Target kedalaman dari input HP/ESP32
bool isLoggingStarted = false;          // Status apakah alat mulai mengambil data

unsigned long waktuLama = 0; 
const char* filePath = "DATA.CSV";

// Deklarasi fungsi dari tab lain
void initSD();
void updatePressure();

void setup() {
  Serial.begin(115200); // Komunikasi Serial dengan ESP32 atau Serial Monitor
  Wire.begin(); 
  sensor.begin();
  
  if (!rtc.begin()) {
    // Penanganan error jika RTC gagal
  }

  initSD(); 
}

void loop() {
  // 1. Selalu cek apakah ada data/perintah masuk dari ESP32
  parseIncomingCommand();

  // 2. Jika perintah 'start' belum diaktifkan, abaikan pembacaan sensor & simpan data
  if (!isLoggingStarted) {
    return; 
  }

  // 3. Update pembacaan sensor tekanan
  updatePressure();
  
  float temperature = readTemperature();
  float tds = readTDS(temperature);
  updatePH(); 

  DateTime now = rtc.now();

  // 4. Pengambilan sampel berdasarkan interval waktu yang dinamis (bisa diubah dari HP)
  if (millis() - waktuLama >= samplingInterval) { 
    waktuLama = millis();
    
    // Format Teks Timestamp
    String timestamp = "";
    if(now.day() < 10) timestamp += '0'; timestamp += String(now.day()) + "-";
    if(now.month() < 10) timestamp += '0'; timestamp += String(now.month()) + "-";
    timestamp += String(now.year()) + " ";
    if(now.hour() < 10) timestamp += '0'; timestamp += String(now.hour()) + ":";
    if(now.minute() < 10) timestamp += '0'; timestamp += String(now.minute()) + ":";
    if(now.second() < 10) timestamp += '0'; timestamp += String(now.second());

    // Simpan ke MicroSD secara paralel
    File dataFile = SD.open(filePath, FILE_WRITE);
    if (dataFile) {
      dataFile.print(timestamp); dataFile.print(',');
      dataFile.print(temperature, 1); dataFile.print(',');
      dataFile.print(waterDepth, 2);  dataFile.print(',');
      dataFile.print(tds, 0);         dataFile.print(',');
      dataFile.print(phValue, 2);     
      dataFile.println();            
      dataFile.flush(); 
      dataFile.close(); 
    }
  }
}

// Fungsi untuk membaca string perintah yang masuk dari ESP32/Serial
void parseIncomingCommand() {
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    inputString.trim();

    // Contoh format string yang diharapkan masuk: "start,kel:2.0,in:10s"
    if (inputString.startsWith("start")) {
      isLoggingStarted = true;

      // Cek parameter kedalaman (misal format "kel:2.0")
      int kelIndex = inputString.indexOf("kel:");
      if (kelIndex != -1) {
        String kelStr = inputString.substring(kelIndex + 4);
        targetDepth = kelStr.toFloat();
      }

      // Cek parameter interval waktu (misal format "in:10s" atau "in:5m")
      int inIndex = inputString.indexOf("in:");
      if (inIndex != -1) {
        String inStr = inputString.substring(inIndex + 3);
        int val = inStr.toInt();
        if (val > 0) {
          if (inStr.indexOf("s") != -1) {
            samplingInterval = val * 1000L;       // Konversi detik ke milidetik
          } else if (inStr.indexOf("m") != -1) {
            samplingInterval = val * 60000L;      // Konversi menit ke milidetik
          }
        }
      }
    } 
    else if (inputString.equalsIgnoreCase("stop")) {
      isLoggingStarted = false;
    }
  }
}

float readTemperature() {
  sensor.requestTemperatures();
  return sensor.getTempCByIndex(0);
}

float readTDS(float temp) {
  return 0.0; 
}

void updatePH() {
}
