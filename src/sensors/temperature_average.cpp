#include <Arduino.h>
#include "temperature_average.h"
#include "temp_sensor.h"
#include "temp_sensor_2.h"

namespace
{
constexpr float MIN_VALID_TEMPERATURE_C = -10.0f;
constexpr float MAX_VALID_TEMPERATURE_C = 50.0f;
}

bool isValidTemperatureC(float temperature)
{
  return isfinite(temperature) &&
         temperature >= MIN_VALID_TEMPERATURE_C &&
         temperature <= MAX_VALID_TEMPERATURE_C;
}

float getAverageTemperatureC()
{
  const float temperature1 = getTemperatureC();
  const float temperature2 = getTemperature2C();
  const bool temperature1Valid = isValidTemperatureC(temperature1);
  const bool temperature2Valid = isValidTemperatureC(temperature2);

  if (temperature1Valid && temperature2Valid)
  {
    return (temperature1 + temperature2) / 2.0f;
  }

  if (temperature1Valid)
  {
    return temperature1;
  }

  if (temperature2Valid)
  {
    return temperature2;
  }

  return NAN;
}
