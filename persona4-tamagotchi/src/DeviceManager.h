// DeviceManager.h - A hardware abstraction + game state controller for M5StickC Plus 2.
//
// GOAL:
// - Centralize hardware sensor reads (battery, IMU, audio) behind simple functions.
// - Provide a simple state machine & a skeleton mini-game loop.
// - Keep main.cpp clean and easy to extend later.
//
// WHY THIS DESIGN:
// - "One class to talk to hardware": avoids scattering sensor code across your app.
// - "State machine + modular API": you can plug in new mini-games without touching the core hardware layer.
// - "Heavily commented": mentoring style to explain the reasoning step-by-step.
//
#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <M5StickCPlus2.h>   // Hardware API for display, IMU, speaker, buttons, etc.
#include <EEPROM.h>          // For non-volatile storage if needed in future features.
// We include your Boyfriend class to update pet happiness, etc.
// NOTE: Your Boyfriend class is currently declared in sprites.h.
#include "sprites.h"         // Provides class Boyfriend with update methods.

// 1) Define the game states as requested.
// WHY: An enum clearly expresses allowed states and prevents "magic numbers".
enum GameState {
  STATE_HOME,
  STATE_EATING,
  STATE_PLAYING_GAME,
  STATE_SLEEPING
};

// 2) DeviceManager: the single place to interact with the device hardware and track game state.
// WHY: Encapsulate complexity, enforce a clear API, enable modular mini-games later.
class DeviceManager {
public:
  // Constructor takes a pointer to your pet model so we can call update methods.
  // WHY: Keeps DeviceManager decoupled from global variables and makes unit testing easier.
  explicit DeviceManager(Boyfriend* pet);

  // Initialize hardware-related subsystems if needed.
  // WHY: A dedicated begin() pattern mirrors Arduino style and keeps setup logic localized.
  bool begin();

  // Optional periodic update hook for future tasks (sensor fusion, debouncing, timers).
  // WHY: Non-blocking place to run background tasks; keeps loop lean.
  void update();

  // FEATURES TO IMPLEMENT (skeleton functions):
  //  - getBatteryLevel(): Returns percentage and reports low-battery via out-parameter.
  // WHY: Centralized battery policy (e.g., "Battery Hangover" threshold).
  int getBatteryLevel(bool* isLow = nullptr);

  //  - checkShake(): Detect "Shake to Clean" using accelerometer magnitude and simple thresholding.
  // WHY: Abstract IMU access behind a simple boolean action detector.
  bool checkShake();

  //  - getTilt(): Returns -1 (Left), 0 (Center), 1 (Right) based on X-axis tilt.
  // WHY: Simple directional info for balancing mini-games without exposing raw sensor noise.
  int getTilt();

  //  - playTone(): Wrapper for short beeps or RTTTL melodies.
  // WHY: Centralize audio output. RTTTL placeholder allows future melody support.
  void playTone(int frequencyHz = 1000, int durationMs = 100, bool stop = false, const char* rtttl = nullptr);

  //  - updateWeather(): Placeholder for future WiFi weather syncing.
  // WHY: Keep network logic separate from game logic; stub now, fill later.
  void updateWeather();

  // Set/Get current game state for a simple state machine.
  // WHY: Clear state transitions make UI and logic predictable.
  void setState(GameState s);
  GameState getState() const;

  // GAME SKELETON:
  // playMiniGame(): called when the user selects "Game".
  // - Simple loop reads tilt to "win".
  // - On win, reward the pet: boyfriend.updateHappiness(5).
  // WHY: Demonstrates a non-blocking loop you can expand later.
  bool playMiniGame(unsigned long timeoutMs = 10000);

private:
  Boyfriend* boyfriend_;          // Pointer to your pet model for updating stats.
  GameState state_;         // Current game state.

  // Tunable thresholds (adjust empirically).
  // WHY: Keep thresholds centralized and self-documented.
  float shakeGThreshold_ = 2.5f;      // g-units magnitude to detect "shake".
  float tiltXThresholdG_ = 0.25f;     // +/- g threshold to decide left/right tilt.

  // Simple debouncing and timing helpers for shake detection.
  unsigned long lastShakeMs_ = 0;
  unsigned long shakeDebounceMs_ = 400;

  // Helper to read accelerometer. Centralized to keep implementation flexible.
  void readAccel(float& ax, float& ay, float& az);
};

#endif // DEVICE_MANAGER_H
