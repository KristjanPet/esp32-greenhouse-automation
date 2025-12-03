#include "change_bus.h"
#include <temp_sensor.h>
#include <temp_sensor_2.h>
#include <wind_sensor.h>

volatile uint32_t sensorsV = 0;
volatile uint32_t statusV = 0;
volatile uint32_t logsV = 0;

String sensorsJson;
String statusJson;

static float lastT1 = NAN, lastT2 = NAN, lastAvg = NAN, lastHum = NAN, lastWind = NAN;
static bool lastManual = false;
static String lastMotorState;

static inline bool changed(float a, float b, float eps)
{
  if (isnan(a) != isnan(b))
    return true;
  if (isnan(a))
    return false;
  return fabsf(a - b) > eps;
}

void updateSensorsCache()
{
  float t1 = getTemperatureC();
  float t2 = getTemperature2C();
  float avg = (t1 + t2) / 2.0;
  float hum = getHumidity();
  float wind = getAverageWindSpeed();
  // tune epsilons to stop flicker/noise
  const float epsT = 0.2f; // °C
  const float epsH = 1.0f; // %RH
  const float epsW = 0.2f; // m/s

  if (changed(t1, lastT1, epsT) || changed(t2, lastT2, epsT) ||
      changed(avg, lastAvg, epsT) || changed(hum, lastHum, epsH) ||
      changed(wind, lastWind, epsW))
  {

    lastT1 = t1;
    lastT2 = t2;
    lastAvg = avg;
    lastHum = hum;
    lastWind = wind;

    sensorsJson.reserve(160);
    sensorsJson = "{";
    sensorsJson += "\"temp1\":" + String(t1, 1) + ",";
    sensorsJson += "\"temp2\":" + String(t2, 1) + ",";
    sensorsJson += "\"avg\":" + String(avg, 1) + ",";
    sensorsJson += "\"humidity\":" + String(hum, 1) + ",";
    sensorsJson += "\"wind\":" + String(wind, 1) + ",";
    sensorsJson += "\"v\":" + String(++sensorsV);
    sensorsJson += "}";
  }
}

void updateStatusCache(bool manual, const char *motorStateStr)
{
  if (manual != lastManual || lastMotorState != motorStateStr)
  {
    lastManual = manual;
    lastMotorState = motorStateStr;

    statusJson.reserve(96);
    statusJson = "{";
    statusJson += "\"manual\":";
    statusJson += (manual ? "true" : "false");
    statusJson += ",";
    statusJson += "\"motorState\":\"";
    statusJson += motorStateStr;
    statusJson += "\",";
    statusJson += "\"v\":";
    statusJson += String(++statusV);
    statusJson += "}";
  }
}

void bumpLogsVersion() { ++logsV; }
