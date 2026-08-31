#include "VibrationManager.h"

VibrationManager::VibrationManager(uint8_t pin) : pin(pin), currentPattern(VibePattern::NONE), lastUpdate(0), step(0), isOn(false) {}

void VibrationManager::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void VibrationManager::setPattern(VibePattern pattern) {
    if (currentPattern != pattern) {
        currentPattern = pattern;
        step = 0;
        isOn = false;
        digitalWrite(pin, LOW);
        lastUpdate = millis();
    }
}

void VibrationManager::update() {
    unsigned long currentMillis = millis();
    
    switch (currentPattern) {
        case VibePattern::NONE:
            if (isOn) {
                isOn = false;
                digitalWrite(pin, LOW);
            }
            break;
            
        case VibePattern::SHORT_PULSE:
            // Single short 200ms pulse, then turn off
            if (step == 0) {
                digitalWrite(pin, HIGH);
                isOn = true;
                if (currentMillis - lastUpdate > 200) {
                    digitalWrite(pin, LOW);
                    isOn = false;
                    step = 1; // Done
                }
            }
            break;
            
        case VibePattern::WARNING_PULSE:
            // 500ms ON, 500ms OFF repeating
            if (currentMillis - lastUpdate > 500) {
                lastUpdate = currentMillis;
                isOn = !isOn;
                digitalWrite(pin, isOn ? HIGH : LOW);
            }
            break;
            
        case VibePattern::SOS_CRITICAL:
            // 150ms ON, 150ms OFF fast pulse
            if (currentMillis - lastUpdate > 150) {
                lastUpdate = currentMillis;
                isOn = !isOn;
                digitalWrite(pin, isOn ? HIGH : LOW);
            }
            break;
    }
}
