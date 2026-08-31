#include "RfidSensor.h"

RfidSensor::RfidSensor(uint8_t ssPin, uint8_t rstPin) 
    : mfrc522(ssPin, rstPin), lastCheckTime(0) {}

void RfidSensor::init() {
    SPI.begin();
    mfrc522.PCD_Init();
    delay(4); // Small delay to let the MFRC522 boot
}

String RfidSensor::checkForCard() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheckTime < CHECK_INTERVAL) {
        return "";
    }
    lastCheckTime = currentMillis;

    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return "";
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return "";
    }

    // Convert UID to Hex String
    String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if(mfrc522.uid.uidByte[i] < 0x10) {
            uidString += "0";
        }
        uidString += String(mfrc522.uid.uidByte[i], HEX);
    }
    
    uidString.toUpperCase();
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    
    return uidString;
}
