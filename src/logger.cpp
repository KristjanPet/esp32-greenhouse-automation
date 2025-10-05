#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <vector>
#include "logger.h"
#include "thresholds.h"
#include "web_server_async.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"
#include "wind_sensor.h"
#include <change_bus.h>

extern Thresholds currentThresholds;
extern String nowStr();
extern const char* trigStr(Trigger t);

void logMove(Trigger trig, MotorState prev, MotorState next)
{
  float temp = getTemperatureC();
  float temp2 = getTemperature2C();
  float tempAvg = (temp + temp2) / 2.0;
  float hum = getHumidity();
  float wind = getAverageWindSpeed();

  String strPrev = getStateStr(prev);
  String strNext = getStateStr(next);

  File f = SPIFFS.open("/log.txt", FILE_APPEND);
  if (!f) return;
  // CSV-ish line; easy to parse
  String line = nowStr() + "," + trigStr(trig) + "," +
    strNext + "->" + strPrev + "," +
    "T=" + String(temp,1) + ",T2=" + String(temp2,1) + ",TAvg=" +
    String(tempAvg,1) + ",H=" + String(hum,1) + ",W=" + String(wind,1) + "," +
    "TOpen=" + String(currentThresholds.tempOpen,1) + ",TClose=" + String(currentThresholds.tempClose,1) +
    ",WClose=" + String(currentThresholds.windClose,1) + ",WReopen=" + String(currentThresholds.windReopen,1) +
    ",UseTOpen=" + currentThresholds.useTempOpen + ",UseTClose=" + currentThresholds.useTempClose +
    ",UseWClose=" + currentThresholds.useWindClose + ",UseWReopen=" + currentThresholds.useWindReopen + "\n";
  f.print(line);
  f.close();
  trimLogIfNeeded(200);
  bumpLogsVersion();
  ssePushLogs();
}

void trimLogIfNeeded(int maxLines){
  File f = SPIFFS.open("/log.txt", FILE_READ); if (!f) return;
  // Count lines quickly
  int lines = 0; while(f.available()) if (f.read()=='\n') lines++;
  f.close();
  if (lines <= maxLines) return;

  // Keep last maxLines: read all, drop head
  f = SPIFFS.open("/log.txt", FILE_READ);
  String content = f.readString();
  f.close();

  int keepStart = 0;
  int toDrop = lines - maxLines;
  for (int i = 0; i < toDrop; i++) {
    int pos = content.indexOf('\n', keepStart);
    if (pos < 0) break;
    keepStart = pos + 1;
  }

  File w = SPIFFS.open("/log.txt", FILE_WRITE);
  if (!w) return;
  w.print(content.substring(keepStart));
  w.close();
}

String readLogsJSON(int maxLines){
  File f = SPIFFS.open("/log.txt", FILE_READ);
  DynamicJsonDocument doc(8192);                // v7 style
  JsonArray arr = doc.to<JsonArray>();
  if (!f) { String out; serializeJson(arr, out); return out; }

  std::vector<String> lines;
  lines.reserve(maxLines + 16);
  while (f.available()){
    String line = f.readStringUntil('\n');
    if (line.length()) lines.push_back(line);
  }
  f.close();

  const int total = (int)lines.size();
  const int start = total > maxLines ? (total - maxLines) : 0;

  // push NEWEST first
  for (int i = total - 1; i >= start; --i) {
    arr.add(lines[i]);
  }

  String out; serializeJson(doc, out); 
  return out;
}

void appendLogsTo(JsonArray arr, int maxLines) {
  File f = SPIFFS.open("/log.txt", FILE_READ);
  if (!f) return;

  std::vector<String> lines;
  lines.reserve(maxLines + 16);
  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (line.length()) lines.push_back(line);
  }
  f.close();

  const int total = (int)lines.size();
  const int start = total > maxLines ? (total - maxLines) : 0;

  // push NEWEST first
  for (int i = total - 1; i >= start; --i) {
    arr.add(lines[i]);
  }
}

String nowStr(){
  struct tm t; if (!getLocalTime(&t)) return "1970-01-01 00:00:00";
  char buf[20]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
  return String(buf);
}

const char* trigStr(Trigger t) {
  switch(t){
    case Trigger::WEBDOWN: return "web_down";
    case Trigger::WEBUP: return "web_up";
    case Trigger::MANUALDOWN: return "manual_down";
    case Trigger::MANUALUP: return "manual_up";
    case Trigger::AUTO_TEMP: return "auto_temp";
    case Trigger::AUTO_WIND: return "auto_wind";
  }
  return "unknown";
}