/*
  This code uses the 5V ready Micro-SD Breakout Board+ from Adafruit, the BMP390 Temp & Pressure sensor from Adafruit, and the Geiger SEN0463 from Gravity.
  In order to run this code you will need to download the libraries listed below. 
  Libraries: 
  Adafruit BMP3XX Library - install all https://github.com/adafruit/Adafruit_BMP3XX 
  SD by Arduino, SparkFun https://github.com/arduino-libraries/SD 
  DFRobot_Geiger-main https://github.com/cdjq/DFRobot_Geiger
  Parts of their example code were used throughout creating this code and you will be able to find that on their GitHub Repositories along with their libraries.

  Wiring for the circuit is listed below:
  Power:
  5V -> +
  GND -> -

  BMP390:
  VIN -> +
  GND -> -
  SCL -> A5
  SDA -> A4

  Micro SD:
  5V -> +
  GND -> GND (Digital)
  CLK -> 13
  DO -> 12
  DI -> 11
  CS -> 4

  GEIGER:
  D1 (Black) -> -
  D2 (Red) -> +
  D3 (Green) -> 3
*/


#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <DFRobot_Geiger.h>

#if defined ESP32
#define detect_pin D3
#else
#define detect_pin 3
#endif

#define SEALEVELPRESSURE_HPA (1013.25)

// This is for the SD Card reader
const int CS_PIN = 4;

File myFile;
Adafruit_BMP3XX bmp;
DFRobot_Geiger geiger(detect_pin);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  
  Serial.println(F("Initializing SD card..."));
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  
  if (!SD.begin(CS_PIN)) {
    Serial.println(F("Card failed, or not present"));
    while (1);
  }

  Serial.println(F("Card initialized."));
  Serial.println(F("Adafruit BMP390 and Geiger test"));
  
  // Starts counting with the Geiger counter
  geiger.start();

  // Initialize BMP390 via I2C
  if (!bmp.begin_I2C(0x77)) {
    Serial.println(F("Could not find BMP390!"));
    while (1);
  }

  Serial.println(F("BMP390 found!"));

  // Configure the sensor
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // Open file to write the header
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    Serial.print(F("Writing header to data.csv..."));
    myFile.println(F("Temperature (*C), Pressure (hPa), Approx. Altitude (meters), CPM, nSv/h, uSv/h"));
    myFile.close();
    Serial.println(F("Done!"));
  } else {
    Serial.println(F("Error opening data.csv during setup"));
  }
}

void loop() {
  // Tell the BMP390 sensor to take a reading
  if (!bmp.performReading()) {
    Serial.println(F("Failed to perform BMP390 reading :("));
    delay(2000);
    return; // Skip this loop iteration if the sensor fails
  }

  // Read the values into local variables once
  float temp     = bmp.temperature;
  float pressure = bmp.pressure / 100.0;
  float altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  
  float cpm   = geiger.getCPM();
  float nSvh  = geiger.getnSvh();
  float uSvh  = geiger.getuSvh();

  // Print values cleanly to the Serial Monitor
  Serial.print(F("Data Line -> "));
  Serial.print(temp);     Serial.print(F(", "));
  Serial.print(pressure); Serial.print(F(", "));
  Serial.print(altitude); Serial.print(F(", "));
  Serial.print(cpm);      Serial.print(F(", "));
  Serial.print(nSvh);     Serial.print(F(", "));
  Serial.println(uSvh);

  // Open the file once and log the data
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    myFile.print(temp);     myFile.print(",");
    myFile.print(pressure); myFile.print(",");
    myFile.print(altitude); myFile.print(",");
    myFile.print(cpm);      myFile.print(",");
    myFile.print(nSvh);     myFile.print(",");
    myFile.println(uSvh);   // use println for the last item to create a new row

    myFile.close();         // Automatically flushes and saves data safely
  } else {
    Serial.println(F("Error opening data.csv for loop writing"));
  }

  // Wait 5 seconds (5000 milliseconds) before recording next interval
  delay(5000);
}