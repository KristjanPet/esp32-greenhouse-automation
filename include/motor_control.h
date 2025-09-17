#pragma once

enum class MotorState { STOPPED, OPENING, CLOSING, OPENED, CLOSED };

void setupMotorPins();
void motorGoUp();
void motorGoDown();
void motorStop();
bool isMotorUpActive();
bool isMotorDownActive();

void updateMotorTimer(); // call this from loop()
void startMotorUpTimed(unsigned long durationMs);
void startMotorDownTimed(unsigned long durationMs);

void setMotorState(MotorState s);
MotorState getMotorState();
const char* motorStateStr();

extern MotorState motorState; // Global motor state variable