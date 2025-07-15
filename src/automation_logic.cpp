#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"

void handleAutoControl(float temp, float wind)
{
  if (currentThresholds.useTempOpen && temp > currentThresholds.tempOpen) {
    if (motorState != OPENING && motorState != OPENED) {
      startMotorUpTimed(10000);
      motorState = OPENING;
    }
  }
  else if ((currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
          (currentThresholds.useTempClose && temp < currentThresholds.tempClose)) {
    if (motorState != CLOSING && motorState != CLOSED) {
      startMotorDownTimed(10000);
      motorState = CLOSING;
    }
  }
  else if (currentThresholds.useWindReopen &&
          wind < currentThresholds.windReopen &&
          motorState == CLOSED &&
          temp <= currentThresholds.tempOpen) {
    startMotorUpTimed(10000);
    motorState = OPENING;
  }
}
