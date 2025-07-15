#pragma once

void setupMotorPins();
void motorGoUp();
void motorGoDown();
void motorStop();
bool isMotorUpActive();
bool isMotorDownActive();

void updateMotorTimer(); // call this from loop()
void startMotorUpTimed(unsigned long durationMs);
void startMotorDownTimed(unsigned long durationMs);

enum MotorState {
  STOPPED,
  OPENING,
  CLOSING,
  OPENED,
  CLOSED
};

extern MotorState motorState; // Global motor state variable