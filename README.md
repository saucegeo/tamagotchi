# Tamagotchi Gift Project

**A custom virtual pet for M5StickC Plus 2 | Learning embedded C++ | From vibecoding to understanding**

> **Current Status:** Week 1 of learning journey - Understanding existing code before adding features  
> **Started:** January 19, 2026  
> **Goal:** Feb 18, 2026 (30-day sprint)  
> **Approach:** No more vibecoding - understand every line before moving forward

---

## 🎯 Project Evolution

### Phase 1: Initial Implementation (Jan 1-18)
- ✅ Core game mechanics (state machine with 17 states)
- ✅ 10 sensor-based features (IMU, microphone, battery)
- ✅ 2 minigames (Catch, Jump)
- ✅ Save/load system (EEPROM persistence)
- ✅ Special days detection (Valentine's, birthdays)
- ❌ **Problem:** Most code was AI-generated without understanding

### Phase 2: Learning & Understanding (Jan 19-25) - CURRENT WEEK
- 🔄 **Goal:** Understand every line of existing code
- 🔄 **Method:** Read docs, study hardware, annotate code
- 🔄 **No new features** until understanding is complete

### Phase 3: Refactoring & Ownership (Jan 26-Feb 8)
- 📋 **Goal:** Refactor vibecoded code into my own clean architecture
- 📋 **Method:** Extract minigames, organize files, add my comments

### Phase 4: Personalization & Polish (Feb 9-18)
- 🎨 **Goal:** Add custom sprites, messages, music
- 🎨 **Method:** Create original content, test thoroughly

---

## 📚 Learning Journey (Documented in Commits)

### Jan 19, 2026 - The Awakening
**Commit:** `Start: Document learning journey`
- Recognized vibecoding problem
- Committed to understanding code properly
- Created LEARNING.md to track progress

### Jan 20, 2026 - Hardware Deep Dive
**Commit:** `Learn: Study M5StickC Plus 2 hardware`
- Read device specifications
- Understood IMU sensor basics
- Tested sensor values with Serial Monitor

### Jan 21, 2026 - State Machine Analysis
**Commit:** `Learn: Document state machine transitions`
- Drew state diagram on paper
- Traced button input to state changes
- Understood cooldown timers and race conditions

### [Continue adding daily commits...]

---

## 🎮 Features (I Understand These Now)

### Core Mechanics
- **Virtual Pet Simulation:** Hunger, happiness, energy stats decay over time
- **State Machine:** 17 states with smooth transitions (IDLE → EATING → SLEEPING, etc.)
- **Persistent Storage:** Pet state survives power-off via EEPROM
- **Special Days:** Heart eyes on Valentine's Day (Feb 14), birthdays

### Sensor-Based Interactions (I Understand How These Work)

**1. Battery Hangover** - Slows animations when battery <20%  
**2. Fortune Cookie** - 8 random fortune messages (will personalize)  
**3. Anime Music Player** - RTTTL format music parsing  
**4. Sleepy Head** - Auto-sleep when device flat (accelZ ≈ 1.0G)  
**5. Stargazing** - Shows stars when tilted up (accelY < -0.7)  
**6. Shake to Clean** - Shake detection via accelerometer  
**7. Pedometer** - Counts steps from rhythmic motion  
**8. Blow to Cool** - Microphone detects blowing (PDM digital)  
**9. Love Meter** - Button mashing game  
**10. Base Minigames** - Catch game, Jump game  

---

## 🛠️ Hardware (I Understand This Now)

**Device:** M5StickC Plus 2

**Key Components I Understand:**
- **CPU:** ESP32-PICO-V3-02 (Dual-core @ 240MHz)
- **RAM:** 320KB SRAM (limited, need optimization)
- **Flash:** 4MB (stores code, constants, EEPROM)
- **Display:** 1.14" LCD (135×240 pixels, RGB565, double-buffered)
- **IMU:** MPU6886 (6-axis: accel + gyro, I2C protocol)
- **Microphone:** SPM1423 (PDM digital, I2S interface)
- **Battery:** 200mAh LiPo (~2 hour runtime)
- **Buttons:** A (GPIO37), B (GPIO39) - digital input with pull-up
- **Buzzer:** Passive speaker (GPIO2, PWM-driven)

---

## 🏗️ Code Architecture (Current State)

### Current Structure (What I'm Learning From)
```
src/
├── main.cpp (1197 lines - monolithic, needs refactoring)
├── Boyfriend.cpp (pet logic - I understand this)
├── sprites.h (graphics data - I understand this)
├── GameState.h (shared types - I created this)
├── minigames/
│   ├── CatchGame.cpp (extracted, I understand physics)
│   └── JumpGame.cpp (extracted, I understand jump arc)
└── states/ (empty - will populate during refactoring)
```

### Target Structure (What I'll Build)
```
src/
├── main.cpp (~150 lines - setup + loop only)
├── GameState.h (shared types - DONE)
├── states/
│   ├── CoreStates.cpp (Idle, Eating, Sleeping, Attention)
│   ├── Minigames.cpp (Menu, Catch, Jump, Result)
│   └── Mechanics.cpp (Fortune, Dance, Stargazing, etc.)
├── minigames/ (individual game files - STARTED)
├── SensorHandler.cpp (IMU, mic, battery - TO DO)
├── UIHelpers.cpp (drawing functions - TO DO)
├── Boyfriend.h/cpp (pet state - EXISTING)
└── sprites.h (graphics - EXISTING)
```

---

## 🔍 Debugging (I Know How to Do This Now)

### Serial Monitor Output
Every 2 seconds, device prints:
```
State:0 Flat:0 TiltUp:0 AccZ:0.98 AccY:-0.12 BtnA:0 BtnB:0
```

**I understand what each value means:**
- `State`: Current game state (0-16)
- `Flat`: Device laying flat (0/1) - based on accelZ ≈ 1.0G
- `TiltUp`: Device tilted up (0/1) - based on accelY < -0.7
- `AccZ`: Z-axis acceleration (G-forces) - gravity sensor
- `AccY`: Y-axis acceleration (G-forces) - tilt sensor
- `BtnA`: Button A pressed (0/1) - digital input
- `BtnB`: Button B pressed (0/1) - digital input

### Common Debug Tasks I Can Do
```cpp
// Check sensor readings
Serial.printf("IMU: X=%.2f Y=%.2f Z=%.2f\n", accelX, accelY, accelZ);

// Check state transitions
Serial.printf("State change: %d -> %d\n", oldState, newState);

// Check memory usage
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

---

## 📖 Learning Resources (What I'm Using)

### Documentation
- **[M5StickC Plus 2 Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)** - Hardware specs
- **[M5Unified GitHub](https://github.com/m5stack/M5Unified)** - Library examples
- **[MPU6886 Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/MPU-6886-000193%2Bv1.1_GHIC_en.pdf)** - IMU sensor

### Learning Guides
- **[LEARNING_RESOURCES.md](docs/LEARNING_RESOURCES.md)** - My C++/embedded roadmap
- **[HARDWARE_GUIDE.md](docs/HARDWARE_GUIDE.md)** - Deep hardware dive
- **[CODE_ARCHITECTURE.md](docs/CODE_ARCHITECTURE.md)** - Code structure guide
- **[30_DAY_SPRINT_PLAN.md](docs/30_DAY_SPRINT_PLAN.md)** - My learning plan

### Online Courses
- **[LearnCpp.com](https://www.learncpp.com/)** - C++ fundamentals (Chapters 6, 10, 12-13)
- **[Embedded Artistry Blog](https://embeddedartistry.com/)** - Embedded systems concepts
- **[Interrupt Blog](https://interrupt.memfault.com/)** - Firmware debugging

### Books
- **"Making Embedded Systems"** by Elecia White (Chapters 1-4)
- **"Clean Code"** by Robert C. Martin (Chapters 3, 10)

---

## 🚀 Getting Started (For Others)

### Prerequisites
1. **Hardware:** M5StickC Plus 2 + USB-C cable
2. **Software:** PlatformIO (already installed at `~/.platformio/`)
3. **Knowledge:** Basic C++ (variables, functions, classes)

### Building and Flashing
```bash
# Compile code
~/.platformio/penv/bin/pio run

# Upload to device
~/.platformio/penv/bin/pio run -t upload

# Open serial monitor (debug output)
~/.platformio/penv/bin/pio device monitor -b 115200
```

### First Time Setup
1. Clone repository
2. Open in VS Code with PlatformIO extension
3. Connect M5StickC Plus 2 via USB-C
4. Build and upload
5. Device will initialize with default pet stats

---

## 🎨 Customization (What I Plan to Add)

### 1. Character Sprites
- **Tools:** Aseprite (paid) or Piskel (free)
- **Process:** Create 16×16 sprite → Convert to C array → Replace in sprites.h
- **Status:** Will do Week 3

### 2. Fortune Messages
- **Current:** 8 generic messages
- **Plan:** 20 personal messages (inside jokes, memories)
- **Status:** Will do Week 3

### 3. Custom Music
- **Current:** Evangelion theme (RTTTL)
- **Plan:** Her favorite song in RTTTL format
- **Status:** Will do Week 3

---

## 🐛 Current Issues (I'm Working On These)

### Fixed ✅
- [x] Microphone API incorrect parameters
- [x] Button race condition (state loops)
- [x] Auto-sleep triggering in menus
- [x] Attention state spam

### In Progress 🔄
- [ ] Main.cpp is 1197 lines (refactoring Week 3)
- [ ] Generic character sprite (personalization Week 3)
- [ ] Generic fortune messages (personalization Week 3)
- [ ] Memory usage optimization (Week 4)

---

## 📊 Project Metrics

### Code Quality
- **Lines of Code:** 1197 (main.cpp) → Target: ~300 total
- **Functions:** 17 state handlers → Target: 1 per file
- **Comments:** Minimal → Target: Detailed documentation
- **Test Coverage:** None → Target: Hardware testing

### Learning Progress
- **C++ Understanding:** 3 months experience → Target: Embedded mastery
- **Hardware Knowledge:** Beginner → Target: ESP32 expert
- **Debugging Skills:** Basic → Target: Systematic methodology
- **Architecture Understanding:** Vibecoded → Target: Clean design

---

## 🤝 Contributing

**This is a personal learning project, but if you're learning embedded C++:**

1. **Fork this repo**
2. **Try refactoring main.cpp yourself**
3. **Compare your solution to mine**
4. **Learn by doing!**

**For collaboration:**
- Open issues for questions
- Pull requests for improvements
- Discussions for learning

---

## 📝 License

Personal project - code is yours to learn from and modify.

**M5Unified Library:** MIT License (see library repo)

---

## 🙏 Acknowledgments

- **M5Stack** - Hardware and library
- **Espressif** - ESP32 platform
- **PlatformIO** - Build system
- **LearnCpp.com** - C++ education
- **My girlfriend** - Inspiration for this project 💕

---

## 📞 Contact

**For Interview/Portfolio Purposes:**

This project demonstrates my journey from vibecoding to understanding embedded systems:

**What I learned:**
- Embedded C++ programming on ESP32
- State machine design and implementation
- Sensor fusion (IMU + microphone + battery)
- Memory management (RAM vs Flash optimization)
- Hardware interfacing (I2C, SPI, I2S, PWM)
- Debugging methodology and systematic problem-solving
- Code architecture and refactoring
- Clean code principles

**Questions? Check the documentation first:**
- Technical questions → [HARDWARE_GUIDE.md](docs/HARDWARE_GUIDE.md)
- Code structure → [CODE_ARCHITECTURE.md](docs/CODE_ARCHITECTURE.md)
- Learning path → [LEARNING_RESOURCES.md](docs/LEARNING_RESOURCES.md)
- My journey → [LEARNING.md](LEARNING.md)

---

## 🎯 Next Steps (My Plan)

### Week 1 (Jan 19-25): Understanding
- [x] Create LEARNING.md to track progress
- [ ] Read and annotate main.cpp (lines 1-300)
- [ ] Study M5StickC Plus 2 hardware
- [ ] Understand IMU sensor basics
- [ ] Document state machine transitions

### Week 2 (Jan 26-Feb 1): First Feature
- [ ] Add "Shake Counter Display" feature
- [ ] Create custom sprite in Piskel
- [ ] Test and debug without AI help

### Week 3 (Feb 2-8): Refactoring
- [ ] Extract minigames to separate files
- [ ] Organize code into logical modules
- [ ] Add detailed comments

### Week 4 (Feb 9-18): Personalization
- [ ] Add 20 personal fortune messages
- [ ] Add custom music
- [ ] Polish and test everything

---

**Last Updated:** January 19, 2026  
**Project Status:** Learning Phase - Understanding existing code  
**Days Until Deadline:** 31 days  
**Current Focus:** Read and understand main.cpp lines 1-300