// DeviceManager.cpp - Implementation of hardware abstraction + game state controller.
//
// We explain each line and decision so you understand both the "how" and the "why".
// This file uses the global StickCP2 instance provided by the M5StickC Plus 2 library,
// following the style you already use in main.cpp.

#include "DeviceManager.h"   // Include our matching header for declarations.

// Constructor: Store a pointer to the pet for later stat updates.
// WHY: Dependency injection (pass-in) makes testing easier, avoids tight coupling.
DeviceManager::DeviceManager(Boyfriend* pet)
  : boyfriend_(pet), state_(STATE_HOME) {
  // Nothing else here; actual hardware init is in begin().
}

// begin(): Prepare subsystems if needed.
// WHY: Many libraries require init calls; we centralize that logic here.
// NOTE: Your main already calls StickCP2.begin(cfg), which initializes most components.
//       We keep begin() lightweight and return true for success.
bool DeviceManager::begin() {
  // Example place to seed random or calibrate sensors later.
  // Display a small status message to confirm startup (optional).
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.setCursor(0, 110);
  StickCP2.Display.printf("DeviceMgr ready   ");
  return true;
}

// update(): Placeholder for future periodic tasks (e.g., sensor fusion, timers).
// WHY: Maintain a single place for non-blocking recurring work outside mini-games.
void DeviceManager::update() {
  // Currently empty. Keep this so main loop can call manager.update().
  // Future ideas:
  // - Smooth tilt readings with a simple low-pass filter.
  // - Periodically check battery and trigger "hangover" state.
  // - Button debouncing beyond library support.
}

// getBatteryLevel(): Return percentage (0-100).
// Also set *isLow if below policy threshold (e.g., 20%).
// WHY: Centralized battery logic; prevent duplicated estimates across code.
//
// IMPLEMENTATION NOTE:
// - Some M5 libraries provide getBatteryVoltage() or getBatteryLevel() via Power/AXP components.
// - On M5StickC Plus 2, StickCP2.Power typically exposes battery info.
// - We fall back to a voltage-to-percent approximation if level is not directly available.
//
// APPROX MODEL:
//   v_min = 3.40V (near empty), v_max = 4.20V (full).
//   percent = clamp( (V - v_min)/(v_max - v_min) * 100 ).
int DeviceManager::getBatteryLevel(bool* isLow) {
  float voltage = 0.0f;

  // Try vendor API for voltage if available.
  // If your library uses a different API, adapt this call accordingly.
  // (Examples in M5 libraries: StickCP2.Power.getBatteryVoltage(), or AXP192 GetBatVoltage())
  if (StickCP2.Power.isBatteryConnected()) {
    voltage = StickCP2.Power.getBatteryVoltage(); // Vendor API: returns volts.
  } else {
    voltage = 3.9f; // Sensible default if not connected (e.g., USB only).
  }

  // Convert voltage to a rough percentage.
  const float vMin = 3.40f;
  const float vMax = 4.20f;
  float percentF = ((voltage - vMin) / (vMax - vMin)) * 100.0f;
  if (percentF < 0.0f) percentF = 0.0f;
  if (percentF > 100.0f) percentF = 100.0f;

  int percent = static_cast<int>(percentF + 0.5f);

  // Policy: "low battery" below 20%.
  if (isLow) {
    *isLow = (percent <= 20);
  }
  return percent;
}

// readAccel(): Helper to read accelerometer axes in g-units.
// WHY: Centralize IMU access in one function to simplify refactors later.
void DeviceManager::readAccel(float& ax, float& ay, float& az) {
  // M5StickC Plus 2 IMU access pattern; adapt if your library differs.
  // Many variants expose getAccel(ax, ay, az) or getAccelData(&ax, &ay, &az).
  // We also ensure IMU is updated before reading.
  StickCP2.Imu.update();
  StickCP2.Imu.getAccel(&ax, &ay, &az);
}

// checkShake(): Detect if the user is shaking the device.
// HOW:
//  - Compute acceleration magnitude: |a| = sqrt(ax^2 + ay^2 + az^2).
//  - Compare to a threshold (e.g., > 2.5g).
//  - Add a debounce so a single shake triggers only once per N ms.
// WHY: Simple and robust for "shake to clean".
bool DeviceManager::checkShake() {
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);

  // Calculate magnitude in g-units.
  float mag = sqrtf(ax * ax + ay * ay + az * az);

  // Basic debounce to prevent multiple triggers per shake.
  unsigned long now = millis();
  bool debounced = (now - lastShakeMs_) > shakeDebounceMs_;

  if (mag > shakeGThreshold_ && debounced) {
    lastShakeMs_ = now;
    return true;
  }
  return false;
}

