/*
  The code demonstrates the usage of FreeRTOS (Real-Time Operating System) to run concurrent tasks.

  One task is responsible for running the loop() logic (in a thread-safe manner), 
  while the other task blinks an LED using the built-in LED on non-Portenta boards or 
  the RGB LED on the Portenta C33 board.
*/

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <WiFiS3.h>
#include <FirebaseClient.h>
#include <WiFiSSLClient.h>

#define WIFI_SSID "hrpwdotaapl"
#define WIFI_PASSWORD "hrpw3333"

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
/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

// TaskHandle_t loop_task, blinky_task;

TaskHandle_t sensor_task, firebase_task;



/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
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

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

// #if defined(ARDUINO_PORTENTA_C33)
//   /* Only the Portenta C33 has an RGB LED. */
//   pinMode(LEDR, OUTPUT);
//   digitalWrite(LEDR, LOW);
// #endif

  /* Init a task that calls 'loop'
   * since after the call to
   * 'vTaskStartScheduler' we'll never
   * get out of setup() and therefore
   * would never get to loop(), as we
   * are leaving the default execution
   * flow.
   */
  auto const rc_sensor_input = xTaskCreate
    (
      sensor_thread_func,
      static_cast<const char*>("Sensor Thread"),
      512 / 2,   /* usStackDepth in words */

      nullptr,   /* pvParameters */
      2,         /* uxPriority */
      &sensor_task /* pxCreatedTask */
    );

  if (rc_sensor_input != pdPASS) {
    Serial.println("Failed to create 'sensor' thread");
    return;
  }

  auto const rc_firebase_update = xTaskCreate
    (
      firebase_thread_func,
      static_cast<const char*>("Firebase Thread"),
      512 / 2,     /* usStackDepth in words */
      nullptr,     /* pvParameters */
      1,           /* uxPriority */
      &firebase_task /* pxCreatedTask */
    );

  if (rc_firebase_update != pdPASS) {
    Serial.println("Failed to create 'firebase' thread");
    return;
  }

  Serial.println("Starting scheduler ...");
  /* Start the scheduler. */
  vTaskStartScheduler();
  /* We'll never get here. */
  // vTaskEndScheduler();
  for( ;; );
}

void loop()
{
// #if defined(ARDUINO_PORTENTA_C33)
//   /* Only the Portenta C33 has an RGB LED. */
//   digitalWrite(LEDR, !digitalRead(LEDR));
// #else
//   Serial.println(millis());
// #endif
  // vTaskDelay(configTICK_RATE_HZ/4);
}

// void loop_thread_func(void *pvParameters)
// {
//   for(;;)
//   {
//     loop();
//     taskYIELD();
//   }
// }

// void blinky_thread_func(void *pvParameters)
// {
//   /* setup() */
//   pinMode(LED_BUILTIN, OUTPUT);
//   digitalWrite(LED_BUILTIN, LOW);

//   /* loop() */
//   for(;;)
//   {
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     vTaskDelay(configTICK_RATE_HZ);
//   }
// }

void sensor_thread_func(void *pvParameters)
{
  /* setup() */
  int *pIntParameter = (int *)pvParameters;
  /* loop() */
  for(;;)
  {
  }
}
void firebase_thread_func(void *pvParameters)
{
    int *pIntParameter = (int *)pvParameters;
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

    bool status = false;
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
  status = true;
  if (status)
    Serial.println("ok");
  else
    printError(aClient.lastError().code(), aClient.lastError().message());

  /* loop() */
  for(;;)
  {
    authHandler();
    Database.loop();
  }
}

