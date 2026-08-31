#include "OledManager.h"
#include <Wire.h>

OledManager::OledManager() 
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
      currentState(OledState::BOOT),
      lastTemp(-999), lastHum(-999), lastGasStatus(""),
      warningMsg("HAZARD"), criticalMsg("EVACUATE"),
      forceRedraw(true), lastFlashTime(0), invertScreen(false) {}

bool OledManager::init() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        return false;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    drawBoot();
    return true;
}

void OledManager::setState(OledState state) {
    if (currentState != state) {
        currentState = state;
        forceRedraw = true;
        
        // Reset inversion if exiting critical state
        if (state != OledState::CRITICAL) {
            display.invertDisplay(false);
            invertScreen = false;
        }
    }
}

void OledManager::setWarningMessage(const String& msg) {
    if (warningMsg != msg) {
        warningMsg = msg;
        if (currentState == OledState::WARNING) forceRedraw = true;
    }
}

void OledManager::setCriticalMessage(const String& msg) {
    if (criticalMsg != msg) {
        criticalMsg = msg;
        if (currentState == OledState::CRITICAL) forceRedraw = true;
    }
}

void OledManager::updateNormalVitals(float temp, float hum, const String& gasStatus) {
    // Only redraw if significant change to save cycles
    if (abs(lastTemp - temp) > 0.5 || abs(lastHum - hum) > 1.0 || lastGasStatus != gasStatus) {
        lastTemp = temp;
        lastHum = hum;
        lastGasStatus = gasStatus;
        if (currentState == OledState::NORMAL) forceRedraw = true;
    }
}

void OledManager::drawBoot() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.println("silentStack");
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.println("SMART HELMET");
    display.setCursor(0, 50);
    display.println("Booting...");
    display.display();
}

void OledManager::drawNormal() {
    display.clearDisplay();
    display.setTextSize(1);
    
    display.setCursor(0, 0);
    display.println("silentStack V1");
    
    display.setCursor(0, 16);
    display.print("TEMP: "); display.print(lastTemp, 1); display.println("C");
    
    display.setCursor(0, 26);
    display.print("HUM:  "); display.print(lastHum, 0); display.println("%");
    
    display.setCursor(0, 36);
    display.print("GAS:  "); display.println(lastGasStatus);
    
    display.setCursor(0, 54);
    display.println("STATUS: SAFE");
    
    display.display();
}

void OledManager::drawWarning() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(15, 5);
    display.println("WARNING");
    
    display.setTextSize(1);
    display.setCursor(0, 30);
    display.println(warningMsg);
    
    display.setCursor(0, 50);
    display.println("CHECK AREA");
    display.display();
}

void OledManager::drawCritical() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 5);
    display.println("CRITICAL!");
    
    display.setTextSize(1);
    display.setCursor(0, 30);
    display.println(criticalMsg);
    
    display.setCursor(0, 50);
    display.println("EVACUATE NOW");
    display.display();
}

void OledManager::update() {
    unsigned long currentMillis = millis();
    
    if (forceRedraw) {
        forceRedraw = false;
        switch (currentState) {
            case OledState::BOOT:     drawBoot(); break;
            case OledState::NORMAL:   drawNormal(); break;
            case OledState::WARNING:  drawWarning(); break;
            case OledState::CRITICAL: drawCritical(); break;
        }
    }
    
    // Critical state screen flashing effect (invert every 500ms)
    if (currentState == OledState::CRITICAL) {
        if (currentMillis - lastFlashTime >= 500) {
            lastFlashTime = currentMillis;
            invertScreen = !invertScreen;
            display.invertDisplay(invertScreen);
        }
    }
}
