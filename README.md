# silentStack Smart Worker Helmet 👷‍♂️⚠️

**silentStack** is an offline-first, modular smart safety helmet prototype built on the ESP32. It provides deterministic, real-time risk assessment for industrial workers by monitoring motion, environmental conditions, and combustible gas levels.

## Features
- **Multi-Hazard Sensor Fusion**: Detects complex failures (e.g., Fall + Gas anomaly simultaneously).
- **Physics-based Fall Detection**: Rejects false positives by requiring a specific physics signature (Impact -> Orientation Change -> Extended Inactivity).
- **Offline Resilience**: Features an `EventManager` with a circular buffer. If Wi-Fi drops, safety events are cached in RAM and automatically flushed to the backend when connectivity returns.
- **Graceful UI**: State-driven 0.96" OLED display and Haptic SOS vibration motor ensure the worker is alerted instantly, even without an internet connection.
- **RFID Session Management**: Dynamically binds a physical helmet to a specific `worker_id` via RC522 card swipes.

---

## 🛠 Hardware Wiring (ESP32 38-pin)

| Component | ESP32 Pin | Protocol / Type | Notes |
| :--- | :--- | :--- | :--- |
| **MPU6050** | GPIO 21 / 22 | I2C (SDA / SCL) | Gyro/Accel for Fall Detection |
| **OLED 0.96"** | GPIO 21 / 22 | I2C (SDA / SCL) | Shared I2C Bus |
| **RC522 RFID** | 23, 19, 18, 5, 4 | SPI | MOSI(23), MISO(19), SCK(18), SS(5), RST(4) |
| **MQ-5 Gas** | GPIO 34 | Analog Input | Combustible gas detection |
| **DHT11** | GPIO 32 | Digital Input | Temp/Humidity context |
| **TTP223** | GPIO 33 | Digital Input | Emergency SOS Button |
| **Vibration**| GPIO 25 | Digital Output | Alerts (Requires MOSFET/Transistor driver) |

---

## 🚀 Full Setup Guide

### 1. Configure Wi-Fi & Backend IP
Before flashing, point the ESP32 to your local Wi-Fi and Python backend.
1. Open `include/config.h`.
2. Scroll to the bottom and set your router's credentials:
   ```cpp
   #define WIFI_SSID "your_actual_wifi_name"
   #define WIFI_PASS "your_actual_wifi_password"
   #define BACKEND_HOST "http://<YOUR_COMPUTER_IP>:8000"
   ```

### 2. Flash the Firmware
The codebase uses **PlatformIO** to manage dependencies (Adafruit GFX, MPU6050, etc.).
1. Open the project folder in **VS Code** (with the PlatformIO extension installed).
2. Connect your ESP32 via USB.
3. Click the **Upload (`→`)** button in the bottom blue toolbar.

### 3. Run the Backend Server
The included Python server catches JSON telemetry and safety events in real-time. It requires **no external dependencies** (no internet required to install).
1. Open a new Terminal in VS Code.
2. Run the server:
   ```bash
   python3 backend/main.py
   ```
3. Once the ESP32 boots and the MQ-5 sensor finishes its 3-minute warmup, you will see `+++ TELEMETRY +++` logs appearing in your terminal.

---

## 🧠 Software Architecture

- **`src/sensors/`**: Drivers for hardware components. The `Mq5Sensor` uses a non-blocking state machine (Warmup -> Calibrate -> Ready) so it doesn't freeze the MCU on boot.
- **`src/safety/`**: The core physics rules. `FallDetector.cpp` manages the IMU thresholds, and `RiskEngine.cpp` aggregates all sensors to determine if the helmet is in `NORMAL`, `MEDIUM`, `HIGH`, or `CRITICAL` state.
- **`src/core/`**: `EventManager` formats standardized JSON payloads and handles offline queueing. `SessionManager` tracks the RFID worker ID.
- **`src/comms/`**: `WifiManager` handles non-blocking background reconnections and HTTP POST requests.
