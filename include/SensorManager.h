#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>

class SensorManager{
public:
  SensorManager(const int* pins, int numSensors);
  ~SensorManager();
  void initSensorPins();
  void readSensor();
  int getMappedSensorValue(int index);
  bool anySensorExceedThreshold(int threshold);

private:
  const int* sensorPins;
  int numSensors;
  int* sensorValues;
  int mapSensorValue(int rawAnalogValue);
};

#endif