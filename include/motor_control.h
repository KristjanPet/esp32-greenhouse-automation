#pragma once
#include "types.h"

void setupMotorPins();
void motorGoUp();
void motorGoDown();
void motorStop();
void moveLogic();

void setTargetPercent(float p);
float getCurrentPercent();
void tickMotion();

void setPendingTrigger(Trigger t);

void setMotorState(MotorState s);
MotorState getMotorState();
const char *motorStateStr();

extern MotorState motorState; // Global motor state variable