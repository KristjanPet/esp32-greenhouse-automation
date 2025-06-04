#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

// Dummy values for testing (replace later with real readings)
float mockTemperature = 23.4;
float mockWindSpeed = 3.7;

void handleRoot() {
  String html = "<html><head><title>Greenhouse</title></head><body>";
  html += "<h1>Greenhouse Status</h1>";
  html += "<p>Temperature: " + String(mockTemperature) + " °C</p>";
  html += "<p>Wind Speed: " + String(mockWindSpeed) + " m/s</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

void handleWebServer() {
  server.handleClient();
}
