#include <Arduino.h>
#include "config.h"
#include "comms/WifiManager.h"
#include "alerts/OledManager.h"
#include "alerts/VibrationManager.h"
#include "sensors/DhtSensor.h"
#include "sensors/Mq5Sensor.h"
#include "sensors/MpuSensor.h"
#include "sensors/TouchSensor.h"
#include "sensors/RfidSensor.h"
#include "safety/FallDetector.h"
#include "core/EventManager.h"
#include "core/SessionManager.h"
#include "safety/RiskEngine.h"
#include <Wire.h>

// System Timing State
unsigned long lastTelemetryTime = 0;
unsigned long lastHeartbeatTime = 0;
unsigned long lastSensorPollTime = 0;
const unsigned long SENSOR_POLL_INTERVAL_MS = 10; // 100Hz

// Transport Interface
WifiManager wifiManager(WIFI_SSID, WIFI_PASS, BACKEND_HOST);

// Session State
SessionManager sessionManager("ESP32-HELMET-01");
EventManager eventManager(&wifiManager, &sessionManager);

// Hardware UI
OledManager oledManager;
VibrationManager vibManager(PIN_VIB_MOTOR);

// Sensors
DhtSensor dhtSensor(PIN_DHT11);
Mq5Sensor mq5Sensor(PIN_MQ5_ANALOG, GAS_WARMUP_MS, GAS_BASELINE_MS);
MpuSensor mpuSensor;
TouchSensor touchSensor(PIN_TTP223);
RfidSensor rfidSensor(PIN_RFID_SS, PIN_RFID_RST);

// Safety Core
FallDetector fallDetector(FALL_IMPACT_G, FALL_ANGLE_CHANGE_DEG, FALL_INACTIVITY_MS);
RiskEngine riskEngine(&eventManager, &oledManager, &vibManager);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);
    Serial.println("\n--- silentStack Booting ---");
    
    // Initialize Hardware UI
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    oledManager.init();
    vibManager.init();
    
    // Initialize Sensors
    dhtSensor.init();
    mq5Sensor.init();
    mpuSensor.init();
    touchSensor.init();
    rfidSensor.init();
    
    // Initialize Transport
    wifiManager.init();
    wifiManager.connect();
    
    oledManager.setState(OledState::BOOT);
    oledManager.update();
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. Maintain Network Connection & Flush Offline Events (Non-blocking)
    wifiManager.maintainConnection();
    eventManager.processQueue();

    // 2. High-Frequency Sensor Polling & Safety Core
    if (currentMillis - lastSensorPollTime >= SENSOR_POLL_INTERVAL_MS) {
        lastSensorPollTime = currentMillis;
        
        // Poll Sensors
        mpuSensor.update();
        if (mpuSensor.isOk()) {
            fallDetector.update(mpuSensor.getGForce(), mpuSensor.getPitch(), mpuSensor.getRoll());
        }
        
        touchSensor.update();
        dhtSensor.update();
        mq5Sensor.update();
        
        // Handle RFID Tag swipes
        String uid = rfidSensor.checkForCard();
        if (uid.length() > 0) {
            sessionManager.setWorkerId(uid);
            Serial.println("Worker ID assigned: " + uid);
            // Flash a tiny pulse to confirm scan
            vibManager.setPattern(VibePattern::SHORT_PULSE);
        }
        
        // Update Normal Vitals on UI cache
        oledManager.updateNormalVitals(
            dhtSensor.isValid() ? dhtSensor.getTemperature() : 0.0,
            dhtSensor.isValid() ? dhtSensor.getHumidity() : 0.0,
            mq5Sensor.getStatusString()
        );
        
        // 3. Evaluate Risk (Aggregates multiple hazards, updates UI, fires events)
        riskEngine.evaluate(fallDetector, mq5Sensor, dhtSensor, touchSensor);
        
        // 4. Draw Hardware UI
        oledManager.update();
        vibManager.update();
    }

    // 5. Periodic Heartbeat
    if (currentMillis - lastHeartbeatTime >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeatTime = currentMillis;
        if (wifiManager.isConnected()) {
            String payload = "{\"helmet_id\":\"" + sessionManager.getHelmetId() + "\",\"status\":\"ONLINE\",\"uptime_s\":" + String(millis()/1000) + "}";
            wifiManager.sendHeartbeat(payload);
        }
    }

    // 6. Periodic Telemetry (Sensors only)
    if (currentMillis - lastTelemetryTime >= TELEMETRY_INTERVAL_MS) {
        lastTelemetryTime = currentMillis;
        
        if (wifiManager.isConnected()) {
            String payload = "{\"helmet_id\":\"" + sessionManager.getHelmetId() + "\",\"worker_id\":\"" + sessionManager.getWorkerId() + "\",\"temp\":" + String(dhtSensor.getTemperature(), 1) + 
                             ",\"hum\":" + String(dhtSensor.getHumidity(), 1) + 
                             ",\"gas_dev_pct\":" + String(mq5Sensor.getDeviationPercent(), 1) + 
                             ",\"g_force\":" + String(mpuSensor.getGForce(), 2) + "}";
            wifiManager.sendTelemetry(payload);
        }
    }
}
