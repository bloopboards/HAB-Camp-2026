#include <Wire.h>
#include <SD.h>

float measureA0;
float voltage;
float power;
float solarpower;
float intensity;

const float area = 0.000007875; // area of sensor in m^2
const float R = 10000;
const float ConvEffToHundredth = 17; // solar panel efficiency (17%)

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
   myFile.println("Voltage (V), Power (mW), Solar Power (mW), Intensity (W/m^2)");
   myFile.close();

}

void loop() {
  measureA0 = analogRead(A0);

  voltage = (5 * measureA0) / 1023;
  power = (voltage * voltage) / R;
  solarpower = (power * 100) / ConvEffToHundredth;
  intensity = solarpower / area;

  Serial.print ("Voltage (Volts): ");
  Serial.println (voltage);

  Serial.print ("Power (mW): ");
  Serial.println(power * 1000);
  
  Serial.print ("Solar Power (mW): ");
  Serial.println(solarpower * 1000);

  Serial.print ("Intensity (W/m^2): ");
  Serial.println(intensity);
  Serial.println();


  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.print(voltage);
  myFile.print(", ");

  myFile.print(power * 1000);
  myFile.print(", ");

  myFile.print(solarpower * 1000);
  myFile.print(", ");

  myFile.print(intensity);

  myFile.println();
  myFile.close();

  delay(2000);
}
