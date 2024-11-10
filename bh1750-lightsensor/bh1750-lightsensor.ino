
#include <BH1750.h>
#include <Wire.h>

// VCC - 3V3 or 5V, SCL - A5, SDA - A4

BH1750 lightMeter(0x23);  // 0x5C or 0x23

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }

}
