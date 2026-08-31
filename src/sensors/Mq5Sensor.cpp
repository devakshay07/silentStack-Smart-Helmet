#include "Mq5Sensor.h"

Mq5Sensor::Mq5Sensor(uint8_t pin, unsigned long warmupMs, unsigned long calibMs)
    : pin(pin), state(Mq5State::WARMUP), startTime(0),
      emaFilter(0.1), baselineFilter(0.01), // Very slow baseline tracking
      currentFilteredVal(0), baselineVal(0), currentDeviationPct(0),
      warmupDuration(warmupMs), calibDuration(calibMs) {}

void Mq5Sensor::init() {
    pinMode(pin, INPUT);
    startTime = millis();
    state = Mq5State::WARMUP;
}

void Mq5Sensor::update() {
    unsigned long elapsed = millis() - startTime;
    int rawValue = analogRead(pin);
    
    // Hardware fault check (0 or 4095 usually indicates disconnected or shorted)
    if (rawValue <= 0 || rawValue >= 4095) {
        state = Mq5State::FAULT;
        currentDeviationPct = 0;
        return;
    }
    
    // Apply pipeline: Median -> EMA
    int medianVal = medianFilter.filter(rawValue);
    currentFilteredVal = emaFilter.filter((float)medianVal);

    if (state == Mq5State::WARMUP) {
        if (elapsed > warmupDuration) {
            state = Mq5State::CALIBRATING;
            startTime = millis(); // Reset timer for calibration phase
        }
    } 
    else if (state == Mq5State::CALIBRATING) {
        baselineVal = baselineFilter.filter(currentFilteredVal);
        if (elapsed > calibDuration) {
            state = Mq5State::READY;
        }
    } 
    else if (state == Mq5State::READY) {
        // Very slow leak of current baseline to adapt to environment
        // but only if deviation is low (to prevent adapting to actual gas leaks)
        if (currentFilteredVal < baselineVal * 1.2) {
            baselineVal = baselineFilter.filter(currentFilteredVal);
        }
        
        // Calculate relative deviation percentage
        if (baselineVal > 0) {
            float dev = ((currentFilteredVal - baselineVal) / baselineVal) * 100.0;
            currentDeviationPct = (dev > 0) ? dev : 0;
        }
    }
}

Mq5State Mq5Sensor::getState() const { return state; }
float Mq5Sensor::getDeviationPercent() const { return currentDeviationPct; }

String Mq5Sensor::getStatusString() const {
    switch(state) {
        case Mq5State::WARMUP: return "WARMUP";
        case Mq5State::CALIBRATING: return "CALIB";
        case Mq5State::FAULT: return "ERR";
        case Mq5State::READY:
            if (currentDeviationPct < 50) return "OK";
            if (currentDeviationPct < 150) return "HIGH";
            return "CRIT";
        default: return "UNK";
    }
}
