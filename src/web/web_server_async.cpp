#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

#include "web_server_async.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"
#include "buttons_control.h"
#include "thresholds.h"
#include "logger.h"
#include "motor_control.h"

// ---- Async server + SSE -----------------------------------------------------
static AsyncWebServer server(80);
static AsyncEventSource events("/events"); // SSE at /events

// Cached snapshots + change detection (small epsilons)
static float lastT1 = NAN, lastT2 = NAN, lastAvg = NAN, lastHum = NAN, lastWind = NAN;
static bool lastManual = false;
static String lastMotor;

static inline bool changed(float a, float b, float eps)
{
  if (isnan(a) != isnan(b))
    return true;
  if (isnan(a))
    return false;
  return fabsf(a - b) > eps;
}

void setupWebServerAsync()
{
  // Static files from SPIFFS
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  // If you want explicit routes too:
  // server.serveStatic("/style.css", SPIFFS, "/style.css");
  // server.serveStatic("/main.js",  SPIFFS, "/main.js");

  // SSE endpoint
  server.addHandler(&events);

  server.on("/api/init", HTTP_GET, [](AsyncWebServerRequest *req)
            {
        StaticJsonDocument<2048> doc;

        // Sensors
        doc["sensors"]["temp1"]    = getTemperatureC();
        doc["sensors"]["temp2"]    = getTemperature2C();
        doc["sensors"]["avg"]      = (getTemperatureC() + getTemperature2C()) / 2.0;
        doc["sensors"]["humidity"] = getHumidity();
        doc["sensors"]["wind"]     = getAverageWindSpeed();

        // Status
        String motorStateString = motorStateStr();
        float currentMotorPercent = getCurrentPercent();
        // printf("Motor state string: %s\n", motorStateString.c_str());
        // printf("Motor percent: %f\n", currentMotorPercent);
        doc["status"]["motorState"] = motorStateString;
        doc["status"]["motorPercent"] = currentMotorPercent;

        //logs
        JsonArray logArr = doc["logs"].to<JsonArray>();
        appendLogsTo(logArr, 100);

        String out; 
        serializeJson(doc, out);
        req->send(200, "application/json", out); });

  server.on("/api/thresholds", HTTP_GET, [](AsyncWebServerRequest *req)
            {
    extern Thresholds currentThresholds;
    StaticJsonDocument<256> doc;
    doc["tempOpen"]    = currentThresholds.tempOpen;
    doc["tempClose"]   = currentThresholds.tempClose;
    doc["windClose"]   = currentThresholds.windClose;
    doc["windReopen"]  = currentThresholds.windReopen;

    doc["useTempOpen"]   = currentThresholds.useTempOpen;
    doc["useTempClose"]  = currentThresholds.useTempClose;
    doc["useWindClose"]  = currentThresholds.useWindClose;
    doc["useWindReopen"] = currentThresholds.useWindReopen;

    String out; serializeJson(doc, out);
    req->send(200, "application/json", out); });

  // --- JSON POST for thresholds (AsyncJson-less but simple) ------------------
  // We’ll read raw body as JSON
  server.on("/api/thresholds", HTTP_POST, [](AsyncWebServerRequest *request) { /* handled in body cb */ }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
      // data = raw JSON body
      StaticJsonDocument<512> doc;
      DeserializationError err = deserializeJson(doc, data, len);
      if (err) { request->send(400, "text/plain", "Invalid JSON"); return; }

      extern Thresholds currentThresholds;
      currentThresholds.tempOpen   = doc["tempOpen"]    | currentThresholds.tempOpen;
      currentThresholds.tempClose  = doc["tempClose"]   | currentThresholds.tempClose;
      currentThresholds.windClose  = doc["windClose"]   | currentThresholds.windClose;
      currentThresholds.windReopen = doc["windReopen"]  | currentThresholds.windReopen;

      currentThresholds.useTempOpen   = doc["useTempOpen"]   | currentThresholds.useTempOpen;
      currentThresholds.useTempClose  = doc["useTempClose"]  | currentThresholds.useTempClose;
      currentThresholds.useWindClose  = doc["useWindClose"]  | currentThresholds.useWindClose;
      currentThresholds.useWindReopen = doc["useWindReopen"] | currentThresholds.useWindReopen;

      saveThresholds(); // persist to SPIFFS if you added that earlier

      request->send(200, "text/plain", "Thresholds updated"); });

  // --- Manual motor control via web (guarded by manual lock) ----------------
  server.on("/api/motor", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t)
            {
      if (manualIsActive()) { request->send(409,"text/plain","Manual override active"); return; }
      StaticJsonDocument<128> doc;
      if (deserializeJson(doc, data, len)) { request->send(400,"text/plain","Invalid JSON"); return; }
      const float dir = doc["direction"] | 0;
      // Serial.println(dir);
      setPendingTrigger(Trigger::WEB);
      setTargetPercent(dir);
      request->send(200,"text/plain","ok"); });

  // --- Logs fetch (still HTTP GET; you could also stream over SSE if you like)
  server.on("/api/logs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String arr = readLogsJSON(100);
    // Serial.println("Logs JSON: " + arr);
    request->send(200, "application/json", arr); });

  server.onNotFound([](AsyncWebServerRequest *req)
                    {
    Serial.printf("404: %s %s\n", req->methodToString(), req->url().c_str());
    req->send(404, "text/plain", "Not found"); });

  server.begin();
  Serial.println("Async Web server + SSE started on :80");
}

// ---- SSE push helpers -------------------------------------------------------
void sseUpdateSensors()
{
  const float epsT = 0.2f, epsH = 1.0f, epsW = 0.2f;

  float t1 = getTemperatureC();
  float t2 = getTemperature2C();
  float avg = (t1 + t2) / 2.0;
  float hum = getHumidity();
  float wind = getAverageWindSpeed();

  if (changed(t1, lastT1, epsT) || changed(t2, lastT2, epsT) ||
      changed(avg, lastAvg, epsT) || changed(hum, lastHum, epsH) ||
      changed(wind, lastWind, epsW))
  {

    lastT1 = t1;
    lastT2 = t2;
    lastAvg = avg;
    lastHum = hum;
    lastWind = wind;

    StaticJsonDocument<192> doc;
    doc["temp1"] = t1;
    doc["temp2"] = t2;
    doc["avg"] = avg;
    doc["humidity"] = hum;
    doc["wind"] = wind;
    String payload;
    serializeJson(doc, payload);

    events.send(payload.c_str(), "sensors", millis()); // event: sensors
  }
}

void sseUpdateStatus(bool manual, const char *motorState)
{
  if (manual != lastManual || lastMotor != motorState)
  {
    lastManual = manual;
    lastMotor = motorState;

    StaticJsonDocument<128> doc;
    doc["manual"] = manual;
    doc["motorState"] = motorState;
    if (motorState == "")
    {
      doc["motorPercent"] = getCurrentPercent();
    }
    String payload;
    serializeJson(doc, payload);

    events.send(payload.c_str(), "status", millis()); // event: status
  }
}

// If you want to push logs to clients (e.g., after a new line is appended)
void ssePushLogs()
{
  String arr = readLogsJSON(50);
  // wrap as object so client can distinguish
  String out = String("{\"lines\":") + arr + "}";
  events.send(out.c_str(), "logs", millis()); // event: logs
}
