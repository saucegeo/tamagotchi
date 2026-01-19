# Code Architecture Guide

**Purpose:** Understand how this codebase is organized and why

---

## Current State vs Target State

### Before Refactoring ❌
```
src/
├── main.cpp (1197 lines - everything in one file!)
├── Boyfriend.cpp (pet logic)
└── sprites.h (graphics data)
```

### After Refactoring ✅
```
src/
├── main.cpp (~150 lines - setup + loop only)
├── StateManager.h/cpp (game state logic)
├── GameMechanics.h/cpp (minigames)
├── SensorHandler.h/cpp (IMU, mic, battery)
├── UIRenderer.h/cpp (all drawing functions)
├── Boyfriend.h/cpp (pet state)
└── sprites.h (graphics)
```

---

## High-Level System Overview

```
┌─────────────────────────────────────────────────┐
│                   main.cpp                      │
│  ┌──────────┐                                   │
│  │  setup() │  Initialize hardware              │
│  └────┬─────┘                                   │
│       │                                          │
│  ┌────▼─────┐                                   │
│  │  loop()  │  Runs 50ms intervals (~20 FPS)   │
│  └────┬─────┘                                   │
│       │                                          │
│       ├──► SensorHandler.update()               │
│       │    ├─ Read IMU (accelerometer)          │
│       │    ├─ Check battery level               │
│       │    └─ Read microphone                   │
│       │                                          │
│       ├──► StateManager.update()                │
│       │    ├─ Handle button presses             │
│       │    ├─ Update game state                 │
│       │    └─ Apply cooldown timers             │
│       │                                          │
│       ├──► Boyfriend.update()                   │
│       │    ├─ Decay stats (hunger, energy)      │
│       │    └─ Check if attention needed         │
│       │                                          │
│       └──► UIRenderer.render()                  │
│            ├─ Clear screen                      │
│            ├─ Draw current state                │
│            └─ Push to display                   │
└─────────────────────────────────────────────────┘
```

---

## File-by-File Breakdown

### main.cpp (~150 lines after refactor)

**Responsibility:** Hardware initialization and main game loop

```cpp
#include <M5Unified.h>
#include "StateManager.h"
#include "SensorHandler.h"
#include "UIRenderer.h"
#include "Boyfriend.h"

// Global hardware objects
M5Canvas canvas(&M5.Display);
Boyfriend boyfriend;
StateManager stateManager;
SensorHandler sensors;
UIRenderer ui;

void setup() {
    M5.begin();
    canvas.createSprite(M5.Display.width(), M5.Display.height());
    boyfriend.load();  // Load from EEPROM
    sensors.begin();
    ui.begin(&canvas);
}

void loop() {
    M5.update();
    sensors.update();
    stateManager.update();
    boyfriend.update();
    ui.render(stateManager.getCurrentState());
    delay(50);  // ~20 FPS
}
```

**Why this is clean:**
- Each subsystem has its own `update()` method
- Clear separation of concerns
- Easy to understand flow
- Easy to test individual components

---

### StateManager.h/cpp (~300 lines)

**Responsibility:** Manage game states and transitions

```cpp
// StateManager.h
enum GameState {
    STATE_IDLE,
    STATE_EATING,
    STATE_SLEEPING,
    STATE_ATTENTION_NEEDED,
    STATE_MINIGAME_MENU,
    STATE_MINIGAME_CATCH,
    // ... etc
};

class StateManager {
public:
    StateManager();
    void update();  // Check buttons, apply state logic
    GameState getCurrentState() { return currentState; }
    void setState(GameState newState);
    
private:
    GameState currentState;
    unsigned long stateStartTime;
    unsigned long lastAttentionTime;
    
    // Cooldown timers
    const unsigned long ATTENTION_COOLDOWN = 10000;
    
    // State transition logic
    void handleIdleInput();
    void handleMenuInput();
    void handleGameInput();
};
```

**Key Concepts:**
- **State Machine Pattern:** Each state knows how to handle itself
- **Cooldown Timers:** Prevent rapid state changes
- **Input Handling:** Buttons trigger state transitions

**Why separate file:**
- State logic is complex (~300 lines)
- Isolates game flow from hardware
- Easy to modify states without touching main.cpp

---

### SensorHandler.h/cpp (~200 lines)

**Responsibility:** Read and interpret all sensor data

```cpp
// SensorHandler.h
struct SensorData {
    float accelX, accelY, accelZ;  // G-forces
    float gyroX, gyroY, gyroZ;     // Angular velocity
    int batteryLevel;               // 0-100%
    int micLevel;                   // Sound amplitude
    
    // Derived states
    bool isFlat;                    // Device laying down
    bool isTiltedUp;                // Pointing at sky
    bool isShaking;                 // Rapid movement
};

class SensorHandler {
public:
    void begin();
    void update();  // Read all sensors
    SensorData getData() { return data; }
    
private:
    SensorData data;
    unsigned long lastShakeTime;
    
    void readIMU();
    void readBattery();
    void readMicrophone();
    void detectMotionPatterns();
};
```

