#include "Wire.h"
#include "Adafruit_BMP085.h"

// Documentation: https://learn.adafruit.com/bmp085/using-the-bmp085
 
Adafruit_BMP085 bmp;
 
void setup() {
  Serial.begin(9600);
  bmp.begin();  
}
 
void loop() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
 
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
 
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");
 
    Serial.println();
    delay(500);
}
