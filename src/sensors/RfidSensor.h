#ifndef RFID_SENSOR_H
#define RFID_SENSOR_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RfidSensor {
private:
    MFRC522 mfrc522;
    unsigned long lastCheckTime;
    const unsigned long CHECK_INTERVAL = 500; // Check twice a second

public:
    RfidSensor(uint8_t ssPin, uint8_t rstPin);
    void init();
    String checkForCard();
};
#endif // RFID_SENSOR_H
