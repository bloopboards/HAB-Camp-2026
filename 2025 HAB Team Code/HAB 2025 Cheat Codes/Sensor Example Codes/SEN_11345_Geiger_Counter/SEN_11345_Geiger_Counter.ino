int val;
int count = 0;

void setup() {
 Serial.begin(9600);
 pinMode(7, OUTPUT);

}

void loop() {
  val = analogRead(A7);
  if (val > 500) {
    count++;
    Serial.println(count);
  }

}
