#include <Arduino.h>
#include "motor_control.h"
#include "../include/secrets.h"

const int MOTOR_DIR_PIN = 14;      // Pin for motor direction (up/down)
const int LEFT_MOTOR_ON_PIN = 26;  // Pin for left motor on/off
const int RIGHT_MOTOR_ON_PIN = 27; // Pin for right motor on/off

static unsigned long motorTimerStart = 0;
static unsigned long motorRunDuration = 0;
static bool motorTimerActive = false;

static float currentPercent = 69; // TEMP
static float targetPercent = NAN;

uint32_t lastTs = 0;

MotorState motorState = MotorState::STOPPED; // Global motor state variable

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
  digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
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
  digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
}

void motorStop()
{
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  setMotorState(MotorState::STOPPED);
  targetPercent = currentPercent;
}

bool isMotorUpActive()
{
  return digitalRead(MOTOR_DIR_PIN) == HIGH && digitalRead(RIGHT_MOTOR_ON_PIN) == HIGH;
}

bool isMotorDownActive()
{
  return digitalRead(MOTOR_DIR_PIN) == LOW && digitalRead(RIGHT_MOTOR_ON_PIN) == HIGH;
}

void startMotorUpTimed(unsigned long durationMs)
{
  motorGoUp();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

void startMotorDownTimed(unsigned long durationMs)
{
  motorGoDown();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

float getCurrentPercent()
{
  return currentPercent;
}

void setTargetPercent(float p)
{
  targetPercent = p;
}

void moveLogic()
{
  if (targetPercent > currentPercent && motorState == MotorState::STOPPED)
  {
    motorGoUp();
    Serial.println("motor up started");
  }
  else if (targetPercent < currentPercent && motorState == MotorState::STOPPED)
  {
    motorGoDown();
    Serial.println("motor down started");
  }
}

void tickMotion()
{
  float movingRate = 100.0 / motorDuration; // % per ms
  float eps = 0.5f;
  uint32_t now = millis();
  uint32_t dt = now - lastTs;
  lastTs = now;
  static float lastPrintPercent = -1; // only for print time

  moveLogic();

  if (motorState == MotorState::OPENING)
  {
    currentPercent += dt * movingRate;
    if (fabs(currentPercent - lastPrintPercent) >= 1.0f)
    { // print only on 1% change
      Serial.printf("%.2f%%\n", currentPercent);
      lastPrintPercent = currentPercent;
    }
    if (currentPercent >= targetPercent - eps || currentPercent >= 100.0)
    {
      motorStop();
    }
  }
  else if (motorState == MotorState::CLOSING)
  {
    currentPercent -= dt * movingRate;
    if (fabs(currentPercent - lastPrintPercent) >= 1.0f)
    { // print only on 1% change
      Serial.printf("%.2f%%\n", currentPercent);
      lastPrintPercent = currentPercent;
    }
    if (currentPercent <= targetPercent + eps || currentPercent <= 0.0)
    {
      motorStop();
    }
  }

  currentPercent = constrain(currentPercent, 0.f, 100.f);
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

const char *getStateStr(MotorState t)
{
  switch (t)
  {
  case MotorState::STOPPED:
    return "Stopped";
  case MotorState::OPENING:
    return "Opening";
  case MotorState::CLOSING:
    return "Closing";
  }
  return "unknown";
}