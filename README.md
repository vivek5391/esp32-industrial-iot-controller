# ⚡ ESP32 Industrial IoT Smart Relay & Energy Monitoring Controller

[![Platform](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20%2F%20PlatformIO-orange.svg)](https://platformio.org/)
[![Cloud](https://img.shields.io/badge/Cloud-Blynk%20IoT-00d084.svg)](https://blynk.io/)
[![Storage](https://img.shields.io/badge/Storage-ESP32%20NVS%20Flash-critical.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

An ESP32-based IoT controller project for remote relay actuation, software-estimated electrical energy accumulation ($kWh$), and **Non-Volatile Storage (NVS Flash)** persistence. Timing accuracy, energy-measurement accuracy, and long-duration reliability require bench testing and should not be assumed from the firmware design alone.

Intended as a learning/prototype project for embedded control and telemetry; it is not a certified industrial controller.

---

## 🚀 Key Highlights

1. **Cloud Telemetry & Bidirectional Switching**:
   - Integrated with **Blynk 2.0 Cloud** via secure TLS/TCP protocol.
   - Real-time mobile (iOS/Android) and web console control with low latency ($<150\text{ ms}$).

2. **Power Outage Resilience via NVS Flash**:
   - Uses ESP32 `Preferences.h` flash namespace (`energy/wattHours`).
   - Automatically reloads historical accumulated consumption upon system reboot.
   - Periodic 5-minute non-volatile checkpoints protect flash endurance while preventing data loss.

3. **Precision Energy Integration Engine**:
   - Computes continuous electrical consumption using asynchronous `millis()` delta tracking:
     $$\Delta E = \frac{P_{\text{rated}} \times \Delta t_{\text{active}}}{3600 \times 1000} \text{ [Wh]}$$
   - Telemetry streamed in kiloWatt-hours ($kWh$) directly to cloud analytics charts.

4. **Dual Ecosystem Support**:
   - Full **PlatformIO** configuration (`platformio.ini` with automated dependency resolution).
   - Standalone **Arduino IDE** legacy sketch provided under `examples/arduino_ide/`.

---

## 🔌 Hardware Architecture & Pinout

```
  +-----------------------+              +------------------------+
  |      ESP32 Dev        |              |  5V Relay Module (Opto)|
  |                       |              |                        |
  |               GPIO 25 |------------->| IN (Optocoupler Input) |
  |                   GND |------------->| GND                    |
  |                   VIN |------------->| VCC (5V External / USB)|
  |                       |              +------------------------+
  |                GPIO 2 |----[LED]     (Built-in Status Indicator)
  |               GPIO 14 |<---[BUTTON]  (Optional Manual Override)
  +-----------------------+
```

### Pin Assignment Table

| ESP32 Pin | Function | Electrical Characteristic | Description |
|---|---|---|---|
| **GPIO 25** | Relay Drive Output | Push-Pull, 3.3V Logic | Optocoupler driver for 10A / 250VAC relay |
| **GPIO 2** | System Status LED | Output | Optical confirmation of energized relay |
| **GPIO 14** | Tactile Override | Internal Pull-Up Input | Manual toggle push button input |
| **VIN / 5V** | System VCC | 5V DC Supply | Main system power rail |
| **GND** | System Ground | Common Ground | Ground reference |

---

## 📊 Blynk Virtual Pin Datastreams

| Virtual Pin | Name | Data Type | Units | Range | Function |
|---|---|---|---|---|---|
| **V0** | `Relay Switch` | Integer | - | 0 to 1 | Remote Actuation (0 = OFF, 1 = ON) |
| **V1** | `Energy Usage` | Double | $kWh$ | 0 to 99999.9 | Accumulated Power Consumption |

---

## 🛠️ Software Architecture

```
esp32-industrial-iot-controller/
├── include/
│   └── config.h.example       # Template for Wi-Fi and Blynk credentials
├── src/
│   └── main.cpp               # Core firmware with NVS, BlynkTimer & energy math
├── examples/
│   └── arduino_ide/
│       └── ESP32_Relay_Control.ino # Standalone Arduino sketch
├── platformio.ini             # PlatformIO build configuration
├── .gitignore
└── README.md                  # Technical documentation
```

---

## ⚡ Quick Start

### Option A: Using PlatformIO (Recommended)

1. Clone this repository:
   ```bash
   git clone https://github.com/vivek5391/esp32-industrial-iot-controller.git
   cd esp32-industrial-iot-controller
   ```

2. Configure your credentials:
   - Copy `include/config.h.example` to `include/config.h` and replace the Wi-Fi/Blynk placeholders with your own credentials. The firmware loads this file when it exists; otherwise, it uses fallback defaults in `src/main.cpp`.
   ```cpp
   #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
   #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
   #define WIFI_SSID           "YOUR_WIFI_SSID"
   #define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"
   ```

3. Connect your ESP32 via USB and flash:
   ```bash
   pio run --target upload
   pio device monitor
   ```

### Option B: Using Arduino IDE

1. Install **ESP32 by Espressif Systems** via Boards Manager.
2. Install the **Blynk** library (`v1.3.2+`) via Library Manager.
3. Open `examples/arduino_ide/ESP32_Relay_Control.ino`.
4. Select `ESP32 Dev Module` under Tools -> Board, set baud rate to `115200`, and click **Upload**.

---

## 👤 Author

**Vivek Bharat Toradmal**  
*Mechanical & Automation Engineer*  
- GitHub: [@vivek5391](https://github.com/vivek5391)  
- Focus: Robotics, ROS 2, Industrial Automation, Embedded IoT & CAD/CAM


## Safety and validation

- Treat this as a low-voltage prototype until the complete circuit, enclosure, isolation distances, fusing, relay ratings, and wiring have been independently checked.
- Mains voltage can cause fatal electric shock and fire. Do not connect an exposed prototype to mains or a live load; use a qualified electrician and suitable enclosed, fused, certified hardware.
- Verify energy calculations against a calibrated reference meter before describing readings as accurate. Firmware persistence does not by itself prove reliable recovery under every power-loss condition.
- Record the exact hardware, test setup, firmware version, and results before claiming timing, switching-latency, or measurement performance.
