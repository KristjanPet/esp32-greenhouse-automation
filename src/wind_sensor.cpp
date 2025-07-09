#include <Arduino.h>
#include "wind_sensor.h"

#define WIND_SENSOR_PIN 34  // ADC1 channel

void setupWindSensor() {
  pinMode(WIND_SENSOR_PIN, INPUT);
}

float getWindSpeed() {
  int raw = analogRead(WIND_SENSOR_PIN);  // 0–4095
  
  float voltage = (raw / 4095.0) * 3.3;    // voltage after divider (0–3.3V)

  // Convert voltage to wind speed (assuming 0–5V = 0–30 m/s, scaled down)
  float originalVoltage = voltage * (5.0 / 3.3);  // scale back up
  float windSpeed = (originalVoltage / 5.0) * 30.0;

  return windSpeed;
}
