#include <Arduino.h>
#include <DHT.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"

#define DHTTYPE DHT11
#define DHTPIN 13 //node mcu pin D7
#define TRIGPIN 14 //D5
#define ECHOPIN 12 //D6

#define WIFI_SSID "hrpw"//change to your wifi ssid
#define WIFI_PASSWORD "hrpw0000"//your wifi password

#define API_KEY "AIzaSyA45Ge33GEJOZZAL2TzvuFT2sOMIsxdaz8" //firebase api key
#define DATABASE_URL "http://agro-rt-default-rtdb.asia-southeast1.firebasedatabase.app/" //RTDB url

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;//Firebase object don't change

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
long duration;
float lvl;

void setup() {
    pinMode(DHTPIN, INPUT);
    dht.begin();
    Serial.begin(115200);
    pinMode(TRIGPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);
    Serial.begin(115200);

  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s exception - one \n", config.signer.signupError.message.c_str());
  }


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
//Don't dare to touch the setup function
void loop() {

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
  }
  delay(1000);

  // For ultrasonic sensor
  // Clears the trigPin  
 digitalWrite(TRIGPIN, LOW);  
 delayMicroseconds(2);  
 // Sets the trigPin on HIGH state for 10 micro seconds  
 digitalWrite(TRIGPIN, HIGH);  
 delayMicroseconds(10);  
 digitalWrite(TRIGPIN, LOW);  
 // Reads the echoPin, returns the sound wave travel time in microseconds  
 duration = pulseIn(ECHOPIN, HIGH);  
 // Calculating the distance  
 lvl= duration*0.034/2;
 if(lvl > 12.7 && lvl<=1197){
  lvl=12.62;
 }
if(lvl > 1197){
  lvl=0;
}

  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (Firebase.ready() && signupOK ) {

    if (Firebase.RTDB.setFloat(&fbdo, "realtime/data/humidity", h)) { // path setting area
      Serial.println("PASSED");
      Serial.print("Humidity: ");
      Serial.println(h);

    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "realtime/data/temperature", t)) {
      Serial.println("PASSED");
      Serial.print("Temperature: ");
      Serial.println(t);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // For writing waterlevel indicator
    if (Firebase.RTDB.setFloat(&fbdo, "realtime/data/waterlevel", lvl)) {
      Serial.println("PASSED");
      Serial.print("Water Level: ");
      Serial.println(lvl);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
