#include <Arduino.h>
#include <WiFiS3.h>
#include <FirebaseClient.h>
#include <WiFiSSLClient.h>

// Library for PID controller
#include <PID_v1.h>

#define PIN_INPUT 0
#define PIN_OUTPUT 3

// WiFi Configuration & Firebase Configuration
#define WIFI_SSID "hrpwdotaapl"
#define WIFI_PASSWORD "hrpw2003"

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

// Code for PID
double computePID(double inp);
double kp=2, ki=5, kd=1;
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double input,output=7,setPoint;
double cummulativeError, rateError;

void setup()
{
 //initialize the variables we're linked to
  Serial.begin(115200);
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
  setPoint = 100;

}

void loop()
{
  setPoint = Database.get(aClient, "/pid-controller/sp-throttle-value");
  // input = Database.get(aClient, "/pid-controller/pv-sensor-feedback");
  // ki = Database.get(aClient, "/pid-controller/ki");
  // kd = Database.get(aClient, "/pid-controller/kd");
  // kp = Database.get(aClient, "/pid-controller/kp");
  // output = computePID(input);
  // // output += input;
  // Database.set<number_t>(aClient, "/pid-controller/output", number_t(output, 4));
  Serial.write(setPoint);
  delay(300);

}

double computePID(double inp){
        currentTime = millis();               
        elapsedTime = (double)(currentTime - previousTime);      
        
        error = setPoint - inp;                              
        cummulativeError += error * elapsedTime;                
        rateError = (error - lastError)/elapsedTime;   

        double out = kp*error + ki*cummulativeError + kd*rateError;                            

        lastError = error;                               
        previousTime = currentTime;                       

        return out;      
}

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