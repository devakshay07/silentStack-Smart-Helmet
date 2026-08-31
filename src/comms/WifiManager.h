#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "../include/ITransport.h"
#include <WiFi.h>
#include <HTTPClient.h>

class WifiManager : public ITransport {
private:
    const char* ssid;
    const char* password;
    const char* backendHost;
    unsigned long lastReconnectAttempt;
    const unsigned long RECONNECT_INTERVAL = 5000;
    
    bool postJson(const String& endpoint, const String& payload);

public:
    WifiManager(const char* ssid, const char* password, const char* backendHost);
    
    void init() override;
    bool connect() override;
    bool isConnected() override;
    
    bool sendTelemetry(const String& jsonPayload) override;
    bool sendEvent(const String& jsonPayload) override;
    bool sendHeartbeat(const String& jsonPayload) override;
    
    // Non-blocking maintainer to be called in main loop
    void maintainConnection(); 
};

#endif // WIFI_MANAGER_H
