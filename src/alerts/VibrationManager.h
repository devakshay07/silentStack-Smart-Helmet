#ifndef VIBRATION_MANAGER_H
#define VIBRATION_MANAGER_H

#include <Arduino.h>

enum class VibePattern {
    NONE,
    SHORT_PULSE,
    WARNING_PULSE,
    SOS_CRITICAL
};

class VibrationManager {
private:
    uint8_t pin;
    VibePattern currentPattern;
    unsigned long lastUpdate;
    int step;
    bool isOn;

public:
    VibrationManager(uint8_t pin);
    void init();
    void setPattern(VibePattern pattern);
    void update(); // Must be called in the non-blocking loop
};

#endif // VIBRATION_MANAGER_H
