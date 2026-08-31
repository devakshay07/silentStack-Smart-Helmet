#include "MpuSensor.h"

MpuSensor::MpuSensor() : currentG(1.0), pitch(0), roll(0), lastUpdate(0), initialized(false) {}

bool MpuSensor::init() {
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        initialized = false;
        return false;
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // Good range for falls
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // Hardware Low-pass
    
    initialized = true;
    lastUpdate = micros();
    return true;
}

void MpuSensor::update() {
    if (!initialized) return;
    
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    unsigned long now = micros();
    float dt = (now - lastUpdate) / 1000000.0;
    lastUpdate = now;
    
    // Convert to G-force
    float ax = a.acceleration.x / 9.81;
    float ay = a.acceleration.y / 9.81;
    float az = a.acceleration.z / 9.81;
    
    currentG = sqrt(ax*ax + ay*ay + az*az);
    
    // Accelerometer based angles
    float accelPitch = atan2(ay, az) * 180.0 / PI;
    float accelRoll = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0 / PI;
    
    // Gyroscope rates (rad/s to deg/s)
    float gyroPitchRate = g.gyro.x * 180.0 / PI;
    float gyroRollRate = g.gyro.y * 180.0 / PI;
    
    // Complementary Filter
    pitch = alpha * (pitch + gyroPitchRate * dt) + (1.0 - alpha) * accelPitch;
    roll  = alpha * (roll + gyroRollRate * dt) + (1.0 - alpha) * accelRoll;
}

float MpuSensor::getGForce() const { return currentG; }
float MpuSensor::getPitch() const { return pitch; }
float MpuSensor::getRoll() const { return roll; }
bool MpuSensor::isOk() const { return initialized; }
