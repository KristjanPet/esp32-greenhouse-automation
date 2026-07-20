#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"
#include "logger.h"
#include "../include/secrets.h"
#include <temperature_average.h>
#include <wind_sensor.h>

void handleAutoControl()
{
  float currentPercent = getCurrentPercent();
  float tempAvg = getAverageTemperatureC();
  bool hasValidTemperature = !isnan(tempAvg);
  float wind = getAverageWindSpeed();

  MotorState prevState = getMotorState();
  // Temp OPEN
  if (hasValidTemperature &&
      currentThresholds.useTempOpen &&
      tempAvg > currentThresholds.tempOpen &&
      prevState != MotorState::OPENING &&
      currentPercent < 95)
  {
    setPendingTrigger(Trigger::AUTO_TEMP);
    setTargetPercent(100);
  }
  // Wind || temp CLOSE
  else if (((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
            (hasValidTemperature &&
             currentThresholds.useTempClose &&
             tempAvg < currentThresholds.tempClose)) &&
           prevState != MotorState::CLOSING && currentPercent > 5)
  {
    if (wind > currentThresholds.windClose)
      setPendingTrigger(Trigger::AUTO_WIND);
    else
    {
      setPendingTrigger(Trigger::AUTO_TEMP);
    }
    setTargetPercent(0);
  }
  // wind REOPEN
  else if (currentThresholds.useWindReopen &&
           wind < currentThresholds.windReopen &&
           ((hasValidTemperature &&
             currentThresholds.useTempClose &&
             tempAvg > currentThresholds.tempClose) ||
            !currentThresholds.useTempClose))
  {
    setPendingTrigger(Trigger::AUTO_WIND);
    setTargetPercent(100);
  }
}
