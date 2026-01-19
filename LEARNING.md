# Learning Journal

**Project:** Tamagotchi Gift for M5StickC Plus 2  
**Goal:** Learn embedded C++ by understanding every line of code  
**Start Date:** January 19, 2026  
**Current C++ Experience:** 3 months  
**Current Embedded Experience:** 0 months  

---

## 🎯 The Awakening

### January 19, 2026 - Day 1

**Realization:** I've been vibecoding this project. Most code was AI-generated without my understanding.

**Commitment:** No more vibecoding. I will understand every line before moving forward.

**Today's Discoveries:**
- The project has 1197 lines in main.cpp (way too much!)
- There are 17 game states in a state machine
- The code uses M5Unified library (not M5StickCPlus2)
- IMU sensor communicates via I2C protocol
- `millis()` is used instead of `delay()` for non-blocking timing

**What I Still Don't Understand:**
- How does the state machine transition work exactly?
- What is `extern` and why are variables declared that way?
- How does RTTTL music parsing work?
- Why use double buffering for the display?
- What's the difference between I2C and SPI?

**Action Plan:**
- Read main.cpp lines 1-300 and add comments
- Study LearnCpp.com Chapter 6 (variables and scope)
- Read M5StickC Plus 2 documentation

---

## 📚 Week 1: Understanding Phase

### January 20, 2026 - Day 2

**Focus:** Hardware Deep Dive

**What I Studied:**
- M5StickC Plus 2 specifications
- MPU6886 IMU sensor basics
- ESP32 architecture

**Key Learnings:**
- **ESP32:** Dual-core @ 240MHz, 320KB RAM, 4MB Flash
- **IMU:** 6-axis sensor (3 accel + 3 gyro), I2C address 0x68
- **Accelerometer:** Measures linear acceleration in G-forces
- **Gyroscope:** Measures angular velocity in degrees/second
- **I2C:** 2-wire communication protocol (SDA + SCL)

**Hands-on Testing:**
```
Device flat on table:    accelZ = 0.98G ✓ (gravity pointing down)
Device vertical:         accelY = -0.95G ✓ (gravity on Y-axis)
Device tilted 45°:       accelX = 0.70G, accelZ = 0.70G ✓ (gravity split)
```

