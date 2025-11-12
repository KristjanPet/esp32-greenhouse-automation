#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"
#include "logger.h"
#include "../include/secrets.h"
#include <temp_sensor.h>
#include <temp_sensor_2.h>
#include <wind_sensor.h>

void handleAutoControl()
{
  float currentPercent = getCurrentPercent();
  float temp1 = getTemperatureC();
  float temp2 = getTemperature2C();
  float tempAvg = (temp1 + temp2) / 2.0;
  float humidity = getHumidity();
  float wind = getAverageWindSpeed();

  MotorState prevState = getMotorState();
  // Temp OPEN
  if (currentThresholds.useTempOpen && tempAvg > currentThresholds.tempOpen && prevState != MotorState::OPENING && currentPercent < 95)
  {
    setTargetPercent(100);
  }
  // Wind || temp CLOSE
  else if (((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
            (currentThresholds.useTempClose && tempAvg < currentThresholds.tempClose)) &&
           prevState != MotorState::CLOSING && currentPercent > 5)
  {
    setTargetPercent(0);
    if (wind > currentThresholds.windClose)
      logMove(Trigger::AUTO_WIND, prevState, MotorState::CLOSING);
    else
    {
      logMove(Trigger::AUTO_TEMP, prevState, MotorState::CLOSING);
    }
  }
  // wind REOPEN
  else if (currentThresholds.useWindReopen &&
           wind < currentThresholds.windReopen &&
           ((currentThresholds.useTempClose && tempAvg > currentThresholds.tempClose) || !currentThresholds.useTempClose))
  {
    setTargetPercent(100);
    logMove(Trigger::AUTO_WIND, prevState, MotorState::OPENING);
  }
}
