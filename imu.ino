#include <Wire.h>
#include "MPU9250.h"

MPU9250 mpu;  // works for MPU9250 and MPU9255

void setup() {
  Serial.begin(115200);     // Serial Monitor must also be set to 9600
  Wire.begin();
  delay(2000);            // give sensor time to power up

  // Initialize MPU9250 at I2C address 0x68 (AD0 = GND)
  mpu.setup(0x68);

  // Optional: you can uncomment these later to calibrate
  // mpu.calibrateAccelGyro();
  // mpu.calibrateMag();

  Serial.println("MPU9250 initialized. Reading sensors...");
}

void loop() {
  // mpu.update() reads new data when available
  if (mpu.update()) {
    // --- Accelerometer (g) ---
    float ax = mpu.getAccX();
    float ay = mpu.getAccY();
    float az = mpu.getAccZ();

    // --- Gyroscope (deg/s) ---
    float gx = mpu.getGyroX();
    float gy = mpu.getGyroY();
    float gz = mpu.getGyroZ();

    // --- Magnetometer (µT or mG depending on settings) ---
    float mx = mpu.getMagX();
    float my = mpu.getMagY();
    float mz = mpu.getMagZ();

    // Print nicely in one block
    Serial.println("==== IMU Data ====");
    Serial.print("ACC (g) : ");
    Serial.print(ax, 3); Serial.print(", ");
    Serial.print(ay, 3); Serial.print(", ");
    Serial.println(az, 3);

    Serial.print("GYR (d/s): ");
    Serial.print(gx, 3); Serial.print(", ");
    Serial.print(gy, 3); Serial.print(", ");
    Serial.println(gz, 3);

    Serial.print("MAG     : ");
    Serial.print(mx, 3); Serial.print(", ");
    Serial.print(my, 3); Serial.print(", ");
    Serial.println(mz, 3);

    Serial.println();   // blank line for readability
  }

  // Small delay so the Serial Monitor is readable
  delay(50); // ~20 readings per second
}
