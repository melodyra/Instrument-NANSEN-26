#include <OneWire.h>
#include <DallasTemperature.h>

extern DallasTemperature sensor;

float readTemperature() {

  sensor.requestTemperatures();

  return sensor.getTempCByIndex(0);
}