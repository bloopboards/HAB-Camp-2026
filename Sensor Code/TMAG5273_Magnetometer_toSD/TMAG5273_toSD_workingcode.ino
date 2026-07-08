#include <SD.h>
#include <SPI.h>
#include "SparkFun_TMAG5273_Arduino_Library.h" // Used to send and recieve specific information from our sensor
#include <Wire.h>                              // Used to establish serial communication on the I2C bus

TMAG5273 sensor; // Initialize hall-effect sensor
File myFile;

// I2C default address
uint8_t i2cAddress = TMAG5273_I2C_ADDRESS_INITIAL;

void setup()
{
    delay(1000);
    Wire.begin();
    // Start serial communication at 115200 baud
    Serial.begin(115200);

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

    Serial.print("Initializing SD card...");

    if (!SD.begin(10)) {
        Serial.println("initialization failed!");
        while(1);
    }
    Serial.println("initialization done.");

    myFile = SD.open("data.csv", FILE_WRITE);
    myFile.println("Mag X (mT),Mag Y (mT),Mag Z (mT),Magnitude (mT),Temp (*C)");
    myFile.close();
}

void loop()
{
    // Checks if mag channels are on - turns on in setup
    if (sensor.getMagneticChannel() != 0)
    {
        myFile = SD.open("data.csv", FILE_WRITE);

        sensor.setTemperatureEn(true);

        float magX = sensor.getXData();
        float magY = sensor.getYData();
        float magZ = sensor.getZData();
        float temp = sensor.getTemp();
        
        float magnitude = sqrt(pow(magX,2)+pow(magY,2)+pow(magZ,2));

        Serial.print(magX);
        Serial.print(",");
        Serial.print(magY);
        Serial.print(",");
        Serial.print(magZ);
        Serial.print(",");
        Serial.print(magnitude);
        Serial.print(",");
        Serial.println(temp);
        Serial.println();

        myFile.print(magX);
        myFile.print(",");
        myFile.print(magY);
        myFile.print(",");
        myFile.print(magZ);
        myFile.print(",");
        myFile.print(magnitude);
        myFile.print(",");
        myFile.println(temp);

        myFile.close();
    }
    else
    {
        // If there is an issue, stop the magnetic readings and restart sensor/example
        Serial.println("Mag Channels disabled, stopping..");
        while (1)
            ;
    }

    delay(1000);
}
