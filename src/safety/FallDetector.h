#ifndef FALL_DETECTOR_H
#define FALL_DETECTOR_H

#include <Arduino.h>

enum class FallState {
    NORMAL,
    IMPACT_SUSPECTED,
    CHECKING_INACTIVITY,
    CONFIRMED_FALL
};

class FallDetector {
private:
    FallState state;
    unsigned long stateStartTime;
    
    float preImpactPitch;
    float preImpactRoll;
    
    float maxGDuringInactivity;
    float minGDuringInactivity;

    // Thresholds
    const float impactGThreshold;
    const float angleChangeThreshold;
    const unsigned long inactivityDurationMs;

    float getAngleDiff(float a1, float a2);

public:
    FallDetector(float impactG, float angleChange, unsigned long inactivityMs);
    void update(float gForce, float pitch, float roll);
    
    FallState getState() const;
    void reset();
};

#endif // FALL_DETECTOR_H
