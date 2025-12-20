// MechanicsManager.h - Handles all mini-game mechanics and hardware interactions
// for the Boyfriend Bot Tamagotchi on M5StickC Plus 2.
//
// DESIGN PHILOSOPHY:
// - Each mechanic is a self-contained function that returns a simple value (bool, int, string).
// - Hardware complexity (IMU, Mic, Power) is abstracted behind these functions.
// - State machine flags allow easy enable/disable of features.
// - Math is explained inline for learning purposes.
//
// REQUIREMENTS:
// - M5StickCPlus2 library (for StickCP2 global instance)
// - Boyfriend class (from Sprites.h) to update pet stats
// - EEPROM for persistent step counter (optional)
//
#ifndef MECHANICS_MANAGER_H
#define MECHANICS_MANAGER_H

#include <M5StickCPlus2.h>   // Hardware API for IMU, Mic, Power, Speaker, Display
#include "Sprites.h"         // Provides Boyfriend class

// Event types for special date-based occasions.
// WHY: Enum provides type-safe event identification.
enum EventType {
  EVENT_NONE = 0,
  EVENT_BIRTHDAY,
  EVENT_ANNIVERSARY,
  EVENT_VALENTINES
};

// Simple state flags for enabling/disabling mechanics at runtime.
// WHY: Allows dynamic feature toggling without recompiling.
struct MechanicsConfig {
  bool enableSpecialEvents = true;
  bool enableSleepyHead = true;
  bool enableShakeToClean = true;
  bool enableStargazing = true;
  bool enableBlowDetection = true;
  bool enableBatteryHangover = true;
  bool enablePedometer = true;
  bool enableFlashlight = true;
  
  // Special dates (Month, Day) - configurable at runtime
  uint8_t birthdayMonth = 3;    // March (1-12)
  uint8_t birthdayDay = 15;     // 15th (1-31)
  uint8_t anniversaryMonth = 7; // July
  uint8_t anniversaryDay = 20;  // 20th
};

class MechanicsManager {
public:
  // Constructor takes pointer to pet model for stat updates.
  // WHY: Dependenpecial Events (Date-Based)
  // Checks M5.Rtc for special dates (Birthday, Anniversary, Valentine's).
  // Returns EventType enum.
  // WHY: Trigger special animations/behaviors on meaningful dates.
  EventType checkSpecialEvents();
  
  // Helper to set special dates dynamically
  void setSpecialDates(uint8_t birthMonth, uint8_t birthDay, 
                       uint8_t anniMonth, uint8_t anniDay);

  // ============================================================
  // MECHANIC 2: Scy injection keeps class testable and decoupled.
  explicit MechanicsManager(Boyfriend* pet);

  // Initialize hardware subsystems (IMU, Mic if needed).
  // WHY: Centralized init keeps setup logic clean.
  bool begin();

  // Periodic update hook for continuous mechanics (pedometer, sleepy head timer).
  // Call this every loop iteration (~100ms recommended).
  // WHY: Non-blocking state tracking for time-based mechanics.
  void update();

  // Configura3ion access
  MechanicsConfig& getConfig();

  // ============================================================
  // MECHANIC 1: Sleepy Head
  // Detects if device is laying flat on back (Z-axis gravity ~1g).
  // Returns true if laying down for > threshold seconds.
  // WHY: Simp4e sleep trigger without button interaction.
  bool checkSleepyHead(float thresholdSeconds = 5.0f);

  // ============================================================
  // MECHANIC 2: Shake to Clean
  // Detects vigorous shaking via acceleration magnitude.
  // Returns true on shake detection (with debounce).
  // WHY: Fun 5hysical interaction for cleaning mini-game.
  bool checkShakeToClean();

  // ============================================================
  // MECHANIC 3: Stargazing Mode
  // Detects if device is tilted up (Pitch angle > threshold).
  // Returns true if "looking at sky".
  // WHY: Trigger special animations or star-gazing events.
  bool checkStargazing(float pitchThresholdDeg = 60.0f);

