#include <Arduino.h>
#include <cmath>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "motor_control.h"
#include "../include/secrets.h"
#include "logger.h"
#include "thresholds.h"

const int MOTOR_DIR_PIN = 14;      // Pin for motor direction (up/down)
const int LEFT_MOTOR_ON_PIN = 27;  // Pin for left motor on/off
const int RIGHT_MOTOR_ON_PIN = 26; // Pin for right motor on/off

static unsigned long motorTimerStart = 0;
static unsigned long motorRunDuration = 0;
static bool motorTimerActive = false;

static float currentPercent = NAN; // TEMP
static float targetPercent = NAN;
float eps = 0.5f; //offset

uint32_t lastTs = 0;

MotorState motorState = MotorState::STOPPED; // Global motor state variable
Trigger pendingTrigger = Trigger::NONE;

void setupMotorPins()
{
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_ON_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_ON_PIN, OUTPUT);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
}

void motorGoUp()
{
  setMotorState(MotorState::OPENING);
  lastTs = millis();
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, HIGH);
  delay(100);
  if( currentThresholds.useLeftMotor) digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  if( currentThresholds.useRightMotor) digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
}

void motorGoDown()
{
  setMotorState(MotorState::CLOSING);
  lastTs = millis();
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  delay(100);
  if( currentThresholds.useLeftMotor) digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  if( currentThresholds.useRightMotor) digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
}

void motorStop()
{
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  setMotorState(MotorState::STOPPED);

  if (pendingTrigger != Trigger::NONE)
  {
    logMove(pendingTrigger);
    pendingTrigger = Trigger::NONE;
  }

  targetPercent = currentPercent;
  saveCurrentState();
}

float getCurrentPercent()
{
  return std::round(currentPercent);
}

void setTargetPercent(float p)
{
  targetPercent = p;
}

void setPendingTrigger(Trigger t)
{
  pendingTrigger = t;
}

void moveLogic()
{
  if (targetPercent - eps > currentPercent && motorState == MotorState::STOPPED)
  {
    motorGoUp();
  }
  else if (targetPercent + eps < currentPercent && motorState == MotorState::STOPPED)
  {
    motorGoDown();
  }
}

void tickMotion()
{
  float movingRate = 100.0 / motorDuration; // % per ms
  uint32_t now = millis();
  uint32_t dt = now - lastTs;
  lastTs = now;
  static float lastPrintPercent = -1; // only for print time

  moveLogic();

  if (motorState == MotorState::OPENING)
  {
    currentPercent += dt * movingRate;
    // if (fabs(currentPercent - lastPrintPercent) >= 5.0f)
    // { // print only on 1% change
    //   Serial.printf("%.2f%%\n", currentPercent);
    //   lastPrintPercent = currentPercent;
    // }
    if (currentPercent >= targetPercent - eps || currentPercent + eps >= 100.0)
    {
      motorStop();
    }
  }
  else if (motorState == MotorState::CLOSING)
  {
    currentPercent -= dt * movingRate;
    // if (fabs(currentPercent - lastPrintPercent) >= 5.0f)
    // { // print only on 1% change
    //   Serial.printf("%.2f%%\n", currentPercent);
    //   lastPrintPercent = currentPercent;
    // }
    if (currentPercent <= targetPercent + eps || currentPercent - eps <= 0.0)
    {
      motorStop();
    }
  }

  currentPercent = constrain(currentPercent, 0.f, 100.f);
}

bool saveCurrentState()
{
  File file = SPIFFS.open("/state.json", "w");
  if (!file) {
    Serial.println("Failed to open state file for writing.");
    return false;
  }

  DynamicJsonDocument doc(128);
  doc["currentPercent"] = currentPercent;

  bool ok = serializeJson(doc, file) > 0;
  file.close();

  if (!ok) Serial.println("Failed to write state.");
  return ok;
}

bool loadCurrentState()
{
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("No state file found.");
    currentPercent = 0;
    targetPercent = 0;
    return false;
  }

  File file = SPIFFS.open("/state.json", "r");
  if (!file) return false;

  DynamicJsonDocument doc(128);
  auto err = deserializeJson(doc, file);
  file.close();
  if (err) return false;

  float p = doc["currentPercent"] | NAN;
  if (!isnan(p)) {
    currentPercent = p;
    targetPercent = p; // prevents immediate move on boot
  }
  return true;
}

void setMotorState(MotorState s) { motorState = s; }
MotorState getMotorState() { return motorState; }

const char *motorStateStr()
{
  switch (motorState)
  {
  case MotorState::OPENING:
    return "odpira se";
  case MotorState::CLOSING:
    return "zapira se";
  default:
    return "";
  }
}
