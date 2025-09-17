#include <WiFi.h>
#include <WebServer.h>
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"
#include "motor_control.h"
#include "thresholds.h"
#include "buttons_control.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

WebServer server(80);

void setupWebServer()
{
  server.on("/", []() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      server.send(500, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/style.css", []() {
    File file = SPIFFS.open("/style.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });

  server.on("/main.js", []() {
  File file = SPIFFS.open("/main.js", "r");
  server.streamFile(file, "application/javascript");
  file.close();
  });

  server.on("/api/sensors", []() {
    float t1 = getTemperatureC();
    float t2 = getTemperature2C();
    float avg = (t1 + t2) / 2.0;
    float hum = getHumidity();
    float wind = getAverageWindSpeed();

    String json = "{";
    json += "\"temp1\":" + String(t1, 1) + ",";
    json += "\"temp2\":" + String(t2, 1) + ",";
    json += "\"avg\":"   + String(avg, 1) + ",";
    json += "\"humidity\":" + String(hum, 1) + ",";
    json += "\"wind\":" + String(wind, 1);
    json += "}";

    server.send(200, "application/json", json);
  });

  server.on("/api/status", HTTP_GET, []() {
    String json = String("{\"manual\":") + (manualIsActive() ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  server.on("/api/motor", HTTP_POST, []() {
    String body = server.arg("plain");

    if (body.indexOf("up") != -1) {
      if (isMotorDownActive()) {
        motorStop();
        setMotorState(MotorState::STOPPED);
        Serial.println("Motor stopped while going down");
      } else {
        motorGoUp();
        setMotorState(MotorState::OPENING);
        Serial.println("Motor going up");
      }
    } else if (body.indexOf("down") != -1) {
      if (isMotorUpActive()) {
        motorStop();
        setMotorState(MotorState::STOPPED);
        Serial.println("Motor stopped while going up");
      } else {
        motorGoDown();
        setMotorState(MotorState::CLOSING);
        Serial.println("Motor going down");
      }
    }

    server.send(200, "text/plain", "Command received");
  });

  server.on("/api/thresholds", HTTP_POST, []() {
    String body = server.arg("plain");

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }

    currentThresholds.tempOpen = doc["tempOpen"] | currentThresholds.tempOpen;
    currentThresholds.tempClose = doc["tempClose"] | currentThresholds.tempClose;
    currentThresholds.windClose = doc["windClose"] | currentThresholds.windClose;
    currentThresholds.windReopen = doc["windReopen"] | currentThresholds.windReopen;

    currentThresholds.useTempOpen = doc["useTempOpen"] | currentThresholds.useTempOpen;
    currentThresholds.useTempClose = doc["useTempClose"] | currentThresholds.useTempClose;
    currentThresholds.useWindClose = doc["useWindClose"] | currentThresholds.useWindClose;
    currentThresholds.useWindReopen = doc["useWindReopen"] | currentThresholds.useWindReopen;

    Serial.println("Thresholds updated");
    saveThresholds();

    server.send(200, "text/plain", "Thresholds updated");
  });

  server.on("/api/thresholds", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    doc["tempOpen"] = currentThresholds.tempOpen;
    doc["tempClose"] = currentThresholds.tempClose;
    doc["windClose"] = currentThresholds.windClose;
    doc["windReopen"] = currentThresholds.windReopen;

    doc["useTempOpen"] = currentThresholds.useTempOpen;
    doc["useTempClose"] = currentThresholds.useTempClose;
    doc["useWindClose"] = currentThresholds.useWindClose;
    doc["useWindReopen"] = currentThresholds.useWindReopen;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.begin();
  Serial.println("Web server started on port 80");
}

void handleWebServer()
{
    server.handleClient();
}
