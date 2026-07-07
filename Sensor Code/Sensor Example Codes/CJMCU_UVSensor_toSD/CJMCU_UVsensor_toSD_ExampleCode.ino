//Example code prints time elapsed and sensor readings, and writes them to a csv file in an SD card.

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

Serial.println("Time (s),Sensor reading,UV Voltage (V)");
Serial.println("");

myFile = SD.open("data.csv", FILE_WRITE);
myFile.println("Time (s),Sensor reading,UV Voltage (V)");
myFile.close();
}
 
void loop() 
{
  float sensorVoltage; 
  float sensorValue;

  myFile = SD.open("data.csv", FILE_WRITE); 
 
  sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*3.3;
  Serial.print(millis()/1000);
  Serial.print(",");
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(sensorVoltage);

  myFile.print(millis()/1000);
  myFile.print(",");
  myFile.print(sensorValue);
  myFile.print(",");
  myFile.println(sensorVoltage);

  Serial.println();
  myFile.close() ;
  delay(1000); //wait 1 second
}
