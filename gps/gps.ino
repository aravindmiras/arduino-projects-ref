#include <SoftwareSerial.h>
#include <TinyGPSPlus.h> 

static const int RXPin = 3, TXPin = 4; static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(4,3);
void setup() { 
  Serial.begin(115200); 
  ss.begin(GPSBaud); 
  Serial.println(F("DeviceExample.ino")); // Printing information for demonstration and version of TinyGPSPlus library // by Mikal Hart 
}

void loop() { 
  while (ss.available() > 0) 
{ if (gps.encode(ss.read())) { 
  displayInfo(); 
}
 }
 if (millis() > 5000 && gps.charsProcessed() < 10) 
{
   Serial.println(F("No GPS detected: check wiring.")); 
while(true); 
}
 }

void displayInfo() { 
// Displaying Google Maps link with latitude and longitude information if GPS location is valid 

if (gps.location.isValid()) { 

Serial.print("http://www.google.com/maps/place/"); 
Serial.print(gps.location.lat(), 6); 
Serial.print(F(",")); 
Serial.println(gps.location.lng(), 6); } else { Serial.print(F("INVALID"));
 } // Displaying date and time information if valid 

if (gps.date.isValid()) { 

// Displaying date in MM/DD/YYYY format // If the date is invalid, it prints "INVALID" 

} else 

{ Serial.print(F("INVALID")); } // Displaying time in HH:MM:SS format if valid // If the time is invalid, it prints "INVALID" 

}