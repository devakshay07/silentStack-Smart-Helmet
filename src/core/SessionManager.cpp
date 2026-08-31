#include "SessionManager.h"

SessionManager::SessionManager(const String& defaultHelmetId) 
    : currentWorkerId("UNAUTHORIZED"), currentHelmetId(defaultHelmetId) {}

void SessionManager::setWorkerId(const String& uid) {
    if (uid.length() > 0) {
        currentWorkerId = uid;
    }
}

String SessionManager::getWorkerId() const {
    return currentWorkerId;
}

String SessionManager::getHelmetId() const {
    return currentHelmetId;
}
