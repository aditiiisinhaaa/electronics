// HC-SR04 Ultrasonic Sensor + Buzzer Proximity Detector (ESP32-S3 Nano)

// Pin definitions for ESP32-S3 Nano
const int TRIG_PIN = 1;      // A0 (GPIO1) - Trigger pin (send pulse)
const int ECHO_PIN = 2;      // A1 (GPIO2) - Echo pin (receive pulse)
const int BUZZER_PIN = 3;    // A2 (GPIO3) - Buzzer pin (PWM capable)

// Distance threshold (in cm) - buzzer activates when hand is closer than this
const int DETECTION_DISTANCE = 15;

// Musical notes (frequencies in Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Simple melody - each pair is [frequency, duration in ms]
int melody[] = {
  NOTE_E4, 200,
  NOTE_G4, 200,
  NOTE_E4, 200,
  NOTE_C4, 400,
  NOTE_D4, 200,
  NOTE_E4, 600
};

int melodyLength = sizeof(melody) / sizeof(melody[0]);

// Function to play the melody
void playMelody() {
  for (int i = 0; i < melodyLength; i += 2) {
    int frequency = melody[i];
    int duration = melody[i + 1];
    
    // Generate square wave at the note frequency
    tone(BUZZER_PIN, frequency, duration);
    
    // Small pause between notes
    delay(duration + 50);
  }
  noTone(BUZZER_PIN);  // Make sure buzzer is off
}

void setup() {
  Serial.begin(115200);  // ESP32 uses higher baud rate
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);  // Make sure buzzer starts off
}

// Function to measure distance from the sensor
int getDistance() {
  // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the echo time (how long the pin stays HIGH)
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Convert time to distance (speed of sound = 343 m/s)
  // Distance = (duration in microseconds / 2) * 0.0343 cm/microsecond
  int distance = duration * 0.0343 / 2;
  
  return distance;
}

void loop() {
  int distance = getDistance();
  
  // Print distance to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // If object is detected, play melody continuously
  if (distance > 0 && distance < DETECTION_DISTANCE) {
    Serial.println("OBJECT DETECTED! Playing melody...");
    playMelody();  // Play the melody while object is detected
  } else {
    noTone(BUZZER_PIN);  // Make sure buzzer is off when no object
  }
}
