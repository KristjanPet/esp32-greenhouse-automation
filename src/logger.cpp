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
extern const char *trigStr(Trigger t);

static const char *LOG_FILE = "/log.json";

// ---------------------------------------------------------------------------
// Load full JSON array (or empty array if file missing)
// ---------------------------------------------------------------------------
static bool loadFullLog(DynamicJsonDocument &doc)
{
  if (!SPIFFS.exists(LOG_FILE))
  {
    doc.to<JsonArray>();
    return false;
  }

  File f = SPIFFS.open(LOG_FILE, FILE_READ);
  if (!f)
  {
    doc.to<JsonArray>();
    return false;
  }

  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err)
  {
    doc.to<JsonArray>();
    return false;
  }

  if (!doc.is<JsonArray>())
  {
    doc.to<JsonArray>();
  }
  return true;
}

// ---------------------------------------------------------------------------
// Save full array back to file
// ---------------------------------------------------------------------------
static void saveFullLog(DynamicJsonDocument &doc)
{
  File f = SPIFFS.open(LOG_FILE, FILE_WRITE);
  if (!f)
    return;
  serializeJson(doc, f);
  f.close();
}

// ---------------------------------------------------------------------------
// Trim log to last maxLines entries
// ---------------------------------------------------------------------------
void trimLogIfNeeded(int maxLines)
{
  DynamicJsonDocument doc(10000);
  loadFullLog(doc);
  JsonArray arr = doc.as<JsonArray>();

  int total = arr.size();
  if (total <= maxLines)
    return;

  int start = total - maxLines;

  DynamicJsonDocument newDoc(10000);
  JsonArray newArr = newDoc.to<JsonArray>();

  for (int i = start; i < total; i++)
  {
    newArr.add(arr[i]);
  }

  saveFullLog(newDoc);
}

// ---------------------------------------------------------------------------
// Append one log entry as JSON
// ---------------------------------------------------------------------------
void logMove(Trigger trig)
{
  float temp = getTemperatureC();
  float temp2 = getTemperature2C();
  float tempAvg = (temp + temp2) / 2.0f;
  float hum = getHumidity();
  float wind = getAverageWindSpeed();

  float state = getCurrentPercent();

  DynamicJsonDocument doc(12000);
  loadFullLog(doc);
  JsonArray arr = doc.as<JsonArray>();

  JsonObject o = arr.createNestedObject();
  o["timestamp"] = nowStr();
  o["trigger"] = trigStr(trig);
  o["state"] = state;

  JsonObject sensors = o.createNestedObject("sensors");
  sensors["T"] = temp;
  sensors["T2"] = temp2;
  sensors["TAvg"] = tempAvg;
  sensors["H"] = hum;
  sensors["W"] = wind;

  JsonObject thr = o.createNestedObject("thresholds");
  thr["TOpen"] = currentThresholds.tempOpen;
  thr["TClose"] = currentThresholds.tempClose;
  thr["WClose"] = currentThresholds.windClose;
  thr["WReopen"] = currentThresholds.windReopen;
  thr["UseTOpen"] = currentThresholds.useTempOpen;
  thr["UseTClose"] = currentThresholds.useTempClose;
  thr["UseWClose"] = currentThresholds.useWindClose;
  thr["UseWReopen"] = currentThresholds.useWindReopen;

  saveFullLog(doc);

  trimLogIfNeeded(200);
  bumpLogsVersion();
  ssePushLogs();
}

// ---------------------------------------------------------------------------
// Return last maxLines logs as JSON array string (newest first)
// ---------------------------------------------------------------------------
String readLogsJSON(int maxLines)
{
  DynamicJsonDocument doc(12000);
  loadFullLog(doc);
  JsonArray arr = doc.as<JsonArray>();

  DynamicJsonDocument outDoc(12000);
  JsonArray outArr = outDoc.to<JsonArray>();

  int total = arr.size();
  int start = total > maxLines ? (total - maxLines) : 0;

  for (int i = total - 1; i >= start; i--)
  {
    outArr.add(arr[i]);
  }

  String out;
  serializeJson(outDoc, out);
  return out;
}

// ---------------------------------------------------------------------------
// Append logs into an existing JSON array
// ---------------------------------------------------------------------------
void appendLogsTo(JsonArray arr, int maxLines)
{
  DynamicJsonDocument doc(12000);
  loadFullLog(doc);
  JsonArray full = doc.as<JsonArray>();

  int total = full.size();
  int start = total > maxLines ? (total - maxLines) : 0;

  for (int i = total - 1; i >= start; i--)
  {
    arr.add(full[i]);
  }
}

// ---------------------------------------------------------------------------
// Time & trigger helpers
// ---------------------------------------------------------------------------
String nowStr()
{
  struct tm t;
  if (!getLocalTime(&t))
    return "1970-01-01 00:00:00";
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
  return String(buf);
}

const char *trigStr(Trigger t)
{
  switch (t)
  {
  case Trigger::WEB:
    return "Splet";
  case Trigger::MANUAL:
    return "Gumb";
  case Trigger::AUTO_TEMP:
    return "Auto temp";
  case Trigger::AUTO_WIND:
    return "Auto veter";
  }
  return "Neznano";
}
