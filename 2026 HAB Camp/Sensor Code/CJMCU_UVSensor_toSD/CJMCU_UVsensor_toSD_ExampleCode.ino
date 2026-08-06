#include <SPI.h>
#include <SD.h>

File myFile;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while(1);
  }
Serial.println("initialization done.");

Serial.println("time elapsed, sensor reading, sensor voltage");
Serial.println("");
}
 
void loop() 
{
  float sensorVoltage; 
  float sensorValue;

  myFile = SD.open("data.csv", FILE_WRITE); 
 
  sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*3.3;
  Serial.print(millis()/1000);
  Serial.print("s,");
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.print(sensorVoltage);
  Serial.println(" V");

  myFile.print(millis()/1000);
  myFile.print("s,");
  myFile.print(sensorValue);
  myFile.print(",");
  myFile.print(sensorVoltage);
  myFile.println(" V");

  Serial.println();
  myFile.close() ;
  delay(1000); //wait 1 second
}