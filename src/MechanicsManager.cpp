// MechanicsManager.cpp - Implementation of all 10 mechanics with detailed comments.
//
// This file demonstrates:
// - IMU sensor math (acceleration, gyroscope, pitch calculation)
// - PDM microphone peak detection
// - Battery voltage-to-percentage conversion
// - Simple RTTTL melody playback
// - State machine patterns for time-based mechanics
//

#include "MechanicsManager.h"
#include "Arduino.h"  // For random(), millis(), etc.

// ============================================================
// RTTTL SONG DEFINITIONS (Static Members)
// ============================================================

// 🎵 Evangelion - Cruel Angel's Thesis (Intro)
const char* MechanicsManager::SONG_EVA = "Evangelion:d=4,o=5,b=140:8c,8e,8g,4c6,8p,8g,8e,8c6,8p,8g,8e,8c6,8p,8g,8e,8c6,8p,8g,8e,8c6,8p,8g,8e,8c6,8p,8g,8e,8c6,8p,8g,8e,8c6";

// 🎵 Pokemon Theme (Gotta Catch 'Em All)
const char* MechanicsManager::SONG_POKEMON = "Pokemon:d=4,o=5,b=160:e,e,b,g,p,e,p,c.6,p,d6,e6,d6,c6,b,a,g,f#,e,p,e,e,b,g";

// 🎵 Totoro Theme
const char* MechanicsManager::SONG_TOTORO = "Totoro:d=4,o=5,b=140:8g,8p,8c6,8g,8p,8c6,8g,8p,8c6,8a#,8a,8g,8f,8g,8a,8p,8g,8p,8f,8p,8e,8p,8d,8c";

// 🎵 Happy Birthday
const char* MechanicsManager::SONG_BIRTHDAY = "Birthday:d=4,o=5,b=120:c,c,d,c,f,e,c,c,d,c,g,f,c,c,c6,a,f,e,d,a#,a#,a,f,g,f";

// 🎵 Love Melody (Simple tune)
const char* MechanicsManager::SONG_LOVE = "Love:d=4,o=5,b=140:c,e,g,c6,e6,c6,g,e,c,p,c,e,g,c6";

// Constructor: Initialize with pet pointer and default config.
MechanicsManager::MechanicsManager(Boyfriend* pet)
  : boyfriend_(pet) {
  // Config defaults set in struct definition
}

// begin(): Initialize hardware subsystems.
// WHY: Some sensors need explicit init calls.
bool MechanicsManager::begin() {
  // IMU is auto-initialized by StickCP2.begin() in main, but we can verify here.
  StickCP2.Imu.update();
  
  // Seed random for Fortune Cookie mechanic.
  randomSeed(analogRead(0));  // Use noise from unconnected pin
  
  // Display startup message.
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.setCursor(0, 110);
  StickCP2.Display.printf("Mechanics ready  ");
  
  return true;
}

// update(): Periodic background tasks (pedometer tracking, sleepy head timer).
// Call this every loop iteration (~100ms).
// WHY: Allows continuous mechanics without blocking.
void MechanicsManager::update() {
  if (!config_.enablePedometer && !config_.enableSleepyHead) {
    return;  // Skip if both continuous mechanics disabled
  }
  
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);
  
  // --- Pedometer Logic (runs every update) ---
  if (config_.enablePedometer) {
    float accelMag = sqrtf(ax * ax + ay * ay + az * az);
    unsigned long now = millis();
    
    // Detect step: acceleration spike above threshold with debounce.
    // MATH: Total acceleration magnitude changes significantly during walking.
    // A step typically shows |a| > 1.3g followed by < 1.0g.
    bool isSpike = (accelMag > stepGThreshold_) && (lastAccelMag_ < 1.0f);
    bool debounced = (now - lastStepMs_) > stepDebounceMs_;
    
    if (isSpike && debounced) {
      stepCount_++;
      lastStepMs_ = now;
      
      // Reward: Increase energy every 20 steps.
      if (stepCount_ % 20 == 0) {
        boyfriend_->updateEnergy(1);
        StickCP2.Speaker.tone(1200, 50);  // Audio feedback
      }
    }
    
    lastAccelMag_ = accelMag;
  }
  
  // --- Sleepy Head Timer (runs every update) ---
  if (config_.enableSleepyHead) {
    // Internal state tracking happens in checkSleepyHead(),
    // but we could also track continuously here if needed.
    // For now, checkSleepyHead() is polled explicitly.
  }
}

