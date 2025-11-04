#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "thresholds.h"

Thresholds currentThresholds;

bool saveThresholds() {
  File file = SPIFFS.open("/thresholds.json", "w");
  if (!file) {
    Serial.println("Failed to open thresholds file for writing.");
    return false;
  }

  DynamicJsonDocument doc(256);
  doc["tempOpen"] = currentThresholds.tempOpen;
  doc["tempClose"] = currentThresholds.tempClose;
  doc["windClose"] = currentThresholds.windClose;
  doc["windReopen"] = currentThresholds.windReopen;

  doc["useTempOpen"] = currentThresholds.useTempOpen;
  doc["useTempClose"] = currentThresholds.useTempClose;
  doc["useWindClose"] = currentThresholds.useWindClose;
  doc["useWindReopen"] = currentThresholds.useWindReopen;

  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write thresholds to file.");
    file.close();
    return false;
  }

  file.close();
  Serial.println("Thresholds saved to SPIFFS.");
  return true;
}

bool loadThresholds() {
  if (!SPIFFS.exists("/thresholds.json")) {
    Serial.println("No thresholds file found. Using defaults.");
    return false;
  }

  File file = SPIFFS.open("/thresholds.json", "r");
  if (!file) {
    Serial.println("Failed to open thresholds file.");
    return false;
  }

  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Failed to parse thresholds file.");
    return false;
  }

  currentThresholds.tempOpen = doc["tempOpen"] | currentThresholds.tempOpen;
  currentThresholds.tempClose = doc["tempClose"] | currentThresholds.tempClose;
  currentThresholds.windClose = doc["windClose"] | currentThresholds.windClose;
  currentThresholds.windReopen = doc["windReopen"] | currentThresholds.windReopen;

  currentThresholds.useTempOpen = doc["useTempOpen"] | currentThresholds.useTempOpen;
  currentThresholds.useTempClose = doc["useTempClose"] | currentThresholds.useTempClose;
  currentThresholds.useWindClose = doc["useWindClose"] | currentThresholds.useWindClose;
  currentThresholds.useWindReopen = doc["useWindReopen"] | currentThresholds.useWindReopen;

  Serial.println("Thresholds loaded from SPIFFS.");
  return true;
}