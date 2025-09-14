#include "buttons_control.h"
#include "motor_control.h"

// pick your pins (INPUT_PULLUP → button to GND)
static const int PIN_L_UP   = 19;
static const int PIN_L_DOWN = 21;
static const int PIN_R_UP   = 22;
static const int PIN_R_DOWN = 23;

static const uint32_t DEBOUNCE_MS = 30;

static bool manualActive = false;
enum class ManualDir { NONE, UP, DOWN };
static ManualDir manualDir = ManualDir::NONE;

// simple debounce state
static bool upStable=false, downStable=false, upLast=false, downLast=false;
static uint32_t tChange=0;

static inline bool readUpRaw() {
  return (digitalRead(PIN_L_UP)==LOW) || (digitalRead(PIN_R_UP)==LOW);
}
static inline bool readDownRaw() {
  return (digitalRead(PIN_L_DOWN)==LOW) || (digitalRead(PIN_R_DOWN)==LOW);
}

void manualInit() {
  pinMode(PIN_L_UP,    INPUT_PULLUP);
  pinMode(PIN_L_DOWN,  INPUT_PULLUP);
  pinMode(PIN_R_UP,    INPUT_PULLUP);
  pinMode(PIN_R_DOWN,  INPUT_PULLUP);
  upStable = upLast   = readUpRaw();
  downStable = downLast = readDownRaw();
  tChange = millis();
}

bool manualIsActive() { return manualActive; }

void manualTick() {
  // debounce both “any UP” and “any DOWN”
  bool upRaw = readUpRaw();
  bool downRaw = readDownRaw();
  uint32_t now = millis();

  if (upRaw != upLast || downRaw != downLast) {
    upLast = upRaw; downLast = downRaw; tChange = now;
  }
  if ((now - tChange) >= DEBOUNCE_MS) {
    upStable = upLast; downStable = downLast;
  }

  // logic: while any button held → manual mode; ignore opposite direction
  if (upStable || downStable) {
    if (!manualActive) {
      manualActive = true;
      if (upStable)  { manualDir = ManualDir::UP;   motorStop(); motorGoUp(); }
      else           { manualDir = ManualDir::DOWN; motorStop(); motorGoDown(); }
    } else {
      if (manualDir == ManualDir::UP && !upStable)   { motorStop(); manualActive=false; manualDir=ManualDir::NONE; }
      if (manualDir == ManualDir::DOWN && !downStable){ motorStop(); manualActive=false; manualDir=ManualDir::NONE; }
      // opposite press while held is ignored by design
    }
  } else {
    if (manualActive) { motorStop(); manualActive=false; manualDir=ManualDir::NONE; }
  }
}
