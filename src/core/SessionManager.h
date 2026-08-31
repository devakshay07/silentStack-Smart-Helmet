#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H
#include <Arduino.h>

class SessionManager {
private:
    String currentWorkerId;
    String currentHelmetId;

public:
    SessionManager(const String& defaultHelmetId);
    void setWorkerId(const String& uid);
    String getWorkerId() const;
    String getHelmetId() const;
};
#endif // SESSION_MANAGER_H
