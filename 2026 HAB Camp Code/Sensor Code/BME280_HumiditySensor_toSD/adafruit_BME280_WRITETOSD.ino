/*
 * Complete Project Details https://randomnerdtutorials.com
*/

#include "Wire.h"
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include<SD.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

File myFile;
Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

   Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while(1);
  }
Serial.println("initialization done.");
  Serial.println("-- Default Test --");

 if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

myFile = SD.open("data.csv", FILE_WRITE);
myFile.println("Temperature (*C), Pressure (Pa), Approx. Altitude (m), Humidity (%)");
myFile.close();

}


void loop() { 

  myFile = SD.open("data.csv", FILE_WRITE);

  printValues();
  delay(delayTime);
}


void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  
  Serial.println();

  myFile.print(bme.readTemperature());
  myFile.print(", ");
  myFile.print(bme.readPressure() / 100.0F);
  myFile.print(", ");
  myFile.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  myFile.print(", ");
  myFile.println(bme.readHumidity());
  
  myFile.close();

  delay(1000);
}