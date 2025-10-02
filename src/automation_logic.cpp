#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"
#include "logger.h"

void handleAutoControl(float temp, float temp2, float tempAvg, float humidity, float wind)
{ 
  int timeout = 10000; // 10 seconds timeout for motor operation
  MotorState prevState = getMotorState();
  if (currentThresholds.useTempOpen && tempAvg > currentThresholds.tempOpen) {
    if (prevState != MotorState::OPENING && prevState != MotorState::OPENED) {
      startMotorUpTimed(timeout);
      setMotorState(MotorState::OPENING);
      logMove(Trigger::AUTO_TEMP, prevState, MotorState::OPENING);
    }
  }
  else if ((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
          (currentThresholds.useTempClose && tempAvg < currentThresholds.tempClose)) {
    if (prevState != MotorState::CLOSING && prevState != MotorState::CLOSED) {
      startMotorDownTimed(timeout);
      setMotorState(MotorState::CLOSING);
      if(wind > currentThresholds.windClose)
        logMove(Trigger::AUTO_WIND, prevState, MotorState::CLOSING);
      else{
        logMove(Trigger::AUTO_TEMP, prevState, MotorState::CLOSING);
      }
    }
  }
  else if (currentThresholds.useWindReopen &&
          wind < currentThresholds.windReopen &&
          prevState == MotorState::CLOSED &&
          tempAvg <= currentThresholds.tempOpen) {
    startMotorUpTimed(timeout);
    setMotorState(MotorState::OPENING);
    logMove(Trigger::AUTO_WIND, prevState, MotorState::OPENING);
  }
}
