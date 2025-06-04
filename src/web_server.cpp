#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

// Dummy values for testing (replace later with real readings)
float mockTemperature = 23.4;
float mockWindSpeed = 3.7;

void handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Greenhouse Control</title>
      <style>
        body { font-family: sans-serif; background: #f5f5f5; padding: 20px; }
        h1 { color: #333; }
        .section { margin-bottom: 20px; padding: 15px; background: white; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        button { padding: 10px 20px; font-size: 16px; margin: 5px; }
        input[type=number] { width: 80px; font-size: 16px; padding: 5px; }
      </style>
    </head>
    <body>
      <h1>Greenhouse Control Panel</h1>

      <div class="section">
        <h2>Status</h2>
        <p><strong>Wind Speed:</strong> 3.7 m/s</p>
        <p><strong>Temperature Sensor 1:</strong> 23.4 &deg;C</p>
        <p><strong>Temperature Sensor 2:</strong> 24.1 &deg;C</p>
        <p><strong>Average Temperature:</strong> 23.8 &deg;C</p>
      </div>

      <div class="section">
        <h2>Manual Motor Control</h2>
        <button onclick="alert('Motor UP')">Up</button>
        <button onclick="alert('Motor DOWN')">Down</button>
      </div>

      <div class="section">
        <h2>Threshold Settings</h2>
        <p>
          <label>Temp Open: <input type="number" value="25"></label>
          <label>Temp Close: <input type="number" value="18"></label>
        </p>
        <p>
          <label>Wind Close: <input type="number" value="10"></label>
          <label>Wind Reopen: <input type="number" value="6"></label>
        </p>
      </div>
    </body>
    </html>
  )rawliteral";

    server.send(200, "text/html", html);
}

void setupWebServer()
{
    server.on("/", handleRoot);
    server.begin();
    Serial.println("Web server started");
}

void handleWebServer()
{
    server.handleClient();
}
