#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <time.h>
#include "../include/secrets.h"
#include "web_server.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "thresholds.h"
#include "wind_sensor.h"
#include "automation_logic.h"
#include "motor_control.h"
#include "buttons_control.h"

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1500; // 1.5 seconds

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
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed.");
    return;
  }
  loadThresholds();
  setupWebServer();
  setupTempSensor();
  setupTempSensor2();
  setupWindSensor();
  setupMotorPins();
  manualInit();
  configTime(3600, 3600, "pool.ntp.org", "time.nist.gov"); // CET/CEST crude: 1h offset + DST 1h
  // Better: use TZ string for Ljubljana:
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1); tzset();
  Serial.println("Setup complete.");
}

void loop()
{
  unsigned long currentMillis = millis();

  ArduinoOTA.handle();
  handleWebServer();
  updateMotorTimer();
  manualTick();

  if (currentMillis - lastPrintTime >= printInterval)
  {
    lastPrintTime = currentMillis;

    float temp1 = getTemperatureC();
    float temp2 = getTemperature2C();
    float avgTemp = (temp1 + temp2) / 2.0;
    updateWindSpeedBuffer(getWindSpeed());
    float windSpeed = getAverageWindSpeed();

    if (!manualIsActive()){          // pause automation while any button is held
      handleAutoControl(avgTemp, windSpeed);
    }
  }
}
