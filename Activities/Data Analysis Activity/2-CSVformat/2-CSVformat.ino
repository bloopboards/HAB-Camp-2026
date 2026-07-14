/*
Lets learn how to format data as a CSV file. This will enable you to analyze it using a spreadsheet program.
CSV stands for Comma Separated Value. Your data should look like this:

column A, column B, column C, ect...
value A-1, value B-2, value C-1, ect..
value A-2, value B-2, value C-2, ect..
value A-3, value B-3, value C-3, ect..

column A should be time. all columns after that should be sensor data.
*/





/*
We are going to simulate storing sensor data.
Use Serial.print and Serial.println functions to output the data to the variables above to the Serial moniter in csv format.
Your data should look like this:

Seconds, Reading
1, 1.2
2, 2.4
3, 4.1
ect..

Remember: Use ctrl + shift + M to open the serial monitor

*/


int seconds = 0;
float sensorReading = 0;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  delay(3000);
  Serial.println();

  //Your code goes here:

}

void loop() {
  
  //Simulated data collection
  // The millis() function returns the number of milliseconds since the program began
  seconds = millis()/1000;
  sensorReading += 1 + random(0,100)/100.0;

  //Your code goes here:
  

  //------------------
  delay(1000);
}
