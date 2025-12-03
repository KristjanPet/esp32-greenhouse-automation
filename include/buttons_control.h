#pragma once
#include <Arduino.h>

void manualInit();     // call once in setup()
void manualTick();     // call every loop()
bool manualIsActive(); // true while a button is pressed
