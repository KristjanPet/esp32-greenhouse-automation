#ifndef WIND_SENSOR_H
#define WIND_SENSOR_H

void setupWindSensor();
float getWindSpeed(); // m/s
float getAverageWindSpeed(); // 2min average
void updateWindSpeedBuffer(float speed); 

#endif
