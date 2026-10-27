#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Initialize I2C with A4 (SDA) and A5 (SCL)
  Wire.begin(A4, A5);
  
  // Initialize MPU9250
  if (!mpu.begin(0x68)) {
    Serial.println("MPU6050 not found!");
    while(1) {
      delay(10);
    }
  }
  
  Serial.println("MPU6050 initialized!");
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  Serial.print("Accel X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" | Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" | Z: ");
  Serial.println(a.acceleration.z);
  
  Serial.print("Gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print(" | Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" | Z: ");
  Serial.println(g.gyro.z);
  
  Serial.println("---");
  delay(500);
}