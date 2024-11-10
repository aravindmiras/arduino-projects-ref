#include <Arduino.h>
#include <WiFiS3.h>
#include <FirebaseClient.h>
#include <WiFiSSLClient.h>

// Libraries for DHT11
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#define DHTTYPE DHT11
#define DHTPIN 3
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Libraries for DS18B20 
#include<OneWire.h>
#include<DallasTemperature.h>
#define DS18BUS 4
OneWire oneWire(DS18BUS);
DallasTemperature ds18temp(&oneWire);

// For LM393
#define LMBUS 5
unsigned long start_time =0;
unsigned long end_time = 0;
int steps =0;
double steps_old=0;
double temp=0;

// For Matrix Display
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

#define WIFI_SSID "Thorfinn"
#define WIFI_PASSWORD "Thankyou"

#define API_KEY "AIzaSyDyo_659Qfh9f27AYXCJGqDu4ngPD6Mqko"

#define USER_EMAIL "hrpw.2003@gmail.com"
#define USER_PASSWORD "hrpw0000"
#define DATABASE_URL "https://iot-firebase-07-default-rtdb.asia-southeast1.firebasedatabase.app/"

void authHandler();

void printResult(AsyncResult &aResult);

void printError(int code, const String &msg);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiSSLClient ssl_client;
using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

AsyncResult aResult_no_callback;

double mt_rt_voltage=60, mt_live_voltage=0, mt_rt_current=120, mt_live_current=0, mt_temperature=0, mt_rt_rpm=3000, mt_live_rpm=0, mt_humidity=0, mt_vibration=0;
double bt_rt_voltage=60, bt_live_voltage=58, bt_rt_current=120, bt_live_current=119, bt_temperature=0, bt_soc=33;
double dhttemp=0;

void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // The JWT token processor required for ServiceAuth and CustomAuth authentications.
        // JWT is a static object of JWTClass and it's not thread safe.
        // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
        // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Sensors and Port Initialization
  dht.begin();
  ds18temp.begin();

  // Wifi & Firebase Section
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
        Serial.print(".");
        delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  Serial.println("Initializing app...");
  initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

  authHandler();
  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);

  aClient.setAsyncResult(aResult_no_callback);

  matrix.begin();

}

void loop() {
  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  // add the text
  const char text[] = "    WELCOME NAAC COMMUNITY!    ";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();


  sensors_event_t dhtevent;
  dht.temperature().getEvent(&dhtevent);
  dht.humidity().getEvent(&dhtevent);
  ds18temp.requestTemperatures();
  // mt_temperature = ds18temp.getTempCByIndex(0);
  dhttemp = dhtevent.temperature;
  dhttemp = (dhttemp-32)*0.55555;
  mt_temperature = dhttemp;
  mt_humidity = dhtevent.relative_humidity;
  bt_temperature = dhttemp;

  // put your main code here, to run repeatedly:
  start_time=millis();
  end_time= start_time+1000;

  while(millis()<end_time){
    if(digitalRead(LMBUS)){
      steps=steps+1;
      while(digitalRead(LMBUS));
    }
  }
  temp=steps-steps_old;
  steps_old=steps;

  mt_live_rpm=(temp/20);

  // put your main code here, to run repeatedly:
    bool status = false;
    // Sending Motor Data
    Database.set<number_t>(aClient, "/ptms/motor/actualVoltage", number_t(mt_live_voltage, 4));
    Database.set<number_t>(aClient, "/ptms/motor/ratedVoltage", number_t(mt_rt_voltage, 4));
    Database.set<number_t>(aClient, "/ptms/motor/actualCurrent", number_t(mt_live_current, 4));
    Database.set<number_t>(aClient, "/ptms/motor/ratedCurrent", number_t(mt_rt_current, 4));
    Database.set<number_t>(aClient, "/ptms/motor/ratedRPM", number_t(mt_rt_rpm, 4));
    Database.set<number_t>(aClient, "/ptms/motor/actualRPM", number_t(mt_live_rpm, 4));
    Database.set<number_t>(aClient, "/ptms/motor/temperature", number_t(mt_temperature, 4));
    Database.set<number_t>(aClient, "/ptms/motor/humidity", number_t(mt_humidity, 4));
    Database.set<number_t>(aClient, "/ptms/motor/vibration", number_t(mt_vibration, 4));
    // // Sending Battery Data
    Database.set<number_t>(aClient, "/ptms/battery/actualVoltage", number_t(bt_live_voltage, 4));
    Database.set<number_t>(aClient, "/ptms/battery/ratedVoltage", number_t(bt_rt_voltage, 4));
    Database.set<number_t>(aClient, "/ptms/battery/actualCurrent", number_t(bt_live_current, 4));
    Database.set<number_t>(aClient, "/ptms/battery/ratedCurrent", number_t(bt_rt_current, 4));
    Database.set<number_t>(aClient, "/ptms/battery/actualTemperature", number_t(bt_temperature, 4));
    Database.set<number_t>(aClient, "/ptms/battery/soc", number_t(bt_soc, 4));
  status = true;
  if (status)
    Serial.println("ok");
  else
    printError(aClient.lastError().code(), aClient.lastError().message());
}
