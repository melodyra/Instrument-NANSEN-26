#define PRESSURE_PIN A3

const float VREF = 5.0;
const float ZERO_VOLTAGE = 0.50; 
const float FULL_VOLTAGE = 4.50;
const float MAX_PRESSURE = 1.60;

float pressureADC = 0.0;
float pressureVoltage = 0.0;
float pressureValue = 0.0;
float waterDepth = 0.0; 

void updatePressure() {
  int analogValue = analogRead(A0); 
  float voltage = analogValue * (5.0 / 1023.0);
  
  waterDepth = (voltage - 0.5) * 2.0; 

  waterDepth = waterDepth - 1.45;
  
  if (waterDepth < 0) {
    waterDepth = 0.0;
  }
}