/*
 * ============================================================
 *  ESP32 Relay Autonomous Controller (Arduino IDE Example)
 *  Author: Vivek Bharat Toradmal (https://github.com/vivek5391)
 *
 *  Features:
 *  - Automated safety cycle & manual actuation loop
 *  - Optocoupler-isolated relay driving on GPIO 25/26
 *  - Board status indicator on GPIO 2
 * ============================================================
 */

#define RELAY_PIN   25
#define LED_PIN      2

// Set logic level depending on your relay module (Active-HIGH vs Active-LOW)
#define RELAY_ON    HIGH
#define RELAY_OFF   LOW

#define CYCLE_ON_MS   5000   // 5 seconds Active
#define CYCLE_OFF_MS  5000   // 5 seconds Inactive

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN,   OUTPUT);

  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_PIN,   LOW);

  Serial.println("=========================================");
  Serial.println("  ESP32 Industrial Relay Controller Init ");
  Serial.println("=========================================");
}

void loop() {
  // Energize
  digitalWrite(RELAY_PIN, RELAY_ON);
  digitalWrite(LED_PIN,   HIGH);
  Serial.println("[CYCLE] Relay State -> ACTIVE (ON)");
  delay(CYCLE_ON_MS);

  // De-energize
  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_PIN,   LOW);
  Serial.println("[CYCLE] Relay State -> STANDBY (OFF)");
  delay(CYCLE_OFF_MS);
}
