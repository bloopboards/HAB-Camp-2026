#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <DFRobot_Geiger.h>
#include "SparkFun_TMAG5273_Arduino_Library.h"


#define detect_pin A1
// const int i2caddress = 0x27;
uint8_t i2cAddress = 0x22; 

#define SEALEVELPRESSURE_HPA (1013.25)

// This is for the SD Card reader
const int CS_PIN = 53;
#define CS_pin = 53

File myFile;
Adafruit_BMP3XX bmp;
TMAG5273 sensor;
DFRobot_Geiger geiger(detect_pin);


void setup()
{
  delay(1000);
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) { ; }
  
  Serial.println(F("Initializing SD card..."));
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);
  
  if (!SD.begin(CS_PIN)) {
    Serial.println(F("Card failed, or not present"));
    while (1);
  }
  
if (sensor.begin(i2cAddress, Wire) == 1)
 {
Serial.println("Begin");
 }
 else // Otherwise, infinite loop
 {
 Serial.println("Device failed to setup - Freezing code.");
 while (1)
 ; // Runs forever
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
    myFile.println("Mag X (mT),Mag Y (mT),Mag Z (mT),Magnitude (mT)");
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
 (sensor.getMagneticChannel() != 0);
 myFile = SD.open("data.csv", FILE_WRITE);

 sensor.setTemperatureEn(true);

 float magX = sensor.getXData();
 float magY = sensor.getYData();
 float magZ = sensor.getZData();
        
 float magnitude = sqrt(pow(magX,2)+pow(magY,2)+pow(magZ,2));

 Serial.print(magX);
 Serial.print(",");
 Serial.print(magY);
 Serial.print(",");
 Serial.print(magZ);
 Serial.print(",");
 Serial.print(magnitude);
 Serial.println();

 myFile.print(magX);
 myFile.print(",");
 myFile.print(magY);
 myFile.print(",");
 myFile.print(magZ);
 myFile.print(",");
 myFile.println(magnitude);

 myFile.close();
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
    Serial.println("Mag Channels disabled, stopping..");
   while (1);

  float sensorVoltage; 
  float sensorValue;
 
  sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*3.3;
  Serial.print("sensor reading = ");
  Serial.print(sensorValue);
  Serial.println("");
  Serial.print("sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  delay(1000);
  
  }

  // Wait 5 seconds (5000 milliseconds) before recording next interval
  delay(3000);
}
