// Include the Servo library 
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 3; 
// Create a servo object 
Servo Servo1; 
void setup() { 
   // We need to attach the servo to the used pin number 
   Servo1.attach(servoPin); 
   Servo1.write(0);
   
}
void loop(){ 
  for(int i=0; i<=180;i++){
    Servo1.write(i);
    delay(25);
   }
   for(int i=180; i>=0;i--){
    Servo1.write(i);
    delay(25);
   }
   // Make servo go to 0 degrees 
  //  Servo1.write(0); 
  //  delay(1000); 
  //  // Make servo go to 90 degrees 
  //  Servo1.write(90); 
  //  delay(1000); 
  //  // Make servo go to 180 degrees 
  //  Servo1.write(180); 
  //  delay(1000);
  //  // Make servo go to 180 degrees 
  //  Servo1.write(270); 
  //  delay(1000);
  //  // Make servo go to 180 degrees 
  //  Servo1.write(360); 
  //  delay(1000); 
}