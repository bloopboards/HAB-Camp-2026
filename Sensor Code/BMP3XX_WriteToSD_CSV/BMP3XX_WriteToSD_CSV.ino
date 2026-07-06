/*
Libraries: 
Adafruit BMP3XX Library - install all
SD by Arduino, SparkFun
*/

/***************************************************************************
  This is a library for the BMP3XX temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP388 Breakout
  ----> http://www.adafruit.com/products/3966

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13;
#define BMP_MISO 12;
#define BMP_MOSI 11;
#define BMP_CS 4;

const int chipSelect = 4;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

int CS_PIN = 4;
File myFile;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
   if (!SD.begin(CS_PIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  Serial.println("Adafruit BMP388 / BMP390 test");
  

  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("Temperature (*C), Pressure (hPa), Approx. Altitude (meters)");
  myFile.close();
}

void loop() {

  myFile = SD.open("data.csv", FILE_WRITE);
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print(bmp.temperature);
  Serial.print(", ");
  
  myFile.print(bmp.temperature);
  myFile.print(", ");

  Serial.print(bmp.pressure / 100.0);
  Serial.print(", ");

  myFile.print(bmp.pressure / 100.0);
  myFile.print(", ");

  Serial.println(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  myFile.println(bmp.readAltitude(SEALEVELPRESSURE_HPA));

  myFile.close();
  delay(3000);
}
