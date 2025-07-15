#include <Arduino.h>
#include "motor_control.h"

const int MOTOR_UP_PIN = 14;
const int MOTOR_DOWN_PIN = 27;

static unsigned long motorTimerStart = 0;
static unsigned long motorRunDuration = 0;
static bool motorTimerActive = false;

MotorState motorState = STOPPED; // Global motor state variable

void setupMotorPins() {
  pinMode(MOTOR_UP_PIN, OUTPUT);
  pinMode(MOTOR_DOWN_PIN, OUTPUT);
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, LOW);
}

void motorGoUp() {
  digitalWrite(MOTOR_DOWN_PIN, LOW);
  digitalWrite(MOTOR_UP_PIN, HIGH);
}

void motorGoDown() {
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, HIGH);
}

void motorStop() {
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, LOW);
}

bool isMotorUpActive() {
  return digitalRead(MOTOR_UP_PIN) == HIGH;
}

bool isMotorDownActive() {
  return digitalRead(MOTOR_DOWN_PIN) == HIGH;
}

void startMotorUpTimed(unsigned long durationMs) {
  motorStop();
  motorGoUp();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

void startMotorDownTimed(unsigned long durationMs) {
  motorStop();
  motorGoDown();
  motorTimerStart = millis();
  motorRunDuration = durationMs;
  motorTimerActive = true;
}

void updateMotorTimer() {
  if (motorTimerActive && millis() - motorTimerStart >= motorRunDuration) {
    motorStop();
    motorTimerActive = false;

    if (motorState == OPENING) motorState = OPENED;
    else if (motorState == CLOSING) motorState = CLOSED;
  }
}