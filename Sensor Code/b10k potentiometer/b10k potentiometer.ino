const int LED_PIN = 3;    // PWM pin for LED
const int POT_PIN = A0;   // analog pin for potentiometer
#include <SPI.h>
#include <SD.h>

File myFile;

// change this to match your SD shield or module;
const int chipSelect = 10;

void setup() {
  pinMode(LED_PIN, OUTPUT); // set LED pin as output
  Serial.begin(9600);       // optional: for Serial Monitor

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("(Brightness, Voltage)");
  // add ", data2, data3" into it for other data, should print: '(Brightness, data2, data3)'
  myFile.close();
}

void loop() {
  // Read the potentiometer value (0–1023)
  int analogValue = analogRead(POT_PIN);

  // Map the analog value to brightness (0–255)
  int brightness = map(analogValue, 0, 1023, 0, 255);

  // Set the LED brightness using PWM
  analogWrite(LED_PIN, brightness);

  // Optional: print the value to Serial Monitor
  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print("Brightness: ");
  Serial.println(brightness);

  myFile = SD.open("data.csv", FILE_WRITE);

  if (myFile) {
    myFile.print(analogValue);
    myFile.print(",");
    myFile.print(brightness);
    // myFile.print(", ");
    // ^ add back when more data (data2, data3)
    myFile.println();
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.csv");
  }

  delay(500);
}
