// Pin definitions
const int trigPin = 7;    // Ultrasonic sensor Trigger pin
const int echoPin = 8;   // Ultrasonic sensor Echo pin
const int led1Pin = 9;    // LED1 pin
const int led2Pin = 10;    // LED2 pin

// Variables
const float distanceThreshold = 10.0; // Distance threshold in cm
long duration;
float distance;

void setup() {
  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  // Start serial communication for debugging (optional)
  Serial.begin(9600);
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the echo pulse duration
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm (speed of sound = 343 m/s, so 0.0343 cm/µs)
  distance = duration * 0.0343 / 2;
  
  // Control LEDs based on distance
  if (distance <= distanceThreshold) {
    digitalWrite(led1Pin, HIGH);  // Turn on LED1
    digitalWrite(led2Pin, LOW);   // Turn off LED2
  } else {
    digitalWrite(led1Pin, LOW);   // Turn off LED1
    digitalWrite(led2Pin, HIGH);  // Turn on LED2
  }
  
  // Print distance for debugging (optional)
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(100); // Small delay to stabilize readings
}