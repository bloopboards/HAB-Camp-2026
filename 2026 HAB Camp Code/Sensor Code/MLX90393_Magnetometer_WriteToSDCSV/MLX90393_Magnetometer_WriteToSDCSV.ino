/*
  MLX90393 Magnetometer Example Code
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 6th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Read the mag fields on three XYZ axis

  Hardware Connections (Breakoutboard to Arduino):
  3.3V = 3.3V
  GND = GND
  SDA = A4
  SCL = A5

  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>
#include <MLX90393.h> //From https://github.com/tedyapo/arduino-MLX90393 by Theodore Yapo
#include <SD.h>

File myFile;

MLX90393 mlx;
MLX90393::txyz data; //Create a structure, called data, of four floats (t, x, y, and z)
float magnitude;

int CS_PIN = 10;

void setup()
{
  Serial.begin(9600);
  Serial.println("MLX90393 Read Example");
  Wire.begin();
  mlx.begin(); //Assumes I2C jumpers are GND. No DRDY pin used.
  mlx.setOverSampling(0);
  mlx.setDigitalFiltering(7);

  Serial.print("Initializing SD card...");

  pinMode(10, OUTPUT);
  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

   myFile = SD.open("data.csv", FILE_WRITE);
   myFile.print("Magnitude (microTeslas), X (microTeslas), Y (microTeslas), Z (microTeslas)");
   myFile.close();
}

void loop()
{
  delay(500);
  mlx.readData(data); //Read the values from the sensor

  magnitude = sqrt(data.x * data.x + data.y * data.y + data.z * data.z);

  Serial.print("magnitude: ");
  Serial.println(magnitude);
  Serial.print("magX: ");
  Serial.println(data.x);
  Serial.print("magY: ");
  Serial.println(data.y);
  Serial.print("magZ: ");
  Serial.println(data.z);
  Serial.println();

  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.print(magnitude);
  myFile.print(", ");

  myFile.print(data.x);
  myFile.print(", ");

  myFile.print(data.y);
  myFile.print(", ");

  myFile.print(data.z);

  myFile.println();
  myFile.close();

}
