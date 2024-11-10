#include "WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager(int ledPin, unsigned long timeoutDuration) : ledPin(ledPin), timeoutDuration(timeoutDuration){
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void WiFiManager::connect(const char* ssid, const char * password){
  Serial.print("Connecting to WiFI");
  digitalWrite(ledPin, HIGH);

  WiFi.begin(ssid, password);
  unsigned long startTime = millis();

  while(WiFi.status() != WL_CONNECTED){
    if(millis() - startTime < timeoutDuration){
      Serial.print(".");
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }else{
      connectionTimeout();
      return;
    }
  }
  digitalWrite(ledPin, HIGH);
  Serial.println("\nConnected with IP: " + String(WiFi.localIP()));
}

int ledPin;
unsigned long timeoutDuration;

void WiFiManager::connectionTimeout(){
  Serial.println("Connection Timeout");
  while(true){
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(2000);

    if(WiFi.status() == WL_CONNECTED){
      digitalWrite(ledPin, HIGH);
      Serial.println("Reconnected with IP: "  + String(WiFi.localIP()));
    }
  }
}