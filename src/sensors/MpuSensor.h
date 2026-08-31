#ifndef MPU_SENSOR_H
#define MPU_SENSOR_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MpuSensor {
private:
    Adafruit_MPU6050 mpu;
    float currentG;
    float pitch;
    float roll;
    unsigned long lastUpdate;
    bool initialized;

    // Complementary filter coefficient
    const float alpha = 0.96;

public:
    MpuSensor();
    bool init();
    void update(); 
    
    float getGForce() const;
    float getPitch() const;
    float getRoll() const;
    bool isOk() const;
};

#endif // MPU_SENSOR_H