// getConfig(): Access configuration for runtime toggling.
MechanicsConfig& MechanicsManager::getConfig() {
  return config_;
}

// ============================================================
// MECHANIC 1: Special Events (Date-Based)
// ============================================================
EventType MechanicsManager::checkSpecialEvents() {
  if (!config_.enableSpecialEvents) return EVENT_NONE;
  
  // Read current date from RTC.
  // WHY: M5StickC Plus 2 has built-in RTC (Real-Time Clock) that persists when powered.
  auto rtcDate = StickCP2.Rtc.getDate();
  
  uint8_t currentMonth = rtcDate.month;  // 1-12
  uint8_t currentDay = rtcDate.date;     // 1-31
  
  // Check Birthday
  // LOGIC: Compare month and day against configured values.
  if (currentMonth == config_.birthdayMonth && currentDay == config_.birthdayDay) {
    return EVENT_BIRTHDAY;
  }
  
  // Check Anniversary
  if (currentMonth == config_.anniversaryMonth && currentDay == config_.anniversaryDay) {
    return EVENT_ANNIVERSARY;
  }
  
  // Check Valentine's Day (hardcoded: February 14)
  // WHY: Universal holiday, no need to configure.
  if (currentMonth == 2 && currentDay == 14) {
    return EVENT_VALENTINES;
  }
  
  return EVENT_NONE;
}

// setSpecialDates(): Configure birthday and anniversary dynamically.
// WHY: Allows personalization without recompiling code.
void MechanicsManager::setSpecialDates(uint8_t birthMonth, uint8_t birthDay, 
                                       uint8_t anniMonth, uint8_t anniDay) {
  config_.birthdayMonth = birthMonth;
  config_.birthdayDay = birthDay;
  config_.anniversaryMonth = anniMonth;
  config_.anniversaryDay = anniDay;
}

// ============================================================
// MECHANIC 2: Sleepy Head
// ============================================================
bool MechanicsManager::checkSleepyHead(float thresholdSeconds) {
  if (!config_.enableSleepyHead) return false;
  
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);
  
  // Detect "laying on back": Z-axis gravity ~1g, X and Y near 0.
  // MATH: When device is flat on back, gravity pulls along +Z axis.
  // We check: az > 0.9g AND |ax| < 0.3g AND |ay| < 0.3g
  bool isLayingDown = (az > 0.9f) && (fabs(ax) < 0.3f) && (fabs(ay) < 0.3f);
  
  unsigned long now = millis();
  
  if (isLayingDown) {
    if (!wasLayingDown_) {
      // Just started laying down, record timestamp.
      layingDownStartMs_ = now;
      wasLayingDown_ = true;
    }
    
    // Check if duration exceeds threshold.
    unsigned long durationMs = now - layingDownStartMs_;
    if (durationMs > (unsigned long)(thresholdSeconds * 1000.0f)) {
      return true;  // Trigger sleep
    }
  } else {
    // Reset timer if device moved.
    wasLayingDown_ = false;
    layingDownStartMs_ = 0;
  }
  
  return false;
}

// ============================================================
// MECHANIC 2: Shake to Clean
// ============================================================
bool MechanicsManager::checkShakeToClean() {
  if (!config_.enableShakeToClean) return false;
  
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);
  
  // Calculate acceleration magnitude.
  // MATH: |a| = sqrt(ax² + ay² + az²)
  // During shake, magnitude spikes above normal gravity (1g).
  float mag = sqrtf(ax * ax + ay * ay + az * az);
  
  unsigned long now = millis();
  bool debounced = (now - lastShakeMs_) > shakeDebounceMs_;
  
  if (mag > shakeGThreshold_ && debounced) {
    lastShakeMs_ = now;
    StickCP2.Speaker.tone(1500, 80);  // Audio feedback
    return true;
  }
  
  return false;
}

