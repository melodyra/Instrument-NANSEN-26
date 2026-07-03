void initRTC() {
  if (!rtc.begin()) {
    Serial.println(F("KACAU: RTC DS3231 Gagal Terdeteksi!"));
    return; 
  }

  DateTime cekWaktu = rtc.now();

  if (rtc.lostPower() || cekWaktu.day() == 85 || cekWaktu.year() < 2020) {
    Serial.println(F("RTC Terkunci Eror! Memaksa reset ke waktu laptop..."));

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  Serial.println(F("RTC Berhasil Diinisialisasi dan Sinkron!"));
}