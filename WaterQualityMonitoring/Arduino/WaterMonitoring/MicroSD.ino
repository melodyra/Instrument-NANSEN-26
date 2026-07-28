#define SD_CS_PIN 4
extern const char* filePath; // Menggunakan variabel filePath dari tab utama

void initSD() {
  // Serial.print opsional jika sewaktu-waktu ingin dipakai untuk debug
  if (!SD.begin(SD_CS_PIN)) {
    return; 
  }

  if (!SD.exists(filePath)) {
    File dataFile = SD.open(filePath, FILE_WRITE);
    if (dataFile) {
      dataFile.println(F("Tanggal,Jam,Suhu(C),Kedalaman(m),TDS(ppm),pH"));
      dataFile.close();
    }
  }
}
