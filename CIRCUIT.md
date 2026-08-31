# silentStack Circuit Diagram

Here is the complete wiring architecture for assembling the prototype.

## Visual Wiring Flow

```mermaid
flowchart LR
    ESP32[ESP32 38-Pin]

    subgraph I2C [I2C Bus - 3.3V Logic]
        MPU[MPU6050 IMU]
        OLED[0.96 OLED]
    end

    subgraph SPI [SPI Bus - 3.3V Logic]
        RFID[RC522 RFID]
    end

    subgraph Sensors [Basic Sensors]
        MQ5[MQ-5 Gas Sensor]
        DHT[DHT11 Temp/Hum]
        TTP[TTP223 Touch SOS]
    end

    subgraph Outputs [Actuators]
        VIB[Vibration Motor Module]
    end

    ESP32 <-->|GPIO 21 SDA\nGPIO 22 SCL| I2C
    ESP32 <-->|GPIO 23 MOSI\nGPIO 19 MISO\nGPIO 18 SCK\nGPIO 5 SS\nGPIO 4 RST| SPI
    MQ5 -->|GPIO 34 Analog| ESP32
    DHT -->|GPIO 32 Digital| ESP32
    TTP -->|GPIO 33 Digital| ESP32
    ESP32 -->|GPIO 25 Digital| VIB
```

## Pin-to-Pin Wiring Table

**Power Supply Caution:** The ESP32 logic runs at **3.3V**. Connecting 5V signals directly to standard GPIOs can damage the board. The MQ-5 requires 5V for its internal heater, but its analog output is safe for ESP32's ADC pins. Follow this table exactly.

| Component | Component Pin | ESP32 Pin | Power Source | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MPU6050** | VCC<br>GND<br>SDA<br>SCL | 3V3<br>GND<br>GPIO 21<br>GPIO 22 | 3.3V | Share the I2C bus lines with the OLED. |
| **OLED (SSD1306)** | VCC<br>GND<br>SDA<br>SCL | 3V3<br>GND<br>GPIO 21<br>GPIO 22 | 3.3V | Share the I2C bus lines with the MPU6050. |
| **RC522 (RFID)** | 3.3V<br>RST<br>GND<br>MISO<br>MOSI<br>SCK<br>SDA (SS) | 3V3<br>GPIO 4<br>GND<br>GPIO 19<br>GPIO 23<br>GPIO 18<br>GPIO 5 | 3.3V | **Do NOT connect this to 5V**, it will permanently damage the RC522 chip. |
| **MQ-5** | VCC<br>GND<br>A0 | VIN / 5V<br>GND<br>GPIO 34 | 5V | Needs 5V for the heater. A0 is safe for `GPIO 34`. |
| **DHT11** | VCC<br>GND<br>DATA | 3V3<br>GND<br>GPIO 32 | 3.3V | Add a 10k resistor between VCC and DATA if needed. |
| **TTP223 (SOS)** | VCC<br>GND<br>SIG / I-O | 3V3<br>GND<br>GPIO 33 | 3.3V | Simple digital high/low trigger. |
| **Vibration Motor**| VCC<br>GND<br>IN / PWM | 3V3 or 5V<br>GND<br>GPIO 25 | -- | **Important:** Use a "Vibration Motor Module" with a built-in transistor, or wire an NPN transistor (like a 2N2222) to drive it safely. |
