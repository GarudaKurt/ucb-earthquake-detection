#include "gyro.h"

#include <Arduino.h>
#include <MPU6050.h>
#include <Wire.h>

// MPU6050 setup
MPU6050 mpu;

GYRO::~GYRO(){};

void GYRO::initMPU() {
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
  }

  delay(1000);
}

float GYRO::fallingStatus() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;
  return sqrt(accX * accX + accY * accY + accZ * accZ);
}