// ============================================================
// MECHANIC 3: Stargazing Mode
// ============================================================
bool MechanicsManager::checkStargazing(float pitchThresholdDeg) {
  if (!config_.enableStargazing) return false;
  
  float ax = 0, ay = 0, az = 0;
  readAccel(ax, ay, az);
  
  // Calculate pitch angle (tilt forward/back).
  float pitch = calculatePitch(ax, ay, az);
  
  // Positive pitch = tilting up (looking at sky).
  return (pitch > pitchThresholdDeg);
}

// ============================================================
// MECHANIC 5: Blow Detection
// ============================================================
bool MechanicsManager::checkBlowDetection(int amplitudeThreshold) {
  if (!config_.enableBlowDetection) return false;
  
  int amplitude = readMicAmplitude();
  
  // Simple threshold check.
  // WHY: Blowing creates a loud, low-frequency noise spike.
  if (amplitude > amplitudeThreshold) {
    StickCP2.Speaker.tone(800, 100);  // Audio feedback
    return true;
  }
  
  return false;
}

// ============================================================
// MECHANIC 6: Battery Hangover
// ============================================================
int MechanicsManager::checkBatteryHangover(bool* isHungover) {
  if (!config_.enableBatteryHangover) {
    if (isHungover) *isHungover = false;
    return 100;  // Return fake 100% if disabled
  }
  
  float voltage = 0.0f;
  
  // Read battery voltage from Power module.
  if (StickCP2.Power.getBatteryCurrent()) {
    voltage = StickCP2.Power.getBatteryVoltage();
  } else {
    voltage = 4.0f;  // Default if USB-only (no battery)
  }
  
  // Convert voltage to percentage.
  // MATH: LiPo battery range ~3.3V (empty) to 4.2V (full).
  // Linear approximation: % = (V - 3.3) / (4.2 - 3.3) * 100
  const float vMin = 3.3f;
  const float vMax = 4.2f;
  float percentF = ((voltage - vMin) / (vMax - vMin)) * 100.0f;
  
  // Clamp to 0-100 range.
  if (percentF < 0.0f) percentF = 0.0f;
  if (percentF > 100.0f) percentF = 100.0f;
  
  int percent = static_cast<int>(percentF + 0.5f);
  
  // Set hangover flag if below 20%.
  if (isHungover) {
    *isHungover = (percent < 20);
  }
  
  return percent;
}

