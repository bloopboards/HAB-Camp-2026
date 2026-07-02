  /*
  GUVA-S12SD-UV-Module
  Made on 03 Feb 2020
  by Amir Mohammad Shojaee @ Electropeak
  Home

  Source: https://electropeak.com/learn/interfacing-guva-s12sd-uv-light-sensor-module-with-arduino/
*/

void setup() 
{
  Serial.begin(9600);
}
 
void loop() 
{
  float sensorVoltage; 
  float sensorValue;
 
  sensorValue = analogRead(A0); // connect SIG pin to A0 on circuit board
  sensorVoltage = sensorValue/1024*5.0;
  Serial.print("sensor reading = ");
  Serial.print(sensorValue);
  Serial.print("        sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  delay(1000);
}
