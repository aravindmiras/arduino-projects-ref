#include<OneWire.h>
#include<DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures();

  Serial.print("Temperature in celsius:");
  Serial.println(sensors.getTempCByIndex(0));

  Serial.print("Temperature in fahrenheit:");
  Serial.println(sensors.getTempFByIndex(0));

}
