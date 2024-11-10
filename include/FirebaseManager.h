#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <Firebase_ESP_Client.h>

class FirebaseManager{
public:
  FirebaseManager();
  void init(const char* apiKey, const char* databaseUrl, bool &signupOK);
  void sendSensorData(int sensorValues[], int numSensors);
  void sendHeartbeat();
  void beginStreams();
  void handleStreams(bool &isInitBuzzerTest, int &smokeThreshold);

private: 
  FirebaseData fbdo, fbdo_s1, fbdo_s2;
  FirebaseAuth auth;
  FirebaseConfig config;
};

#endif