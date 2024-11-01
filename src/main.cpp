#include <Arduino.h>
#include "env.h"
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"

const int buzzerPin[] {5, 18, 19, 21};
const int sensorPin[] {36, 39, 35, 33};

bool isInitBuzzerTest = false;
int smokeThreshold = 20;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; 
const int daylightOffset_sec = 3600; 

const unsigned long interval = 1000;
unsigned long previousMillis = 0;
int buzzerState = 0;

int sensorVal1; 
int sensorVal2;
int sensorVal3;
int sensorVal4;

FirebaseData fbdo, fbdo_s1, fbdo_s2;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false; 

void buzzerTest();
void sendSensorData();
void handleFirebaseStreams();
void serialDebug();

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.RTDB.beginStream(&fbdo_s1, "/userInput/buzzerTest")){     
    Serial.printf("Stream 1 begin error, %s\n\n", fbdo_s1.errorReason().c_str());
  }
  if (!Firebase.RTDB.beginStream(&fbdo_s2, "/userInput/smokeThreshold")){     
    Serial.printf("Stream 2 begin error, %s\n\n", fbdo_s1.errorReason().c_str());
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pinMode(22, INPUT_PULLUP);

  for(int i = 0; i < 4; i++){
    pinMode(buzzerPin[i], OUTPUT);
    digitalWrite(buzzerPin[i], LOW);
  }
  for(int i = 0; i < 4; i++){
    pinMode(sensorPin[i], INPUT);
  }
}

void loop(){
  if(Firebase.ready() && signupOK && millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0){
    sendDataPrevMillis = millis();
    sendSensorData();
  }

  if (Firebase.ready() && signupOK) {
    handleFirebaseStreams();
  }

  if(isInitBuzzerTest){
    buzzerTest();
  }else{
    for(int i = 0; i < 4; i++){
      digitalWrite(buzzerPin[i], LOW);
    }
  }

  //serialDebug();
}

void serialDebug(){
    Serial.print(sensorVal1);     Serial.println(" %");
    Serial.print(sensorVal2);     Serial.println(" %");
    Serial.print(sensorVal3);     Serial.println(" %");
    Serial.print(sensorVal4);     Serial.println(" %");
    Serial.println("<-->");
    delay(500);
}

int convertToPercent(int analogValue){
  return map(analogValue, 0, 4095, 0, 100);
}

void sendSensorData() {
  sensorVal1 = convertToPercent(analogRead(sensorPin[0])); 
  sensorVal2 = convertToPercent(analogRead(sensorPin[1]));
  sensorVal3 = convertToPercent(analogRead(sensorPin[2]));
  sensorVal4 = convertToPercent(analogRead(sensorPin[3]));

  if(Firebase.RTDB.setInt(&fbdo, "sensorValue/sensor1", sensorVal1)){
    Serial.println("Sensor Value A: " + String(sensorVal1));
  }else{
    Serial.println("Failed to save sensor value A: " + fbdo.errorReason());
  }

  if(Firebase.RTDB.setInt(&fbdo, "sensorValue/sensor2", sensorVal2)){
    Serial.println("Sensor Value B: " + String(sensorVal2));
  }else{
    Serial.println("Failed to save sensor value B: " + fbdo.errorReason());
  }

  if(Firebase.RTDB.setInt(&fbdo, "sensorValue/sensor3", sensorVal3)){
    Serial.println("Sensor Value C: " + String(sensorVal3));
  }else{
    Serial.println("Failed to save sensor value C: " + fbdo.errorReason());
  }

  if(Firebase.RTDB.setInt(&fbdo, "sensorValue/sensor4", sensorVal4)){
    Serial.println("Sensor Value D: " + String(sensorVal4));
  }else{
    Serial.println("Failed to save sensor value D: " + fbdo.errorReason());
  }

  unsigned long currentTime = time(nullptr);
  if (Firebase.RTDB.setInt(&fbdo, "thingStat/lastSeen", currentTime)) {
    Serial.println("Sent heartbeat to Firebase");
  } else {
    Serial.println("Failed to send heartbeat: " + fbdo.errorReason());
  }
}

void handleFirebaseStreams() {
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
      Serial.println("Smoke threshold changed: " + String(smokeThreshold));
    }
  }
}

void buzzerTest(){
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

      Serial.print("Buzzer state: ");
      Serial.println(buzzerState);

      switch(buzzerState){
        case 0:
          digitalWrite(buzzerPin[0], HIGH);
          digitalWrite(buzzerPin[1], LOW);
          digitalWrite(buzzerPin[2], LOW);
          digitalWrite(buzzerPin[3], LOW);
          break;
        case 1:
          digitalWrite(buzzerPin[0], LOW);
          digitalWrite(buzzerPin[1], HIGH);
          digitalWrite(buzzerPin[2], LOW);
          digitalWrite(buzzerPin[3], LOW);
          break;
        case 2:
          digitalWrite(buzzerPin[0], LOW);
          digitalWrite(buzzerPin[1], LOW);
          digitalWrite(buzzerPin[2], HIGH);
          digitalWrite(buzzerPin[3], LOW);
          break;
        case 3:
          digitalWrite(buzzerPin[0], LOW);
          digitalWrite(buzzerPin[1], LOW);
          digitalWrite(buzzerPin[2], LOW);
          digitalWrite(buzzerPin[3], HIGH);
          break;
        case 4:
          digitalWrite(buzzerPin[0], HIGH);
          digitalWrite(buzzerPin[1], HIGH);
          digitalWrite(buzzerPin[2], HIGH);
          digitalWrite(buzzerPin[3], HIGH);
          break;
        case 5:
          digitalWrite(buzzerPin[0], LOW);
          digitalWrite(buzzerPin[1], LOW);
          digitalWrite(buzzerPin[2], LOW);
          digitalWrite(buzzerPin[3], LOW);
          break;
        }
        buzzerState = (buzzerState + 1) % 6;
    }
}