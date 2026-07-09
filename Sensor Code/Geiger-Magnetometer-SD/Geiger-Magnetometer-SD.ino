//this is /Users/hazypaze/Documents/Arduino/Magnetometer-RM310-WritetoSD/Magnetometer-RM310-WritetoSD.inoforked from hnguy169
//https://github.com/hnguy169/RM3100-Arduino

//TROUBLESHOOTING --- THIS IS THE MAGNETOMETER OF SORROW AND DESPAIR
// - TRY UPLOADING TO THE ARDUINO, UNPLUGGING THE ARDUINO FROM THE COMPTUER, THEN PLUGGING IT BACK IN
// - DOUBLE CHECK BAUD RATE

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <DFRobotGieger.h>

//pin definitions
#define PIN_DRDY 1 //Set pin D9 to be the Data Ready Pin
#define RM_CS 10 //Chip Select (SS) is set to Pin 10
#define SD_CS 9 //Chip Select is set to pin 9
#define LED_Pin 8 //SD Failed warning LED is set to pin 8
#define Gei_Pen 7 //This is the pin that the geiger counter is connected to

//internal register values without the R/W bit
#define RM3100_REVID_REG 0x36 // Hexadecimal address for the Revid internal register
#define RM3100_POLL_REG 0x00 // Hexadecimal address for the Poll internal register
#define RM3100_CMM_REG 0x01 // Hexadecimal address for the Continuous Measurement Mode internal register
#define RM3100_STATUS_REG 0x34 // Hexadecimal address for the Status internal register
#define RM3100_CCX1_REG 0x04 // Hexadecimal address for the Cycle Count X1 internal register
#define RM3100_CCX0_REG 0x05 // Hexadecimal address for the Cycle Count X0 internal register

//options
#define initialCC 200 // Set the cycle count
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode
#define useDRDYPin 0 //0 = not using DRDYPin ; 1 = using DRDYPin to wait for data
#define usingLED 1 //0 = not usng warning LED ; 1= using warning LED
#define measurementGap 10 //How long to wait between measurements, in seconds

uint8_t revid;
uint16_t cycleCount;
float gain;
double timeS = 0.0;
int columns = 8; //Number of Columns of Data, if you add more headers you MUST increase this value
const char* header[] = {"Time Elapsed (s),"," X component (uT),"," Y component (uT),"," Z component (uT),", " Magnitude(uT),"," nSvh,"," uSvh,"," CPM" };
bool titles = true;

void setup() {
  pinMode(PIN_DRDY, INPUT);  
  pinMode(RM_CS, OUTPUT);
  digitalWrite(RM_CS, HIGH);
  SPI.begin(); // Initiate the SPI library
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  
  Serial.begin(9600); //set baud rate to 9600
  SD.begin(SD_CS);
  geiger.start();
  delay(100);
  incrementTime(100);
  revid = readReg(RM3100_REVID_REG);
  
  Serial.print("REVID ID = 0x"); //REVID ID should be 0x22
  Serial.println(revid, HEX);

  changeCycleCount(initialCC); //change the cycle count; default = 200 (lower cycle count = higher data rates but lower resolution)

  cycleCount = readReg(RM3100_CCX1_REG);
  cycleCount = (cycleCount << 8) | readReg(RM3100_CCX0_REG);

  Serial.print("Cycle Counts = "); //display cycle count
  Serial.println(cycleCount);

  gain = (0.3671 * (float)cycleCount) + 1.5; //linear equation to calculate the gain from cycle count

  Serial.print("Gain = "); //display gain; default gain should be around 75 for the default cycle count of 200
  Serial.println(gain);

  if (singleMode){
    //set up single measurement mode
    writeReg(RM3100_CMM_REG, 0);
    writeReg(RM3100_POLL_REG, 0x70);
  }
  else{
    // Enable transmission to take continuous measurement with Alarm functions off
    writeReg(RM3100_CMM_REG, 0x79);
  }
  
}

