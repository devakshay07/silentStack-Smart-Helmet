#ifndef MQ5_SENSOR_H
#define MQ5_SENSOR_H

#include <Arduino.h>
#include "../processing/Filters.h"

enum class Mq5State {
    WARMUP,
    CALIBRATING,
    READY,
    FAULT
};

class Mq5Sensor {
private:
    uint8_t pin;
    Mq5State state;
    unsigned long startTime;
    
    Median5Filter medianFilter;
    EMAFilter emaFilter;
    EMAFilter baselineFilter;
    
    float currentFilteredVal;
    float baselineVal;
    float currentDeviationPct;

    const unsigned long warmupDuration;
    const unsigned long calibDuration;

public:
    Mq5Sensor(uint8_t pin, unsigned long warmupMs, unsigned long calibMs);
    void init();
    void update();
    
    Mq5State getState() const;
    float getDeviationPercent() const;
    String getStatusString() const;
};

#endif // MQ5_SENSOR_H
