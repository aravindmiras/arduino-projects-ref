#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#include <Adafruit_BMP085.h>

#include <Wire.h>
#include <DS3231.h>

#include "ADS1X15.h"

#define DHTTYPE DHT11
#define DHTPIN 6

uint32_t delayMS;
sensor_t sensor;
sensors_event_t event;

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

RTClib myRTC;

float temperature, humidity, pressure, altitude, sealevelpressure;

DateTime now;

// defines pins numbers
const int trigPin = 7;
const int echoPin = 8;

long duration;
int distance;

volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 11; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;


ADS1115 ADS(0x48);

float convertDegree(){
  return (temperature-32)*(5.0/9.0);
}
void flow () // Interrupt function
{
   flow_frequency++;
}
void flowStart(){
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  interrupts(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
}

void flowLoop(){
  currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      Serial.print("8. Water irrigation speed = ");
      Serial.print(l_hour, DEC); // Print litres/hour
      Serial.println();
   }
}
void setupUltrasonic(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void UltrasonicLoop(){
    // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
}
int16_t val_0;
float f;
void solarIntensity(){
  val_0 = ADS.readADC(0);
  f = ADS.toVoltage(1);
  Serial.print("Analog0 = "); Serial.print(val_0); Serial.print(" "); Serial.print(val_0 * f, 3);
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  bmp.begin();
  Wire.begin();
  setupUltrasonic();
  ADS.begin();
  flowStart();
  delayMS = sensor.min_delay / 1000;


}

void loop() {
  // put your main code here, to run repeatedly:
  dht.temperature().getEvent(&event);
  dht.humidity().getEvent(&event);
  temperature = event.temperature;
  temperature = convertDegree();

  humidity = event.relative_humidity; 
  pressure = bmp.readPressure();
  altitude = bmp.readAltitude();
  sealevelpressure = bmp.readSealevelPressure();

  UltrasonicLoop();

  now = myRTC.now();
  // Time-stamp
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  // Print
  Serial.print("1. Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("2. Humidity = ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("3. ATM Pressure = ");
  Serial.print(pressure);
  Serial.println("Pa");
  Serial.print("4. Altitude = ");
  Serial.print(altitude);
  Serial.println("m");
  Serial.print("5. Sea Level Pressure = ");
  Serial.print(sealevelpressure);
  Serial.println("Pa");
  Serial.print("6. Water level = ");
  Serial.print(distance);
  Serial.println("m");
  Serial.print("7. Solar intensity = ");
  solarIntensity();
  flowLoop();
  //Line space
  Serial.println();
  delay(2000);





}
