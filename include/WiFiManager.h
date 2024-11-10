#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
  WiFiManager(int ledPin, unsigned long timeoutDuration);
  void connect(const char* ssid, const char* password);

private:
  int ledPin;
  unsigned long timeoutDuration;
  void connectionTimeout();
};

#endif