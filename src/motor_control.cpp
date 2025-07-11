#include <Arduino.h>
#include "motor_control.h"

const int MOTOR_UP_PIN = 14;
const int MOTOR_DOWN_PIN = 27;

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
