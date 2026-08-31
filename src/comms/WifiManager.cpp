#include "WifiManager.h"

WifiManager::WifiManager(const char* ssid, const char* password, const char* backendHost) 
    : ssid(ssid), password(password), backendHost(backendHost), lastReconnectAttempt(0) {}

void WifiManager::init() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

bool WifiManager::connect() {
    if (isConnected()) return true;
    
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    // Non-blocking check will be handled by maintainConnection()
    return true; 
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WifiManager::maintainConnection() {
    if (!isConnected()) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) {
            lastReconnectAttempt = currentMillis;
            Serial.println("WiFi disconnected. Attempting reconnect...");
            WiFi.reconnect();
        }
    }
}

bool WifiManager::postJson(const String& endpoint, const String& payload) {
    if (!isConnected()) return false;
    
    HTTPClient http;
    String url = String(backendHost) + endpoint;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(payload);
    bool success = false;
    
    if (httpResponseCode > 0) {
        if (httpResponseCode == 200 || httpResponseCode == 201) {
            success = true;
        } else {
            Serial.printf("HTTP %d on %s\n", httpResponseCode, endpoint.c_str());
        }
    } else {
        Serial.printf("HTTP request failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    
    http.end();
    return success;
}

bool WifiManager::sendTelemetry(const String& jsonPayload) {
    return postJson("/api/helmet/telemetry", jsonPayload);
}

bool WifiManager::sendEvent(const String& jsonPayload) {
    return postJson("/api/helmet/events", jsonPayload);
}

bool WifiManager::sendHeartbeat(const String& jsonPayload) {
    return postJson("/api/helmet/heartbeat", jsonPayload);
}
