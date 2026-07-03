#define SD_CS_PIN 4
const char* filePath = "DATA.CSV";

void initSD() {
  Serial.print(F("Menginisialisasi MicroSD... "));
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("GAGAL!"));
    return; 
  }
  Serial.println(F("SUKSES!"));

  if (!SD.exists(filePath)) {
    File dataFile = SD.open(filePath, FILE_WRITE);
    if (dataFile) {
      dataFile.println(F("Tanggal,Jam,Suhu(C),Kedalaman(m),TDS(ppm),pH"));
      dataFile.close();
    }
  }
}