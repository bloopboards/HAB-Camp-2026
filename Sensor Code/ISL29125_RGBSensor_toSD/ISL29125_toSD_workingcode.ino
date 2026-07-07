#include <SD.h>
#include <SPI.h> 
#include <Wire.h>
#include "SparkFunISL29125.h"

SFE_ISL29125 RGB_sensor;
File myFile;

void setup()
{
  Serial.begin(115200);

  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while(1);
  }
  Serial.println("initialization done.");

  Serial.println("Red,Green,Blue");

  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("Red,Green,Blue");
  myFile.close();
}

// Read sensor values for each color and print them to serial monitor
void loop()
{

  myFile = SD.open("data.csv", FILE_WRITE);

  // Read sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();
  
  // Print out readings, change HEX to DEC if you prefer decimal output
  Serial.print(red,DEC);
  Serial.print(",");
  Serial.print(green,DEC);
  Serial.print(",");
  Serial.print(blue,DEC);
  Serial.println();

  myFile.print(red,DEC);
  myFile.print(",");
  myFile.print(green,DEC);
  myFile.print(",");
  myFile.println(blue,DEC);

  myFile.close();
  delay(2000);
}
