#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include <Arduino.h>

class ITransport {
public:
    virtual ~ITransport() {}
    
    virtual void init() = 0;
    virtual bool connect() = 0;
    virtual bool isConnected() = 0;
    
    // Abstracted payload transmission
    virtual bool sendTelemetry(const String& jsonPayload) = 0;
    virtual bool sendEvent(const String& jsonPayload) = 0;
    virtual bool sendHeartbeat(const String& jsonPayload) = 0;
};

#endif // ITRANSPORT_H
