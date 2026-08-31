#include "DhtSensor.h"

DhtSensor::DhtSensor(uint8_t pin) 
    : dht(pin, DHT11), temperature(-999), humidity(-999), lastReadTime(0) {}

void DhtSensor::init() {
    dht.begin();
}

void DhtSensor::update() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastReadTime >= READ_INTERVAL || lastReadTime == 0) {
        lastReadTime = currentMillis;
        
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        
        if (!isnan(t) && !isnan(h)) {
            temperature = t;
            humidity = h;
        }
    }
}

float DhtSensor::getTemperature() const { return temperature; }
float DhtSensor::getHumidity() const { return humidity; }

bool DhtSensor::isValid() const {
    return temperature > -100.0 && humidity > -100.0;
}
