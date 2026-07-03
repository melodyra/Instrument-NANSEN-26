void initSD(int csPin) {
  Serial.print("Menginisialisasi MicroSD... ");
  
  if (!SD.begin(csPin)) {
    Serial.println("Gagal! Periksa modul atau kartu memori.");
    return;
  }
  Serial.println("Sukses! Kartu terdeteksi.");
}

void simpanKeSD() {
  File dataFile = SD.open("/datalog.txt", FILE_WRITE);
  
  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.printf("%02d-%02d-%04d | %02d:%02d:%02d | Suhu: %.2f C\n", 
                    now.day(), now.month(), now.year(), 
                    now.hour(), now.minute(), now.second(),
                    rtc.getTemperature());
    dataFile.close();
    Serial.println("Data berhasil disimpan ke SD Card.");
  } else {
    Serial.println("Gagal membuka file datalog.txt");
  }
}