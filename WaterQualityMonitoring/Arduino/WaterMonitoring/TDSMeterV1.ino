#define TDS_PIN A0

const float TDS_VREF = 5.0;
const int SCOUNT = 30;

int analogBuffer[SCOUNT];
int analogBufferIndex = 0;

int tdsADC = 0;
float tdsVoltage = 0;

int getMedian(int array[], int len) {

  int temp[len];

  for (int i = 0; i < len; i++)
    temp[i] = array[i];

  for (int j = 0; j < len - 1; j++) {
    for (int i = 0; i < len - j - 1; i++) {

      if (temp[i] > temp[i + 1]) {
        int t = temp[i];
        temp[i] = temp[i + 1];
        temp[i + 1] = t;
      }
    }
  }

  if (len % 2 == 0)
    return (temp[len / 2] + temp[len / 2 - 1]) / 2;

  return temp[len / 2];
}

void updateTDS() {

  static unsigned long sampleTime = 0;

  if (millis() - sampleTime >= 40) {

    sampleTime = millis();

    analogBuffer[analogBufferIndex] = analogRead(TDS_PIN);

    analogBufferIndex++;

    if (analogBufferIndex >= SCOUNT)
      analogBufferIndex = 0;
  }

  int bufferCopy[SCOUNT];

  for (int i = 0; i < SCOUNT; i++)
    bufferCopy[i] = analogBuffer[i];

  tdsADC = getMedian(bufferCopy, SCOUNT);
  tdsVoltage = tdsADC * TDS_VREF / 1023.0;
}

float readTDS(float temperature) {

  float compensationCoefficient =
      1.0 + 0.02 * (temperature - 25.0);

  float compensationVoltage =
      tdsVoltage / compensationCoefficient;

  return (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
        - 255.86 * compensationVoltage * compensationVoltage
        + 857.39 * compensationVoltage) * 0.5;
}