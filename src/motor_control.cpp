#include <Arduino.h>
#include "motor_control.h"

const int MOTOR_DIR_PIN = 14; // Pin for motor direction (up/down)
const int LEFT_MOTOR_ON_PIN = 26; // Pin for left motor on/off
const int RIGHT_MOTOR_ON_PIN = 27; // Pin for right motor on/off

static unsigned long motorTimerStart = 0;
static unsigned long motorRunDuration = 0;
static bool motorTimerActive = false;

MotorState motorState = MotorState::STOPPED; // Global motor state variable

void setupMotorPins() {
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_ON_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_ON_PIN, OUTPUT);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
}

void motorGoUp() {
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, HIGH);
  delay(100);
  digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
}

void motorGoDown() {
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
  delay(100);
  digitalWrite(LEFT_MOTOR_ON_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_ON_PIN, HIGH);
}

void motorStop() {
  digitalWrite(LEFT_MOTOR_ON_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_ON_PIN, LOW);
  delay(100);
  digitalWrite(MOTOR_DIR_PIN, LOW);
}

bool isMotorUpActive() {
  return digitalRead(MOTOR_DIR_PIN) == HIGH && digitalRead(RIGHT_MOTOR_ON_PIN) == HIGH;
}

bool isMotorDownActive() {
  return digitalRead(MOTOR_DIR_PIN) == LOW && digitalRead(RIGHT_MOTOR_ON_PIN) == HIGH;
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

const char* getStateStr(MotorState t) {
  switch(t){
    case MotorState::STOPPED: return "Stopped";
    case MotorState::OPENING: return "Opening";
    case MotorState::CLOSING: return "Closing";
    case MotorState::OPENED: return "Opened";
    case MotorState::CLOSED: return "Closed";
  }
  return "unknown";
}