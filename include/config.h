#ifndef CONFIG_H
#define CONFIG_H

// System Configuration
#define SERIAL_BAUD_RATE 115200
#define HEARTBEAT_INTERVAL_MS 15000
#define TELEMETRY_INTERVAL_MS 30000

// Hardware Pins (ESP32 38-pin layout)
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22
#define PIN_SPI_MOSI 23
#define PIN_SPI_MISO 19
#define PIN_SPI_SCK 18
#define PIN_RFID_SS 5
#define PIN_RFID_RST 4
#define PIN_MQ5_ANALOG 34
#define PIN_DHT11 32
#define PIN_TTP223 33
#define PIN_VIB_MOTOR 25

// Fall Detection Thresholds
#define FALL_IMPACT_G 2.5
#define FALL_ANGLE_CHANGE_DEG 60.0
#define FALL_INACTIVITY_MS 3000

// Gas (MQ-5) Thresholds
#define GAS_WARMUP_MS 180000       // 3 mins for basic warmup
#define GAS_BASELINE_MS 60000      // 1 min for baseline averaging
#define GAS_WARNING_DEV_PCT 50     // 50% deviation from baseline
#define GAS_CRITICAL_DEV_PCT 150   // 150% deviation from baseline
#define GAS_PERSISTENCE_MS 3000

// Thermal (DHT11) Thresholds
#define TEMP_WARNING_C 35.0
#define TEMP_CRITICAL_C 45.0
#define HUMIDITY_WARNING_PCT 80.0

#endif // CONFIG_H

// Network Configuration
#define WIFI_SSID "lalit kumar"
#define WIFI_PASS "10101980"
#define BACKEND_HOST "http://192.168.29.101:8000"

