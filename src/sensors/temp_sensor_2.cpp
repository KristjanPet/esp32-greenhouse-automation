#include <OneWire.h>
#include <DallasTemperature.h>
#include "temp_sensor_2.h"

#define ONE_WIRE_BUS 5  // GPIO 5 for DS18B20 data

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float lastTemp2 = -100;

void setupTempSensor2() {
  sensors.begin();
  delay(100);
}

void readTemperature2C(){
  sensors.requestTemperatures();

  float t = sensors.getTempCByIndex(0);  // Assume one sensor
  if (t != DEVICE_DISCONNECTED_C) {
    lastTemp2 = t;
  }
}

float getTemperature2C() {
  return lastTemp2;
}
