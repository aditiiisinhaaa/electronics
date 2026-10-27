void setup() {
  // Initialize digital pins as outputs
  pinMode(3, OUTPUT);  // IN1
  pinMode(4, OUTPUT);  // IN2
}
void loop() {
  // Motor forward
  digitalWrite(3, HIGH);  // IN1
  digitalWrite(4, LOW);   // IN2
  delay(2000);            // Run for 2 seconds
  // Motor backward
  digitalWrite(3, LOW);   // IN1
  digitalWrite(4, HIGH);  // IN2
  delay(2000);            // Run for 2 seconds
}