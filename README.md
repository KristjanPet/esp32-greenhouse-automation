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

## Setup
