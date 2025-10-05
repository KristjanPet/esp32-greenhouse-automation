#pragma once
void setupWebServerAsync();
void sseUpdateSensors(float t1, float t2, float avg, float hum, float wind);
void sseUpdateStatus(bool manual, const char* motorState);
void ssePushLogs(); // call when you want to refresh logs to clients