
  /*
  Knowing how to format your data is very important, as failing to do so can make it very difficult to analyze later. 
  In this sketch you will learn how to use the “print” and “pintln” functions to record you data.


  While recording data to your SD card, you will use “[file name].print” or “[file name].println” 
  We are going to practice by using the “Serial.print”/”Serial.println” functions so we can immediately see the results in the Serial Monitor. 
  The Serial Monitor is also a great debugging tool so learning how to use it will greatly benefit you during payload development
  */

void setup() {
  //Start Serial Communication
  //Capitalization matters
  Serial.begin(9600);
  // The numer is called the "baud rate" it represents how fast the device will communicate. 
  //If the sender and reciver are not at the same baud rate the data will be corrupted.
  //We choose 9600 because the is the ""default"" and the rate the the Arduino IDE Serail Moniter defaults to.

  //gives the board some time to establish communication.
  delay(3000);
  Serial.println();

  //Use ctl + shift + M to open the Serial Moniter


  //Simply prints an integer 
  Serial.print(11);

  //Prints a string of characters. All character must be within quote marks.
  //If the space was not included at the star the output would be: 11HAB 2025
  Serial.print(" HAB 2025");

  //If we want to move to the next line, we can use:
  Serial.println(" Get off this line!");

  Serial.print("Am i on the next line?");

  // Notice how we move to the next line AFTER printing the message when using println
  // If i wanted to move to the next line without altering the previous function to a println i could use an empty println statment like so:
  Serial.println();

  Serial.print("Down here now");

  //Lets make some space for the next section
  Serial.println();
  Serial.println();
  Serial.println();

  //We can also print variables

  int variableA = 455;

  Serial.print("Variable A = "); Serial.println(variableA);

  variableA = 2;

  Serial.print("Variable A = "); Serial.println(variableA);

  // it is possible to put multiple functions on the same line. However, it is only a good idea to do this when using print and println functions.

  Serial.println();
  Serial.println();
  Serial.println("Your Output:");

  //Now you try
  int Temp = 20;
  float money = 1.23;
  /* Produce the following output. Use both variables
  The temperature is 20 degrees Celsius
  I have 1.23 USD in my bank account :(
  I Love science!
  */

 // Continue on to "2-dataFormating" 

}

void loop() {
//nothing to see here
}
