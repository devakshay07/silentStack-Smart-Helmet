#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include <Arduino.h>
#include "../../include/ITransport.h"
#include "SessionManager.h"

#define MAX_EVENT_QUEUE 10

class EventManager {
private:
    ITransport* transport;
    SessionManager* session;
    unsigned long eventCounter;
    
    String eventQueue[MAX_EVENT_QUEUE];
    int queueHead;
    int queueTail;
    
    unsigned long lastFlushAttempt;

public:
    EventManager(ITransport* transport, SessionManager* session);
    void triggerEvent(const String& hazards, const String& severityStr, const String& evidence);
    void processQueue(); // Call this in the main loop to handle offline caching
};
#endif // EVENT_MANAGER_H
