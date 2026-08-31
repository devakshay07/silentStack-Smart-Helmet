#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H
#include <Arduino.h>

class TouchSensor {
private:
    uint8_t pin;
    bool active;
    bool lastState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay = 50;

public:
    TouchSensor(uint8_t pin);
    void init();
    void update();
    bool isPressed() const;
};
#endif // TOUCH_SENSOR_H
