#include <Wire.h>

#include <Bonezegei_DS3231.h>

#include "ADS1X15.h"

#include <Adafruit_BMP085.h>

#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define DHTPIN 6
// 74HC4067 selection pins
uint32_t delayMS;
sensor_t sensor;
sensors_event_t event;

DHT_Unified dht(DHTPIN, DHTTYPE);

const int s0 = 2;
const int s1 = 4;

Adafruit_BMP085 bmp;

Bonezegei_DS3231 rtc(0x68);

ADS1115 ADS(0x48);

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


float pressure, altitude, sealevelpressure, temperature, humidity;

void selectChannel(int channel) {
  digitalWrite(s0, channel & 0x01);
  digitalWrite(s1, (channel >> 1) & 0x01);
}

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

// bool tr_stat = true;
// bool *tr_pt = &tr_stat;
void solarIntensity(){
  val_0 = ADS.readADC(0);
  f = ADS.toVoltage(1);
  Serial.print("Analog0 = "); Serial.print(val_0); Serial.print(" "); Serial.print(val_0 * f, 3);
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  setupUltrasonic();
  flowStart();
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);

  selectChannel(0);
  rtc.begin();

  selectChannel(1);
  bmp.begin();

  selectChannel(2);
  ADS.begin();

  delayMS = sensor.min_delay / 1000;

}

void loop() {
  // put your main code here, to run repeatedly:
  // put your main code here, to run repeatedly:
  dht.temperature().getEvent(&event);
  dht.humidity().getEvent(&event);
  temperature = event.temperature;
  temperature = convertDegree();

  UltrasonicLoop();

  selectChannel(0);
  delay(1000);
  // Time-stamp
if(rtc.getTime()){
  Serial.print(rtc.getHour());
  Serial.print(":");
  Serial.print(rtc.getMinute());
  Serial.print(":");
  Serial.print(rtc.getSeconds());
  Serial.print(" ");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getDate());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.println();
}
 
  selectChannel(1);
  pressure = bmp.readPressure();
  altitude = bmp.readAltitude();
  sealevelpressure = bmp.readSealevelPressure();
  
  
  selectChannel(2);
  delay(1000);
  solarIntensity();

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
  flowLoop();

}
