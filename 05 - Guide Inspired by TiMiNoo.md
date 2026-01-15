# 🎮 Boyfriend Bot - Learning Assignment

## 📖 About This Assignment

This assignment is inspired by three reference projects:
- **[TiMiNoo](https://github.com/TME520/TiMiNoo)** - A cute 1-bit cat Tamagotchi for Arduino (feed, cuddle, clean, educate your cat!)
- **[OpenELAB M5StickCPlus2 Slot](https://github.com/OpenELAB/OpenELAB-M5StickCPlus2-Slot)** - A slot machine game showing M5GFX sprite techniques
- **[M5StickC-Plus](https://github.com/m5stack/M5StickC-Plus)** - Official M5Stack library (deprecated in favor of M5Unified/M5GFX)

**Your Code Status**: You already have a working foundation! Your `main.cpp` includes:
- ✅ Proper hardware initialization with GPIO4 power hold
- ✅ EEPROM persistence with magic byte detection
- ✅ Progress bars for stats display
- ✅ Simple breathing animation for the character
- ✅ Mood-based expressions (happy/sad eyes and mouth)
- ✅ Stat decay timers using `millis()`
- ✅ Button input for feeding and playing

Each task includes:
- 🎯 **Goal** - What you're building
- 💡 **Hints** - Concepts and code snippets to guide you
- 🧪 **Test** - How to verify your implementation
- 📚 **Learn** - What programming concept this teaches

---

## 🌟 Part 1: Core Stability (Already Done!)

### Task 1.1: The Minimal Bootable Program ✅ COMPLETED

**Your current `main.cpp` already has proper initialization!**

```cpp
// Your working setup() pattern:
void setup() {
    // 1. Hardware first
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    
    // 2. CRITICAL: Hold power on (required for M5StickC Plus 2!)
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    
    // 3. Display setup
    StickCP2.Display.setRotation(1);
    StickCP2.Display.setBrightness(80);
    StickCP2.Display.fillScreen(TFT_BLACK);
    StickCP2.Display.setTextColor(TFT_WHITE);
    StickCP2.Display.setCursor(10, 50);
    StickCP2.Display.print("Starting...");
    
    // 4. EEPROM initialization with magic byte
    EEPROM.begin(512);
    // ... first boot detection ...
}
```

**Key insight from OpenELAB Slot project**: They use `M5.Lcd.setSwapBytes(true)` for proper color handling with sprites. Consider adding this if you add RGB565 sprites later.

**Learn**: Proper initialization order prevents flickering and boot loops!

---

### Task 1.2: Button Detection ✅ COMPLETED

**Your code already handles buttons correctly!**

```cpp
// Your working button pattern in loop():
StickCP2.update();  // MUST call this every loop!

if (StickCP2.BtnA.wasPressed()) {
    boyfriend.updateHappiness(2);
    StickCP2.Speaker.tone(1000, 50);
}

if (StickCP2.BtnB.wasPressed()) {
    boyfriend.updateHunger(-2);
    if (boyfriend.hunger < 0) boyfriend.hunger = 0;
    StickCP2.Speaker.tone(1500, 50);
}
```

**How TiMiNoo handles buttons** (for reference):
```cpp
// TiMiNoo uses debouncing with timing:
void checkButton() {
    unsigned long currentTime = millis();
    boolean buttonIsPressed = digitalRead(ButtonPin) == HIGH;
    
    if (buttonIsPressed != ButtonWasPressed &&
        currentTime - ButtonStateChangeTime > DebounceTime) {
        ButtonWasPressed = buttonIsPressed;
        ButtonStateChangeTime = currentTime;
        
        if (!ButtonWasPressed) {
            // Button was just released - take action
        }
    }
}
```

**Note**: M5StickCPlus2 library handles debouncing for you, so `wasPressed()` is simpler!

**Learn**: Polling input systems and the importance of calling `update()` every frame

---

### Task 1.3: Frame Rate Control ⭐⭐ (Enhancement Opportunity)

**Goal**: Upgrade from direct drawing to double buffering for smoother graphics.

**Your current approach** (direct drawing - works but may flicker):
```cpp
void loop() {
    // ...
    StickCP2.Display.fillScreen(TFT_BLACK);  // Clear
    // Draw everything...
    delay(50);  // ~20 FPS
}
```

**TiMiNoo's approach** (using U8glib's page buffer):
```cpp
// TiMiNoo uses u8g.firstPage() / u8g.nextPage() for buffered rendering
u8g.firstPage();
do {
    // All drawing happens here
    u8g.drawXBMP(8, 8, cat_width, cat_height, cat_bits);
} while( u8g.nextPage() );
delay(10);  // Small delay for stability
```

**OpenELAB Slot approach** (using M5GFX sprites):
```cpp
// From OpenELAB: They use pushImage for sprites
M5.Lcd.pushImage(x, y + PAD_Y, SYM_WIDTH, SYM_HEIGHT, data);

// With byte swapping for proper colors:
M5.Lcd.setSwapBytes(true);
```

**Upgrade to M5Canvas (double buffering)**:
```cpp
// Global: Create an off-screen canvas
M5Canvas canvas(&StickCP2.Display);

void setup() {
    // ... hardware init ...
    
    // Create sprite AFTER StickCP2.begin()
    canvas.createSprite(240, 135);  // Full screen buffer
}

void loop() {
    canvas.fillScreen(TFT_BLACK);   // Draw to buffer (not visible yet)
    canvas.setCursor(10, 50);
    canvas.print("Smooth!");
    
    // Draw your progress bars, character, etc. to canvas
    drawProgressBar(canvas, 5, 20, 80, 8, boyfriend.hunger, 24, TFT_ORANGE);
    drawCharacter(canvas, 150, 75);
    
    canvas.pushSprite(0, 0);        // Copy entire buffer to screen at once
    
    delay(33);  // ~30 FPS
}
```

**Your Task**: 
1. Your current direct drawing works - test if you notice any flicker
2. If flickering occurs, upgrade to the M5Canvas approach
3. Compare performance and visual smoothness

**Test**: Move an object across the screen. Does it look smooth or jittery?

**Learn**: Double buffering is a fundamental graphics technique used in games and UI!

---

## 🐱 Part 2: Pet Stats System (Inspired by TiMiNoo)

### Task 2.1: Display Status Bars ✅ COMPLETED

**Your `drawProgressBar()` function is already working!**

```cpp
// Your implementation:
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    StickCP2.Display.drawRect(x, y, width, height, TFT_WHITE);
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        StickCP2.Display.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}

// Usage in loop():
drawProgressBar(5, 20, 80, 8, boyfriend.hunger, 24, 
    (boyfriend.hunger > 18) ? TFT_RED : TFT_ORANGE);
drawProgressBar(5, 32, 80, 8, boyfriend.happiness, 24, TFT_YELLOW);
drawProgressBar(5, 44, 80, 8, boyfriend.energy, 24, TFT_CYAN);
```

**TiMiNoo's stats system** (for reference):
```cpp
// TiMiNoo uses a 0-3 scale (simpler than your 0-24):
long catHunger = random(1, 4);    // 0=depleted, 1=low, 2=average, 3=full
long catHygiene = random(1, 4);
long catMorale = random(1, 4);
long catEducation = random(1, 3);
long catEntertainment = random(1, 4);
```

**Enhancement idea**: Add icons next to bars using emoji-style symbols:
```cpp
// Draw emoji before bar
StickCP2.Display.setCursor(90, 22);
StickCP2.Display.printf("🍔%d", boyfriend.hunger);  // May not render - use text instead
// Or use simple text labels like you already do:
StickCP2.Display.printf("H:%d", boyfriend.hunger);
```

**Learn**: UI rendering and mapping values to visual representation

---

### Task 2.2: Stat Decay Over Time ✅ COMPLETED

**Your code already implements TiMiNoo-style stat decay!**

```cpp
// Your implementation using millis() timers:
const unsigned long HUNGER_INTERVAL = 30000;     // 30 seconds
const unsigned long HAPPINESS_INTERVAL = 45000;  // 45 seconds
const unsigned long ENERGY_INTERVAL = 60000;     // 60 seconds

unsigned long lastHungerUpdate = 0;
unsigned long lastHappinessUpdate = 0;
unsigned long lastEnergyUpdate = 0;

void loop() {
    unsigned long now = millis();
    
    if (now - lastHungerUpdate > HUNGER_INTERVAL) {
        boyfriend.updateHunger(1);  // Hunger increases over time
        lastHungerUpdate = now;
    }
    
    if (now - lastHappinessUpdate > HAPPINESS_INTERVAL) {
        boyfriend.updateHappiness(-1);  // Happiness decreases
        lastHappinessUpdate = now;
    }
    
    if (now - lastEnergyUpdate > ENERGY_INTERVAL) {
        boyfriend.updateEnergy(-1);  // Energy decreases
        lastEnergyUpdate = now;
    }
}
```

**How TiMiNoo does it** (using frame counters instead of millis):
```cpp
// TiMiNoo uses randomized intervals for more organic feel:
unsigned long catHungerStep = random(5000, 8000);  // Random frames between decay
unsigned long catHygieneStep = random(10000, 20000);
unsigned long catMoraleStep = random(4000, 5000);

void loop() {
    frameCounter += 1;
    
    // Hunger check
    if (frameCounter == lastCatHungerCheck + catHungerStep) {
        catHunger -= 1;
        if (catHunger < 0) catHunger = 0;
        lastCatHungerCheck = frameCounter;
    }
}
```

**Enhancement**: Add randomness like TiMiNoo for a more "alive" feel:
```cpp
// Instead of fixed intervals, use random ranges:
unsigned long hungerInterval = random(25000, 35000);  // 25-35 seconds

if (now - lastHungerUpdate > hungerInterval) {
    boyfriend.updateHunger(1);
    lastHungerUpdate = now;
    hungerInterval = random(25000, 35000);  // New random interval
}
```

**Learn**: Non-blocking timers, the `millis()` pattern vs frame counters

---

### Task 2.3: Need Icons (Like TiMiNoo's Speech Bubbles) ✅ PARTIALLY DONE

**Your code shows text notifications - upgrade to TiMiNoo-style icons!**

**Your current implementation**:
```cpp
// Notifications (like TiMiNoo's speech bubbles)
if (boyfriend.hunger > 18) {
    StickCP2.Display.setTextColor(TFT_RED, TFT_BLACK);
    StickCP2.Display.setCursor(5, 60);
    StickCP2.Display.print("HUNGRY!");
} else if (boyfriend.happiness < 5) {
    StickCP2.Display.setTextColor(TFT_BLUE, TFT_BLACK);
    StickCP2.Display.setCursor(5, 60);
    StickCP2.Display.print("SAD :(");
} else if (boyfriend.energy < 5) {
    StickCP2.Display.setTextColor(TFT_PURPLE, TFT_BLACK);
    StickCP2.Display.setCursor(5, 60);
    StickCP2.Display.print("TIRED!");
}
```

**How TiMiNoo does it** (speech bubbles with icons):
```cpp
// TiMiNoo draws a speech bubble frame, then an icon inside:
u8g.drawXBMP(69, 1, speech_bubble_56x48_width, speech_bubble_56x48_height, speech_bubble_56x48_bits);

// Then draws the appropriate need icon inside:
switch (currentIcon) {
    case 2:  // Play
        u8g.drawXBMP(85, 16, play_32x20_width, play_32x20_height, play_32x20_bits);
        break;
    case 4:  // Cuddle
        u8g.drawXBMP(89, 14, cuddle_24x24_width, cuddle_24x24_height, cuddle_24x24_bits);
        break;
    case 6:  // Pizza (hungry)
        u8g.drawXBMP(88, 12, pizza_26x28_width, pizza_26x28_height, pizza_26x28_bits);
        break;
}

// TiMiNoo's game mode system:
if (catHunger == 0 && gameMode == 0) {
    currentIcon = 6;  // Pizza icon
    gameMode = 1;     // Enter "needs attention" mode
}
```

**Enhancement - Add one-time alert sound**:
```cpp
static bool hungryNotified = false;

if (boyfriend.hunger > 18) {
    if (!hungryNotified) {
        StickCP2.Speaker.tone(800, 100);  // Alert beep ONCE
        hungryNotified = true;
    }
    StickCP2.Display.setTextColor(TFT_RED, TFT_BLACK);
    StickCP2.Display.setCursor(5, 60);
    StickCP2.Display.print("HUNGRY!");
} else {
    hungryNotified = false;  // Reset when hunger is satisfied
}
```

**Your Task**: 
1. Add one-time sound alerts (already have Speaker working!)
2. Consider adding a `gameMode` variable like TiMiNoo for different states

**Learn**: Edge detection vs level detection, static variables for state

---

## 🎨 Part 3: Animation System

### Task 3.1: Simple Sprite Animation ✅ COMPLETED

**Your breathing animation is already working!**

```cpp
// Your implementation:
int animationFrame = 0;
unsigned long lastAnimUpdate = 0;

void drawCharacter(int x, int y) {
    // Breathing animation (like TiMiNoo's cat idle)
    int breathOffset = 0;
    switch (animationFrame % 4) {
        case 0: breathOffset = 0; break;
        case 1: breathOffset = 1; break;
        case 2: breathOffset = 2; break;
        case 3: breathOffset = 1; break;
    }
    
    int radius = 20 + breathOffset;
    StickCP2.Display.fillCircle(x, y, radius, TFT_WHITE);
    // ... eyes and mouth based on mood ...
}

void loop() {
    // Update animation frame every 200ms (5 FPS animation)
    if (now - lastAnimUpdate > 200) {
        animationFrame++;
        lastAnimUpdate = now;
    }
}
```

**How TiMiNoo animates** (8 frames of cat sprites):
```cpp
// TiMiNoo cycles through 8 pre-drawn cat frames:
animationStep += 1;
if (animationStep > animationStepMax) {
    animationStep = 1;
}

switch (animationStep) {
    case 1:
        u8g.drawXBMP(8, 8, cat_sitting_upscaled4x_001_width, 
                     cat_sitting_upscaled4x_001_height, 
                     cat_sitting_upscaled4x_001_bits);
        break;
    case 2:
        u8g.drawXBMP(8, 8, cat_sitting_upscaled4x_002_width, ...);
        break;
    // ... cases 3-8 ...
}
```

**Enhancement - Add eye blinking**:
```cpp
void drawCharacter(int x, int y) {
    // ... body drawing ...
    
    // Blinking: Every ~3 seconds, close eyes for 2 frames
    bool isBlinking = (animationFrame % 60 >= 58);  // Blink on frames 58-59
    
    int eyeY = y - 5;
    if (isBlinking) {
        // Closed eyes (horizontal lines)
        StickCP2.Display.drawLine(x - 8, eyeY, x - 4, eyeY, TFT_BLACK);
        StickCP2.Display.drawLine(x + 4, eyeY, x + 8, eyeY, TFT_BLACK);
    } else {
        // Open eyes (circles)
        StickCP2.Display.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        StickCP2.Display.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    }
}
```

**Learn**: Frame-based animation, modulo operator for cycling

---

### Task 3.2: Mood-Based Expressions ✅ COMPLETED

**Your character already changes expression based on happiness!**

```cpp
// Your implementation in drawCharacter():
void drawCharacter(int x, int y) {
    // ... body and breathing ...
    
    // Eyes (expression based on happiness)
    int eyeY = y - 5;
    if (boyfriend.happiness > 15) {
        // Happy eyes (arcs)
        StickCP2.Display.drawLine(x - 8, eyeY, x - 4, eyeY - 2, TFT_BLACK);
        StickCP2.Display.drawLine(x + 4, eyeY - 2, x + 8, eyeY, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Sad eyes (small dots)
        StickCP2.Display.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        StickCP2.Display.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
    } else {
        // Normal eyes
        StickCP2.Display.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        StickCP2.Display.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    }
    
    // Mouth
    int mouthY = y + 5;
    if (boyfriend.happiness > 15) {
        StickCP2.Display.drawArc(x, mouthY - 2, 6, 4, 180, 360, TFT_BLACK);  // Smile
    } else if (boyfriend.happiness < 5) {
        StickCP2.Display.drawArc(x, mouthY + 4, 6, 4, 0, 180, TFT_BLACK);    // Frown
    } else {
        StickCP2.Display.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);  // Neutral
    }
}
```

**TiMiNoo's expression system** (uses separate icon sprites):
```cpp
// TiMiNoo has happy and super-happy states:
if (superHappyCounter < 1) {
    u8g.drawXBMP(87, 12, happy_cat_28x28_width, happy_cat_28x28_height, happy_cat_28x28_bits);
} else {
    u8g.drawXBMP(87, 12, super_happy_28x28_width, super_happy_28x28_height, super_happy_28x28_bits);
    superHappyCounter -= 1;
}

// Super happy triggers after feeding/cuddling:
if (cuddleCounter == 240) {
    superHappyCounter = 100;  // Show super happy for 100 frames
    score += 50;
    catMorale = 3;
    gameMode = 0;
}
```

**Enhancement - Add hungry expression**:
```cpp
// In drawCharacter(), add hungry face:
if (boyfriend.hunger > 18) {
    // Big eyes (wanting food!)
    StickCP2.Display.fillCircle(x - 6, eyeY, 4, TFT_BLACK);
    StickCP2.Display.fillCircle(x + 6, eyeY, 4, TFT_BLACK);
    // Open mouth
    StickCP2.Display.fillCircle(x, mouthY + 2, 5, TFT_BLACK);
}
```

**Learn**: State-driven rendering, data-driven design

---

## 🎰 Part 4: Mini-Games (Inspired by TiMiNoo's "Catsino")

### Task 4.1: Slot Machine Game ⭐⭐⭐ (OpenELAB Style)

**Goal**: Create a slot machine mini-game like OpenELAB's M5StickCPlus2-Slot project!

**OpenELAB's Slot Machine Architecture**:
```cpp
// They use a Slot class with state machine:
enum SlotState { SLOT_INIT, SLOT_START, SLOT_STOP, SLOT_DRIFT };

class Slot {
    int index;
    float degree;     // Current rotation angle (0-360)
    int vel;          // Velocity in deg/s
    int acc;          // Acceleration
    SlotState state;
    
    void start(int acc = 12, int maxVel = 800);  // Start spinning
    void stop(int acc = -20, int minVel = 50);   // Begin slowing down
    bool update();    // Update physics each frame
    void draw();      // Render current state
};
```

**OpenELAB's main loop pattern**:
```cpp
void loop() {
    unsigned long tick = millis();
    
    M5.update();
    if (M5.BtnA.wasPressed()) {
        if (state == SLOTS_INIT) {
            // Start all reels spinning
            for (int i = 0; i < SLOT_COUNT; i++) {
                slots[i].start();
            }
            state++;
        } else if (state < SLOTS_STOP) {
            // Stop one reel at a time
            slots[state - 1].stop();
            state++;
        }
    }
    
    // Update and draw all slots
    for (int i = 0; i < SLOT_COUNT; i++) {
        if (slots[i].update()) {
            slots[i].draw();
        }
    }
    
    delay(30);  // ~30 FPS
}
```

**Simplified version for Boyfriend Bot**:
```cpp
// Game state variables
int slotValue = 0;
bool slotSpinning = false;
unsigned long slotStartTime = 0;

void playSlotGame() {
    if (!slotSpinning) {
        slotSpinning = true;
        slotStartTime = millis();
    }
    
    unsigned long elapsed = millis() - slotStartTime;
    
    if (elapsed < 2000) {
        // Spinning - show random values quickly
        slotValue = random(0, 10);
        // Draw spinning effect
        StickCP2.Display.fillRect(80, 40, 60, 40, TFT_BLACK);
        StickCP2.Display.setTextSize(3);
        StickCP2.Display.setCursor(100, 50);
        StickCP2.Display.printf("%d", slotValue);
    } else if (elapsed < 2100) {
        // Just stopped - check result
        slotValue = random(0, 10);
        if (slotValue == 7) {
            boyfriend.updateHappiness(5);
            StickCP2.Speaker.tone(1500, 200);
        }
        slotSpinning = false;
    }
}
```

**TiMiNoo's Catsino approach**:
```cpp
// TiMiNoo cycles icons across the screen:
if ((gameCounter % 3) == 0) {
    randomGameIconXPos = random(0, 3);
    randomFoodType = random(0, 7);
}

// Draw icon at position
switch (randomGameIconXPos) {
    case 0: gameIconXPos = 9; break;
    case 1: gameIconXPos = 50; break;
    case 2: gameIconXPos = 91; break;
}

// Player presses button to "stop" and win the displayed item
if (StickCP2.BtnA.wasPressed()) {
    gamePick = randomFoodType;
    gameSequence = 1;  // Show result
}
```

**Learn**: Game loops, physics simulation, state machines

---

### Task 4.2: Tilt Game (Using IMU) ⭐⭐⭐

**Goal**: Use the M5StickC Plus 2's built-in accelerometer for a tilt-based game.

**Hardware**: The M5StickC Plus 2 has an MPU6886 IMU (Inertial Measurement Unit).

**Reading tilt values**:
```cpp
void setup() {
    // IMU is initialized automatically by StickCP2.begin()
}

void loop() {
    // Update IMU readings
    StickCP2.Imu.update();
    
    // Get acceleration values
    auto data = StickCP2.Imu.getImuData();
    float ax = data.accel.x;  // -1.0 to +1.0 (tilt left/right)
    float ay = data.accel.y;  // -1.0 to +1.0 (tilt forward/back)
    float az = data.accel.z;  // -1.0 to +1.0 (up/down - gravity)
    
    // Or use the older API:
    float ax2, ay2, az2;
    StickCP2.Imu.getAccel(&ax2, &ay2, &az2);
}
```

**Simple tilt game - Balance the ball**:
```cpp
int ballX = 120;  // Center of screen
int ballY = 67;

void tiltGame() {
    StickCP2.Imu.update();
    auto data = StickCP2.Imu.getImuData();
    
    // Move ball based on tilt (scale factor determines sensitivity)
    ballX += (int)(data.accel.x * 5);
    ballY += (int)(data.accel.y * 5);
    
    // Keep ball on screen
    if (ballX < 10) ballX = 10;
    if (ballX > 230) ballX = 230;
    if (ballY < 10) ballY = 10;
    if (ballY > 125) ballY = 125;
    
    // Draw ball
    StickCP2.Display.fillCircle(ballX, ballY, 8, TFT_YELLOW);
    
    // Draw target zone in center
    StickCP2.Display.drawRect(100, 50, 40, 35, TFT_GREEN);
    
    // Check if ball is in target
    if (ballX >= 100 && ballX <= 140 && ballY >= 50 && ballY <= 85) {
        // Score points while balanced!
        static unsigned long lastScoreTime = 0;
        if (millis() - lastScoreTime > 500) {
            boyfriend.updateHappiness(1);
            lastScoreTime = millis();
        }
    }
}
```

**Test**: Tilt your device. Does the ball respond naturally?

**Learn**: Sensor input, physics-like behavior, game scoring

---

## 🔊 Part 5: Sound & Music

### Task 5.1: Simple Sound Effects ✅ PARTIALLY DONE

**Your code already uses Speaker.tone() for button feedback!**

```cpp
// Your current implementation:
if (StickCP2.BtnA.wasPressed()) {
    boyfriend.updateHappiness(2);
    StickCP2.Speaker.tone(1000, 50);  // 1000Hz for 50ms
}

if (StickCP2.BtnB.wasPressed()) {
    boyfriend.updateHunger(-2);
    StickCP2.Speaker.tone(1500, 50);  // Higher pitch for feeding
}
```

**Enhanced sound effects**:
```cpp
// Happy sound (ascending - like Mario coin!)
void playHappySound() {
    StickCP2.Speaker.tone(800, 50);
    delay(60);
    StickCP2.Speaker.tone(1000, 50);
    delay(60);
    StickCP2.Speaker.tone(1200, 100);
}

// Sad sound (descending - like losing)
void playSadSound() {
    StickCP2.Speaker.tone(600, 100);
    delay(110);
    StickCP2.Speaker.tone(400, 150);
}

// Eating sound (nom nom)
void playEatSound() {
    for (int i = 0; i < 3; i++) {
        StickCP2.Speaker.tone(300, 50);
        delay(100);
    }
}

// Alert/Warning beep
void playAlertSound() {
    StickCP2.Speaker.tone(1500, 100);
    delay(50);
    StickCP2.Speaker.tone(1500, 100);
}

// Game win celebration!
void playWinSound() {
    int notes[] = {523, 659, 784, 1047};  // C E G C (victory chord)
    for (int i = 0; i < 4; i++) {
        StickCP2.Speaker.tone(notes[i], 100);
        delay(110);
    }
}
```

**TiMiNoo note**: TiMiNoo is specifically designed to be silent ("no sound") for office use. Your Boyfriend Bot can have sounds as a feature!

**Learn**: Audio feedback in UX design, musical scales

---

### Task 5.2: RTTTL Melody Player ⭐⭐⭐

**Goal**: Play actual melodies using RTTTL format (Ring Tone Text Transfer Language).

**What is RTTTL?**
A simple text format for ringtone melodies, originally from Nokia phones.

**Format**: `name:d=default_duration,o=default_octave,b=bpm:notes`

**Example melodies**:
```cpp
// Happy Birthday
const char* happyBirthday = "HappyBday:d=4,o=5,b=125:8g,8g,a,g,c6,2b,8g,8g,a,g,d6,2c6";

// Super Mario Bros theme (first few notes)
const char* mario = "Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g";

// Zelda Secret Sound
const char* zeldaSecret = "Zelda:d=4,o=5,b=125:g,f#,d#,a4,g#4,e,g#,c6";
```

**Simple RTTTL player for M5StickC Plus 2**:
```cpp
// Note frequencies (octave 4)
const int noteFreq[] = {262, 294, 330, 349, 392, 440, 494};  // c,d,e,f,g,a,b
const char noteNames[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b'};

int getNoteFreq(char note, int octave) {
    for (int i = 0; i < 7; i++) {
        if (tolower(note) == noteNames[i]) {
            int freq = noteFreq[i];
            // Adjust for octave (4 is base)
            if (octave > 4) freq <<= (octave - 4);  // Multiply by 2^n
            if (octave < 4) freq >>= (4 - octave);  // Divide by 2^n
            return freq;
        }
    }
    return 0;  // Rest or unknown
}

void playSimpleMelody() {
    // Simple Happy Birthday (just the notes, hardcoded)
    int notes[] = {392, 392, 440, 392, 523, 494};  // G G A G C B
    int durations[] = {200, 200, 400, 400, 400, 800};
    
    for (int i = 0; i < 6; i++) {
        StickCP2.Speaker.tone(notes[i], durations[i]);
        delay(durations[i] + 50);
    }
}
```

**Non-blocking melody player** (advanced):
```cpp
// Global state for background music
const int* currentMelody = nullptr;
int melodyIndex = 0;
unsigned long noteStartTime = 0;
int currentNoteDuration = 0;

void startMelody(const int* melody, int length) {
    currentMelody = melody;
    melodyIndex = 0;
    noteStartTime = millis();
}

void updateMelody() {
    if (currentMelody == nullptr) return;
    
    if (millis() - noteStartTime > currentNoteDuration) {
        // Play next note
        int freq = currentMelody[melodyIndex * 2];
        int dur = currentMelody[melodyIndex * 2 + 1];
        
        if (freq > 0) {
            StickCP2.Speaker.tone(freq, dur);
        }
        
        currentNoteDuration = dur + 50;
        noteStartTime = millis();
        melodyIndex++;
        
        // Check if melody ended
        if (melodyIndex >= 6) {  // Adjust based on melody length
            currentMelody = nullptr;
        }
    }
}
```

**Learn**: String parsing, frequency/music theory, non-blocking patterns

---

## 💾 Part 6: Persistence (EEPROM)

### Task 6.1: Save and Load Pet Stats ✅ COMPLETED

**Your Boyfriend class already has full EEPROM persistence!**

**Your sprites.h class definition**:
```cpp
class Boyfriend {
public:
    bool sleeping;      // EEPROM address 0
    int sleep;          // address 1
    int happiness;      // address 2
    int hunger;         // address 3
    int energy;         // address 4
    int age;            // address 5
    int mood;           // address 6
    
    void loadFromEEPROM();
    void saveToEEPROM();
    void updateHappiness(int val);  // Auto-saves to EEPROM
    // ... other update methods ...
};
```

**Your Boyfriend.cpp implementation**:
```cpp
void Boyfriend::updateHappiness(int happinessVal) {
    happiness += happinessVal;
    if (happiness < 0) happiness = 0;
    if (happiness > 24) happiness = 24;
    EEPROM.write(2, happiness);  // Auto-save after every change!
}

void Boyfriend::loadFromEEPROM() {
    sleeping = EEPROM.read(0);
    sleep = EEPROM.read(1);
    happiness = EEPROM.read(2);
    hunger = EEPROM.read(3);
    energy = EEPROM.read(4);
    age = EEPROM.read(5);
    mood = EEPROM.read(6);
    
    // Validate loaded values
    if (sleep > 24) sleep = 8;
    if (happiness > 24) happiness = 12;
    if (hunger > 24) hunger = 0;
    if (energy > 24) energy = 12;
    if (mood > 3) mood = 0;
}
```

**Important**: ESP32 requires `EEPROM.commit()` to actually write data!
```cpp
void Boyfriend::saveToEEPROM() {
    EEPROM.write(0, sleeping);
    EEPROM.write(1, sleep);
    EEPROM.write(2, happiness);
    EEPROM.write(3, hunger);
    EEPROM.write(4, energy);
    EEPROM.write(5, age);
    EEPROM.write(6, mood);
    EEPROM.commit();  // REQUIRED for ESP32!
}
```

**Optimization tip**: Writing to EEPROM after every stat change can wear out the flash. Consider periodic saves:
```cpp
// In loop():
static unsigned long lastSave = 0;
if (millis() - lastSave > 60000) {  // Save every minute
    boyfriend.saveToEEPROM();
    EEPROM.commit();
    lastSave = millis();
}
```

**Learn**: Non-volatile storage, data persistence, flash wear considerations

---

### Task 6.2: First Boot Detection ✅ COMPLETED

**Your code already has magic byte first-boot detection!**

```cpp
// Your implementation in setup():
EEPROM.begin(512);

// First boot detection using magic byte
uint8_t magicByte = EEPROM.read(100);
if (magicByte != 0x42) {
    // First boot! Initialize defaults
    EEPROM.write(100, 0x42);  // Magic number to mark as initialized
    EEPROM.write(0, 0);   // sleeping = false
    EEPROM.write(1, 8);   // sleep = 8
    EEPROM.write(2, 12);  // happiness = 12
    EEPROM.write(3, 0);   // hunger = 0
    EEPROM.write(4, 12);  // energy = 12
    EEPROM.write(5, 0);   // age = 0
    EEPROM.write(6, 0);   // mood = neutral
    EEPROM.commit();
}

// Load pet data (works for both first boot and returning)
boyfriend.loadFromEEPROM();
```

**Enhancement - Welcome messages**:
```cpp
// Show different messages based on first boot
if (magicByte != 0x42) {
    // First boot
    StickCP2.Display.setCursor(10, 50);
    StickCP2.Display.print("Welcome! <3");
    delay(2000);
    // ... initialize defaults ...
} else {
    // Returning user
    boyfriend.loadFromEEPROM();
    StickCP2.Display.setCursor(10, 50);
    StickCP2.Display.print("Welcome back!");
    delay(1000);
}
```

**Enhancement - Reset functionality**:
```cpp
// Add to loop(): Hold both buttons for 3 seconds to reset
static unsigned long bothButtonsStart = 0;

if (StickCP2.BtnA.isPressed() && StickCP2.BtnB.isPressed()) {
    if (bothButtonsStart == 0) {
        bothButtonsStart = millis();
    } else if (millis() - bothButtonsStart > 3000) {
        // Reset!
        EEPROM.write(100, 0x00);  // Clear magic byte
        EEPROM.commit();
        ESP.restart();  // Reboot to trigger first-boot setup
    }
} else {
    bothButtonsStart = 0;  // Reset timer if buttons released
}
```

**Learn**: Data validation, initialization patterns, device reset

---

## 🎓 Bonus Challenges

### Challenge A: Random Events (Like TiMiNoo's Visitors) 💪

**How TiMiNoo does random visits**:
```cpp
// TiMiNoo checks for random visitors when feeding:
randomVisit = random(0, 3136);
if (randomVisit < 2000 && randomVisit % 2 == 0) {
    randomVisitSequence = 0;
    randomVisitCounter = 0;
    gameMode = 7;  // Enter visitor mode!
}

// Visitor sequence:
switch (randomVisitSequence) {
    case 0:  // Knock knock
        u8g.drawXBMP(50, 14, door_28x30_width, door_28x30_height, door_28x30_bits);
        u8g.drawStr(40, 59, "Knock knock!");
        break;
    case 1:  // Hello
        u8g.drawXBMP(96, 14, cindy_28x26_width, cindy_28x26_height, cindy_28x26_bits);
        u8g.drawStr(45, 59, "Hi friend!");
        break;
    case 2:  // Gift
        u8g.drawStr(45, 59, "I got matcha tea!");
        break;
    case 3:  // Enjoy gift together
        u8g.drawStr(45, 59, "Have some <3");
        score += 10000;
        catHunger = 3;
        break;
}
```

**Your Task**: Create random events:
```cpp
void checkRandomEvents() {
    static unsigned long lastEventCheck = 0;
    if (millis() - lastEventCheck < 60000) return;  // Check every minute
    lastEventCheck = millis();
    
    int roll = random(0, 100);
    
    if (roll < 5) {
        // 5% chance: "Found a snack!"
        boyfriend.updateHunger(-3);
        showMessage("Found a snack!", 2000);
        playHappySound();
    } else if (roll < 8) {
        // 3% chance: "It's raining!"
        boyfriend.updateHappiness(-2);
        showMessage("It's raining :(", 2000);
    } else if (roll < 9) {
        // 1% chance: "Dance party!"
        boyfriend.updateHappiness(5);
        showMessage("DANCE PARTY!", 2000);
        playWinSound();
    }
}
```

---

### Challenge B: Sleep/Wake Cycle 💪

**Your Boyfriend class already has a `sleeping` boolean!**

**Your Task**: 
```cpp
void checkSleepState() {
    if (!boyfriend.sleeping && boyfriend.energy < 3) {
        // Too tired - fall asleep!
        boyfriend.sleeping = true;
        boyfriend.updateSleeping();
        showMessage("Zzz...", 1000);
    }
    
    if (boyfriend.sleeping) {
        // Regenerate energy while sleeping
        static unsigned long lastSleepRegen = 0;
        if (millis() - lastSleepRegen > 5000) {  // Every 5 seconds
            boyfriend.updateEnergy(1);
            lastSleepRegen = millis();
        }
        
        // Wake up when rested
        if (boyfriend.energy > 15) {
            boyfriend.sleeping = false;
            boyfriend.updateSleeping();
            showMessage("Good morning!", 1000);
        }
    }
}

void drawCharacter(int x, int y) {
    if (boyfriend.sleeping) {
        // Sleeping animation - closed eyes, Z's floating up
        StickCP2.Display.fillCircle(x, y, 20, TFT_WHITE);
        StickCP2.Display.drawLine(x - 8, y - 5, x - 4, y - 5, TFT_BLACK);  // Closed eye
        StickCP2.Display.drawLine(x + 4, y - 5, x + 8, y - 5, TFT_BLACK);
        
        // Floating Z's
        int zOffset = (animationFrame % 10) * 2;
        StickCP2.Display.setCursor(x + 15, y - 20 - zOffset);
        StickCP2.Display.setTextSize(1);
        StickCP2.Display.print("Z");
        StickCP2.Display.setCursor(x + 22, y - 30 - zOffset);
        StickCP2.Display.print("z");
    } else {
        // Normal awake character...
    }
}
```

---

### Challenge C: Custom Sprite Art (Like OpenELAB) 💪

**OpenELAB's approach** - RGB565 color sprites stored in PROGMEM:
```cpp
// From OpenELAB Slot project - 48x48 pixel color icons:
#include <Arduino.h>

const uint16_t PROGMEM slot_cherry[] = {
    0xFFFF, 0xFFFF, 0xFFFF, ...  // RGB565 pixel data
};

// Drawing with M5GFX:
M5.Lcd.pushImage(x, y, SYM_WIDTH, SYM_HEIGHT, slot_cherry);
```

**TiMiNoo's approach** - 1-bit XBM sprites:
```cpp
// TiMiNoo uses 1-bit bitmaps (black and white only, smaller files):
#define cat_sitting_001_width 12
#define cat_sitting_001_height 12
static unsigned char cat_sitting_001_bits[] U8G_PROGMEM = {
    0x60, 0x0c, 0xa0, 0x0b, 0x20, 0x08, ...
};

// Drawing with u8glib:
u8g.drawXBMP(8, 8, cat_sitting_001_width, cat_sitting_001_height, cat_sitting_001_bits);
```

**Your Task - Create custom sprites**:

1. **Create pixel art** using a tool like:
   - [Piskel](https://www.piskelapp.com/) - Free online pixel art editor
   - [GIMP](https://www.gimp.org/) - Full image editor
   - [Aseprite](https://www.aseprite.org/) - Professional pixel art tool

2. **Convert to C array** using:
   - [image2cpp](https://javl.github.io/image2cpp/) - For 1-bit or indexed images
   - [LCD Image Converter](https://lcd-image-converter.riuson.com/) - For RGB565

3. **Store in PROGMEM** to save RAM:
```cpp
// In a new header file (sprites_custom.h):
const uint16_t PROGMEM boyfriend_happy[] = {
    // 32x32 RGB565 data from converter
    0xFFFF, 0xFFFF, ...
};

// Drawing:
void drawCustomSprite(int x, int y) {
    StickCP2.Display.pushImage(x, y, 32, 32, boyfriend_happy);
}
```

**Tip**: Start small (16x16 or 32x32) to keep memory usage low!

---

### Challenge D: Special Date Events (RTC Integration) 💪

**The M5StickC Plus 2 has a built-in Real-Time Clock (BM8563)!**

**Using the RTC**:
```cpp
void setup() {
    // ... other init ...
    
    // Set time (only needed once, or sync from phone/computer)
    // StickCP2.Rtc.setDateTime({{2026, 1, 15}, {12, 0, 0}});
}

void checkSpecialDates() {
    auto rtcDate = StickCP2.Rtc.getDate();
    int month = rtcDate.month;
    int day = rtcDate.day;
    
    // Birthday! (set your girlfriend's birthday here)
    if (month == 6 && day == 15) {
        showBirthdayAnimation();
        playHappyBirthday();
        boyfriend.updateHappiness(10);
    }
    
    // Valentine's Day
    if (month == 2 && day == 14) {
        showHearts();
        boyfriend.updateHappiness(5);
    }
    
    // Your Anniversary
    if (month == 8 && day == 20) {
        showMessage("Anniversary! <3", 3000);
        boyfriend.updateHappiness(5);
    }
    
    // Christmas
    if (month == 12 && day == 25) {
        showSnowflakes();
        boyfriend.updateHappiness(3);
    }
}

void showBirthdayAnimation() {
    // Draw cake and balloons!
    StickCP2.Display.fillScreen(TFT_MAGENTA);
    StickCP2.Display.setTextColor(TFT_WHITE);
    StickCP2.Display.setTextSize(2);
    StickCP2.Display.setCursor(20, 50);
    StickCP2.Display.print("HAPPY BIRTHDAY!");
    StickCP2.Display.setCursor(60, 80);
    StickCP2.Display.print("<3 <3 <3");
    delay(3000);
}
```

**Getting accurate time** (without WiFi):
- Set time via USB serial: Create a command that accepts the current time
- Or: Just use the day/month from RTC (date survives power cycles if set once)

---

### Challenge E: Pedometer Integration (Using IMU) 💪

**The M5StickC Plus 2's MPU6886 can detect steps!**

**Simple step detection algorithm**:
```cpp
int stepCount = 0;
float lastMagnitude = 0;
bool stepDetected = false;
const float STEP_THRESHOLD = 1.3;  // Adjust based on testing

void updatePedometer() {
    StickCP2.Imu.update();
    auto data = StickCP2.Imu.getImuData();
    
    // Calculate acceleration magnitude
    float magnitude = sqrt(
        data.accel.x * data.accel.x + 
        data.accel.y * data.accel.y + 
        data.accel.z * data.accel.z
    );
    
    // Simple peak detection
    if (magnitude > STEP_THRESHOLD && !stepDetected) {
        stepDetected = true;
        stepCount++;
        
        // Reward every 50 steps
        if (stepCount % 50 == 0) {
            boyfriend.updateEnergy(1);
            StickCP2.Speaker.tone(800, 30);
        }
    }
    
    if (magnitude < 1.0) {
        stepDetected = false;  // Ready for next step
    }
    
    lastMagnitude = magnitude;
}

void drawStepCounter() {
    StickCP2.Display.setCursor(5, 120);
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.printf("Steps: %d", stepCount);
}
```

**Enhancement - Daily step goal**:
```cpp
const int DAILY_GOAL = 500;

void checkDailyGoal() {
    if (stepCount >= DAILY_GOAL) {
        static bool goalCelebrated = false;
        if (!goalCelebrated) {
            showMessage("GOAL REACHED!", 2000);
            boyfriend.updateHappiness(5);
            playWinSound();
            goalCelebrated = true;
        }
    }
}

// Reset at midnight (requires RTC):
void checkMidnightReset() {
    auto time = StickCP2.Rtc.getTime();
    if (time.hours == 0 && time.minutes == 0) {
        static bool resetDone = false;
        if (!resetDone) {
            stepCount = 0;
            goalCelebrated = false;
            resetDone = true;
        }
    } else {
        resetDone = false;
    }
}
```

---

## 🎯 Part 7: Easy Wins (Logic & Screen)

These features add personality with minimal hardware complexity!

### Task 7.1: Battery Hangover 🔋

**🎯 Goal**: If battery drops below 20%, the character moves slowly and refuses to play games.

**💡 Why It's Cool**: This gives the hardware "life" - the device's real battery affects your virtual pet!

**Implementation**:
```cpp
// Battery percentage (M5StickC Plus 2 uses AXP2101 power management)
int getBatteryPercent() {
    // M5StickCPlus2 provides battery voltage
    float voltage = StickCP2.Power.getBatteryVoltage();
    // Approximate: 4.2V = 100%, 3.0V = 0%
    int percent = (int)((voltage - 3.0) / 1.2 * 100);
    return constrain(percent, 0, 100);
}

// Check if low battery mode
bool isLowBattery() {
    return getBatteryPercent() < 20;
}

// Modify animation speed based on battery
int getAnimationDelay() {
    if (isLowBattery()) {
        return 500;  // Slow, sluggish movement
    }
    return 150;  // Normal peppy movement
}

// Block games when tired
void handlePlayButton() {
    if (isLowBattery()) {
        showMessage("Too tired...", 1500);
        // Draw sleepy face
        StickCP2.Display.fillCircle(100, 80, 5, TFT_DARKGREY);  // Droopy eyes
        StickCP2.Display.fillCircle(140, 80, 5, TFT_DARKGREY);
        return;
    }
    // Normal game logic
    startMiniGame();
}
```

**Visual feedback**:
```cpp
void drawBatteryWarning() {
    if (isLowBattery()) {
        // Flash a low battery icon
        static bool showIcon = true;
        static unsigned long lastFlash = 0;
        
        if (millis() - lastFlash > 500) {
            showIcon = !showIcon;
            lastFlash = millis();
        }
        
        if (showIcon) {
            // Draw empty battery icon in corner
            StickCP2.Display.drawRect(200, 5, 30, 15, TFT_RED);
            StickCP2.Display.fillRect(230, 8, 3, 9, TFT_RED);
            StickCP2.Display.drawLine(205, 5, 225, 20, TFT_RED);  // X through it
        }
    }
}
```

**🧪 Test**: Drain battery below 20% (or simulate with `#define DEBUG_LOW_BATTERY true`) and verify sluggish behavior.

---

### Task 7.2: Fortune Cookie 🥠

**🎯 Goal**: A "Talk" button that displays random cute messages.

**💡 The Love Notes**: Write personalized messages she'll discover over time!

**Implementation**:
```cpp
// Store your cute messages (customize these!)
const char* loveMessages[] = {
    "You look pretty today!",
    "I'm lucky to have you",
    "Let's go on an adventure!",
    "You make me smile",
    "I believe in you!",
    "You're my favorite human",
    "Thanks for taking care of me",
    "I love our time together",
    "You're doing great!",
    "Don't forget to drink water!",
    "I missed you!",
    "You're the best!",
    "Let's cuddle later?",
    "*happy noises*",
    "Thinking about you..."
};
const int NUM_MESSAGES = sizeof(loveMessages) / sizeof(loveMessages[0]);

// Track shown messages to avoid repeats
int lastMessageIndex = -1;

void showFortuneCookie() {
    // Pick random message (avoid repeating last one)
    int msgIndex;
    do {
        msgIndex = random(NUM_MESSAGES);
    } while (msgIndex == lastMessageIndex && NUM_MESSAGES > 1);
    lastMessageIndex = msgIndex;
    
    // Clear area and show speech bubble
    StickCP2.Display.fillRect(10, 10, 220, 60, TFT_WHITE);
    StickCP2.Display.drawRect(10, 10, 220, 60, TFT_BLACK);
    
    // Draw speech bubble tail
    StickCP2.Display.fillTriangle(50, 70, 70, 70, 60, 85, TFT_WHITE);
    StickCP2.Display.drawLine(50, 70, 60, 85, TFT_BLACK);
    StickCP2.Display.drawLine(70, 70, 60, 85, TFT_BLACK);
    
    // Display message
    StickCP2.Display.setTextColor(TFT_BLACK);
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(20, 30);
    StickCP2.Display.print(loveMessages[msgIndex]);
    
    // Play cute sound
    StickCP2.Speaker.tone(800, 100);
    delay(100);
    StickCP2.Speaker.tone(1000, 100);
    
    delay(2500);  // Show message for 2.5 seconds
    StickCP2.Display.setTextColor(TFT_WHITE);  // Reset
}
```

**Add to your menu/button handling**:
```cpp
// In your button handler (e.g., long-press BtnB)
if (StickCP2.BtnB.pressedFor(1000)) {
    showFortuneCookie();
}
```

**🧪 Test**: Press the Talk button multiple times - you should get different messages!

---

### Task 7.3: Anime Music Player 🎵

**🎯 Goal**: Play her favorite anime song chorus when she selects "Dance"!

**💡 RTTTL Format**: Ring Tone Text Transfer Language - a simple way to encode melodies.

**The Evangelion Theme** (from your request):
```cpp
// RTTTL melody string
const char* evangelionTheme = "Evangelion:d=8,o=3,b=120:8a5,8p,8c6,8p,32c#6,8d6,32p,6c6,4d6,8d6,8g6,8f6,16e6,16d6,16p,4e6";

// More anime themes you can add:
const char* mySongList[] = {
    "Evangelion:d=8,o=3,b=120:8a5,8p,8c6,8p,32c#6,8d6,32p,6c6,4d6,8d6,8g6,8f6,16e6,16d6,16p,4e6",
    "Tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6",
    "Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p",
    "Zelda:d=4,o=5,b=125:a#,8f.,16a#,16c6,16d6,16d#6,2f6",
    "PinkPanther:d=4,o=5,b=160:8d#,8e,2p,8f#,8g,2p,8d#,8e,16p,8f#,8g"
};
const int NUM_SONGS = sizeof(mySongList) / sizeof(mySongList[0]);
```

**RTTTL Parser** (based on common implementations):
```cpp
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

int notes[] = { 0,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

void playRTTTL(const char* p) {
    // Skip name
    while (*p != ':') p++;
    p++;  // Skip ':'
    
    int defaultDuration = 4;
    int defaultOctave = 6;
    int bpm = 63;
    
    // Parse defaults (d=, o=, b=)
    while (*p != ':') {
        if (*p == 'd') {
            p += 2;  // Skip "d="
            defaultDuration = atoi(p);
            while (isdigit(*p)) p++;
        }
        else if (*p == 'o') {
            p += 2;  // Skip "o="
            defaultOctave = atoi(p);
            while (isdigit(*p)) p++;
        }
        else if (*p == 'b') {
            p += 2;  // Skip "b="
            bpm = atoi(p);
            while (isdigit(*p)) p++;
        }
        p++;
    }
    p++;  // Skip ':'
    
    int wholenote = (60 * 1000L / bpm) * 4;
    
    // Parse notes
    while (*p) {
        int duration = 0;
        int note = 0;
        int octave = defaultOctave;
        
        // Duration prefix
        if (isdigit(*p)) {
            duration = atoi(p);
            while (isdigit(*p)) p++;
        } else {
            duration = defaultDuration;
        }
        
        // Note letter
        switch (tolower(*p)) {
            case 'c': note = 1; break;
            case 'd': note = 3; break;
            case 'e': note = 5; break;
            case 'f': note = 6; break;
            case 'g': note = 8; break;
            case 'a': note = 10; break;
            case 'b': note = 12; break;
            case 'p': note = 0; break;  // Pause
        }
        p++;
        
        // Sharp?
        if (*p == '#') {
            note++;
            p++;
        }
        
        // Dotted?
        bool dotted = false;
        if (*p == '.') {
            dotted = true;
            p++;
        }
        
        // Octave override
        if (isdigit(*p)) {
            octave = *p - '0';
            p++;
        }
        
        // Skip comma
        if (*p == ',') p++;
        while (*p == ' ') p++;
        
        // Calculate duration in ms
        int noteDuration = wholenote / duration;
        if (dotted) noteDuration += noteDuration / 2;
        
        // Play the note
        if (note) {
            int freq = notes[(octave - 4) * 12 + note];
            StickCP2.Speaker.tone(freq, noteDuration * 0.9);
        }
        delay(noteDuration);
        StickCP2.Speaker.stop();
    }
}
```

**Dance mode with animation**:
```cpp
bool isDancing = false;

void startDance() {
    isDancing = true;
    int songIndex = random(NUM_SONGS);
    
    // Start playing in a simple way (blocks until done)
    // For non-blocking, you'd need a state machine
    playRTTTL(mySongList[songIndex]);
    
    isDancing = false;
}

void drawDancingCharacter() {
    static int danceFrame = 0;
    
    // Alternate between poses
    if (danceFrame % 2 == 0) {
        // Arms up!
        StickCP2.Display.fillRect(80, 100, 10, 30, TFT_WHITE);   // Left arm up
        StickCP2.Display.fillRect(150, 100, 10, 30, TFT_WHITE);  // Right arm up
    } else {
        // Arms down
        StickCP2.Display.fillRect(80, 130, 10, 30, TFT_WHITE);   // Left arm down
        StickCP2.Display.fillRect(150, 130, 10, 30, TFT_WHITE);  // Right arm down
    }
    
    // Musical notes floating around
    StickCP2.Display.setTextSize(2);
    int noteX = 30 + (danceFrame * 20) % 180;
    int noteY = 50 + sin(danceFrame * 0.5) * 20;
    StickCP2.Display.setCursor(noteX, noteY);
    StickCP2.Display.print("♪");
    
    danceFrame++;
}
```

**🧪 Test**: Trigger dance mode and hear the Evangelion theme!

---

## 🎮 Part 8: Motion Mechanics (IMU/Gyroscope)

These features use the MPU6886 IMU (accelerometer + gyroscope) to detect physical movement!

### Task 8.1: Sleepy Head 😴

**🎯 Goal**: Character automatically sleeps when device is laid flat on a table.

**💡 How It Works**: When flat, the Z-axis acceleration ≈ 1.0g (gravity pointing down through device).

**Implementation**:
```cpp
bool isLyingFlat() {
    StickCP2.Imu.update();
    
    auto data = StickCP2.Imu.getImuData();
    float accX = data.accel.x;
    float accY = data.accel.y;
    float accZ = data.accel.z;
    
    // Device is flat when:
    // - Z acceleration is close to 1.0 (face up) or -1.0 (face down)
    // - X and Y are close to 0 (not tilted sideways)
    bool isFaceUp = (accZ > 0.8 && abs(accX) < 0.3 && abs(accY) < 0.3);
    bool isFaceDown = (accZ < -0.8 && abs(accX) < 0.3 && abs(accY) < 0.3);
    
    return isFaceUp || isFaceDown;
}

// Add debouncing to prevent flickering
unsigned long flatStartTime = 0;
const unsigned long FLAT_DELAY = 2000;  // 2 seconds before sleeping

bool shouldAutoSleep() {
    if (isLyingFlat()) {
        if (flatStartTime == 0) {
            flatStartTime = millis();
        }
        return (millis() - flatStartTime > FLAT_DELAY);
    } else {
        flatStartTime = 0;
        return false;
    }
}

void checkAutoSleep() {
    if (shouldAutoSleep() && !boyfriend.sleeping) {
        boyfriend.sleeping = true;
        showMessage("*yawn* Goodnight...", 1500);
        // Dim screen
        StickCP2.Display.setBrightness(20);
        drawSleepingCharacter();
    }
}

void drawSleepingCharacter() {
    // Closed eyes (horizontal lines)
    StickCP2.Display.drawLine(90, 100, 110, 100, TFT_WHITE);   // Left eye closed
    StickCP2.Display.drawLine(130, 100, 150, 100, TFT_WHITE);  // Right eye closed
    
    // Floating Z's
    static int zOffset = 0;
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(160 + (zOffset % 30), 80 - (zOffset % 30));
    StickCP2.Display.print("z");
    StickCP2.Display.setCursor(170 + (zOffset % 30), 70 - (zOffset % 30));
    StickCP2.Display.print("Z");
    StickCP2.Display.setCursor(180 + (zOffset % 30), 60 - (zOffset % 30));
    StickCP2.Display.print("Z");
    zOffset++;
}
```

**Wake up on pickup**:
```cpp
void checkWakeUp() {
    if (boyfriend.sleeping && !isLyingFlat()) {
        boyfriend.sleeping = false;
        StickCP2.Display.setBrightness(80);
        showMessage("Good morning!", 1000);
    }
}
```

**🧪 Test**: Place device flat on table for 3 seconds → character sleeps. Pick it up → character wakes!

---

### Task 8.2: Stargazing ✨

**🎯 Goal**: When tilted up toward the sky (> 60° angle), background changes to stars.

**💡 The Math**: Use the accelerometer to calculate pitch angle.

**Implementation**:
```cpp
bool isTiltedUp() {
    StickCP2.Imu.update();
    auto data = StickCP2.Imu.getImuData();
    
    // Calculate pitch angle (rotation around X-axis)
    // When screen faces up toward sky, Y accelerometer reads ~1.0
    float pitch = atan2(data.accel.y, sqrt(data.accel.x * data.accel.x + data.accel.z * data.accel.z));
    float pitchDegrees = pitch * 180.0 / PI;
    
    // Check if tilted more than 60 degrees toward sky
    return pitchDegrees > 60;
}

// Star positions (randomize once at boot)
struct Star {
    int x, y;
    int brightness;  // 0-255
    int twinkleSpeed;
};
Star stars[20];

void initStars() {
    for (int i = 0; i < 20; i++) {
        stars[i].x = random(240);
        stars[i].y = random(135);
        stars[i].brightness = random(100, 255);
        stars[i].twinkleSpeed = random(50, 200);
    }
}

void drawStarryBackground() {
    StickCP2.Display.fillScreen(0x000020);  // Dark blue
    
    unsigned long now = millis();
    
    for (int i = 0; i < 20; i++) {
        // Twinkle effect
        int twinkle = (sin(now / stars[i].twinkleSpeed) + 1) * 0.5 * stars[i].brightness;
        uint16_t color = StickCP2.Display.color565(twinkle, twinkle, twinkle);
        
        // Draw star
        StickCP2.Display.drawPixel(stars[i].x, stars[i].y, color);
        // Make some stars bigger
        if (stars[i].brightness > 200) {
            StickCP2.Display.drawPixel(stars[i].x + 1, stars[i].y, color);
            StickCP2.Display.drawPixel(stars[i].x, stars[i].y + 1, color);
        }
    }
    
    // Optional: Draw a moon
    StickCP2.Display.fillCircle(200, 30, 15, TFT_YELLOW);
    StickCP2.Display.fillCircle(195, 25, 12, 0x000020);  // Crescent shadow
}

// In your main loop:
void updateBackground() {
    if (isTiltedUp()) {
        drawStarryBackground();
        // Draw character in "looking up" pose
        drawCharacterLookingUp();
    } else {
        // Normal background
        StickCP2.Display.fillScreen(TFT_BLACK);
    }
}
```

**🧪 Test**: Tilt device screen-up toward ceiling → stars appear with twinkling!

---

### Task 8.3: Shake to Clean 🧹

**🎯 Goal**: When room is messy, shake the device vigorously to clean it!

**💡 Detection**: Measure acceleration magnitude spikes to detect shaking.

**Implementation**:
```cpp
// Room cleanliness stat
int roomCleanliness = 100;  // 0-100

// Shake detection
int shakeCount = 0;
unsigned long lastShakeTime = 0;
const int SHAKES_NEEDED = 10;

bool detectShake() {
    StickCP2.Imu.update();
    auto data = StickCP2.Imu.getImuData();
    
    // Calculate total acceleration magnitude
    float magnitude = sqrt(
        data.accel.x * data.accel.x +
        data.accel.y * data.accel.y +
        data.accel.z * data.accel.z
    );
    
    // A strong shake creates acceleration > 2.0g
    return magnitude > 2.0;
}

void checkShakeToCclean() {
    if (roomCleanliness >= 80) return;  // Room is clean enough
    
    if (detectShake()) {
        unsigned long now = millis();
        
        // Debounce: only count if 100ms since last shake
        if (now - lastShakeTime > 100) {
            shakeCount++;
            lastShakeTime = now;
            
            // Visual feedback
            StickCP2.Display.fillRect(10, 200, shakeCount * 20, 20, TFT_GREEN);
            StickCP2.Speaker.tone(400 + shakeCount * 50, 50);
            
            if (shakeCount >= SHAKES_NEEDED) {
                // Cleaning complete!
                roomCleanliness = 100;
                shakeCount = 0;
                showMessage("Sparkly clean!", 1500);
                StickCP2.Speaker.tone(1000, 100);
                delay(100);
                StickCP2.Speaker.tone(1200, 100);
                delay(100);
                StickCP2.Speaker.tone(1500, 200);
            }
        }
    }
    
    // Reset shake count if no shakes for 1 second
    if (millis() - lastShakeTime > 1000) {
        shakeCount = 0;
    }
}

void drawMessyRoom() {
    if (roomCleanliness < 50) {
        // Draw dust bunnies
        for (int i = 0; i < 5; i++) {
            int x = 20 + i * 45;
            StickCP2.Display.fillCircle(x, 200, 8, TFT_DARKGREY);
            StickCP2.Display.drawLine(x-5, 195, x-10, 190, TFT_DARKGREY);  // Dust swirl
        }
        
        // Character looks disgusted
        StickCP2.Display.setCursor(80, 50);
        StickCP2.Display.print("It's dirty...");
    }
}

// Room gets messy over time
void decayRoomCleanliness() {
    static unsigned long lastDecay = 0;
    if (millis() - lastDecay > 60000) {  // Every minute
        roomCleanliness = max(0, roomCleanliness - 5);
        lastDecay = millis();
    }
}
```

**🧪 Test**: Wait for room to get dirty, then shake vigorously 10 times → room sparkles clean!

---

### Task 8.4: Pedometer (Walking) 🚶

**🎯 Goal**: Character gains energy when she carries him while walking.

**💡 Step Detection**: Look for rhythmic acceleration peaks typical of walking.

**Implementation**:
```cpp
// Step detection variables
int stepCount = 0;
float lastMagnitude = 0;
bool stepPending = false;
unsigned long lastStepTime = 0;

void detectSteps() {
    StickCP2.Imu.update();
    auto data = StickCP2.Imu.getImuData();
    
    float magnitude = sqrt(
        data.accel.x * data.accel.x +
        data.accel.y * data.accel.y +
        data.accel.z * data.accel.z
    );
    
    unsigned long now = millis();
    
    // Step detection algorithm:
    // 1. Magnitude crosses threshold going UP
    // 2. Minimum 250ms between steps (max 4 steps/second = running)
    // 3. Maximum 2000ms between steps (otherwise it's not walking)
    
    const float STEP_THRESHOLD = 1.2;  // G-force threshold
    
    if (!stepPending && magnitude > STEP_THRESHOLD && lastMagnitude <= STEP_THRESHOLD) {
        // Rising edge detected
        if (now - lastStepTime > 250 && now - lastStepTime < 2000) {
            stepCount++;
            lastStepTime = now;
            
            // Every 50 steps, gain energy!
            if (stepCount % 50 == 0) {
                boyfriend.updateEnergy(5);
                showMessage("Exercise is fun!", 500);
            }
        }
        stepPending = true;
    }
    
    if (magnitude < STEP_THRESHOLD - 0.2) {  // Hysteresis
        stepPending = false;
    }
    
    lastMagnitude = magnitude;
}

void drawStepCounter() {
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(180, 220);
    StickCP2.Display.printf("%d", stepCount);
    
    // Small walking icon
    StickCP2.Display.setCursor(160, 220);
    StickCP2.Display.print("🚶");
}
```

**🧪 Test**: Walk around with the device → step counter increases → energy goes up every 50 steps!

---

## 🎤 Part 9: Magic Mechanics (Microphone & Input)

These features use the microphone and creative button interactions!

### Task 9.1: Blow to Cool/Wish 💨

**🎯 Goal**: Detect blowing into the microphone to cool hot food or blow out birthday candles.

**💡 How It Works**: Blowing creates low-frequency noise that we can detect as sustained microphone activity.

> **Note**: The M5StickC Plus 2 has a built-in PDM microphone!

**Implementation**:
```cpp
#include <driver/i2s.h>

// I2S microphone configuration for M5StickC Plus 2
#define I2S_PIN_CLK      0
#define I2S_PIN_DATA     34
#define SAMPLE_RATE      16000
#define SAMPLE_BUFFER    256

int16_t sampleBuffer[SAMPLE_BUFFER];

void setupMicrophone() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = SAMPLE_BUFFER,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = I2S_PIN_CLK,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_PIN_DATA
    };
    
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

bool detectBlowing() {
    size_t bytesRead;
    i2s_read(I2S_NUM_0, sampleBuffer, sizeof(sampleBuffer), &bytesRead, portMAX_DELAY);
    
    // Calculate average amplitude (blowing = sustained loud noise)
    long sum = 0;
    for (int i = 0; i < SAMPLE_BUFFER; i++) {
        sum += abs(sampleBuffer[i]);
    }
    int avgAmplitude = sum / SAMPLE_BUFFER;
    
    // Blowing threshold (adjust based on testing)
    return avgAmplitude > 3000;
}

// Track sustained blowing
unsigned long blowStartTime = 0;
int blowProgress = 0;

void checkBlowing() {
    if (detectBlowing()) {
        if (blowStartTime == 0) {
            blowStartTime = millis();
        }
        
        blowProgress = (millis() - blowStartTime) / 100;  // Progress bar
        
        // Draw blowing feedback
        drawBlowMeter(blowProgress);
        
        if (blowProgress >= 20) {  // 2 seconds of blowing
            onBlowComplete();
            blowStartTime = 0;
            blowProgress = 0;
        }
    } else {
        // Reset if stopped blowing
        if (millis() - blowStartTime > 500) {
            blowStartTime = 0;
            blowProgress = 0;
        }
    }
}

void drawBlowMeter(int progress) {
    // Wind lines animation
    for (int i = 0; i < 5; i++) {
        int x = 50 + i * 30;
        int len = random(10, 30);
        StickCP2.Display.drawLine(x, 120, x + len, 120 + random(-5, 5), TFT_CYAN);
    }
    
    // Progress bar
    StickCP2.Display.fillRect(50, 200, progress * 7, 10, TFT_CYAN);
    StickCP2.Display.drawRect(50, 200, 140, 10, TFT_WHITE);
}

void onBlowComplete() {
    // Context-dependent action
    if (isHotFoodServed) {
        coolDownFood();
        showMessage("*phew* Nice and cool!", 1500);
    } else if (isBirthday) {
        blowOutCandles();
        showMessage("Make a wish! ✨", 2000);
    } else {
        // Default: character's hair blows
        showMessage("Woosh! Windy!", 1000);
    }
}
```

**🧪 Test**: Blow gently into the microphone → wind effect appears → action triggers!

---

### Task 9.2: Love Meter (Button Mash) 💕

**🎯 Goal**: A mini-game where she mashes the button as fast as possible to fill a heart bar!

**💡 Game Design**: 
- 10 second timer
- Track button presses per second
- Fill heart based on speed
- Fun encouraging messages

**Implementation**:
```cpp
// Game state
enum LoveMeterState { LM_IDLE, LM_COUNTDOWN, LM_PLAYING, LM_RESULT };
LoveMeterState loveMeterState = LM_IDLE;

int buttonPresses = 0;
unsigned long gameStartTime = 0;
unsigned long lastPressTime = 0;
const unsigned long GAME_DURATION = 10000;  // 10 seconds

void startLoveMeter() {
    loveMeterState = LM_COUNTDOWN;
    buttonPresses = 0;
    
    // Countdown: 3, 2, 1, GO!
    for (int i = 3; i > 0; i--) {
        StickCP2.Display.fillScreen(TFT_BLACK);
        StickCP2.Display.setTextSize(4);
        StickCP2.Display.setCursor(110, 60);
        StickCP2.Display.printf("%d", i);
        StickCP2.Speaker.tone(440, 200);
        delay(1000);
    }
    
    StickCP2.Display.fillScreen(TFT_BLACK);
    StickCP2.Display.setCursor(80, 60);
    StickCP2.Display.setTextColor(TFT_GREEN);
    StickCP2.Display.print("GO!");
    StickCP2.Speaker.tone(880, 300);
    delay(500);
    
    loveMeterState = LM_PLAYING;
    gameStartTime = millis();
    StickCP2.Display.setTextColor(TFT_WHITE);
}

void updateLoveMeter() {
    if (loveMeterState != LM_PLAYING) return;
    
    unsigned long elapsed = millis() - gameStartTime;
    unsigned long remaining = GAME_DURATION - elapsed;
    
    // Check for button press
    StickCP2.update();
    if (StickCP2.BtnA.wasPressed()) {
        buttonPresses++;
        lastPressTime = millis();
        
        // Satisfying click sound
        StickCP2.Speaker.tone(600 + buttonPresses * 10, 30);
    }
    
    // Draw game screen
    StickCP2.Display.fillScreen(TFT_BLACK);
    
    // Timer
    StickCP2.Display.setTextSize(2);
    StickCP2.Display.setCursor(10, 10);
    StickCP2.Display.printf("Time: %.1f", remaining / 1000.0);
    
    // Press counter
    StickCP2.Display.setCursor(150, 10);
    StickCP2.Display.printf("x%d", buttonPresses);
    
    // Instruction
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(60, 50);
    StickCP2.Display.print("MASH BUTTON A!");
    
    // Heart fill meter (0-100 based on presses)
    int fillPercent = min(100, buttonPresses * 2);  // 50 presses = full
    drawHeartMeter(fillPercent);
    
    // Encouraging messages based on speed
    float pressesPerSecond = buttonPresses / (elapsed / 1000.0);
    StickCP2.Display.setCursor(60, 200);
    if (pressesPerSecond > 8) {
        StickCP2.Display.print("INCREDIBLE!! <3 <3 <3");
    } else if (pressesPerSecond > 5) {
        StickCP2.Display.print("Amazing! Keep going!");
    } else if (pressesPerSecond > 3) {
        StickCP2.Display.print("Good rhythm!");
    } else {
        StickCP2.Display.print("Faster! Show your love!");
    }
    
    // Game over check
    if (elapsed >= GAME_DURATION) {
        loveMeterState = LM_RESULT;
        showLoveMeterResult();
    }
}

void drawHeartMeter(int percent) {
    int centerX = 120;
    int centerY = 130;
    int size = 40;
    
    // Heart outline
    // Left bump
    StickCP2.Display.drawCircle(centerX - size/3, centerY - size/4, size/3, TFT_RED);
    // Right bump
    StickCP2.Display.drawCircle(centerX + size/3, centerY - size/4, size/3, TFT_RED);
    // Bottom point (triangle)
    StickCP2.Display.drawLine(centerX - size/2 - 5, centerY, centerX, centerY + size, TFT_RED);
    StickCP2.Display.drawLine(centerX + size/2 + 5, centerY, centerX, centerY + size, TFT_RED);
    
    // Fill from bottom up based on percent
    int fillHeight = (size + size/4) * percent / 100;
    for (int y = 0; y < fillHeight; y++) {
        int yPos = centerY + size - y;
        // Calculate width at this height (simplified)
        int width = min(size, y * 2);
        StickCP2.Display.drawLine(centerX - width/2, yPos, centerX + width/2, yPos, TFT_RED);
    }
}

void showLoveMeterResult() {
    StickCP2.Display.fillScreen(TFT_BLACK);
    
    float pressesPerSecond = buttonPresses / 10.0;
    
    StickCP2.Display.setTextSize(2);
    StickCP2.Display.setCursor(40, 40);
    StickCP2.Display.printf("Score: %d", buttonPresses);
    
    StickCP2.Display.setCursor(40, 70);
    StickCP2.Display.printf("%.1f pps", pressesPerSecond);
    
    StickCP2.Display.setTextSize(1);
    StickCP2.Display.setCursor(40, 110);
    
    // Result message
    if (buttonPresses >= 80) {
        StickCP2.Display.print("LOVE OVERLOAD!!");
        // Play victory fanfare
        StickCP2.Speaker.tone(523, 150); delay(150);
        StickCP2.Speaker.tone(659, 150); delay(150);
        StickCP2.Speaker.tone(784, 150); delay(150);
        StickCP2.Speaker.tone(1047, 400);
        boyfriend.updateHappiness(10);
    } else if (buttonPresses >= 50) {
        StickCP2.Display.print("True love confirmed!");
        boyfriend.updateHappiness(5);
    } else if (buttonPresses >= 30) {
        StickCP2.Display.print("Warm and fuzzy!");
        boyfriend.updateHappiness(3);
    } else {
        StickCP2.Display.print("Keep practicing! ;)");
        boyfriend.updateHappiness(1);
    }
    
    delay(3000);
    loveMeterState = LM_IDLE;
}
```

**Integration with menu**:
```cpp
// Add to your menu options
const char* menuOptions[] = {
    "Feed",
    "Play", 
    "Talk",
    "Dance",
    "Love Test"  // <-- NEW!
};

void handleMenuSelection(int selection) {
    switch (selection) {
        case 4:  // Love Test
            startLoveMeter();
            break;
        // ... other cases
    }
}
```

**🧪 Test**: Select "Love Test" → countdown appears → mash button for 10 seconds → get score & happiness boost!

---

## 📝 Progress Tracker

Use this to track your progress:

| Task                   | Difficulty | Status        |
| ---------------------- | ---------- | ------------- |
| 1.1 Minimal Boot       | ⭐          | ✅ Completed   |
| 1.2 Button Detection   | ⭐          | ✅ Completed   |
| 1.3 Frame Rate Control | ⭐⭐         | 🔄 Enhancement |
| 2.1 Status Bars        | ⭐⭐         | ✅ Completed   |
| 2.2 Stat Decay         | ⭐⭐         | ✅ Completed   |
| 2.3 Need Icons         | ⭐⭐         | ✅ Completed   |
| 3.1 Animation          | ⭐⭐         | ✅ Completed   |
| 3.2 Expressions        | ⭐⭐         | ✅ Completed   |
| 4.1 Slot Game          | ⭐⭐⭐        | ⬜ Not Started |
| 4.2 Tilt Game          | ⭐⭐⭐        | ⬜ Not Started |
| 5.1 Sound Effects      | ⭐          | ✅ Partial     |
| 5.2 RTTTL Player       | ⭐⭐⭐        | ⬜ Not Started |
| 6.1 Save/Load          | ⭐⭐         | ✅ Completed   |
| 6.2 First Boot         | ⭐⭐         | ✅ Completed   |
| 7.1 Battery Hangover   | ⭐⭐         | ⬜ Not Started |
| 7.2 Fortune Cookie     | ⭐          | ⬜ Not Started |
| 7.3 Anime Music        | ⭐⭐⭐        | ⬜ Not Started |
| 8.1 Sleepy Head        | ⭐⭐         | ⬜ Not Started |
| 8.2 Stargazing         | ⭐⭐         | ⬜ Not Started |
| 8.3 Shake to Clean     | ⭐⭐         | ⬜ Not Started |
| 8.4 Pedometer          | ⭐⭐         | ⬜ Not Started |
| 9.1 Blow to Cool       | ⭐⭐⭐        | ⬜ Not Started |
| 9.2 Love Meter         | ⭐⭐         | ⬜ Not Started |
| Bonus A: Events        | 💪         | ⬜ Not Started |
| Bonus B: Sleep         | 💪         | ⬜ Not Started |
| Bonus C: Sprites       | 💪         | ⬜ Not Started |
| Bonus D: Special Dates | 💪         | ⬜ Not Started |
| Bonus E: Pedometer     | 💪         | ⬜ Not Started |

---

## 🔗 Helpful Resources

### Official Documentation
- [M5StickC Plus 2 Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
- [M5StickCPlus2 Library](https://github.com/m5stack/M5StickCPlus2)
- [M5GFX Graphics Library](https://github.com/m5stack/M5GFX)
- [M5Unified Library](https://github.com/m5stack/M5Unified) - Recommended modern approach

### Reference Projects (Study These!)
- [TiMiNoo](https://github.com/TME520/TiMiNoo) - Arduino cat Tamagotchi with feeding, sleeping, mini-games
  - Great for: Animation patterns, stat systems, button handling, frame counters
  - Key files: `TiMiNoo.ino` (main loop), `sprites.h` (XBM graphics)
- [OpenELAB M5StickCPlus2 Slot](https://github.com/OpenELAB/OpenELAB-M5StickCPlus2-Slot) - Slot machine game
  - Great for: M5GFX sprites, RGB565 graphics, state machines, game architecture
  - Key files: `Slot.cpp`, `Slot.h`, `M5StickCPlus2_slot.ino`
- [M5StickC Plus 2 User Demo](https://github.com/m5stack/M5StickCPlus2-UserDemo)

### Tools
- [RTTTL Ringtones Collection](https://picaxe.com/rtttl-ringtones-for-tune-command/)
- [Image2CPP Converter](https://javl.github.io/image2cpp/) - Convert images to C arrays for pushImage
- [Piskel](https://www.piskelapp.com/) - Free pixel art editor
- [LVGL Online Image Converter](https://lvgl.io/tools/imageconverter) - For color sprites

### M5StickC Plus 2 Hardware Quick Reference
```
Screen: 135 x 240 pixels (1.14" TFT, ST7789V2)
CPU: ESP32-PICO-V3-02 @ 240MHz
RAM: 520KB + 2MB PSRAM
Flash: 8MB
Battery: 200mAh
IMU: MPU6886 (accelerometer + gyroscope)
Buttons: BtnA (front), BtnB (side), Power
Speaker: Buzzer (GPIO2)
RTC: BM8563 (real-time clock)
GPIO: 4 (power hold - CRITICAL!), 2 (speaker)
```

### Key API Patterns
```cpp
// M5StickC Plus 2 Initialization
auto cfg = M5.config();
StickCP2.begin(cfg);
pinMode(4, OUTPUT);      // Power hold
digitalWrite(4, HIGH);   // Keep device on!

// Display
StickCP2.Display.fillScreen(TFT_BLACK);
StickCP2.Display.pushImage(x, y, w, h, imageArray);  // For sprites

// Buttons
StickCP2.update();
if (StickCP2.BtnA.wasPressed()) { /* ... */ }

// IMU
StickCP2.Imu.getAccel(&ax, &ay, &az);

// Speaker
StickCP2.Speaker.tone(1000, 100);  // 1kHz for 100ms

// RTC
StickCP2.Rtc.getDateTime(&dt);
```

---

## 💡 Final Tips

1. **GPIO4 Power Hold** - Your code already has this! It's CRITICAL for M5StickC Plus 2
2. **Test after every change** - Don't write 50 lines then wonder why it broke
3. **Use the reference projects** - TiMiNoo for game logic, OpenELAB Slot for graphics
4. **Read error messages** - They usually tell you exactly what's wrong
5. **Take breaks** - Fresh eyes catch bugs faster

**Your Progress**: You've already completed the core foundation (Tasks 1.1, 1.2, 2.1, 2.2, 2.3, 3.1, 3.2, 6.1, 6.2). Consider adding a mini-game (Task 4.1 using OpenELAB Slot patterns) or polishing the animations next!

Good luck! 🚀 Your girlfriend is going to love this gift! 💝
