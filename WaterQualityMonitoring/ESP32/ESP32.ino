#include "BluetoothSerial.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Cek ketersediaan Bluetooth di ESP32
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it
#endif

BluetoothSerial SerialBT;

// Variabel konfigurasi dari HP
String ssid = "";
String password = "";
String targetDepthStr = "";
String intervalStr = "";
bool isConfigured = false;

// URL Google Apps Script / Webhook Cloud kamu (ganti dengan URL deployment kamu nanti)
const char* serverUrl = "https://script.google.com/macros/s/AKfycbyp9BhQD1rD3UYtDFpFLMxmgDb_JeF5VNDGy1m0th5VcDSIjv6XYb-13pVakpXfOcCp/exec"; 

void setup() {
  // Serial Monitor bawaan ESP32 & Serial ke Arduino Uno (menggunakan Serial2 atau Serial biasa)
  Serial.begin(115200); 
  
  // Inisialisasi Bluetooth dengan nama sesuai manual book OceaSense
  SerialBT.begin("ESP32_WATERLOGGER"); 
  Serial.println("ESP32 Bluetooth Siap. Menunggu konfigurasi dari HP...");
}

void loop() {
  // Cek apakah ada data masuk dari aplikasi Serial Bluetooth Terminal di HP
  if (SerialBT.available()) {
    String incomingCommand = SerialBT.readStringUntil('\n');
    incomingCommand.trim();
    
    Serial.print("Perintah diterima dari HP: ");
    Serial.println(incomingCommand);

    // Contoh format input dari HP di terminal:
    // start; kel:2.0; in:10s; wifi:Namaku; pass:12345678
    // Atau jika offline: start; kel:2.0; in:10s; wifi:-; pass:-
    
    if (incomingCommand.startsWith("start")) {
      parseConfig(incomingCommand);
      isConfigured = true;

      // Teruskan perintah parameter ke Arduino Uno via Serial Hardware (TX/RX)
      // Contoh meneruskan string ke Arduino Uno:
      Serial.println(incomingCommand); 

      // Cek apakah mode WiFi aktif atau offline (-)
      if (ssid != "-" && password != "-") {
        connectToWiFi();
      } else {
        Serial.println("Mode Offline: WiFi dinonaktifkan, data hanya dicatat lokal (SD Card).");
        SerialBT.println("Mode Offline aktif. Alat berjalan tanpa WiFi.");
      }
    } 
    else if (incomingCommand.equalsIgnoreCase("stop")) {
      isConfigured = false;
      WiFi.disconnect();
      Serial.println("Sistem dihentikan oleh pengguna.");
      SerialBT.println("Sistem dihentikan.");
    }
  }

  // Jika sistem sudah berjalan dan terhubung ke WiFi, contoh fungsi kirim data tiruan ke Cloud/Sheets
  // (Nanti data sensor aslinya bisa dilempar dari Arduino Uno ke ESP32 via Serial, lalu dikirim ke web)
  if (isConfigured && WiFi.status() == WL_CONNECTED) {
    // Contoh pengiriman data periodik ke Google Sheets via HTTP POST
    // sendDataToCloud(28.5, 2.0, 450.0, 7.2); 
    // delay(10000); // Sesuaikan interval pengiriman
  }
}

// Fungsi untuk memecah (parsing) string input dari HP
void parseConfig(String cmd) {
  // Ambil SSID WiFi
  int wifiIdx = cmd.indexOf("wifi:");
  if (wifiIdx != -1) {
    int endIdx = cmd.indexOf(";", wifiIdx);
    if (endIdx == -1) endIdx = cmd.length();
    ssid = cmd.substring(wifiIdx + 5, endIdx);
    ssid.trim();
  }

  // Ambil Password WiFi
  int passIdx = cmd.indexOf("pass:");
  if (passIdx != -1) {
    int endIdx = cmd.indexOf(";", passIdx);
    if (endIdx == -1) endIdx = cmd.length();
    password = cmd.substring(passIdx + 5, endIdx);
    password.trim();
  }
}

// Fungsi koneksi WiFi
void connectToWiFi() {
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  SerialBT.print("Menghubungkan ke WiFi: ");
  SerialBT.println(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());
  
  unsigned long startAttemptTime = millis();
  
  // Coba konek selama 10 detik
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Berhasil Terhubung!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    SerialBT.println("WiFi Terhubung! Data dikirim ke Cloud & SD Card.");
  } else {
    Serial.println("\nGagal terhubung ke WiFi! Beralih ke Mode Offline (SD Card saja).");
    SerialBT.println("Gagal konek WiFi. Sistem berjalan offline.");
    ssid = "-"; // Paksa set offline agar tidak mengganggu looping
  }
}

// Fungsi opsional untuk mengirim data ke Google Sheets/Cloud via HTTP POST
void sendDataToCloud(float temp, float depth, float tds, float ph) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Format data yang dikirim ke Webhook/Google Apps Script
    String httpRequestData = "temp=" + String(temp) + "&depth=" + String(depth) + "&tds=" + String(tds) + "&ph=" + String(ph);
    
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode > 0) {
      Serial.print("Data terkirim ke Cloud, Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error kirim data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}
