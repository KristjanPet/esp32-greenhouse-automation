#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "../include/secrets.h"
#include "web_server.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000; // 5 seconds

void setupWiFi()
{
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void setupOTA()
{
  ArduinoOTA
      .onStart([]()
               { Serial.println("Start OTA update"); })
      .onEnd([]()
             { Serial.println("\nEnd OTA"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               { Serial.printf("Error[%u]: ", error); });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  setupWiFi();
  setupOTA();
  setupWebServer();
  setupTempSensor();
  setupTempSensor2();
  setupWindSensor();
}

void loop()
{
  unsigned long currentMillis = millis();

  ArduinoOTA.handle();
  handleWebServer();

  if (currentMillis - lastPrintTime >= printInterval)
  {
    lastPrintTime = currentMillis;

    float temp1 = getTemperatureC();
    float temp2 = getTemperature2C();
    float avgTemp = (temp1 + temp2) / 2.0;

    // Serial.print("Temp1: ");
    // Serial.print(temp1);
    // Serial.print(" °C | Temp2: ");
    // Serial.print(temp2);
    // Serial.print(" °C | Avg: ");
    // Serial.print(avgTemp);
    // Serial.print(" °C | Humidity: ");
    // Serial.print(getHumidity());
    Serial.println(" %");
    Serial.print("Wind Speed: ");
    Serial.print(getWindSpeed());
    Serial.println(" m/s");
  }
}
