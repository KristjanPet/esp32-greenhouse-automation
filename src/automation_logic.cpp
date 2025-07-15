#include <Arduino.h>
#include "automation_logic.h"
#include "thresholds.h"
#include "motor_control.h"

void handleAutoControl(float temp, float wind)
{
  static bool isOpen = false;

  Serial.println("Current Temperature Open Threshold: " + String(currentThresholds.tempOpen));
  Serial.println("Current Temperature Close Threshold: " + String(currentThresholds.tempClose));
  Serial.println("Current Wind Close Threshold: " + String(currentThresholds.windClose));
  Serial.println("Current Wind Reopen Threshold: " + String(currentThresholds.windReopen));

  if (currentThresholds.useTempOpen && temp > currentThresholds.tempOpen)
  {
    Serial.println("Opening due to high temperature");
    motorStop();
    motorGoUp();
    isOpen = true;
  }
  else if (
      (currentThresholds.useWindClose && wind > currentThresholds.windClose) ||
      (currentThresholds.useTempClose && temp < currentThresholds.tempClose))
  {
    Serial.println("Closing due to wind or low temperature");
    motorStop();
    motorGoDown();
    isOpen = false;
  }
  else if (currentThresholds.useWindReopen && wind < currentThresholds.windReopen && !isOpen && temp <= currentThresholds.tempOpen)
  {
    Serial.println("Reopening due to calm wind");
    motorStop();
    motorGoUp();
    isOpen = true;
  }
}
