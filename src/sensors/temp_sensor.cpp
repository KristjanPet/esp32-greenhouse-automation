#include <DHT.h>
#include "temp_sensor.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float lastTemp = -100; // default for failure
float lastH = -1;

void setupTempSensor()
{
    dht.begin();
}

void readTemperatureC()
{
    float t = dht.readTemperature();
    if (!isnan(t))
    {
        lastTemp = t;
    }

    float h = dht.readHumidity();
    if (!isnan(h))
    {
        lastH = h;
    }
}

float getTemperatureC(){
    return lastTemp;
}

float getHumidity()
{
    return lastH;
}