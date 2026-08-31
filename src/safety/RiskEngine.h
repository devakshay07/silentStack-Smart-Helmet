#ifndef RISK_ENGINE_H
#define RISK_ENGINE_H
#include <Arduino.h>
#include "FallDetector.h"
#include "../sensors/Mq5Sensor.h"
#include "../sensors/DhtSensor.h"
#include "../sensors/TouchSensor.h"
#include "../alerts/OledManager.h"
#include "../alerts/VibrationManager.h"
#include "../core/EventManager.h"

enum class Severity { SEV_NORMAL, SEV_MEDIUM, SEV_HIGH, SEV_CRITICAL };

class RiskEngine {
private:
    EventManager* eventManager;
    OledManager* oled;
    VibrationManager* vib;
    
    Severity currentSeverity;
    unsigned long lastEventTriggerTime;
    const unsigned long EVENT_COOLDOWN_MS = 10000;

public:
    RiskEngine(EventManager* em, OledManager* oled, VibrationManager* vib);
    void evaluate(const FallDetector& fall, const Mq5Sensor& gas, const DhtSensor& thermal, const TouchSensor& sos);
};
#endif // RISK_ENGINE_H