**Hardware Concepts:**

**IMU (Inertial Measurement Unit):**
- Accelerometer measures linear acceleration (gravity + movement)
- Gyroscope measures rotation
- I2C protocol (uses two wires: SDA + SCL)
- Update rate: 100Hz typical

**Why `accelZ = 1.0` means flat:**
- Gravity is 1G downward
- When device flat, Z-axis points up → measures 1.0G
- When vertical, Y-axis measures gravity instead

**Microphone:**
- PDM (Pulse Density Modulation) digital mic
- Samples at 16kHz
- Returns amplitude (0-255 typical)

**Battery:**
- Measures voltage, converts to percentage
- ADC (Analog-to-Digital Converter) reads pin
- Power management IC (AXP192) handles charging

---

### GameMechanics.h/cpp (~400 lines)

**Responsibility:** Implement all minigames and interactive mechanics

```cpp
// GameMechanics.h
class GameMechanics {
public:
    // Minigames
    void updateCatchGame(M5Canvas& canvas);
    void updateJumpGame(M5Canvas& canvas);
    
    // Interactive mechanics
    void updateFortuneCookie(M5Canvas& canvas);
    void updateMusicPlayer(M5Canvas& canvas);
    void updateStargazing(M5Canvas& canvas);
    void updateShakeClean(M5Canvas& canvas, bool isShaking);
    void updateBlowCandle(M5Canvas& canvas, int micLevel);
    void updateLoveMeter(M5Canvas& canvas);
    
    // Game state
    int getScore() { return gameScore; }
    void resetGame();
    
private:
    int gameScore;
    int objectX, objectY;
    int playerX, playerY;
    bool gameActive;
    
    // Helper functions
    void drawPlayer(M5Canvas& canvas, int x, int y);
    void drawObject(M5Canvas& canvas, int x, int y);
    void checkCollision();
};
```

**Design Pattern: Command Pattern**
- Each game is a method that takes canvas + input
- Returns void, modifies internal state
- Easy to add new games without touching other code

---

### UIRenderer.h/cpp (~300 lines)

**Responsibility:** All drawing and display logic

```cpp
// UIRenderer.h
class UIRenderer {
public:
    void begin(M5Canvas* canvasPtr);
    void render(GameState state);
    
private:
    M5Canvas* canvas;
    Boyfriend* pet;
    int animationFrame;
    
    // Drawing functions
    void drawIdleScreen();
    void drawStatsBar();
    void drawCharacter(int x, int y, bool heartEyes = false);
    void drawMenu();
    void drawAttentionAlert();
    
    // Helper utilities
    void drawBattery(int level);
    void drawProgressBar(int x, int y, int value, int max, uint32_t color);
};
```

**Graphics Concepts:**

**Double Buffering:**
```cpp
canvas.fillScreen(TFT_BLACK);     // Clear offscreen buffer
canvas.fillCircle(60, 60, 20);    // Draw to buffer
canvas.pushSprite(0, 0);          // Push buffer to screen
```

**Why:** Prevents flicker, smooth animations

**Color Format (RGB565):**
- 16-bit color: 5 bits red, 6 bits green, 5 bits blue
- `TFT_WHITE = 0xFFFF`, `TFT_BLACK = 0x0000`
- Saves memory vs 24-bit RGB

**Sprite Data:**
```cpp
const uint8_t heart_sprite[] = {
    0b00000000,  // ........
    0b01100110,  // .##..##.
    0b11111111,  // ########
    0b11111111,  // ########
    0b01111110,  // .######.
    0b00111100,  // ..####..
    0b00011000,  // ...##...
    0b00000000   // ........
};
```

**Memory:** Stored in flash (PROGMEM) not RAM

---

### Boyfriend.h/cpp (~150 lines)

**Responsibility:** Pet state and stat management

```cpp
// Boyfriend.h
class Boyfriend {
public:
    Boyfriend();
    void update();  // Decay stats over time
    void updateHunger(int delta);
    void updateHappiness(int delta);
    void updateEnergy(int delta);
    
    void save();  // Write to EEPROM
    void load();  // Read from EEPROM
    
    // Public stats
    int hunger;     // 0-24 (higher = more hungry)
    int happiness;  // 0-24 (higher = happier)
    int energy;     // 0-24 (higher = more awake)
    
private:
    unsigned long lastHungerUpdate;
    unsigned long lastHappinessUpdate;
    unsigned long lastEnergyUpdate;
    
    const unsigned long HUNGER_INTERVAL = 30000;    // Decay every 30s
    const unsigned long HAPPINESS_INTERVAL = 60000; // Decay every 60s
    const unsigned long ENERGY_INTERVAL = 45000;    // Decay every 45s
};
```

