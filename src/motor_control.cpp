#include <Arduino.h>
#include "motor_control.h"

const int MOTOR_DIR_PIN = 14; // Pin for motor direction (up/down)
const int MOTOR_ON_PIN = 27; // Pin for motor on/off

static unsigned long motorTimerStart = 0;
static unsigned long motorRunDuration = 0;
static bool motorTimerActive = false;

MotorState motorState = MotorState::STOPPED; // Global motor state variable

void setupMotorPins() {
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_ON_PIN, OUTPUT);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  digitalWrite(MOTOR_ON_PIN, LOW);
}

void motorGoUp() {
  digitalWrite(MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, HIGH);
  delay(100);
  digitalWrite(MOTOR_ON_PIN, HIGH);
}

void motorGoDown() {
  digitalWrite(MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_ON_PIN, HIGH);
}

void motorStop() {
  digitalWrite(MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
}

bool isMotorUpActive() {
  return digitalRead(MOTOR_DIR_PIN) == HIGH && digitalRead(MOTOR_ON_PIN) == HIGH;
}

bool isMotorDownActive() {
  return digitalRead(MOTOR_DIR_PIN) == LOW && digitalRead(MOTOR_ON_PIN) == HIGH;
}

void startMotorUpTimed(unsigned long durationMs) {
  motorGoUp();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

void startMotorDownTimed(unsigned long durationMs) {
  motorGoDown();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

void updateMotorTimer() {
  if (motorTimerActive && millis() - motorTimerStart >= motorRunDuration) {
    motorStop();
    motorTimerActive = false;

    if (motorState == MotorState::OPENING) motorState = MotorState::OPENED;
    else if (motorState == MotorState::CLOSING) motorState = MotorState::CLOSED;
  }
}

void setMotorState(MotorState s) { motorState = s; }
MotorState getMotorState() { return motorState; }

const char* motorStateStr() {
  switch (motorState) {
    case MotorState::OPENED:  return "odprto";
    case MotorState::CLOSED:  return "zaprto";
    case MotorState::OPENING: return "odpira se";
    case MotorState::CLOSING: return "zapira se";
    default:                  return "ročno premaknjeno";
  }
}