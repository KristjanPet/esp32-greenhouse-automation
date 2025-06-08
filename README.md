# ESP32 Greenhouse Automation

An ESP32-based system for automating greenhouse ventilation by controlling roll-up film motors, using temperature and wind data.

## Features
- ESP32 Web Interface
- Wind speed sensors
- Temperature-based control logic
- Relay-controlled motors (2 sides)
- Powered via shared 24V system with buck converters

## Components
- ESP32 DevKit v1 (30-pin)
- DS18B20 Waterproof Temperature Sensor
- Analog Wind Speed Sensor (0–5V)
- 4-Channel Relay Module
- 24V Motor System

## Branches
- `web-ui`: Web interface and server logic
- `temp-sensor`: Temperature readings and logic
- `wind-sensor`: Wind speed/direction logic
- `motor-control`: Motor control and relay handling
- `system-diagram`: Diagrams, wiring, and docs

## 🔌 Wiring Pinout

Below is the wiring layout for sensors and modules connected to the ESP32 board.

| Component              | ESP32 GPIO     | Notes                                                                 |
|------------------------|----------------|------------------------------------------------------------------------|
| **DHT11** (temp + humidity) | GPIO 4         | Connect to 3.3V, GND and data pin. Pull-up resistor is built into the module. |
| **DS18B20** (waterproof)    | GPIO 5         | 3.3V or 5V power, GND, data pin (pull-up resistor is onboard)          |
| **Wind Sensor** (analog)    | GPIO 36 (A0)   | Analog 0–5V input → use voltage divider to scale to 0–3.3V             |
| **Relay Module** (4-channel)| GPIO 16–19     | Define as needed – each GPIO controls one relay                        |
| **OLED Display** (I2C)      | GPIO 21 (SDA), GPIO 22 (SCL) | You can configure these pins in the code                            |
| **ESP32 VIN** (power input) | 5V             | Connect from buck converter (24V → 5V)                                 |

### ⚠️ Notes:

- All **GND connections must be shared** (sensors, ESP32, power).
- If you’re using those infamous "China jumper wires" – double check the ground line. A loose GND will cause phantom 85.00 °C readings.
- The DS18B20 must be detected via `sensors.getDeviceCount() == 1`, or it means wiring or power is wrong.
- DHT11 modules typically include their own pull-up resistor, so no need to add one manually.