// ============================================================
// MECHANIC 7: Anime Music Player
// ============================================================
void MechanicsManager::playTune(const char* rtttl) {
  if (rtttl == nullptr) return;
  
  // RTTTL FORMAT SPECIFICATION:
  // "name:d=default_duration,o=default_octave,b=bpm:notes"
  //
  // DURATION: 1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth, 32=thirty-second
  // OCTAVE: 4-7 (5 is middle C)
  // BPM: Beats per minute (tempo)
  // NOTES: Comma-separated list
  //   Format: [duration]note[octave][dot]
  //   - duration: optional, overrides default
  //   - note: c, c#, d, d#, e, f, f#, g, g#, a, a#, b, p (pause)
  //   - octave: optional, overrides default
  //   - dot: . extends duration by 50%
  //
  // EXAMPLE: "8c6" = eighth-note C in octave 6
  //          "4f#." = dotted quarter-note F# (default octave)
  //          "p" = pause (rest)
  
  // --- STEP 1: Parse Header ---
  // Find the colon separators
  const char* ptr = rtttl;
  
  // Skip name (up to first colon)
  while (*ptr && *ptr != ':') ptr++;
  if (!*ptr) return;  // Invalid format
  ptr++;  // Skip colon
  
  // Parse defaults: d=4,o=5,b=120
  int defaultDuration = 4;
  int defaultOctave = 5;
  int bpm = 120;
  
  // Read default values
  while (*ptr && *ptr != ':') {
    if (*ptr == 'd' && *(ptr + 1) == '=') {
      ptr += 2;
      defaultDuration = atoi(ptr);
    } else if (*ptr == 'o' && *(ptr + 1) == '=') {
      ptr += 2;
      defaultOctave = atoi(ptr);
    } else if (*ptr == 'b' && *(ptr + 1) == '=') {
      ptr += 2;
      bpm = atoi(ptr);
    }
    
    // Skip to next parameter or colon
    while (*ptr && *ptr != ',' && *ptr != ':') ptr++;
    if (*ptr == ',') ptr++;
  }
  
  if (!*ptr || *ptr != ':') return;  // Invalid format
  ptr++;  // Skip second colon
  
  // Calculate note duration in milliseconds
  // Formula: (60000 * 4) / (bpm * duration)
  // WHY: 60000 ms/min, multiply by 4 for quarter note basis
  int wholeNoteDuration = (60000 * 4) / bpm;
  
  // --- STEP 2: Parse and Play Notes ---
  while (*ptr) {
    // Skip whitespace and commas
    while (*ptr == ' ' || *ptr == ',') ptr++;
    if (!*ptr) break;
    
    // Parse duration (optional)
    int duration = defaultDuration;
    if (isdigit(*ptr)) {
      duration = atoi(ptr);
      while (isdigit(*ptr)) ptr++;
    }
    
    // Parse note (c, c#, d, d#, e, f, f#, g, g#, a, a#, b, p)
    char note = tolower(*ptr++);
    bool isSharp = false;
    
    if (*ptr == '#') {
      isSharp = true;
      ptr++;
    }
    
    // Parse octave (optional)
    int octave = defaultOctave;
    if (isdigit(*ptr)) {
      octave = *ptr - '0';
      ptr++;
    }
    
    // Parse dot (extends duration by 50%)
    bool isDotted = false;
    if (*ptr == '.') {
      isDotted = true;
      ptr++;
    }
    
    // Calculate note duration in milliseconds
    int noteDuration = wholeNoteDuration / duration;
    if (isDotted) {
      noteDuration += noteDuration / 2;  // Add 50%
    }
    
    // Convert note to frequency
    // WHY: Musical notes follow exponential frequency spacing
    // Formula: f = 440 * 2^((n-49)/12) where n is MIDI note number
    int frequency = 0;
    
    if (note != 'p') {  // 'p' is a pause/rest
      // Map note letter to semitone offset within octave
      int semitone = 0;
      switch (note) {
        case 'c': semitone = 0; break;
        case 'd': semitone = 2; break;
        case 'e': semitone = 4; break;
        case 'f': semitone = 5; break;
        case 'g': semitone = 7; break;
        case 'a': semitone = 9; break;
        case 'b': semitone = 11; break;
        default: semitone = 0; break;
      }
      
      if (isSharp) semitone++;
      
      // Calculate MIDI note number (C4 = 60)
      int midiNote = (octave * 12) + semitone;
      
      // Frequency lookup table (more accurate than formula for real-time)
      // Based on A4 = 440 Hz
      static const int frequencies[] = {
        // Octave 4
        262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
        // Octave 5
        523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
        // Octave 6
        1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
        // Octave 7
        2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
      };
      
      // Map MIDI note to frequency array index
      int freqIndex = midiNote - 48;  // C4 = 0 in our table
      if (freqIndex >= 0 && freqIndex < 48) {
        frequency = frequencies[freqIndex];
      }
    }
    
    // Play the note
    if (frequency > 0) {
      StickCP2.Speaker.tone(frequency, noteDuration);
      delay(noteDuration);
    } else {
      // Pause/rest
      StickCP2.Speaker.stop();
      delay(noteDuration);
    }
    
    // Add small gap between notes for clarity (10% of note duration)
    StickCP2.Speaker.stop();
    delay(noteDuration / 10);
  }
  
  // Ensure speaker is stopped at end
  StickCP2.Speaker.stop();
}

// ============================================================
// MECHANIC 8: Pedometer
// ============================================================
int MechanicsManager::getPedometer() {
  return stepCount_;
}

void MechanicsManager::resetPedometer() {
  stepCount_ = 0;
  lastAccelMag_ = 0.0f;
  lastStepMs_ = 0;
}

