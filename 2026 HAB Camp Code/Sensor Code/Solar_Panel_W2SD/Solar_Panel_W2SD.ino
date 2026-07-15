#include <Wire.h>
#include <SD.h>

float measureA0;
float voltage;
float power;
float solarpower;
float intensity;
const float emvado = 0.000007875; // original project guy was greek, idk what this means
const float R = 10000;
const float ConvEftoisekato=17; // same here

File myFile;

int CS_PIN = 10;

void setup() {
 Serial.begin(9600);
 
 Serial.print("Initializing SD card...");

  pinMode(10, OUTPUT);
  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

   myFile = SD.open("data.csv", FILE_WRITE);
   myFile.print("Voltage (volts), Power (mW), Solar Power (mW), Intensity (W/m^2)");
   myFile.close();

}

void loop() {
  measureA0 = analogRead(A0);

  voltage = (5 * measureA0) / 1023;
  power = voltage * voltage / R;
  solarpower = power * 100 / 17;
  intensity = solarpower / emvado;

  Serial.print ("Voltage (Volts): ");
  Serial.println (voltage);

  Serial.print ("Power 1000 (mW): ");
  Serial.println(power * 1000);
  
  Serial.print ("Solar Power 1000 (mW): ");
  Serial.println(solarpower * 1000);

  Serial.print ("Intensity (W/m^2): ");
  Serial.println(intensity);
  Serial.println();


  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.print(voltage);
  myFile.print(", ");

  myFile.print(power);
  myFile.print(", ");

  myFile.print(solarpower);
  myFile.print(", ");

  myFile.print(intensity);

  myFile.println();
  myFile.close();

  delay(2000);
}