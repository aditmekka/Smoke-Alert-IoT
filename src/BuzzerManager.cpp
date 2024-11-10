#include <BuzzerManager.h>

BuzzerManager::BuzzerManager(const int* pins){
  buzzerPins = const_cast<int*>(pins);
}

void BuzzerManager::initBuzzerPins(){
  for (int i = 0; i < 4; i++){
    pinMode(buzzerPins[i], OUTPUT);
    digitalWrite(buzzerPins[i], LOW);
  }
}

void BuzzerManager::buzzerTest(bool isInitBuzzerTest){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval && isInitBuzzerTest){
    previousMillis = currentMillis;
    Serial.print("Buzzer state: ");
    Serial.println(buzzerState);
    setBuzzerState(static_cast<BuzzerState>(buzzerState));
    buzzerState = (buzzerState + 1) % 6;
  }
}

void BuzzerManager::setBuzzerState(BuzzerState state){
  bool states[6][4] = {
    {HIGH, LOW, LOW, LOW},
    {LOW, HIGH, LOW, LOW},
    {LOW, LOW, HIGH, LOW},
    {LOW, LOW, LOW, HIGH},
    {HIGH, HIGH, HIGH, HIGH},
    {LOW, LOW, LOW, LOW}
  };

  for(int i = 0; i < 4; i++){
    digitalWrite(buzzerPins[i], states[state][i]);
  }
}