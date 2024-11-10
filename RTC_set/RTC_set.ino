#include <DS3231.h>
#include <Wire.h>

DS3231 myRTC;

// 74HC4067 selection pins
const int s0 = 4;
const int s1 = 5;


byte year=24;
byte month=8;
byte date=28;
byte dOW=4;
byte hour=19;
byte minute=32;
byte second=0;
void selectChannel(int channel) {
  digitalWrite(s0, channel & 0x01);
  digitalWrite(s1, (channel >> 1) & 0x01);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);

  selectChannel(0);
    // Start the I2C interface
  Wire.begin();
  myRTC.setClockMode(false);  // set to 24h
        //setClockMode(true); // set to 12h
        
  myRTC.setYear(year);
  myRTC.setMonth(month);
  myRTC.setDate(date);
  myRTC.setDoW(dOW);
  myRTC.setHour(hour);
  myRTC.setMinute(minute);
  myRTC.setSecond(second);

}

void loop() {
  // put your main code here, to run repeatedly:

}
