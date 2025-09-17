#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"

void handleAutoControl(float temp, float wind)
{ 
  int timeout = 10000; // 10 seconds timeout for motor operation
  if (currentThresholds.useTempOpen && temp > currentThresholds.tempOpen) {
    if (getMotorState() != MotorState::OPENING && getMotorState() != MotorState::OPENED) {
      startMotorUpTimed(timeout);
      setMotorState(MotorState::OPENING);
    }
  }
  else if ((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
          (currentThresholds.useTempClose && temp < currentThresholds.tempClose)) {
    if (getMotorState() != MotorState::CLOSING && getMotorState() != MotorState::CLOSED) {
      startMotorDownTimed(timeout);
      setMotorState(MotorState::CLOSING);
    }
  }
  else if (currentThresholds.useWindReopen &&
          wind < currentThresholds.windReopen &&
          getMotorState() == MotorState::CLOSED &&
          temp <= currentThresholds.tempOpen) {
    startMotorUpTimed(timeout);
    setMotorState(MotorState::OPENING);
  }
}
