#include <WiFi.h>
#include <WebServer.h>
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"
#include <SPIFFS.h>

WebServer server(80);

// Dummy values for testing (replace later with real readings)
float mockTemperature = 23.4;
float mockWindSpeed = 3.7;


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
    float wind = getWindSpeed();

    String json = "{";
    json += "\"temp1\":" + String(t1, 1) + ",";
    json += "\"temp2\":" + String(t2, 1) + ",";
    json += "\"avg\":"   + String(avg, 1) + ",";
    json += "\"humidity\":" + String(hum, 1) + ",";
    json += "\"wind\":" + String(wind, 1);
    json += "}";

    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Web server started on port 80");
}

void handleWebServer()
{
    server.handleClient();
}
