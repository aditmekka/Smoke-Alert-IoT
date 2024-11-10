#include <Arduino.h>
#include <FirebaseManager.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

FirebaseManager::FirebaseManager(){
  config.api_key = "";
  config.database_url = "";
}

void FirebaseManager::init(const char* apiKey, const char* databaseUrl, bool &signupOK){
  config.api_key = apiKey;
  config.database_url = databaseUrl;

  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    Serial.println();
    signupOK = true;
  }else{
    Serial.printf("Sign-up failed: %s\n", config.signer.signupError.message.c_str());
    signupOK = false;
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void FirebaseManager::beginStreams(){
  if (!Firebase.RTDB.beginStream(&fbdo_s1, "/userInput/buzzerTest")){     
    Serial.printf("Stream 1 begin error, %s\n\n", fbdo_s1.errorReason().c_str());
  }
  if (!Firebase.RTDB.beginStream(&fbdo_s2, "/userInput/smokeThreshold")){     
    Serial.printf("Stream 2 begin error, %s\n\n", fbdo_s1.errorReason().c_str());
  }
}

void FirebaseManager::sendSensorData(int sensorValues[], int numSensors){
  for(int i = 0; i < numSensors; i++){
    if(Firebase.RTDB.setInt(&fbdo, "sensorValue/sensor" + String(i + 1), sensorValues[i])){
      Serial.println("Sensor Value" + String(i + 1) + ": " + String(sensorValues[i]));
    }else{
      Serial.println("Failed to save sensor value" + String(i + 1) + ": " + String(sensorValues[i]));
    }
  }
}

void FirebaseManager::sendHeartbeat(){
  unsigned long currentTime = time(nullptr);
  if (Firebase.RTDB.setInt(&fbdo, "thingStat/lastSeen", currentTime)) {
    Serial.println("Sent heartbeat to Firebase");
  } else {
    Serial.println("Failed to send heartbeat: " + fbdo.errorReason());
  }
}

void FirebaseManager::handleStreams(bool &isInitBuzzerTest, int &smokeThreshold){
  if (!Firebase.RTDB.readStream(&fbdo_s1)) {
    Serial.printf("Stream 1 read error, %s\n\n", fbdo_s1.errorReason().c_str());
  }
  if (fbdo_s1.streamAvailable()) {
    if (fbdo_s1.dataType() == "boolean") {
      isInitBuzzerTest = fbdo_s1.boolData();
      Serial.println("Buzzer test initiated: " + String(isInitBuzzerTest));
    }
  }

  if (!Firebase.RTDB.readStream(&fbdo_s2)) {
    Serial.printf("Stream 2 read error, %s\n\n", fbdo_s2.errorReason().c_str());
  }
  if (fbdo_s2.streamAvailable()) {
    if (fbdo_s2.dataType() == "int") {
      smokeThreshold = fbdo_s2.intData();
      Serial.println("Smoke threshold changed to: " + String(smokeThreshold) + "%");
    }
  }
}