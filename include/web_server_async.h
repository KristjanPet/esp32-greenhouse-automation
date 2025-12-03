#pragma once
void setupWebServerAsync();
void sseUpdateSensors();
void sseUpdateStatus(bool manual, const char* motorState);
void ssePushLogs(); // call when you want to refresh logs to clients