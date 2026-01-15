# Boyfriend Bot Tamagotchi - M5StickC Plus 2

A virtual pet Tamagotchi-style game for the M5StickC Plus 2, featuring multiple interactive mechanics and a cute boyfriend character.

## 🎮 Project Overview

This project implements a digital pet (boyfriend) that you can care for, play with, and interact with using the M5StickC Plus 2's sensors and buttons. The pet has various needs (hunger, happiness, sleep, energy) that you must manage to keep it healthy and happy.

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Current Features](#current-features)
- [Known Issues & Solutions](#known-issues--solutions)
- [Troubleshooting Guide](#troubleshooting-guide)
- [Development Roadmap](#development-roadmap)
- [Testing Each Mechanic](#testing-each-mechanic)
- [Contributing](#contributing)

## 🔧 Hardware Requirements

- **M5StickC Plus 2** (ESP32-based microcontroller with built-in display, IMU, RTC, speaker)
- USB-C cable for programming and power
- Battery (built-in rechargeable)

## 💻 Software Requirements

- **PlatformIO** (VS Code extension recommended) or Arduino IDE
- **M5StickCPlus2 Library** (automatically installed via platformio.ini)
- **ESP32 Board Support** (installed via PlatformIO)

## 🚀 Installation

### Method 1: Using PlatformIO (Recommended)

1. Install Visual Studio Code
2. Install the PlatformIO IDE extension
3. Clone this repository:
   ```bash
   git clone https://github.com/saucegeo/tamagotchi-gift.git
   cd tamagotchi-gift
   ```
4. Open the project folder in VS Code
5. PlatformIO will automatically install dependencies
6. Connect your M5StickC Plus 2 via USB-C
7. Click "Upload" in PlatformIO (or press Ctrl+Alt+U)

### Method 2: Using Arduino IDE

1. Install the Arduino IDE
2. Add ESP32 board support: File → Preferences → Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Install M5StickCPlus2 library via Library Manager
4. Open `src/main.cpp` in Arduino IDE
5. Select Board: "M5Stick-C" and your COM port
6. Click Upload

## 📁 Project Structure

```
tamagotchi-gift/
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp           # Main program loop with canvas rendering
│   ├── Boyfriend.cpp      # Virtual pet class implementation
│   ├── sprites.h          # Pet class definition and sprite data
│   ├── DeviceManager.cpp  # Hardware abstraction (IMU, buttons, etc.)
│   ├── DeviceManager.h    # Device manager interface
│   ├── MechanicsManager.cpp # Game mechanics (10 features)
│   └── MechanicsManager.h   # Mechanics interface
├── include/               # Additional headers
├── lib/                   # Custom libraries
└── README.md             # This file
```

## ✨ Current Features

### Core Mechanics ✅
- **Pet Stats System**: Hunger, Happiness, Energy, Sleep, Age, Mood
- **EEPROM Persistence**: Pet state saves automatically
- **Button Controls**:
  - Button A: Increase Happiness
  - Button B: Feed (decrease Hunger)
- **Canvas-Based Rendering**: Flicker-free display using sprite buffer
- **Breathing Animation**: Character pulses for lifelike feel
- **Battery Monitor**: Real-time battery percentage display
- **Status Indicators**: Visual alerts for hunger/sleep needs

### Advanced Mechanics (Ready but needs testing) 🔧

1. **Special Events** (Date-Based)
   - Birthday detection
   - Anniversary alerts
   - Valentine's Day special
   
2. **Sleepy Head** (Time-Based)
   - Morning wakeup notifications
   - Bedtime reminders
   
3. **Shake to Clean**
   - IMU-based shake detection
   - Cleaning mini-game
   
4. **Stargazing** (Gyroscope)
   - Tilt-based stargazing mode
   - Pitch angle detection
   
5. **Blow Detection** (Microphone)
   - PDM mic input
   - Blow to play mechanic
   
6. **Battery Hangover**
   - Low battery mood effects
   
7. **Pedometer**
   - Step counting using accelerometer
   - Energy rewards for walking
   
8. **Flashlight**
   - Display-based flashlight mode
   
9. **Fortune Cookie**
   - Random daily fortunes
   - RTTTL melody playback
   
10. **Mini-Games**
    - Tilt-based challenges (currently disabled)

## 🐛 Known Issues & Solutions

### Issue 1: Screen Flickering and Rapid Reboots ✅ FIXED

**Symptoms**: Display shows "mechanics ready" then turns off/on repeatedly, device appears to reboot continuously.

**Root Cause**: Direct display writes in `MechanicsManager::begin()` and `DeviceManager::playMiniGame()` conflicted with canvas-based rendering in `main.cpp`.

**Solution Applied**:
- Removed direct `StickCP2.Display` writes from `MechanicsManager::begin()`
- Disabled `playMiniGame()` function temporarily
- All rendering now goes through the canvas system in `main.cpp`

**Status**: ✅ **FIXED** - Device should now run smoothly without flickering

### Issue 2: analogWriteResolution Compilation Error

**Symptoms**: 
```
error: too many arguments to function 'void analogWriteResolution(uint8_t)'
```

**Root Cause**: Third-party library (DFRobot_GP8XXX) uses incorrect API for ESP32.

**Solution**: 
```cpp
// In .pio/libdeps/m5stick-c-plus2/DFRobot_GP8XXX/DFRobot_GP8XXX.cpp line 303:
// Change from:
analogWriteResolution(_pin0, 16);
// To:
analogWriteResolution(16);
```

**Note**: This library isn't used by this project - safe to ignore or remove dependency.

### Issue 3: Missing loop() Function Error

**Symptoms**:
```
undefined reference to `loop()`
```

**Root Cause**: Arduino framework requires `setup()` and `loop()` functions.

**Solution**: Ensure `src/main.cpp` contains both functions (already present in this project).

## 🔍 Troubleshooting Guide

### Device Won't Upload Code

1. **Check USB Connection**
   - Use a data-capable USB-C cable (not charge-only)
   - Try a different USB port
   
2. **Driver Issues (Windows)**
   - Install CP210x USB driver from Silicon Labs
   
3. **Port Selection**
   - Verify correct COM/serial port is selected
   - On Mac: usually `/dev/cu.usbserial-*`
   - On Windows: `COM3`, `COM4`, etc.

4. **Reset During Upload**
   - Hold button A while connecting USB
   - Release after upload starts

### Display Issues

1. **Blank Screen**
   - Check battery level (charge via USB-C)
   - Press Power button (side button) to wake
   
2. **Dim Screen**
   - Adjust brightness in code: `StickCP2.Display.setBrightness(128);` (0-255)
   
3. **Flickering** (Should be fixed now)
   - Verify latest code from this repo
   - Check that no code writes directly to `StickCP2.Display` outside `main.cpp`

### Sensor Issues

1. **IMU Not Responding**
   - Calibrate: Place device flat, power cycle
   - Check `StickCP2.Imu.update()` is called regularly
   
2. **Speaker Silent**
   - Check volume: `StickCP2.Speaker.setVolume(128);` (0-255)
   - Verify speaker not muted in code

### Memory Issues

1. **Brownout Detector Reset**
   - Reduce brightness: `StickCP2.Display.setBrightness(100);`
   - Disable unused mechanics in `MechanicsManager`
   - Check battery health
   
2. **Heap Allocation Failed**
   - Reduce sprite buffer size
   - Use 8-bit color depth: `canvas.setColorDepth(8);`

## 🗺️ Development Roadmap

### Phase 1: Core Stability ✅ (Current)
- [x] Fix screen flickering issue
- [x] Implement basic pet stats
- [x] Add button controls
- [x] EEPROM persistence
- [x] Canvas rendering system

### Phase 2: Mechanics Testing 🔄 (Next)
- [ ] Test each mechanic independently
- [ ] Add debug Serial output for sensors
- [ ] Validate IMU calibration
- [ ] Test RTTTL melody system
- [ ] Implement mechanic toggle menu

### Phase 3: Visual Polish
- [ ] Design character sprites
- [ ] Create emotion animations (happy, sad, hungry, sleeping)
- [ ] Add background scenes
- [ ] Implement smooth transitions
- [ ] Create mini-game graphics

### Phase 4: Advanced Features
- [ ] WiFi weather integration
- [ ] Bluetooth pet trading
- [ ] Achievement system
- [ ] Evolution mechanics
- [ ] Multiple pet types

## 🧪 Testing Each Mechanic

### Before Testing
```cpp
// Add to main.cpp loop() for debugging:
Serial.print("Hunger: "); Serial.println(boyfriend.hunger);
Serial.print("Happiness: "); Serial.println(boyfriend.happiness);
Serial.print("Energy: "); Serial.println(boyfriend.energy);
```

### 1. Special Events (Date-Based)
```cpp
// In setup():
mechanics.setSpecialDates(3, 15, 7, 20); // Birthday Mar 15, Anniversary Jul 20

// In loop():
EventType event = mechanics.checkSpecialEvents();
if (event == EVENT_BIRTHDAY) {
  Serial.println("🎂 Birthday detected!");
  // Add birthday UI to canvas
}
```

**Test**: Set RTC to target date, verify event triggers.

### 2. Sleepy Head
```cpp
// In loop():
String sleepMsg = mechanics.checkSleepyHead();
if (sleepMsg.length() > 0) {
  Serial.println(sleepMsg);
  // Display on canvas
}
```

**Test**: Set RTC to 22:00 (bedtime) or 07:00 (wakeup), verify messages.

### 3. Shake to Clean
```cpp
// In loop():
if (mechanics.detectShake()) {
  Serial.println("🧹 Shake detected!");
  boyfriend.updateHappiness(2);
  StickCP2.Speaker.tone(1500, 100);
}
```

**Test**: Shake device vigorously, watch Serial output.

### 4. Stargazing
```cpp
// In loop():
float pitch = mechanics.getStargazingPitch();
Serial.print("Pitch: "); Serial.println(pitch);
if (mechanics.isStargazing()) {
  Serial.println("✨ Stargazing mode active!");
}
```

**Test**: Tilt device upward (face-up), verify pitch > 45°.

### 5. Blow Detection
```cpp
// In loop():
if (mechanics.detectBlow()) {
  Serial.println("💨 Blow detected!");
  boyfriend.updateHappiness(1);
}
```

**Test**: Blow into microphone hole (bottom of device).

### 6. Battery Hangover
```cpp
// In loop():
if (mechanics.checkBatteryHangover()) {
  Serial.println("🪫 Low battery - mood affected!");
  // Display sad face
}
```

**Test**: Let battery drain below 20%, verify mood change.

### 7. Pedometer
```cpp
// Call in loop():
mechanics.update(); // Tracks steps automatically

// Check count:
uint32_t steps = mechanics.getStepCount();
Serial.print("Steps: "); Serial.println(steps);
```

**Test**: Walk around with device, watch step count increase.

### 8. Flashlight
```cpp
// Toggle with button:
if (StickCP2.BtnA.wasPressed()) {
  mechanics.toggleFlashlight();
}
```

**Test**: Press button, screen should turn bright white.

### 9. Fortune Cookie
```cpp
// In loop():
String fortune = mechanics.getFortune();
Serial.println(fortune);
mechanics.playFortuneMelody(); // Plays RTTTL tune
```

**Test**: Request fortune, verify melody plays.

### 10. Mini-Game
**Status**: Currently disabled due to display conflicts.

**TODO**: Reimplement using canvas system:
```cpp
// Planned implementation:
// 1. Draw game UI to canvas
// 2. Read tilt from mechanics.getTilt()
// 3. Update canvas each frame
// 4. Avoid direct StickCP2.Display writes
```

## 📚 Useful Resources

### M5StickC Plus 2 Documentation
- [Official M5Stack Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
- [M5StickCPlus2 Library GitHub](https://github.com/m5stack/M5StickCPlus2)
- [ESP32 Arduino Reference](https://docs.espressif.com/projects/arduino-esp32/en/latest/)

### Similar Projects
- [Alegotchi](https://github.com/GaryZ88/Alegotchi) - Original inspiration
- [M5Stack Tamagotchi](https://github.com/tobozo/M5Stack-Tamagotchi)
- [ESP32 Pet Game Examples](https://github.com/topics/tamagotchi)

### Development Tools
- [PlatformIO Docs](https://docs.platformio.org/)
- [M5GFX Graphics Library](https://github.com/m5stack/M5GFX)
- [Arduino EEPROM Guide](https://docs.arduino.cc/learn/programming/eeprom-guide)

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style Guidelines
- Use descriptive variable names
- Add comments explaining WHY, not just WHAT
- Keep functions under 50 lines when possible
- Test on actual hardware before submitting

## 📝 License

This project is released into the public domain. Feel free to use, modify, and distribute as you wish.

## 🙏 Acknowledgments

- M5Stack for the excellent hardware and libraries
- Alegotchi project for inspiration
- ESP32 community for documentation and support

## 📧 Contact

- GitHub: [@saucegeo](https://github.com/saucegeo)
- Repository: [tamagotchi-gift](https://github.com/saucegeo/tamagotchi-gift)

---

**Happy pet keeping! 🎮❤️**

*Last Updated: January 15, 2026*
