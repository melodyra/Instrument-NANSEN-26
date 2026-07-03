#include <time.h>

const char* ntpServer = "id.pool.ntp.org";
const long  gmtOffset_sec = 25200; // UTC+7 (WIB)

void initRTCandWiFi(const char* ssid, const char* pass) {
  if (!rtc.begin()) { while (1); }

  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, pass);
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500); Serial.print("."); timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, 0, ntpServer);
    struct tm t;
    if (getLocalTime(&t)) {
      rtc.adjust(DateTime(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec));
      Serial.println("\nRTC Synced!");
    }
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  } else {
    Serial.println("\nWiFi Offline. Using RTC standalone.");
  }
}

void tampilkanDataRTC() {
  DateTime now = rtc.now();
  Serial.printf("%02d-%02d-%04d | %02d:%02d:%02d | Suhu: %.2f C\n", 
                now.day(), now.month(), now.year(), 
                now.hour(), now.minute(), now.second(),
                rtc.getTemperature());
}