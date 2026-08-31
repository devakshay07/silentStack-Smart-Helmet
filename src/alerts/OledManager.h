#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

enum class OledState {
    BOOT,
    NORMAL,
    WARNING,
    CRITICAL
};

class OledManager {
private:
    Adafruit_SSD1306 display;
    OledState currentState;
    
    // Cached values to prevent full redraws
    float lastTemp;
    float lastHum;
    String lastGasStatus;
    
    String warningMsg;
    String criticalMsg;
    
    bool forceRedraw;
    unsigned long lastFlashTime;
    bool invertScreen;

    void drawBoot();
    void drawNormal();
    void drawWarning();
    void drawCritical();

public:
    OledManager();
    bool init();
    
    void setState(OledState state);
    void setWarningMessage(const String& msg);
    void setCriticalMessage(const String& msg);
    void updateNormalVitals(float temp, float hum, const String& gasStatus);
    
    void update(); // Call in loop
};

#endif // OLED_MANAGER_H
