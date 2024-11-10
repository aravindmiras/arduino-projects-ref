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
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define DHTTYPE DHT11
#define DHTPIN 0

#define WIFI_SSID "Thorfinn"
#define WIFI_PASSWORD "Thankyou"

#define API_KEY "AIzaSyCuG8BGhF0LOqL4hXC5_9SIOBEVpRT1zkU"
#define DATABASE_URL "https://mini-project-a1c36-default-rtdb.asia-southeast1.firebasedatabase.app/" 

DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const int irSensorPin = 4; 
volatile int revCount =0;
unsigned long lastTime = 0;
int offset = 20;
int relayPin = 14;
float tempThreshold = 25;

void IRAM_ATTR handleSensorInterrupt() {
  revCount++;
}

void setup(){
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  
  config.token_status_callback = tokenStatusCallback; 
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  sensors.begin();
  pinMode(irSensorPin, INPUT_PULLUP);
  pinMode(relayPin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), handleSensorInterrupt, RISING);
}


void loop(){  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }
  delay(1000);
  float h = dht.readHumidity();
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  
  if (Firebase.ready() && signupOK ) {   
    if (Firebase.RTDB.setFloat(&fbdo, "motordat/motor/humidity",h)){
       Serial.println("PASSED");
       Serial.print("Humidity: ");
       Serial.println(h);      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if(tempC>=tempThreshold){
      digitalWrite(relayPin,HIGH);
    }else{
        digitalWrite(relayPin,LOW);
    }
    if (Firebase.RTDB.setFloat(&fbdo, "motordat/motor/temperature", tempC)){
       Serial.println("PASSED");
       Serial.print("Temperature: ");
       Serial.println(tempC);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  
  detachInterrupt(digitalPinToInterrupt(irSensorPin)); // Detach interrupt to safely read revCount
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;
  lastTime = currentTime;
  int currentRevCount = revCount;
  revCount = 0;
  attachInterrupt(digitalPinToInterrupt(irSensorPin), handleSensorInterrupt, RISING); // Reattach interrupt

  float revPerSecond = static_cast<float>(currentRevCount) / (elapsedTime / 1000.0);
  int rpm = static_cast<int>(revPerSecond * 60);

  Serial.print("RPM: ");
  Serial.println(rpm);
  if (Firebase.RTDB.setFloat(&fbdo, "motordat/motor/actualRPM", rpm)){
      Serial.println("PASSED");
      Serial.print("RPM: ");
      Serial.println(rpm);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  int volt = analogRead(A0);
  double voltage = map(volt,0,1023,0,2500)+offset;
  voltage = voltage/100;
  voltage = voltage*1.92;
  Serial.print("Voltage: ");     
  Serial.print(voltage);                  
  if(voltage>1){
  if (Firebase.RTDB.setInt(&fbdo, "motordat/motor/actualVoltage", voltage)){
          Serial.print("Voltage: ");
          Serial.println  (voltage);
          Serial.println("V");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  } 
  }
  Serial.println("__________");
}
