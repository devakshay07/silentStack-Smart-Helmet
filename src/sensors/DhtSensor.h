#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

class DhtSensor {
private:
    DHT dht;
    float temperature;
    float humidity;
    unsigned long lastReadTime;
    const unsigned long READ_INTERVAL = 2000; // DHT11 is slow

public:
    DhtSensor(uint8_t pin);
    void init();
    void update(); 
    
    float getTemperature() const;
    float getHumidity() const;
    bool isValid() const;
};

#endif // DHT_SENSOR_H
