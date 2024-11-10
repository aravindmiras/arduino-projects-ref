// defining the pins
const int trigPinHorizontal = 7;
const int echoPinHorizontal = 8;
const int trigPin1 = 9;
const int echoPin1 = 10;
const int ledPin = 11;
const int ledPinHorizontal = 12;
// defining variables
long duration;
long durationHorizontal;
int distanceVertical;
int distanceHorizontal;
void setup() {
pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
pinMode(trigPinHorizontal,OUTPUT);
pinMode(echoPinHorizontal,INPUT);
pinMode(ledPin, OUTPUT);
pinMode(ledPinHorizontal,OUTPUT);
Serial.begin(9600); // Starts the serial communication
}
void loop() {
// Clears the trigPin
digitalWrite(trigPin1, LOW);
delayMicroseconds(2);
digitalWrite(trigPinHorizontal,LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin1, HIGH);
delayMicroseconds(10);
digitalWrite(trigPinHorizontal,HIGH);
delayMicroseconds(10);
digitalWrite(trigPin1, LOW);
digitalWrite(trigPinHorizontal,LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin1, HIGH);
durationHorizontal = pulseIn(echoPinHorizontal,HIGH);
// Calculating the distance
distanceVertical= duration*0.034/2;
distanceHorizontal = durationHorizontal*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distanceVertical);
Serial.print("Distance Horizontal: ");
Serial.println(distanceHorizontal);
 if(distanceVertical <= 150){
     digitalWrite(ledPin, HIGH);
     delay(5000);
   }else{
     digitalWrite(ledPin,LOW);
  }
  // if(distanceHorizontal <=50){
  //   digitalWrite(ledPinHorizontal,HIGH);
  //   delay(5000);
  // }else{
  //   digitalWrite(ledPinHorizontal,LOW);
  // }
delay(1000);
}