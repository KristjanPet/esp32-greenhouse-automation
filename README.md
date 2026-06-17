# 🌱 ESP32 Greenhouse Automation


An ESP32-based controller for automatic greenhouse ventilation using temperature, humidity, and wind readings. Motors can be controlled automatically (thresholds), manually (buttons), or via the web interface. All movements are logged directly on the ESP32 and displayed on the web interface, together with temperature, humidity, wind speed, and motor states.

---

## 🔧 Main Features
- ESP32 DevKit V1 with web interface & automation logic  
- Non-blocking WiFi reconnect keeps local automation, sensors, and manual controls running while the ESP32 retries the network in the background
- Average temperature & humidity from **DS18B20** + **DHT11**  
- Average wind speed from 0–5V wind sensor (ADC + voltage divider)  
- Two roll-up motors controlled via:
  - manual buttons  
  - web interface  
  - automation thresholds (wind + temperature)
- Motor position/state is calculated from the configured time between fully open and fully closed, which must be entered manually
- All actions stored in onboard log (`log.json`)
- Power system:
  - **230V → 24V** (motors)
  - **24V → 5V** (ESP32 + relays)
  - **24V → 12V** (wind sensor)

---

## 🧩 ESP32 Pin Assignments

| Component              | ESP32     | Notes                                                                 |
|------------------------|----------------|------------------------------------------------------------------------|
| **DHT11** (temp + humidity) | GPIO 4         | 3.3V powered, built-in pull-up |
| **DS18B20** (temp)    | GPIO 5         | 3.3V power      |
| **Wind Sensor** (analog)    | GPIO 34 (A0)   | 0–5V scaled to 0–3.3V using 1.8k + 3.6k divider             |
| **Relay Module** (4-channel)| GPIO 14, 26, 27     | **14** = Direction (SW1 & SW2), **27** = Motor 1 (SW3), **26** = Motor 2 (SW4)                       |
| **ESP32 VIN** (power input) | 5V             | Connect from buck converter (24V → 5V)                                 |
| **Manual buttons** (2x up/0/down)| GPIO 13, 14, 17, 18     | 13, 14 Motor 1 (up, down), 17, 18 Motor 2 (up, down), wired to GND (internal pull-ups) + 220ohm resistor and 47nF capacitor                      |

All modules share a common GND.

---

## 📐 System Schematic
<img width="1190" height="845" alt="SCH_Main_1-P1_2026-02-28" src="https://github.com/user-attachments/assets/84364f9a-ab05-4275-aa27-67a97340e421" />

## 🧠 Program Architecture
<img width="1118" height="894" alt="architecture" src="https://github.com/user-attachments/assets/e9a6e672-17f8-450c-9936-d92533b930a3" />


## 🧾 Bill of Materials (BOM)

| Item | Price |
|------|-------|
| M8 Sensor Connector 4-pin (female) | € 3.44 |
| M8 Sensor Connector 3-pin (female) | € 3.18 |
| M12 4-pin Socket (female) | € 2.77 |
| Waterproof Connector SP11 2-pin set | € 5.45 |
| IP65 Distribution Box HT-12WAY | € 24.23 |
| OMCH M12 4-pin PVC Male Cable 5m | € 7.29 |
| Waterproof IP67 M8 Solder Connector | € 7.24 |
| Relay Module 4-channel 5V | € 3.85 |
| Buck Converter 10–80V → 5V 1A | € 5.42 |
| 3-Cup Anemometer RS485 (0–5V Output) | € 25.29 |
| ESP32 Micro-Board | € 7.80 |
| DS18B20 Temperature Sensor Kit | € 4.05 |
| Electric Film Roller Motor Set (2x) | € 187.86 |
| Buck Module 15–50V → 12V 3A | € 4.71 |
| DHT11 Temperature Sensor Module | € 1.16 |
| Cable Clips / "Sponke" | € 6.00 |
| Super-thin AC/DC Transformer 230V → 24V 300W | € 12.59 |

---
