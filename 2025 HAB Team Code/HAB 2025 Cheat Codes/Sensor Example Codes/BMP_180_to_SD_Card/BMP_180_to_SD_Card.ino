/*
  SD card read/write

  Documentation: https://www.arduino.cc/reference/en/libraries/sd/

  This example shows how to write data from a BMP 180 sensor to an SD card file
 ** CS - pin 53 on Arudino Mega


*/

#include <SPI.h>
#include <SD.h>
#include "Wire.h"
#include "Adafruit_BMP085.h"

Adafruit_BMP085 bmp;
File myFile;

void setup() {
  // Open serial communications and wait for port to open: (magnifying glass button in upper right corner)
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

//CS Pin on Adafruit SD breakout board needs to be connected to pin 53, or change this number in the next line
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  bmp.begin();  

}

void loop() {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write data to it:
  if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println("Temperature = ");
    myFile.print(bmp.readTemperature());
    myFile.println(" *C");
 
    myFile.print("Pressure = ");
    myFile.print(bmp.readPressure());
    myFile.println(" Pa");

    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    myFile.print("Altitude = ");
    myFile.print(bmp.readAltitude());
    myFile.println(" meters");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
    //now print data to serial monitor
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
    delay(500); //wait half a second
 
}
