#ifndef BUZZERMANAGER_H
#define BUZZERMANAGER_H

#include <Arduino.h>

enum BuzzerState { BUZZER_A, BUZZER_B, BUZZER_C, BUZZER_D, BUZZER_ALL, BUZZER_OFF };

class BuzzerManager{
public:
  BuzzerManager(const int* pins);
  void initBuzzerPins();
  void buzzerTest(bool isInitBuzzerTest);
  void setBuzzerState(BuzzerState state);

private:
  int* buzzerPins;
  unsigned long previousMillis = 0;
  const unsigned long interval = 1000;
  int buzzerState = 0;
};

#endif