// ============================================================
// MECHANIC 9: Love Meter Mini-Game
// ============================================================
bool MechanicsManager::playLoveMeter(unsigned long timeoutMs) {
  // UI: Show instructions.
  StickCP2.Display.fillScreen(BLACK);
  StickCP2.Display.setTextColor(MAGENTA, BLACK);
  StickCP2.Display.setCursor(0, 0);
  StickCP2.Display.printf("LOVE METER!\n");
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.printf("Press A 10 times!\n");
  StickCP2.Display.printf("Time: %lus\n", timeoutMs / 1000);
  
  StickCP2.Speaker.tone(1000, 100);
  delay(120);
  
  int pressCount = 0;
  unsigned long startMs = millis();
  
  // Game loop: count button presses until 10 or timeout.
  while (millis() - startMs < timeoutMs) {
    StickCP2.update();
    
    if (StickCP2.BtnA.wasPressed()) {
      pressCount++;
      StickCP2.Speaker.tone(1200 + (pressCount * 100), 80);
      
      // Update UI with count.
      StickCP2.Display.setCursor(0, 40);
      StickCP2.Display.printf("Presses: %d/10  ", pressCount);
      
      // Win condition: 10 presses.
      if (pressCount >= 10) {
        StickCP2.Display.setCursor(0, 60);
        StickCP2.Display.setTextColor(GREEN, BLACK);
        StickCP2.Display.printf("YOU WIN! +10 ♡  ");
        StickCP2.Speaker.tone(2000, 200);
        delay(300);
        
        // Reward: Big happiness boost.
        boyfriend_->updateHappiness(10);
        
        delay(500);
        return true;
      }
    }
    
    delay(30);  // Keep loop responsive
  }
  
  // Timeout: lose condition.
  StickCP2.Display.setCursor(0, 60);
  StickCP2.Display.setTextColor(RED, BLACK);
  StickCP2.Display.printf("Time up! :(     ");
  StickCP2.Speaker.tone(400, 200);
  delay(500);
  
  return false;
}

// ============================================================
// MECHANIC 10: Fortune Cookie
// ============================================================
const char* MechanicsManager::getRandomAffection() {
  int index = random(0, NUM_AFFECTIONS);
  return affections_[index];
}

// ============================================================
// MECHANIC 11: Flashlight (Bonus)
// ============================================================
void MechanicsManager::toggleFlashlight() {
  if (!config_.enableFlashlight) return;
  
  flashlightOn_ = !flashlightOn_;
  
  if (flashlightOn_) {
    // Turn screen white at max brightness.
    StickCP2.Display.setBrightness(255);
    StickCP2.Display.fillScreen(WHITE);
    StickCP2.Speaker.tone(1000, 50);
  } else {
    // Restore normal brightness and clear screen.
    StickCP2.Display.setBrightness(128);
    StickCP2.Display.fillScreen(BLACK);
    StickCP2.Speaker.tone(800, 50);
  }
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

// readAccel(): Get accelerometer values in g-units.
void MechanicsManager::readAccel(float& ax, float& ay, float& az) {
  StickCP2.Imu.update();
  StickCP2.Imu.getAccel(&ax, &ay, &az);
}

// readGyro(): Get gyroscope values in degrees/second.
void MechanicsManager::readGyro(float& gx, float& gy, float& gz) {
  StickCP2.Imu.update();
  StickCP2.Imu.getGyro(&gx, &gy, &gz);
}

// calculatePitch(): Compute pitch angle from accelerometer.
// MATH EXPLANATION:
// Pitch measures rotation around Y-axis (tilting forward/back).
// Formula: pitch = atan2(-ax, sqrt(ay² + az²)) * 180/π
// WHY: atan2 gives signed angle; negative ax for correct orientation.
// Returns angle in degrees (-90 to +90).
float MechanicsManager::calculatePitch(float ax, float ay, float az) {
  return atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / PI;
}

// readMicAmplitude(): Simple PDM microphone peak detection.
// IMPLEMENTATION NOTE:
// M5StickC Plus 2 has a PDM microphone (SPM1423).
// The M5 library provides Mic.record() or similar API.
// Here's a simplified placeholder using averaged samples.
int MechanicsManager::readMicAmplitude() {
  // TODO: Implement actual PDM microphone reading.
  // Example API (check M5StickCPlus2 docs):
  // int16_t samples[128];
  // StickCP2.Mic.record(samples, 128, 16000);  // 16kHz sample rate
  // Then calculate peak or RMS amplitude.
  
  // PLACEHOLDER: Return random value for testing.
  // Replace with real mic implementation.
  return random(0, 300);
}
