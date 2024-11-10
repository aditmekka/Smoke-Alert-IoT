#include <Arduino.h>
#include "secrets.h"
#include <WiFiManager.h>
#include <FirebaseManager.h>
#include <BuzzerManager.h>
#include "time.h"

const int buzzerPins[] {5, 18, 19, 21};
const int sensorPins[] {36, 39, 35, 33};
const int LED_BUILTIN = 2;
const int WIFI_TIMEOUT = 10000;
const int NUM_SENSORS = sizeof(sensorPins) / sizeof(sensorPins[0]);
const unsigned long serialDebugInterval = 5000;
const unsigned long interval = 1000;

bool isInitBuzzerTest = false;
int smokeThreshold = 20;
int buzzerState = 0;
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; 
const int daylightOffset_sec = 3600; 
unsigned long previousSerialDebugMillis = 0;
unsigned long previousMillis = 0;
int sensorValues[NUM_SENSORS];

WiFiManager wifiManager(LED_BUILTIN, WIFI_TIMEOUT);
FirebaseManager firebaseManager;
BuzzerManager buzzerManager(buzzerPins);

unsigned long sendDataPrevMillis = 0;
bool signupOK = false; 

//Prototype Function
void initializePins();
void initializeNTP();
void initializeFirebase();
void beginFirebaseStream();
void sendHeartbeat();
void buzzerTest();
void setBuzzerState(BuzzerState state);
void sendSensorData();
void handleFirebaseStreams();
void serialDebug();
int convertToPercent(int analogValue);

void setup(){
  Serial.begin(115200);
  buzzerManager.initBuzzerPins();
  initializePins();
  wifiManager.connect(ssid, password);
  initializeNTP();
  firebaseManager.init(API_KEY, DATABASE_URL, signupOK);
  if (Firebase.ready() && signupOK) {
    Serial.println("Stream initialized properly");
    firebaseManager.beginStreams();
  }else{
    Serial.println("Failed to initialize stream");
  }
}

void loop(){
  int mappedSensorValues[NUM_SENSORS];
  for(int i = 0; i < NUM_SENSORS; i++){
    int rawValue = analogRead(sensorPins[i]);
    mappedSensorValues[i] = convertToPercent(rawValue);
  }

  if(Firebase.ready() && signupOK && millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0){
    sendDataPrevMillis = millis();
    firebaseManager.sendSensorData(mappedSensorValues, NUM_SENSORS);
    firebaseManager.sendHeartbeat();
  }

  if (Firebase.ready() && signupOK) {
    firebaseManager.handleStreams(isInitBuzzerTest, smokeThreshold);
  }

  bool anySensorExceedThrehold = false;
  for(int i = 0; i < NUM_SENSORS; i++){
    if(mappedSensorValues[i] > smokeThreshold){
      anySensorExceedThrehold = true;
      break;
    }else{
      anySensorExceedThrehold = false;
    }
  }

  if(anySensorExceedThrehold){
    Serial.println("Buzzer Triggered");
    buzzerManager.setBuzzerState(BUZZER_ALL);
  }else if(!isInitBuzzerTest){
    buzzerManager.setBuzzerState(BUZZER_OFF);
  }

  if(isInitBuzzerTest){
    buzzerManager.buzzerTest(isInitBuzzerTest);
  }

  //serialDebug();
}

void initializeNTP(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void initializePins(){
  for(int i = 0; i < 4; i++){
    pinMode(sensorPins[i], INPUT);
  }
}

void serialDebug(){
  if(millis() - previousSerialDebugMillis >= serialDebugInterval){
    previousSerialDebugMillis = millis();
    for(int i = 0; i < NUM_SENSORS; i++){
      sensorValues[i] = convertToPercent(analogRead(sensorPins[i]));
      Serial.println("Sensor Value" + String(i + 1) + ": " + String(sensorValues[i]));
    }
  }
}

int convertToPercent(int analogValue){
  return map(analogValue, 0, 4095, 0, 100);
}



