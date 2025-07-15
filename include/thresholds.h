#ifndef THRESHOLDS_H
#define THRESHOLDS_H

struct Thresholds {
  float tempOpen = 25.0;
  float tempClose = 18.0;
  float windClose = 10.0;
  float windReopen = 6.0;

  bool useTempOpen = true;
  bool useTempClose = true;
  bool useWindClose = true;
  bool useWindReopen = true;
};

// Declare the global variable
extern Thresholds currentThresholds;

bool saveThresholds();
bool loadThresholds();

#endif