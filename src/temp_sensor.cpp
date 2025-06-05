#include <DHT.h>
#include "temp_sensor.h"

#define DHTPIN 4      
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);
float lastTemp = -100; // default for failure

void setupTempSensor() {
  dht.begin();
}

float getTemperatureC() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    return lastTemp;  // fallback to last known value
  } else {
    lastTemp = t;
    return t;
  }
}
