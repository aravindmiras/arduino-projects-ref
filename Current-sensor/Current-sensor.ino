#include <Arduino.h>

// Define the analog pin that the ACS712 current sensor module is connected to
const int CURRENT_SENSOR_PIN = A0;

// Define the maximum current that the ACS712 current sensor module can measure
const float MAX_CURRENT = 20.0; // Amps

void setup() {
  // Set the analog pin to input mode
  pinMode(CURRENT_SENSOR_PIN, INPUT);
}

void loop() {
  // Read the voltage from the ACS712 current sensor module
  float voltage = analogRead(CURRENT_SENSOR_PIN);

  // Convert the voltage to current
  float current = (voltage / 1024.0) * MAX_CURRENT;

  // Print the current to the serial monitor
  Serial.print("Current: ");
  Serial.println(current);
  delay(1000);
}