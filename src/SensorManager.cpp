#include "SensorManager.h"

SensorManager::SensorManager(const int* pins, int numSensors)
    : sensorPins(pins), numSensors(numSensors) {
    sensorValues = new int[numSensors]; // Allocate memory for sensor values
}

SensorManager::~SensorManager() {
    delete[] sensorValues; // Free allocated memory
}

void SensorManager::initSensorPins() {
    for (int i = 0; i < numSensors; i++) {
        pinMode(sensorPins[i], INPUT); // Set each pin as input
    }
}

void SensorManager::readSensor() {
    for (int i = 0; i < numSensors; i++) {
        sensorValues[i] = analogRead(sensorPins[i]); // Read raw sensor value
    }
}

int SensorManager::getMappedSensorValue(int index) {
    if (index < 0 || index >= numSensors) {
        return -1; // Return -1 for invalid index
    }
    return mapSensorValue(sensorValues[index]); // Return the mapped value
}

int SensorManager::mapSensorValue(int rawValue) {
    // Map the raw value to a range (0 to 100 for example)
    return map(rawValue, 0, 4095, 0, 100); // Adjust mapping range based on your needs
}

bool SensorManager::anySensorExceedThreshold(int threshold) {
    for (int i = 0; i < numSensors; i++) {
        if (getMappedSensorValue(i) > threshold) { // Compare against threshold
            return true; // If any sensor exceeds the threshold, return true
        }
    }
    return false; // If none exceed, return false
}