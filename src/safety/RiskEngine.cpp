#include "RiskEngine.h"
#include "../../include/config.h"

RiskEngine::RiskEngine(EventManager* em, OledManager* oled, VibrationManager* vib)
    : eventManager(em), oled(oled), vib(vib), currentSeverity(Severity::SEV_NORMAL), lastEventTriggerTime(0) {}

void RiskEngine::evaluate(const FallDetector& fall, const Mq5Sensor& gas, const DhtSensor& thermal, const TouchSensor& sos) {
    String hazards = "";
    String evidence = "";
    Severity calculatedSeverity = Severity::SEV_NORMAL;

    // 1. Evaluate SOS (Highest Priority)
    if (sos.isPressed()) {
        hazards += "\"EMERGENCY\",";
        evidence += "\"sos\":\"button_pressed\",";
        calculatedSeverity = Severity::SEV_CRITICAL;
    }

    // 2. Evaluate Fall
    if (fall.getState() == FallState::CONFIRMED_FALL) {
        hazards += "\"CONFIRMED_FALL\",";
        evidence += "\"fall\":\"impact_and_inactivity\",";
        calculatedSeverity = Severity::SEV_CRITICAL;
    } else if (fall.getState() == FallState::IMPACT_SUSPECTED) {
        hazards += "\"POSSIBLE_FALL\",";
        if (calculatedSeverity < Severity::SEV_MEDIUM) calculatedSeverity = Severity::SEV_MEDIUM;
    }

    // 3. Evaluate Gas
    if (gas.getState() == Mq5State::READY) {
        float dev = gas.getDeviationPercent();
        if (dev > GAS_CRITICAL_DEV_PCT) {
            hazards += "\"GAS_CRITICAL\",";
            evidence += "\"gas_dev_pct\":" + String(dev) + ",";
            if (calculatedSeverity < Severity::SEV_CRITICAL) calculatedSeverity = Severity::SEV_CRITICAL;
        } else if (dev > GAS_WARNING_DEV_PCT) {
            hazards += "\"GAS_HIGH\",";
            evidence += "\"gas_dev_pct\":" + String(dev) + ",";
            if (calculatedSeverity < Severity::SEV_HIGH) calculatedSeverity = Severity::SEV_HIGH;
        }
    }

    // 4. Evaluate Thermal
    if (thermal.isValid()) {
        if (thermal.getTemperature() > TEMP_CRITICAL_C) {
            hazards += "\"THERMAL_CRITICAL\",";
            evidence += "\"temp\":" + String(thermal.getTemperature()) + ",";
            if (calculatedSeverity < Severity::SEV_HIGH) calculatedSeverity = Severity::SEV_HIGH;
        }
    }

    // Clean up trailing commas in JSON strings
    if (hazards.endsWith(",")) hazards = hazards.substring(0, hazards.length() - 1);
    if (evidence.endsWith(",")) evidence = evidence.substring(0, evidence.length() - 1);

    // 5. Update UI
    if (calculatedSeverity == Severity::SEV_CRITICAL) {
        oled->setState(OledState::CRITICAL);
        oled->setCriticalMessage("EVACUATE");
        vib->setPattern(VibePattern::SOS_CRITICAL);
    } else if (calculatedSeverity == Severity::SEV_HIGH) {
        oled->setState(OledState::WARNING);
        oled->setWarningMessage("CHECK AREA");
        vib->setPattern(VibePattern::WARNING_PULSE);
    } else if (calculatedSeverity == Severity::SEV_MEDIUM) {
        // Medium (like possible fall) gets a short vibration but UI stays normal
        vib->setPattern(VibePattern::SHORT_PULSE);
        oled->setState(OledState::NORMAL);
    } else {
        vib->setPattern(VibePattern::NONE);
        // Only return to normal if MQ5 is out of warmup
        if (gas.getState() == Mq5State::READY || gas.getState() == Mq5State::CALIBRATING) {
            oled->setState(OledState::NORMAL);
        }
    }

    // 6. Trigger Event Transmission (with Cooldown & Edge detection)
    if (calculatedSeverity > Severity::SEV_NORMAL) {
        unsigned long currentMillis = millis();
        // If severity escalated OR cooldown expired
        if (calculatedSeverity > currentSeverity || (currentMillis - lastEventTriggerTime > EVENT_COOLDOWN_MS)) {
            lastEventTriggerTime = currentMillis;
            String sevStr = (calculatedSeverity == Severity::SEV_CRITICAL) ? "CRITICAL" : 
                            (calculatedSeverity == Severity::SEV_HIGH) ? "HIGH" : "MEDIUM";
            eventManager->triggerEvent(hazards, sevStr, evidence);
        }
    }

    currentSeverity = calculatedSeverity;
}
