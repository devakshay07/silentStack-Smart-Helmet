#include "TouchSensor.h"

TouchSensor::TouchSensor(uint8_t pin) : pin(pin), active(false), lastState(false), lastDebounceTime(0) {}

void TouchSensor::init() {
    pinMode(pin, INPUT);
}

void TouchSensor::update() {
    bool reading = (digitalRead(pin) == HIGH);

    if (reading != lastState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != active) {
            active = reading;
        }
    }
    lastState = reading;
}

bool TouchSensor::isPressed() const {
    return active;
}
