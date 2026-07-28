#define PH_PIN A1

float phADC = 0;
float phVoltage = 0;
float phValue = 0;

// offset kalibrasi
float offset = -2.44;

void updatePH() {

  long total = 0;

  for (int i = 0; i < 20; i++) {
    total += analogRead(PH_PIN);
    delay(5);
  }

  phADC = total / 20.0;

  phVoltage = phADC * 5.0 / 1023.0;

  phValue = 3.5 * phVoltage + offset;

  if (phValue < 0) phValue = 0;
  if (phValue > 14) phValue = 14;
}