// getTilt(): Return -1 (Left), 0 (Center), 1 (Right) using X-axis.
// HOW:
//  - Read accel X in g-units.
//  - If ax < -threshold => Left, if ax > +threshold => Right, else Center.
// WHY: Tiny API that mini-games can consume without learning IMU intricacies.
int DeviceManager::getTilt() {
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);

  if (ax < -tiltXThresholdG_) return -1;  // Tilted left.
  if (ax >  tiltXThresholdG_) return  1;  // Tilted right.
  return 0;                               // Center-ish.
}

// playTone(): Make a simple beep or (future) play an RTTTL melody.
// HOW:
//  - If rtttl != nullptr: placeholder hook for RTTTL playback.
//  - Else: call Speaker.tone(frequency, duration).
// WHY: Centralized audio policy (volume/duration defaults, future muting).
void DeviceManager::playTone(int frequencyHz, int durationMs, bool stop, const char* rtttl) {
  // RTTTL placeholder: to implement later with a small RTTTL parser or library.
  if (rtttl != nullptr) {
    // TODO: Implement RTTTL melody playback.
    // For now, we emit a short "start" beep as feedback.
    StickCP2.Speaker.tone(1200, 80);
    return;
  }

  if (stop) {
    // Stop any current tone.
    StickCP2.Speaker.stop();
    return;
  }

  // Simple beep.
  StickCP2.Speaker.tone(frequencyHz, durationMs);
}

// updateWeather(): Placeholder for future WiFi weather syncing.
// WHY: Keep network interactions separated and stubbed for now.
void DeviceManager::updateWeather() {
  // TODO: Connect to WiFi, fetch weather from an API, cache result.
  // For now, just print a placeholder message.
  Serial.println("Weather update placeholder");
}

// setState/getState: Basic state machine accessors.
// WHY: Encapsulate current state and allow easy transitions from UI input.
void DeviceManager::setState(GameState s) {
  state_ = s;
}

GameState DeviceManager::getState() const {
  return state_;
}

// playMiniGame(): Skeleton mini-game driven by tilt.
// RULES:
//  - Instruct user to tilt Right to win within timeout.
//  - Read tilt until win or timeout.
//  - On win: reward pet happiness via boyfriend_->updateHappiness(5).
//  - Always non-blocking friendly: short delays and early exits.
//
// WHY: Demonstrates how to structure a small loop that polls sensors,
//      updates the display minimally, and exits cleanly.
bool DeviceManager::playMiniGame(unsigned long timeoutMs) {
  // Enter game state.
  setState(STATE_PLAYING_GAME);

  // Show instructions (minimal UI).
  StickCP2.Display.fillScreen(BLACK);
  StickCP2.Display.setTextColor(YELLOW, BLACK);
  StickCP2.Display.setCursor(0, 0);
  StickCP2.Display.printf("Tilt RIGHT to win!\n");
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.setCursor(0, 20);
  StickCP2.Display.printf("Timer: %lus\n", timeoutMs / 1000);

  // Audible start cue.
  playTone(1000, 80);

  unsigned long startMs = millis();

  // Game loop: short, sensor-driven, exits on win or timeout.
  while (millis() - startMs < timeoutMs) {
    StickCP2.update();  // Keep button & peripheral states fresh.

    // Read tilt and show a tiny live indicator.
    int tilt = getTilt();
    StickCP2.Display.setCursor(0, 40);
    StickCP2.Display.printf("Tilt: %d   ", tilt);

    // Win condition: tilt RIGHT.
    if (tilt == 1) {
      // Win sound.
      playTone(1500, 100);
      playTone(1800, 100);

      // Reward pet happiness.
      // Calls your existing method implemented in Boyfriend.cpp:
      // See: Boyfriend::updateHappiness -> writes to EEPROM and clamps [0..24].
      boyfriend_->updateHappiness(5);

      // Simple "You win!" UI.
      StickCP2.Display.setCursor(0, 60);
      StickCP2.Display.setTextColor(GREEN, BLACK);
      StickCP2.Display.printf("You win!        ");

      // Brief pause for feedback, then exit.
      delay(500);
      setState(STATE_HOME);
      return true;
    }

    // Allow user to cancel via Button A (optional quality-of-life).
    if (StickCP2.BtnA.wasPressed()) {
      playTone(800, 120);
      StickCP2.Display.setCursor(0, 60);
      StickCP2.Display.setTextColor(RED, BLACK);
      StickCP2.Display.printf("Canceled        ");
      delay(300);
      setState(STATE_HOME);
      return false;
    }

    // Keep loop responsive.
    delay(30);
  }

  // Timeout -> lose condition.
  playTone(400, 160);
  StickCP2.Display.setCursor(0, 60);
  StickCP2.Display.setTextColor(RED, BLACK);
  StickCP2.Display.printf("Time up!        ");
  delay(500);

  setState(STATE_HOME);
  return false;
}
