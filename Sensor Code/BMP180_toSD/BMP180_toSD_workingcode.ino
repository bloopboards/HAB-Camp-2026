#include "Wire.h"
#include "Adafruit_BMP085.h"
#include <SD.h>
#include <SPI.h>

// Documentation: https://learn.adafruit.com/bmp085/using-the-bmp085
 
Adafruit_BMP085 bmp;
File myFile;
 
void setup() {
  Serial.begin(9600);
  bmp.begin();  

   Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while(1);
  }
Serial.println("initialization done.");

myFile = SD.open("data.csv", FILE_WRITE);
myFile.println("Temperature (*C),Pressure (Pa),Altitude (m)");
myFile.close();
}
 
void loop() {

  myFile = SD.open("data.csv", FILE_WRITE);

    Serial.print(bmp.readTemperature());
    Serial.print(",");
    Serial.print(bmp.readPressure());
    Serial.print(","); 
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.println(bmp.readAltitude());
    Serial.println();

    myFile.print(bmp.readTemperature());
    myFile.print(",");
    myFile.print(bmp.readPressure());
    myFile.print(","); 
    myFile.println(bmp.readAltitude());
    myFile.println();

    myFile.close();

    delay(1000);
}
