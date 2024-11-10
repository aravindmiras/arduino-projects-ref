// Define the pin connected to the relay
const int relayPin = 7; // You can change this to the pin you're using

void setup() {
  // Initialize the digital pin as an output
  pinMode(relayPin, OUTPUT);
  // digitalWrite(relayPin, LOW);
}

void loop() {
  // Turn the relay on
  digitalWrite(relayPin, HIGH);
  // delay(1000); // Wait for 1 second (1000 milliseconds)

  // Turn the relay off
  digitalWrite(relayPin, LOW);
  // delay(1000); // Wait for 1 second (1000 milliseconds)
}
