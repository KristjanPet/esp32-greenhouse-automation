#pragma once

enum class MotorState
{
    STOPPED,
    OPENING,
    CLOSING,
    OPENED,
    CLOSED
};

void setupMotorPins();
void motorGoUp();
void motorGoDown();
void motorStop();
bool isMotorUpActive();
bool isMotorDownActive();

void updateMotorTimer(); // call this from loop()
void startMotorUpTimed(unsigned long durationMs);
void startMotorDownTimed(unsigned long durationMs);

void setTargetPercent(float p);
float getCurrentPercent();
void tickMotion();

void setMotorState(MotorState s);
MotorState getMotorState();
const char *motorStateStr();
const char *getStateStr(MotorState t);

extern MotorState motorState; // Global motor state variable