  // ============================================================
  // MECHANIC 4: Blow Detection
  // Reads PDM microphone amplitude. Returns true if blow detected.
  // WHY: "Blow to cool food" or "Make a wish" mechanics.
  bool checkBlowDetection(int amplitudeThreshold = 200);

  // ============================================================
  // MECHANIC 6: Battery Hangover
  // Returns battery percentage. Sets *isHungover if < 20%.
  // WHY: Limit interactions when battery low (cute mechanic).
  int checkBatteryHangover(bool* isHungover = nullptr);

  // ============================================================
  // MECHANIC 7: Anime Music Player
  // Plays RTTTL melody on buzzer.
  // WHY: Add audio feedback for events (feeding, playing, etc).
  void playTune(const char* rtttl);
  
  // Pre-defined anime/game melodies in RTTTL format
  static const char* SONG_EVA;        // Evangelion - Cruel Angel's Thesis
  static const char* SONG_POKEMON;    // Pokemon Theme
  static const char* SONG_TOTORO;     // Totoro Theme
  static const char* SONG_BIRTHDAY;   // Happy Birthday
  static const char* SONG_LOVE;       // Love melody

  // ============================================================
  // MECHANIC 8: Pedometer
  // Counts steps via IMU spikes. Returns total steps since last reset.
  // Automatically increments energy every N steps.
  // WHY: Reward user for physical activity.
  int getPedometer();
  void resetPedometer();

  // ============================================================
  // MECHANIC 9: Love Meter Mini-Game
  // Returns true if user pressed Button A 10 times within timeout.
  // WHY: Simple button-mashing game to increase affection.
  bool playLoveMeter(unsigned long timeoutMs = 5000);

  // ============================================================
  // MECHANIC 10: Fortune Cookie
  // Returns a random cute compliment string.
  // WHY: Add personality and replayability.
  const char* getRandomAffection();

  // ============================================================
  // MECHANIC 11: Flashlight (Bonus)
  // Toggles screen to max brightness white or back to normal.
  // WHY: Utility feature for real-world use.
  void toggleFlashlight();

private:
  Boyfriend* boyfriend_;         // Pointer to pet for stat updates
  MechanicsConfig config_;       // Feature flags

  // --- Sleepy Head State ---
  unsigned long layingDownStartMs_ = 0;  // Timestamp when laying down detected
  bool wasLayingDown_ = false;           // Previous frame state

  // --- Shake Detection State ---
  unsigned long lastShakeMs_ = 0;
  const unsigned long shakeDebounceMs_ = 500;
  const float shakeGThreshold_ = 2.5f;  // g-units

  // --- Pedometer State ---
  int stepCount_ = 0;
  float lastAccelMag_ = 0.0f;
  unsigned long lastStepMs_ = 0;
  const unsigned long stepDebounceMs_ = 300;  // Min time between steps
  const float stepGThreshold_ = 1.3f;         // Acceleration spike threshold

  // --- Flashlight State ---
  bool flashlightOn_ = false;

  // --- Fortune Cookie Data ---
  static const int NUM_AFFECTIONS = 10;
  const char* affections_[NUM_AFFECTIONS] = {
    "You're my sunshine!",
    "Your smile brightens my day",
    "I'm so lucky to have you",
    "You make everything better",
    "My heart skips for you",
    "You're absolutely adorable",
    "Can't stop thinking about you",
    "You're my favorite person",
    "Every moment with you is precious",
    "You complete me ♡"
  };

  // --- Helper Functions ---
  void readAccel(float& ax, float& ay, float& az);
  void readGyro(float& gx, float& gy, float& gz);
  
  // Calculate pitch angle from accelerometer (for Stargazing).
  // MATH EXPLANATION:
  // Pitch = atan2(-ax, sqrt(ay^2 + az^2)) * 180/PI
  // WHY: Pitch measures rotation around Y-axis (tilting forward/back).
  float calculatePitch(float ax, float ay, float az);
  
  // Read PDM microphone amplitude (simple peak detection).
  int readMicAmplitude();
};

#endif // MECHANICS_MANAGER_H
