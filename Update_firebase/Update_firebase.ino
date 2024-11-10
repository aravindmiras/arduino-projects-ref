/**
 * SYNTAX:
 *
 * bool RealtimeDatabase::update(<AsyncClient>, <path>, <object_t>);
 *
 * <AsyncClient> - The async client.
 * <path> - The node path to set the value.
 * <object_t> - The JSON representation data (object_t) to update.
 *
 * This function returns bool status when task is complete.
 *
 * In case of error, the operation error information can be obtain from AsyncClient via aClient.lastError().message() and
 * aClient.lastError().code().
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#include <WiFiS3.h>
// #if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA)
// #include <WiFi.h>
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
// #include <WiFiNINA.h>
// #elif __has_include(<WiFi101.h>)
// #include <WiFi101.h>
// #elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
// #include <WiFiS3.h>
// #elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
// #include <WiFiC3.h>
// #elif __has_include(<WiFi.h>)
// #include <WiFi.h>
// #endif

#include <FirebaseClient.h>

#define WIFI_SSID "hrpwdotaapl"
#define WIFI_PASSWORD "hrpw3333"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "AIzaSyDyo_659Qfh9f27AYXCJGqDu4ngPD6Mqko"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "hrpw.2003@gmail.com"
#define USER_PASSWORD "hrpw0000"
#define DATABASE_URL "https://iot-firebase-07-default-rtdb.asia-southeast1.firebasedatabase.app/"

void authHandler();

void printResult(AsyncResult &aResult);

void printError(int code, const String &msg);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

// #if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
// #include <WiFiClientSecure.h>
// WiFiClientSecure ssl_client;
// #elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
// #endif

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

RealtimeDatabase Database;

AsyncResult aResult_no_callback;

void setup()
{

    Serial.begin(115200);
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

// #if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
//     ssl_client.setInsecure();
// #if defined(ESP8266)
//     ssl_client.setBufferSizes(4096, 1024);
// #endif
// #endif

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    authHandler();

    // Binding the FirebaseApp for authentication handler.
    // To unbind, use Database.resetApp();
    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // In case setting the external async result to the sync task (optional)
    // To unset, use unsetAsyncResult().
    aClient.setAsyncResult(aResult_no_callback);

    // Library does not provide JSON parser library, the following JSON writer class will be used with
    // object_t for simple demonstration.

    object_t json;
    JsonWriter writer;
    writer.create(json, "data/value", random(1000, 2000)); //-> {"data":{"value":x}}
    // Or set the seialized JSON string to the object_t as object_t("{\"data\":{\"value\":x}}")

    Serial.print("Update... ");
    bool status = Database.update(aClient, "/test/json", json);

    if (status)
        Serial.println("ok");
    else
        printError(aClient.lastError().code(), aClient.lastError().message());
}

void loop()
{
    authHandler();

    Database.loop();
    object_t json;
    JsonWriter writer;
    // For Motor Data
    writer.create(json, "motor/actual_voltage", mt_live_voltage); //-> {"data":{"value":x}}
    writer.create(json, "motor/rated_voltage", mt_rt_voltage);
    writer.create(json, "motor/actual_current", mt_live_current);
    writer.create(json, "motor/rated_current", mt_rt_current);
    writer.create(json, "motor/actual_RPM", mt_rt_rpm);
    writer.create(json, "motor/rated_RPM", mt_live_rpm);
    writer.create(json, "motor/temperature", mt_temperature);
    writer.create(json, "motor/humidity", mt_humidity);
    writer.create(json, "motor/vibration", mt_vibration);
    // For Battery Data
    writer.create(json, "battery/actual_voltage", bt_live_voltage);
    writer.create(json, "battery/rated_voltage", bt_rt_voltage);
    writer.create(json, "battery/actual_current", bt_live_current);
    writer.create(json, "battery/rated_current", bt_rt_current);
    writer.create(json, "battery/temperature", bt_temperature);
    writer.create(json, "battery/soc", bt_soc);
    Database.update(aClient, "/ptms", json);
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