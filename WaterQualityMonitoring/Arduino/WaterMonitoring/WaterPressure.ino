#define PRESSURE_PIN A3

extern float waterDepth;

// Hasil kalibrasi sensor
const float VOLTAGE_AT_0M = 0.50;   // Tegangan saat 0 meter
const float VOLTAGE_AT_4M = 0.60;   // Tegangan saat 4 meter

void updatePressure() {

  int adc = analogRead(PRESSURE_PIN);

  // Konversi ADC ke Volt
  float voltage = adc * (5.0 / 1023.0);

  // Batasi agar tidak keluar dari hasil kalibrasi
  if (voltage < VOLTAGE_AT_0M)
    voltage = VOLTAGE_AT_0M;

  if (voltage > VOLTAGE_AT_4M)
    voltage = VOLTAGE_AT_4M;

  // Konversi ke meter
  waterDepth = (voltage - VOLTAGE_AT_0M) *
               (4.0 / (VOLTAGE_AT_4M - VOLTAGE_AT_0M));

  // Pembulatan 0.5 meter
  waterDepth = round(waterDepth * 2.0) / 2.0;

  // Pengaman
  if (waterDepth < 0.0)
    waterDepth = 0.0;

  if (waterDepth > 4.0)
    waterDepth = 4.0;
}
