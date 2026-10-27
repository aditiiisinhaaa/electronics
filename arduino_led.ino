int greenLED = 13;
int yellowLED = 12;
int redLED = 11;
void setup() {
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
}
void loop() {
  digitalWrite(greenLED, HIGH);
  delay(1000);
  digitalWrite(greenLED, LOW);
  delay(1000);
  digitalWrite(yellowLED, HIGH);
  delay(1000);
  digitalWrite(yellowLED, LOW);
  delay(1000);
  digitalWrite(redLED, HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  delay(1000);
}