void loop() {
  long x = 0;
  long y = 0;
  long z = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;

  //wait until data is ready using 1 of two methods (chosen in options at top of code)
  if(useDRDYPin){ 
    while(digitalRead(PIN_DRDY) == LOW); //check RDRY pin
  }
  else{
    while((readReg(RM3100_STATUS_REG) & 0x80) != 0x80); //read internal status register
  }
  
  //read measurements
  digitalWrite(RM_CS, LOW);
  delay(100);
  incrementTime(100);
  SPI.transfer(0xA4);
  x2 = SPI.transfer(0xA5);
  x1 = SPI.transfer(0xA6);
  x0 = SPI.transfer(0xA7);
  
  y2 = SPI.transfer(0xA8);
  y1 = SPI.transfer(0xA9);
  y0 = SPI.transfer(0xAA);
  
  z2 = SPI.transfer(0xAB);
  z1 = SPI.transfer(0xAC);
  z0 = SPI.transfer(0);
  
  digitalWrite(RM_CS, HIGH);

  double nSvh = (geiger.getnSvh())*(double);
  double uSvh = (geiger.getuSvh())*(double);
  double CPM = (geiger.getCPM())*(double);

  //special bit manipulation since there is not a 24 bit signed int data type
  if (x2 & 0x80){
      x = 0xFF;
  }
  if (y2 & 0x80){
      y = 0xFF;
  }
  if (z2 & 0x80){
      z = 0xFF;
  }

  //format results into single 32 bit signed value
  x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  //calculate magnitude of results
  double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  // Convert to uT
  double xF = (double)(x)/gain;
  double yF = (double)(x)/gain;
  double zF = (double)(x)/gain;

  //display results
  Serial.print("Data in counts:");
  Serial.print("   X:");
  Serial.print(x);
  Serial.print("   Y:");
  Serial.print(y);
  Serial.print("   Z:");
  Serial.println(z);



  Serial.print("Data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print(xF);
  Serial.print("   Y:");
  Serial.print(xY);
  Serial.print("   Z:");
  Serial.print(xZ);
 

  //Magnitude should be around 45 uT (+/- 15 uT)
  Serial.print("Magnitude(uT):");
  Serial.println(uT);
  Serial.println();

  serial.print("Geiger Counter Data")
  Serial.print("   nSvh:");
  Serial.print()
  double results[] = {timeS, xF, yF, zF, uT, nSvh, uSvh, CPM};

  File sensorData = SD.open("rmdata.csv", FILE_WRITE);
  if(sensorData){
    if(titles){
      for(int i = 0; i < columns; i++){
        if(i == (columns - 1)){
          Serial.println(header[i]);
          sensorData.println(header[i]); 
          sensorData.close();
          titles = false;
        }
        else{
          Serial.print(header[i]);
          sensorData.print(header[i]); 
        }
      }
    }
    else{
      for(int i = 0; i < columns; i++){
        if(i == (columns - 1)){
          Serial.println(results[i]);
          sensorData.println(results[i]); 
          sensorData.close();
        }
        else{
          Serial.print(results[i]);
          sensorData.print(results[i]);
           
        }
      }
    }
  }
  else{
    if(usingLED == 1){
      Serial.println("SD CARD FAILED");
      for(int i = 0; i < 5; i++){
        digitalWrite(LED_Pin, HIGH);
        delay(500);
        digitalWrite(LED_Pin, LOW);
        delay(500);
        incrementTime(1000);
        sensorData.close();
      }
    }
    else{
      Serial.println("SD CARD FAILED");
      sensorData.close();
    }
  }
  delay(measurementGap * 1000);
  incrementTime(measurementGap * 1000);
}

//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t readReg(uint8_t addr){
  uint8_t data = 0;
  digitalWrite(RM_CS, LOW);
  delay(100);
  SPI.transfer(addr | 0x80); //OR with 0x80 to make first bit(read/write bit) high for read
  data = SPI.transfer(0);
  digitalWrite(RM_CS, HIGH);
  incrementTime(100);
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void writeReg(uint8_t addr, uint8_t data){
  digitalWrite(RM_CS, LOW); 
  delay(100);
  incrementTime(100);
  SPI.transfer(addr & 0x7F); //AND with 0x7F to make first bit(read/write bit) low for write
  SPI.transfer(data);
  digitalWrite(RM_CS, HIGH);
}

//newCC is the new cycle count value (16 bits) to change the data acquisition
void changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
    
  digitalWrite(RM_CS, LOW); 
  delay(100);
  incrementTime(100);
  SPI.transfer(RM3100_CCX1_REG & 0x7F); //AND with 0x7F to make first bit(read/write bit) low for write
  SPI.transfer(CCMSB);  //write new cycle count to ccx1
  SPI.transfer(CCLSB);  //write new cycle count to ccx0
  SPI.transfer(CCMSB);  //write new cycle count to ccy1
  SPI.transfer(CCLSB);  //write new cycle count to ccy0
  SPI.transfer(CCMSB);  //write new cycle count to ccz1
  SPI.transfer(CCLSB);  //write new cycle count to ccz0
  digitalWrite(RM_CS, HIGH);
}

void incrementTime(int timeMS){
  timeS += (timeMS/1000);
}