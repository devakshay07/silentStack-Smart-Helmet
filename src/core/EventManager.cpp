#include "EventManager.h"

EventManager::EventManager(ITransport* transport, SessionManager* session) 
    : transport(transport), session(session), eventCounter(0), queueHead(0), queueTail(0), lastFlushAttempt(0) {}

void EventManager::triggerEvent(const String& hazards, const String& severityStr, const String& evidence) {
    eventCounter++;
    
    String payload = "{";
    payload += "\"event_id\":\"EVT-" + String(millis()) + "-" + String(eventCounter) + "\",";
    payload += "\"helmet_id\":\"" + session->getHelmetId() + "\",";
    payload += "\"worker_id\":\"" + session->getWorkerId() + "\",";
    payload += "\"timestamp_ms\":" + String(millis()) + ",";
    payload += "\"severity\":\"" + severityStr + "\",";
    payload += "\"active_events\":[" + hazards + "],";
    payload += "\"evidence\":{" + evidence + "}";
    payload += "}";

    // Try sending immediately if connected
    if (transport->isConnected()) {
        if (transport->sendEvent(payload)) {
            Serial.println("Event Sent: " + payload);
            return; // Success
        }
    }
    
    // If offline or send failed, queue it locally
    eventQueue[queueHead] = payload;
    queueHead = (queueHead + 1) % MAX_EVENT_QUEUE;
    
    // If queue fills up, advance the tail to overwrite the oldest event
    if (queueHead == queueTail) {
        queueTail = (queueTail + 1) % MAX_EVENT_QUEUE;
        Serial.println("Warning: Event Queue Full. Oldest event dropped.");
    } else {
        Serial.println("Offline/Failed: Event Queued.");
    }
}

void EventManager::processQueue() {
    if (queueHead == queueTail) return; // Queue empty
    
    unsigned long currentMillis = millis();
    // Try flushing one event every 2 seconds to avoid blocking the main loop
    if (currentMillis - lastFlushAttempt > 2000) {
        lastFlushAttempt = currentMillis;
        
        if (transport->isConnected()) {
            if (transport->sendEvent(eventQueue[queueTail])) {
                Serial.println("Queued Event Flushed Successfully.");
                queueTail = (queueTail + 1) % MAX_EVENT_QUEUE;
            }
        }
    }
}
