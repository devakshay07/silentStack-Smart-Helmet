#include "FallDetector.h"

FallDetector::FallDetector(float impactG, float angleChange, unsigned long inactivityMs)
    : state(FallState::NORMAL), stateStartTime(0),
      preImpactPitch(0), preImpactRoll(0),
      maxGDuringInactivity(0), minGDuringInactivity(0),
      impactGThreshold(impactG), angleChangeThreshold(angleChange), inactivityDurationMs(inactivityMs) {}

float FallDetector::getAngleDiff(float a1, float a2) {
    float diff = abs(a1 - a2);
    if (diff > 180.0) diff = 360.0 - diff;
    return diff;
}

void FallDetector::reset() {
    state = FallState::NORMAL;
}

void FallDetector::update(float gForce, float pitch, float roll) {
    unsigned long currentMillis = millis();

    switch (state) {
        case FallState::NORMAL:
            // Continually track orientation while normal to know "before" angle
            preImpactPitch = pitch;
            preImpactRoll = roll;
            
            if (gForce > impactGThreshold) {
                state = FallState::IMPACT_SUSPECTED;
                stateStartTime = currentMillis;
            }
            break;

        case FallState::IMPACT_SUSPECTED:
            // Allow 1 second for the fall physics to settle (tumbling)
            if (currentMillis - stateStartTime > 1000) {
                float pitchChange = getAngleDiff(preImpactPitch, pitch);
                float rollChange = getAngleDiff(preImpactRoll, roll);
                
                // Did the helmet end up in a drastically different orientation?
                if (pitchChange > angleChangeThreshold || rollChange > angleChangeThreshold) {
                    state = FallState::CHECKING_INACTIVITY;
                    stateStartTime = currentMillis;
                    maxGDuringInactivity = gForce;
                    minGDuringInactivity = gForce;
                } else {
                    // Just a bump/jump, return to normal
                    reset();
                }
            }
            break;

        case FallState::CHECKING_INACTIVITY:
            // Track movement variance
            if (gForce > maxGDuringInactivity) maxGDuringInactivity = gForce;
            if (gForce < minGDuringInactivity) minGDuringInactivity = gForce;
            
            // Wait for inactivity window to close
            if (currentMillis - stateStartTime > inactivityDurationMs) {
                float gVariance = maxGDuringInactivity - minGDuringInactivity;
                
                // If G force varied by less than 0.3G over 3 seconds, they are motionless
                if (gVariance < 0.3) {
                    state = FallState::CONFIRMED_FALL;
                } else {
                    // They are moving/recovering
                    reset();
                }
            }
            break;

        case FallState::CONFIRMED_FALL:
            // Locked in state until manually reset or acknowledged by backend (future)
            break;
    }
}

FallState FallDetector::getState() const { return state; }
