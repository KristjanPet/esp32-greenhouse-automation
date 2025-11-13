#pragma once
#include "motor_control.h"
#include "types.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// Log a movement event (JSON log)
void logMove(Trigger trig);

// Read last X log entries as JSON array string
String readLogsJSON(int maxLines = 100);

// Trim log file to last X lines
void trimLogIfNeeded(int maxLines = 200);

// Append last X logs into an existing JSON array
void appendLogsTo(JsonArray arr, int maxLines = 100);

// Current timestamp string
String nowStr();

// Convert trigger enum to readable string
const char *trigStr(Trigger t);
