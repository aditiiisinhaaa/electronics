// HC-SR04 Ultrasonic Sensor + Buzzer Proximity Detector (ESP32-S3 Nano)

// Pin definitions for ESP32-S3 Nano
const int TRIG_PIN = 1;      // A0 (GPIO1) - Trigger pin (send pulse)
const int ECHO_PIN = 2;      // A1 (GPIO2) - Echo pin (receive pulse)
const int BUZZER_PIN = 3;    // A2 (GPIO3) - Buzzer pin (PWM capable)

// Distance threshold (in cm) - buzzer activates when hand is closer than this
const int DETECTION_DISTANCE = 20;

void setup() {
  Serial.begin(115200);  // ESP32 uses higher baud rate
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);  // Make sure buzzer starts off
}

void loop() {
  // Send a 10 microsecond pulse to trigger the sensor
int melody[] = {
  NOTE_C4, 200,
  NOTE_C4, 200,
  NOTE_D4, 200,
  NOTE_C4, 200,
  NOTE_F4, 200,
  NOTE_E4, 600
};
  
  // Convert time to distance (speed of sound = 343 m/s)
  // Distance = (duration in microseconds / 2) * 0.0343 cm/microsecond
  int distance = duration * 0.0343 / 2;
  
  // Print distance to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // If hand is close enough, turn on buzzer
  if (distance > 0 && distance < DETECTION_DISTANCE) {
    digitalWrite(BUZZER_PIN, HIGH);  // Simple digital on
    // Alternative: For PWM tone control, use: analogWrite(BUZZER_PIN, 200);
    Serial.println("BUZZER ON!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // Buzzer OFF
  }
  
  delay(100);  // Wait 100ms before next measurement
}