**Questions Answered:**
- Why `accelZ = 1.0` means flat: Gravity is 1G downward, Z-axis points up when flat
- How IMU works: I2C protocol, reads registers 0x3B-0x40 for accel data
- What G-forces are: 1G = 9.8 m/s² (Earth's gravity)

**Still Confused About:**
- How does I2C actually work at the bit level?
- What is the difference between `wasPressed()` and `isPressed()`?
- Why does the code use `extern` for global variables?

---

### January 21, 2026 - Day 3

**Focus:** State Machine Analysis

**What I Studied:**
- State machine pattern
- Button input handling
- State transitions

**Key Learnings:**
- **State Machine:** Each state handles its own logic and transitions
- **State Transitions:** Triggered by buttons, timers, or conditions
- **Cooldown Timers:** Prevent rapid state changes (attention spam)
- **Button Handling:** `wasPressed()` detects single press, `isPressed()` detects hold

**State Machine Diagram (I drew this):**
```
IDLE ──(B)──► EATING ──(2s)──► IDLE
 │
 ├─(A)──► MENU ──(select)──► GAMES ──(end)──► RESULT ──(2s)──► IDLE
 │
 └─(bad stats)──► ATTENTION ──(button)──► IDLE
```

**Code Analysis:**
```cpp
// This is in handleIdleState()
if (M5.BtnB.wasPressed()) {
    currentState = STATE_EATING;  // Change state
    stateStartTime = millis();   // Record when we entered
    boyfriend.updateHunger(-5); // Update pet stats
    M5.Speaker.tone(1500, 50);  // Play sound
}
```

**Questions Answered:**
- `wasPressed()` vs `isPressed()`: `wasPressed()` returns true once per press, `isPressed()` returns true while held
- State transitions: Change `currentState` variable, set `stateStartTime` for timing
- Cooldown timers: Use `millis() - lastTime > interval` pattern

**Still Confused About:**
- How does the main loop know which state to render?
- What happens if two buttons are pressed simultaneously?
- Why use `extern` for variables across files?

---

### January 22, 2026 - Day 4

**Focus:** Graphics and Display

**What I Studied:**
- M5Canvas and double buffering
- RGB565 color format
- Drawing functions

**Key Learnings:**
- **Double Buffering:** Draw to offscreen buffer, then push to screen (prevents flicker)
- **RGB565:** 16-bit color (5 red, 6 green, 5 blue bits)
- **Canvas:** Offscreen buffer in RAM (64KB for 135×240 display)
- **Push Sprite:** Copy buffer to actual display

**Color Format Understanding:**
```cpp
// RGB565 format
TFT_WHITE = 0b11111_000000_11111 = 0xFFFF
TFT_BLACK = 0b00000_000000_00000 = 0x0000
TFT_RED   = 0b11111_000000_00000 = 0xF800
```

**Drawing Code Analysis:**
```cpp
// This draws the character
canvas.fillCircle(centerX, centerY, 21, TFT_WHITE);  // Face
canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 7, TFT_BLACK);  // Left eye
canvas.drawArc(centerX, centerY + 3, 8, 6, 180, 360, TFT_BLACK);  // Smile
```

**Questions Answered:**
- Why double buffer: Prevents flicker during animations
- How colors work: RGB565 packs 16 bits for 24-bit color
- Drawing coordinates: (0,0) is top-left, X increases right, Y increases down

**Still Confused About:**
- How much RAM does the canvas actually use?
- What happens if I draw outside the screen bounds?
- How do sprites work vs drawing shapes?

---

### January 23, 2026 - Day 5

**Focus:** Memory and Persistence

**What I Studied:**
- ESP32 memory layout
- EEPROM emulation
- Boyfriend class save/load

**Key Learnings:**
- **Memory Layout:** 320KB RAM (variables), 4MB Flash (code + data)
- **EEPROM:** Actually stored in Flash NVS partition, not real EEPROM
- **Persistence:** Pet stats survive power-off via EEPROM writes
- **Write Cycles:** Flash has ~100,000 write cycles, so batch writes

**Memory Usage Analysis:**
```cpp
// Canvas buffer: 135×240×2 = 64,800 bytes (~64KB)
// Global variables: ~5KB
// Stack (function calls): ~8KB
// Total used: ~77KB / 320KB (24%)
```

**EEPROM Layout:**
```
Address | Data
0       | Magic byte (0xBF)
1       | Age (days)
2       | Happiness (0-24)
3       | Hunger (0-24)
4       | Energy (0-24)
5-511   | Reserved
```

**Save/Load Code Analysis:**
```cpp
void Boyfriend::save() {
    EEPROM.write(0, 0xBF);  // Magic number
    EEPROM.write(1, age);
    EEPROM.write(2, happiness);
    EEPROM.write(3, hunger);
    EEPROM.write(4, energy);
    // Note: Each write is slow, should batch!
}
```

**Questions Answered:**
- Why EEPROM: Persistent storage for pet stats
- Memory constraints: 320KB RAM is tight, need optimization
- Write cycles: Limited, so batch writes to extend life

**Still Confused About:**
- What is the difference between stack and heap?
- How does `PROGMEM` work for storing data in Flash?
- Why use `extern` for global variables?

---

### January 24, 2026 - Day 6

**Focus:** Sensors and Input

**What I Studied:**
- IMU sensor reading
- Button input
- Microphone basics

**Key Learnings:**
- **IMU Reading:** `M5.Imu.getAccelData()` returns G-forces
- **Button Input:** Digital with pull-up resistors (HIGH when idle, LOW when pressed)
- **Microphone:** PDM digital mic, I2S interface, amplitude 0-255

**IMU Reading Code:**
```cpp
// This reads accelerometer data
float accelX, accelY, accelZ;
M5.Imu.getAccelData(&accelX, &accelY, &accelZ);

// Flat detection
bool isFlat = (abs(accelZ - 1.0) < 0.2);  // Z ≈ 1G when flat
```

**Button Input Code:**
```cpp
// Button A is GPIO37, Button B is GPIO39
// Both have pull-up resistors (10kΩ to 3.3V)
// When pressed: connects to GND → reads LOW
if (M5.BtnA.wasPressed()) {
    // Handle button press
}
```

**Microphone Code:**
```cpp
// PDM microphone returns amplitude
int micLevel = M5.Mic.getLevel();  // 0-255
if (micLevel > 50) {  // Threshold for "loud"
    blowDetected = true;
}
```

**Questions Answered:**
- IMU reading: I2C communication, returns float values in G-forces
- Button debouncing: M5Unified handles this internally
- Microphone: PDM digital, amplitude-based detection

**Still Confused About:**
- How does I2C work at the register level?
- What is PDM and how does it convert to amplitude?
- Why use `extern` for variables? (I keep asking this!)

---

### January 25, 2026 - Day 7

**Focus:** Week 1 Review and Self-Exam

**What I Accomplished:**
- ✅ Read and annotated main.cpp lines 1-300
- ✅ Studied M5StickC Plus 2 hardware
- ✅ Understood IMU sensor basics
- ✅ Analyzed state machine transitions
- ✅ Learned graphics and double buffering
- ✅ Studied memory and EEPROM
- ✅ Understood sensor reading and button input

**What I Can Explain Now:**
- How the state machine works (17 states, transitions, timing)
- How IMU sensor detects flat/tilt (G-forces, gravity)
- How double buffering prevents flicker
- How EEPROM saves pet stats
- How button input works (pull-up, debouncing)
- How graphics are drawn (canvas, push sprite)

**What I Still Don't Understand:**
- `extern` keyword and global variable sharing
- I2C protocol at bit level
- PDM microphone conversion
- RTTTL music parsing
- Stack vs heap memory
- `PROGMEM` for Flash storage

**Self-Exam Results:**
- **Code Understanding:** 70% (can explain most of main.cpp)
- **Hardware Knowledge:** 60% (understand sensors, buttons, display)
- **Architecture Understanding:** 50% (state machine, memory, persistence)
- **Debugging Skills:** 40% (can use Serial.print, but not systematic)

**Week 1 Grade:** B- (Good progress, but need more depth)

---

## 📊 Learning Metrics

### Understanding Progress
- **Day 1:** 10% (realized vibecoding problem)
- **Day 2:** 25% (hardware basics)
- **Day 3:** 40% (state machine)
- **Day 4:** 50% (graphics)
- **Day 5:** 60% (memory)
- **Day 6:** 70% (sensors)
- **Day 7:** 70% (review)

### Confidence Level
- **Reading Code:** Medium (can follow most logic)
- **Modifying Code:** Low (afraid to break things)
- **Debugging:** Low (use Serial.print, but not systematic)
- **Architecture:** Medium (understand high-level, not details)

---

## 🎯 Next Week Goals

### Week 2: Building My First Feature

**Feature:** "Shake Counter Display"
- Show step count on idle screen
- Reset every 100 steps
- Add visual feedback

**Learning Goals:**
- Write code without AI help
- Debug my own mistakes
- Test on hardware
- Understand the code I write

**Success Metrics:**
- Feature works on hardware
- I can explain how it works
- No AI assistance for implementation

---

## 🤔 Reflections

### What Worked Well
- Daily documentation helped track progress
- Hands-on testing with Serial monitor
- Drawing diagrams on paper
- Taking notes in notebook

### What Didn't Work Well
- Still confused about `extern` (need to research this)
- Haven't tried modifying code yet (afraid to break)
- Haven't studied I2C protocol in depth
- Still rely on Serial.print for debugging

### What I Need to Improve
- Be more brave about modifying code
- Study low-level protocols (I2C, PDM)
- Learn systematic debugging
- Understand memory management better

---

## 📚 Resources Used

### Documentation
- [M5StickC Plus 2 Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [MPU6886 Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/MPU-6886-000193%2Bv1.1_GHIC_en.pdf)

### Learning
- [LearnCpp.com](https://www.learncpp.com/) (Chapters 6, 10)
- [Embedded Artistry Blog](https://embeddedartistry.com/)
- [Interrupt Blog](https://interrupt.memfault.com/)

### Tools
- Serial Monitor for debugging
- Notebook for notes and diagrams
- GitHub for tracking progress

---

## 🚀 Next Steps

### Immediate (Tomorrow)
- Research `extern` keyword and global variables
- Try modifying one small thing in code
- Test the modification on hardware

### This Week
- Implement "Shake Counter Display" feature
- Debug without AI help
- Document the process

### Next Week
- Start refactoring main.cpp
- Extract minigames to separate files
- Add my own comments and understanding

---

## 💭 Final Thoughts

**Week 1 was successful.** I went from vibecoding to understanding most of the codebase.

**Key insight:** Understanding takes time, but it's worth it. I can now explain how most of the project works.

**Next challenge:** Actually write code myself without AI help. This will be the real test.

**Feeling:** More confident, but still nervous about breaking things. Need to be more brave.

---

**Last Updated:** January 25, 2026  
**Current Focus:** Implement first feature without AI help  
**Next Milestone:** Complete "Shake Counter Display" feature