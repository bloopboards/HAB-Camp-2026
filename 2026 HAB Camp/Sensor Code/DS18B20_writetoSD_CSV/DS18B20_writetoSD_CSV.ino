/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
  Based on the Dallas Temperature Library example
*********/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

const int chipSelect = 53;
int CS_PIN = 53;
File myFile;

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // Start serial communication for debugging purposes
  Serial.begin(9600);
  // Start up the library
  while (!Serial);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
   if (!SD.begin(CS_PIN)) {
    Serial.println("Card failed, or not present");
    // SD card setting up, don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  sensors.begin();

   myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("Temperature (*C), Temperature (*F)");
  myFile.close();
}

void loop(void){ 
  //Write to SD card
   myFile = SD.open("data.csv", FILE_WRITE);
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensors.getTempFByIndex(0));

  myFile.print(sensors.getTempCByIndex(0));
  myFile.print(", ");
  myFile.println(sensors.getTempFByIndex(0));

  myFile.close();
  delay(1000);
}