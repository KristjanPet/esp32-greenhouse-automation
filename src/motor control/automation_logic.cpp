#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"
#include "logger.h"
#include "../include/secrets.h"

void handleAutoControl(float temp, float temp2, float tempAvg, float humidity, float wind)
{

  MotorState prevState = getMotorState();
  if (currentThresholds.useTempOpen && tempAvg > currentThresholds.tempOpen)
  {
    if (prevState != MotorState::OPENING && prevState != MotorState::OPENED)
    {
      startMotorUpTimed(motorDuration);
      setMotorState(MotorState::OPENING);
      logMove(Trigger::AUTO_TEMP, prevState, MotorState::OPENING);
    }
  }
  else if ((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
           (currentThresholds.useTempClose && tempAvg < currentThresholds.tempClose))
  {
    if (prevState != MotorState::CLOSING && prevState != MotorState::CLOSED)
    {
      startMotorDownTimed(motorDuration);
      setMotorState(MotorState::CLOSING);
      if (wind > currentThresholds.windClose)
        logMove(Trigger::AUTO_WIND, prevState, MotorState::CLOSING);
      else
      {
        logMove(Trigger::AUTO_TEMP, prevState, MotorState::CLOSING);
      }
    }
  }
  else if (currentThresholds.useWindReopen &&
           wind < currentThresholds.windReopen &&
           prevState == MotorState::CLOSED &&
           tempAvg <= currentThresholds.tempOpen)
  {
    startMotorUpTimed(motorDuration);
    setMotorState(MotorState::OPENING);
    logMove(Trigger::AUTO_WIND, prevState, MotorState::OPENING);
  }
}
