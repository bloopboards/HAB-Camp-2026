 #include <Arduino.h>
 #include <SparkFun_AS7331.h>
 #include <Wire.h>

 SfeAS7331ArdI2C myUVSensor;

 void setup()
 {
     Serial.begin(115200);
     while (!Serial)
     {
         delay(100);
     };
     Serial.println("AS7331 UV A/B/C Command (One-shot) mode Example.");

     Wire.begin();

     // Initialize sensor and run default setup.
     if (myUVSensor.begin() == false)
     {
         Serial.println("Sensor failed to begin. Please check your wiring!");
         Serial.println("Halting...");
         while (1)
             ;
     }

     Serial.println("Sensor began.");

     // Set measurement mode and change device operating mode to measure.
     if (myUVSensor.prepareMeasurement(MEAS_MODE_CMD) == false)
     {
         Serial.println("Sensor did not get set properly.");
         Serial.println("Halting...");
         while (1)
             ;
     }

     Serial.println("Set mode to command.");
 }

 void loop()
 {

     // Send a start measurement command.
     if (ksfTkErrOk != myUVSensor.setStartState(true))
         Serial.println("Error starting reading!");

     // Wait for a bit longer than the conversion time.
     delay(2 + myUVSensor.getConversionTimeMillis());

     // Read UV values.
     if (ksfTkErrOk != myUVSensor.readAllUV())
         Serial.println("Error reading UV.");

     Serial.print("UVA:");
     Serial.print(myUVSensor.getUVA());
     Serial.print(" UVB:");
     Serial.print(myUVSensor.getUVB());
     Serial.print(" UVC:");
     Serial.println(myUVSensor.getUVC());

     delay(2000);
 }