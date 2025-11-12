#pragma once
#include <Arduino.h>
#include <WString.h>

extern volatile uint32_t sensorsV;
extern volatile uint32_t statusV;
extern volatile uint32_t logsV;

extern String sensorsJson;
extern String statusJson;

void updateSensorsCache();
void updateStatusCache(bool manual, const char *motorStateStr);
void bumpLogsVersion(); // call when you append to log
