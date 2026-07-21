#include <Wire.h>
#include <Adafruit_BMP085.h>
int heaterPin = 3;
int temp = 29;

Adafruit_BMP085 bmp;

void setup() {
Serial.begin(115200);
bmp.begin();
Wire.begin();
pinMode(heaterPin, OUTPUT);
digitalWrite(heaterPin, LOW);

delay(2000);

Serial.print("starting loop.");
}

void loop() {

float celsius = bmp.readTemperature();

 Serial.print("Celsius: ");
 Serial.println(celsius);
 if (celsius > temp) {
  digitalWrite(heaterPin, LOW);
  delay(30000); }
 else if (celsius < temp) {
  digitalWrite(heaterPin, HIGH);
  delay(30000); }
  else {
    delay(1000); 
}
}