#include <WiFi.h>
#include <WebServer.h>
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"

WebServer server(80);

// Dummy values for testing (replace later with real readings)
float mockTemperature = 23.4;
float mockWindSpeed = 3.7;

void handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="sl">
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <meta charset="UTF-8">
      <title>Nadzor Rastlinjaka</title>
      <style>
        body {
          font-family: sans-serif;
          background: #e0f7fa;
          padding: 20px;
          display: flex;
          flex-direction: column;
          align-items: center;
        }
        h1 {
          color: #006064;
        }
        .container {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
          gap: 20px;
          width: 100%;
          max-width: 900px;
        }
        .section {
          flex: 1 1 250px;
          background: white;
          border-radius: 10px;
          padding: 15px;
          box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        button {
          padding: 10px 20px;
          font-size: 16px;
          margin: 5px;
          background: #0097a7;
          color: white;
          border: none;
          border-radius: 5px;
        }
        input[type=number] {
          width: 80px;
          font-size: 16px;
          padding: 5px;
        }
        label {
          display: inline-block;
          margin: 10px 5px;
        }
      </style>
    </head>
    <body>
      <h1>Nadzorna plošča rastlinjaka</h1>

      <div class="container">
        <div class="section">
          <h2>Status</h2>
          <p><strong>Hitrost vetra:</strong> 3.7 m/s</p>
          <p><strong>Temperatura senzor 1:</strong> 23.4 &deg;C</p>
          <p><strong>Temperatura senzor 2:</strong> 24.1 &deg;C</p>
          <p><strong>Povprečna temperatura:</strong> 23.8 &deg;C</p>
        </div>

        <div class="section">
          <h2>Ročni nadzor motorjev</h2>
          <button onclick="alert('Motor GOR')">Gor</button>
          <button onclick="alert('Motor DOL')">Dol</button>
        </div>

        <div class="section">
          <h2>Nastavitve pragov</h2>
          <p>
            <label>Temp odpri: <input type="number" value="25"></label>
            <label>Temp zapri: <input type="number" value="18"></label>
          </p>
          <p>
            <label>Veter zapri: <input type="number" value="10"></label>
            <label>Veter ponovno odpri: <input type="number" value="6"></label>
          </p>
        </div>
      </div>
    </body>
    </html>
  )rawliteral";

    server.send(200, "text/html", html);
}

void setupWebServer()
{
void setupWebServer()
{
  server.on("/", []() {
    server.send(200, "text/html", "<h1>Greenhouse ESP32 says hi</h1>");
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
