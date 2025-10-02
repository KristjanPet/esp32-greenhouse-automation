#pragma once
#include "motor_control.h"
#include <Arduino.h>
#include <ArduinoJson.h> 

enum class Trigger { WEBUP, WEBDOWN, MANUALUP, MANUALDOWN, AUTO_TEMP, AUTO_WIND };

void logMove(Trigger trig, MotorState prev, MotorState next);
String readLogsJSON(int maxLines = 100); // returns JSON array of latest lines
void trimLogIfNeeded(int maxLines = 200);
void appendLogsTo(JsonArray arr, int maxLines = 100);

String nowStr();
