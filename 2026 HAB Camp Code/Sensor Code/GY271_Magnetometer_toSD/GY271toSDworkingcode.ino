#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;
File myFile;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.setMode(0x01,0x00,0x00,0x00);

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while(1);
  }
Serial.println("initialization done.");

Serial.println("Magnetic field X (uT), Magnetic field Y (uT), Magnetic field Z (uT), Heading (°), Magnitude (uT)");

myFile = SD.open("data.csv", FILE_WRITE);
myFile.println("Magnetic field X (uT), Magnetic field Y (uT), Magnetic field Z (uT), Heading (°), Magnitude (uT)");
myFile.close();
}

void loop() {

  myFile = SD.open("data.csv", FILE_WRITE);

  compass.read();
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  int heading = compass.getAzimuth();

float magnitude = sqrt(pow(x,2)+pow(y,2)+pow(z,2));

  Serial.print(x/120); 
  Serial.print(",");
  Serial.print(y/120); 
  Serial.print(",");
  Serial.print(z/120); 
  Serial.print(",");
  Serial.print(heading);
  Serial.print(",");
  Serial.println(magnitude/120); 
  Serial.println();

  myFile.print(x/120); 
  myFile.print(",");
  myFile.print(y/120); 
  myFile.print(",");
  myFile.print(z/120); 
  myFile.print(",");
  myFile.print(heading);
  myFile.print(",");
  myFile.println(magnitude/120); 

  myFile.close();
  delay(1000);
}
