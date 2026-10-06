/**
 * @file main.cpp
 * @brief Industrial IoT Smart Relay & Energy Monitoring Controller
 * @author Vivek Bharat Toradmal (https://github.com/vivek5391)
 *
 * ESP32-based industrial controller featuring:
 * - Real-time cloud telemetry and bidirectional control via Blynk IoT.
 * - Non-Volatile Flash Storage (ESP32 Preferences / NVS) for power outage resilience.
 * - Millisecond-accurate electrical energy accumulation (kWh).
 * - Safe relay actuation with configurable active-level logic.
 */

// Configure via include/config.h or replace defaults below
#ifndef BLYNK_TEMPLATE_ID
  #define BLYNK_TEMPLATE_ID   "TMPL3smSuwBLF"
  #define BLYNK_TEMPLATE_NAME "ESP32 Relay"
  #define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h>

// ============================================================
// Hardware Pin Mapping
// ============================================================
constexpr uint8_t RELAY_PIN      = 25;
constexpr uint8_t STATUS_LED_PIN = 2;
constexpr uint8_t RELAY_ON       = HIGH;
constexpr uint8_t RELAY_OFF      = LOW;

// Connected load power rating in Watts (e.g. 12W LED bulb / valve)
constexpr float LOAD_WATTS = 12.0F;

// Wi-Fi Credentials
const char *WIFI_SSID     = "YOUR_WIFI_SSID";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// System State
Preferences preferences;
BlynkTimer timer;
bool relayOn = false;
float energyWh = 0.0F;
unsigned long lastEnergyUpdate = 0;

/**
 * @brief Calculates electrical energy consumed during active intervals
 * and transmits latest kWh metrics to Blynk Virtual Pin V1.
 */
void updateEnergy() {
  const unsigned long now = millis();
  if (relayOn) {
    energyWh += LOAD_WATTS * (now - lastEnergyUpdate) / 3600000.0F;
  }
  lastEnergyUpdate = now;

  // Stream current accumulated energy (kWh) to Blynk cloud
  Blynk.virtualWrite(V1, energyWh / 1000.0F);
}

/**
 * @brief Sets relay output state and logs telemetry
 * @param on Boolean flag (true = energize, false = de-energize)
 */
void setRelay(bool on) {
  updateEnergy();
  relayOn = on;
  digitalWrite(RELAY_PIN, on ? RELAY_ON : RELAY_OFF);
  digitalWrite(STATUS_LED_PIN, on ? HIGH : LOW);
  Serial.printf("[RELAY] State changed: %s | Total Energy: %.4f kWh\n",
                on ? "ACTIVE" : "STANDBY", energyWh / 1000.0F);
}

/**
 * @brief Synchronization hook invoked when ESP32 establishes link with Blynk Cloud.
 */
BLYNK_CONNECTED() {
  Serial.println("[BLYNK] Connected to Blynk Cloud successfully.");
  Blynk.virtualWrite(V0, relayOn ? 1 : 0);
  Blynk.virtualWrite(V1, energyWh / 1000.0F);
}

/**
 * @brief Receives remote control signals from Blynk mobile / web switch (V0).
 */
BLYNK_WRITE(V0) {
  const int command = param.asInt();
  setRelay(command == 1);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n========================================================");
  Serial.println("  ESP32 Industrial IoT Relay & Power Monitor Controller");
  Serial.println("========================================================");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  setRelay(false);

  // Initialize NVS Flash storage namespace
  preferences.begin("energy", false);
  energyWh = preferences.getFloat("wattHours", 0.0F);
  lastEnergyUpdate = millis();
  Serial.printf("[NVS] Historical energy restored: %.4f kWh\n", energyWh / 1000.0F);

  // Initialize WiFi & Blynk Cloud
  Serial.print("[WIFI] Connecting to network...");
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  // Periodic Tasks:
  // Every 10 seconds: update energy consumption & cloud telemetry
  timer.setInterval(10000L, updateEnergy);

  // Every 5 minutes: save accumulated reading to NVS flash memory
  timer.setInterval(300000L, []() {
    preferences.putFloat("wattHours", energyWh);
    Serial.println("[NVS] Flash memory sync: energy metrics saved.");
  });
}

void loop() {
  Blynk.run();
  timer.run();
}