**EEPROM (Persistent Storage):**
- Non-volatile memory (survives power-off)
- 512 bytes on ESP32
- Limited write cycles (~100,000)
- Batch writes to extend life

**Memory Layout:**
```
Address | Data
--------|------------------
0       | Magic byte (0xBF)
1       | Age (days)
2       | Happiness (0-24)
3       | Hunger (0-24)
4       | Energy (0-24)
5-511   | Reserved
```

---

## Key Design Patterns Used

### 1. State Machine Pattern
**Problem:** Complex game flow with many states
**Solution:** Each state handles its own logic

```cpp
switch (currentState) {
    case STATE_IDLE:
        handleIdleState();
        break;
    case STATE_EATING:
        handleEatingState();
        break;
    // ...
}
```

### 2. Singleton Pattern
**Problem:** Only one IMU, one display, one pet
**Solution:** Global objects initialized once

```cpp
M5Canvas canvas(&M5.Display);  // Only one display
Boyfriend boyfriend;            // Only one pet
```

### 3. Update Pattern
**Problem:** Need to update many subsystems each frame
**Solution:** Every class has `update()` method

```cpp
void loop() {
    sensors.update();
    stateManager.update();
    boyfriend.update();
    ui.render();
}
```

### 4. Data-Driven Design
**Problem:** Hard-coded values make code brittle
**Solution:** Use constants and data structures

```cpp
const unsigned long HUNGER_INTERVAL = 30000;  // Easy to tune
const char* fortunes[] = { "...", "...", };  // Easy to add messages
```

---

## Memory Management

### ESP32 Memory Layout
```
┌────────────────────────┐
│  Flash (4MB)           │  Code, constants, sprites
│  ├─ Program Code       │  Read-only after upload
│  ├─ Constants          │  Stored here if using PROGMEM
│  └─ EEPROM (512 bytes) │  Persistent data
├────────────────────────┤
│  RAM (320KB)           │  Variables, stack, heap
│  ├─ Stack              │  Local variables, function calls
│  ├─ Heap               │  Dynamic allocation (avoid!)
│  └─ Global Variables   │  canvas, boyfriend, etc.
└────────────────────────┘
```

### RAM Optimization Tips

**❌ Bad: String in RAM**
```cpp
char message[] = "Hello World";  // Wastes 12 bytes of RAM
```

**✅ Good: String in Flash**
```cpp
const char message[] = "Hello World";  // Stored in flash
```

**❌ Bad: Large arrays in RAM**
```cpp
int values[1000];  // 4000 bytes of RAM!
```

**✅ Good: Use PROGMEM**
```cpp
const int values[] PROGMEM = { ... };  // Stored in flash
int val = pgm_read_word(&values[i]);   // Read when needed
```

---

## Timing and Interrupts

### Non-Blocking Code

**❌ Bad: Blocking**
```cpp
void showAnimation() {
    for (int i = 0; i < 10; i++) {
        draw(i);
        delay(100);  // BLOCKS everything for 1 second total!
    }
}
```

**✅ Good: Non-Blocking**
```cpp
void updateAnimation() {
    if (millis() - lastFrameTime > 100) {
        frame++;
        if (frame >= 10) frame = 0;
        lastFrameTime = millis();
    }
    draw(frame);
}
```

### Why `millis()` is Critical
- Returns milliseconds since boot (unsigned long)
- Doesn't block
- Rolls over after ~49 days (handle with care!)

---

## Code Quality Checklist

### For Every Function
- ✅ Less than 50 lines (ideally 20)
- ✅ Does ONE thing (single responsibility)
- ✅ Clear name (verb for actions, noun for getters)
- ✅ Comments explain WHY, not WHAT

### For Every File
- ✅ Single purpose (state management, sensors, etc.)
- ✅ Include guards in .h files
- ✅ Const correctness (mark what doesn't change)
- ✅ Less than 500 lines

### For Every Class
- ✅ Public interface is minimal
- ✅ Private data, public behavior
- ✅ No god objects (classes that do everything)

---

## Next Steps

1. **Understand current code** - Read main.cpp, add comments
2. **Extract one state handler** - Move to StateManager.cpp
3. **Extract sensor code** - Move to SensorHandler.cpp
4. **Extract UI code** - Move to UIRenderer.cpp
5. **Test after each step** - Don't break working code!

Each refactor should:
- Compile successfully
- Work identically on hardware
- Reduce main.cpp line count
- Improve readability

**Goal:** main.cpp under 200 lines, no function over 50 lines.

---

## Further Reading

- **"Clean Code" Chapter 3:** Functions (keep them small)
- **"Making Embedded Systems" Chapter 2:** System Architecture
- **M5Unified Examples:** Study how library organizes code
- **ESP32 FreeRTOS:** Advanced task management (optional)
