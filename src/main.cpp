#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <time.h>
#include "../include/secrets.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "thresholds.h"
#include "wind_sensor.h"
#include "automation_logic.h"
#include "motor_control.h"
#include "buttons_control.h"
#include "web_server_async.h"

#include "wind_sensor.h"

unsigned long lastPrintTime = 0;

const unsigned long wifiReconnectInterval = 10000;
unsigned long lastWiFiReconnectAttempt = 0;
bool wasWiFiConnected = false;
bool otaReady = false;
bool timeConfigured = false;

void setupWiFi()
{
  Serial.println("Starting WiFi connection...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWiFiReconnectAttempt = millis();
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
  otaReady = true;
  Serial.println("OTA Ready");
}

void setupNetworkTime()
{
  configTime(3600, 3600, "pool.ntp.org", "time.nist.gov"); // CET/CEST crude: 1h offset + DST 1h
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();
  timeConfigured = true;
  Serial.println("Network time configured.");
}

void maintainWiFi()
{
  const bool connected = WiFi.status() == WL_CONNECTED;
  const unsigned long now = millis();

  if (connected)
  {
    if (!wasWiFiConnected)
    {
      Serial.println("WiFi connected!");
      Serial.println("IP address: " + WiFi.localIP().toString());
      wasWiFiConnected = true;
    }

    if (!otaReady)
    {
      setupOTA();
    }

    if (!timeConfigured)
    {
      setupNetworkTime();
    }

    return;
  }

  if (wasWiFiConnected)
  {
    Serial.println("WiFi lost. Reconnecting in background...");
    wasWiFiConnected = false;
  }

  if (now - lastWiFiReconnectAttempt >= wifiReconnectInterval)
  {
    Serial.println("Trying to reconnect to WiFi...");
    WiFi.disconnect(false);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    lastWiFiReconnectAttempt = now;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  setupWiFi();
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS mount failed.");
    return;
  }
  loadThresholds();
  loadCurrentState();
  setupTempSensor();
  setupTempSensor2();
  setupWindSensor();

  setupMotorPins();
  manualInit();
  setupWebServerAsync();
  Serial.println("Setup complete.");
}

void loop()
{
  unsigned long currentMillis = millis();

  maintainWiFi();

  if (otaReady)
  {
    ArduinoOTA.handle();
  }

  manualTick();
  tickMotion();

  if (currentMillis - lastPrintTime >= sensorReadInterval)
  {
    lastPrintTime = currentMillis;

    readTemperatureC();
    readTemperature2C();
    updateWindSpeedBuffer(getWindSpeed());

    if (!manualIsActive())
    { // pause automation while any button is held
      handleAutoControl();
    }

    sseUpdateSensors();
    sseUpdateStatus(manualIsActive(), motorStateStr());
  }
}
