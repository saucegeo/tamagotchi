/*
 * Boyfriend Bot - M5StickC Plus 2 Tamagotchi
 * Simplified single-file version inspired by TiMiNoo
 * Easy to debug, easy to extend! 
 */


#include <M5Unified.h>
#include <EEPROM.h>
#include "sprites.h"
#include "GameState.h"

// Note: We don't include .cpp files!
// Each .cpp compiles separately. The linker connects them using
// declarations in GameState.h

// ===== GLOBAL OBJECTS =====

// Create the sprite object (our canvas) -> reduce flickering/smoother
M5Canvas canvas(&M5.Display);

const int SAMPLE_RATE = 16000;  // Microphone sample rate (16 kHz)
const int SAMPLE_COUNT = 256;   // Number of samples to read
int16_t *micSamples;              // Buffer for microphone samples, pointer because of size

// ===== GLOBAL STATE (like TiMiNoo does) =====
Boyfriend boyfriend;
GameState currentState = STATE_IDLE;
unsigned long stateStartTime = 0;
int animationFrame = 0;

// Timers for stat decay (like TiMiNoo's frame counters)
unsigned long lastAnimUpdate = 0;
unsigned long lastHungerUpdate = 0;
unsigned long lastHappinessUpdate = 0;
unsigned long lastEnergyUpdate = 0;

// Intervals (in milliseconds)
const unsigned long HUNGER_INTERVAL = 30000;     // 30 seconds
const unsigned long HAPPINESS_INTERVAL = 45000;  // 45 seconds
const unsigned long ENERGY_INTERVAL = 60000;     // 60 seconds

// IMU/Gyro state for motion mechanics
unsigned long lastShakeTime = 0;
int stepCount = 0;
unsigned long lastStepTime = 0;

// Prevent immediate auto-sleep when just starting
unsigned long lastAutoStateChange = 0;
const unsigned long AUTO_STATE_COOLDOWN = 3000;  // 3 seconds before auto-sleep/stargazing can trigger

// RTTTL music player state
const char* evangelionRTTTL = "Evangelion:d=8,o=3,b=120:8a5,8p,8c6,8p,32c#6,8d6,32p,6c6,4d6,8d6,8g6,8f6,16e6,16d6,16p,4e6";
bool isMusicPlaying = false;
int musicNoteIndex = 0;

const SpecialDay specialDays[] = {
    {2, 14, "Valentine's", "Happy Valentine's Day! I love you! ❤"},
    {2, 20, "Birthday", "Happy Birthday, my love! You're amazing!"},
    {6, 7, "Birthday", "It's my birthday! Thanks for being here!"}
};

// ===== SETUP =====
void setup() {
    // 1. Hardware first
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Initialize Serial for debugging
    Serial.begin(115200);
    Serial.println("\n=== Tamagotchi Starting ===");
    
    // Allocate buffer for microphone samples
    micSamples = (int16_t*)malloc(SAMPLE_COUNT * sizeof(int16_t));
    
    // Initialize microphone (no parameters - uses default config)
    M5.Mic.begin();
    
    // 2. CRITICAL: Power hold pin (FIX #1)
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);  // Keep device powered on! 
    
    // 3. Display setup
    M5.Display.setRotation(1); // Landscape mode
    M5.Display.setBrightness(100);  // Reduced from 128 to save power
    
    // 4. Canvas setup with error handling
    canvas.setColorDepth(8);
    if (!canvas.createSprite(M5.Display.width(), M5.Display.height())) {
        // Fallback:  reduce color depth if creation fails
        canvas.setColorDepth(4);
        canvas.createSprite(M5.Display.width(), M5.Display.height());
    }
    
    // 5. EEPROM initialization
    EEPROM.begin(512);
    
    // First boot detection
    uint8_t magicByte = EEPROM.read(100);
    if (magicByte != 0x42) {
        // First boot!  Initialize defaults for Tamagotchi lifecycle
        EEPROM.write(100, 0x42);
        EEPROM.write(0, 0);   // sleeping = false
        EEPROM.write(1, 8);   // sleep = 8
        EEPROM.write(2, 4);   // happiness = 4 hearts
        EEPROM.write(3, 4);   // hunger = 4 hearts
        EEPROM.write(4, 12);  // energy = 12
        EEPROM.write(5, 0);   // age = 0
        EEPROM.write(6, 0);   // mood = neutral
        EEPROM.write(7, 10);  // weight = 10
        EEPROM.write(8, 0);   // discipline = 0%
        EEPROM.write(9, 0);   // careMistakes = 0
        EEPROM.write(10, 0);  // poopCount = 0
        EEPROM.write(11, 0);  // isSick = false
        EEPROM.write(12, 1);  // stage = BABY
        EEPROM.write(13, 1);  // lightsOn = true
        EEPROM.commit();
    }
    
    // Load pet data
    boyfriend.loadFromEEPROM();
    
    // Set clock time for lifecycle (start of game)
    clockSetTime = millis();
    hatchTime = millis();  // Set hatch time for evolution tracking
    
    // Initialize IMU (gyroscope/accelerometer)
    M5.Imu.begin();
    
    // Initialize microphone
    M5.Mic.begin();
    
    // Check for special days
    // Note: M5StickC Plus 2 doesn't have RTC by default, so we check on boot
    // You can manually set the date or add WiFi time sync later
    // For now, this is a template - you'll need to implement date checking
    
    // TODO: Get current date (from RTC or WiFi)
    // For testing, you can hardcode: int currentMonth = 2; int currentDay = 14;
    // Then check: checkSpecialDay(currentMonth, currentDay);
    
    Serial.println("=== Setup Complete ===");
    
    // Small delay to stabilize power
    delay(100);
}



// ===== STATE HANDLERS =====
// Note: State handlers are now in separate files in /states and /minigames folders

// main loop
void loop() {
    unsigned long now = millis();
    
    // Feed watchdog (FIX #2)
    yield();
    
    // === 1. UPDATE BUTTONS & SENSORS ===
    M5.update();
    
    // DEBUG: Print state and sensor values every 2 seconds
    static unsigned long lastDebugPrint = 0;
    if (now - lastDebugPrint > 2000) {
        Serial.printf("State:%d Flat:%d TiltUp:%d AccZ:%.2f AccY:%.2f BtnA:%d BtnB:%d\n",
                      currentState, isFlat, isTiltedUp, accelZ, accelY, 
                      M5.BtnA.isPressed(), M5.BtnB.isPressed());
        lastDebugPrint = now;
    }
    
    // Read IMU data for motion mechanics
    M5.Imu.getAccel(&accelX, &accelY, &accelZ);
    M5.Imu.getGyro(&gyroX, &gyroY, &gyroZ);
    
    // Read microphone level
    if (M5.Mic.isEnabled() && micSamples != nullptr) {
        M5.Mic.record(micSamples, SAMPLE_COUNT);

        int32_t sum = 0;
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            sum += abs(micSamples[i]);
        }
        micLevel = sum / SAMPLE_COUNT;
    }
    
    // === BATTERY HANGOVER: Slow movement if low battery ===
    int batteryLevel = M5.Power.getBatteryLevel();
    bool lowBattery = batteryLevel < 20;
    
    // === SLEEPY HEAD: Detect if device is flat on back ===
    // When flat, Z acceleration ~= 1.0g, X and Y ~= 0
    // CRITICAL FIX: Add cooldown to prevent accidental triggers!
    isFlat = (abs(accelZ - 1.0) < 0.2 && abs(accelX) < 0.2 && abs(accelY) < 0.2);
    
    // Only auto-sleep if:
    // 1. Currently in IDLE state (NOT in menus, games, or attention state)
    // 2. Device has been flat for 3+ seconds (cooldown expired)
    // 3. User isn't actively playing
    // CRITICAL: Don't auto-sleep during attention/menu/game states!
    if (isFlat && currentState == STATE_IDLE && 
        (now - lastAutoStateChange > AUTO_STATE_COOLDOWN)) {
        currentState = STATE_SLEEPING;
        stateStartTime = now;
        lastAutoStateChange = now;  // Reset cooldown
    }
    
    // Wake from sleep when device picked up (only if actually sleeping)
    if (!isFlat && currentState == STATE_SLEEPING) {
        currentState = STATE_IDLE;
        lastAutoStateChange = now;  // Reset cooldown
    }
    
    // === STARGAZING: Detect tilt up (towards sky) ===
    // When tilted up, Y acceleration becomes more negative
    // CRITICAL FIX: Add cooldown to prevent accidental triggers!
    isTiltedUp = (accelY < -0.7);  // Made less sensitive (was -0.6)
    
    // Only auto-stargaze if in idle state (don't interrupt other activities)
    if (isTiltedUp && currentState == STATE_IDLE && 
        (now - lastAutoStateChange > AUTO_STATE_COOLDOWN)) {
        currentState = STATE_STARGAZING;
        stateStartTime = now;
        lastAutoStateChange = now;  // Reset cooldown
    }
    
    // === SHAKE DETECTION: For cleaning mini-game ===
    float totalAccel = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
    if (totalAccel > 2.0) {  // Strong shake detected
        if (now - lastShakeTime > 300) {  // Debounce
            shakeCount++;
            lastShakeTime = now;
            if (currentState == STATE_SHAKE_CLEAN) {
                M5.Speaker.tone(1500, 30);
            }
        }
    }
    
    // === PEDOMETER: Step detection (simple version) ===
    // Detect rhythmic up-down motion
    static float lastAccelZ = 0;
    if (accelZ > 1.2 && lastAccelZ < 1.0) {
        if (now - lastStepTime > 400 && now - lastStepTime < 1200) {
            stepCount++;
            if (stepCount % 10 == 0) {
                boyfriend.updateEnergy(1);  // Gain energy from walking
            }
        }
        lastStepTime = now;
    }
    lastAccelZ = accelZ;
    
    // === POWER BUTTON: Check for press (GPIO39 on M5StickC Plus2) ===
    // This is a simple template - adjust based on your hardware
    // M5.BtnPWR can be used if available in M5Unified
    // For now, this is a placeholder for you to implement
    
    // If battery is low, reduce animation speed
    int animSpeed = lowBattery ? 400 : 200;
    
    // === 2. HANDLE STAT DECAY (FIX #3 - removed immediate EEPROM writes from Boyfriend class) ===
    // Hearts decrease over time (4-heart system)
    if (now - lastHungerUpdate > HUNGER_INTERVAL) {
        boyfriend.hunger--;  // Lose 1 hunger heart every 30 seconds
        if (boyfriend.hunger < 0) boyfriend.hunger = 0;
        lastHungerUpdate = now;
    }
    
    if (now - lastHappinessUpdate > HAPPINESS_INTERVAL) {
        boyfriend.happiness--;  // Lose 1 happiness heart every 45 seconds
        if (boyfriend.happiness < 0) boyfriend.happiness = 0;
        lastHappinessUpdate = now;
    }
    
    if (now - lastEnergyUpdate > ENERGY_INTERVAL) {
        boyfriend.energy--;
        if (boyfriend.energy < 0) boyfriend.energy = 0;
        lastEnergyUpdate = now;
    }
    
    // === POOP SCHEDULING ===
    if (currentState != STATE_DEAD) {
        schedulePoop();
    }
    
    // === EVOLUTION CHECK ===
    if (boyfriend.stage != STAGE_ADULT && currentState != STATE_DEAD) {
        checkEvolution();
    }
    
    // === RANDOM DISCIPLINE CALLS ===
    // Random call every 30-60 minutes when idle
    if (currentState == STATE_IDLE && boyfriend.stage >= STAGE_CHILD) {
        if (nextDisciplineCall == 0) {
            nextDisciplineCall = now + random(1800000, 3600000);  // 30-60 min
        }
        if (now >= nextDisciplineCall) {
            // Trigger discipline call
            currentState = STATE_DISCIPLINE_CALL;
            stateStartTime = now;
            nextDisciplineCall = 0;  // Reset for next time
        }
    }
    
    // === CHECK FOR DEATH CONDITIONS ===
    // Only check if not already dead
    if (currentState != STATE_DEAD && !isDead) {
        if (checkDeathConditions()) {
            isDead = true;
            currentState = STATE_DEAD;
            stateStartTime = now;
            // Play sad death sound
            M5.Speaker.tone(800, 200);
            delay(200);
            M5.Speaker.tone(600, 200);
            delay(200);
            M5.Speaker.tone(400, 400);
        }
    }
    
    // === 3. UPDATE ANIMATION ===
    if (now - lastAnimUpdate > animSpeed) {
        animationFrame++;
        lastAnimUpdate = now;
    }
    
    // === 4. CLEAR CANVAS ===
    canvas. fillScreen(TFT_BLACK);
    
    // === 5. STATE MACHINE ===
    switch (currentState) {
        case STATE_IDLE:
            handleIdleState();
            break;
        case STATE_EATING:
            handleEatingState();
            break;
        case STATE_PLAYING: 
            handlePlayingState();
            break;
        case STATE_SLEEPING:
            handleSleepingState();
            break;
        case STATE_ATTENTION_NEEDED:
            handleAttentionNeededState();
            break;
        case STATE_LOVE_NOTE:
            handleLoveNoteState();
            break;
        case STATE_MINIGAME_MENU:
            handleMinigameMenu();
            break;
        case STATE_MINIGAME_CATCH:
            handleMinigameCatch();
            break;
        case STATE_MINIGAME_JUMP:
            handleMinigameJump();
            break;
        case STATE_MINIGAME_RESULT:
            handleMinigameResult();
            break;
        case STATE_DANCE_MUSIC:
            handleDanceMusic();
            break;
        case STATE_STARGAZING:
            handleStargazing();
            break;

        case STATE_BLOW_CANDLE:
            handleBlowCandle();
            break;

        case STATE_FORTUNE_COOKIE:
            handleFortuneCookie();
            break;
            
        case STATE_SHAKE_CLEAN:
            handleShakeClean();
            break;
        case STATE_LOVE_METER:
            handleLoveMeter();
            break;
        case STATE_DEAD:
            handleDeathState();
            break;
        case STATE_DISCIPLINE_CALL:
            handleDisciplineCall();
            break;
        case STATE_FEED_MENU:
            handleFeedMenu();
            break;
        case STATE_SICK:
            drawCharacter(canvas.width() / 2 + 30, canvas.height() / 2 + 10);
            break;
    }
    
    // === 6. DRAW STATS ON TOP (only in non-minigame states) ===
    if (currentState != STATE_MINIGAME_MENU && 
        currentState != STATE_MINIGAME_CATCH && 
        currentState != STATE_MINIGAME_JUMP && 
        currentState != STATE_MINIGAME_RESULT &&
        currentState != STATE_FORTUNE_COOKIE &&
        currentState != STATE_DANCE_MUSIC &&
        currentState != STATE_STARGAZING &&
        currentState != STATE_SHAKE_CLEAN &&
        currentState != STATE_BLOW_CANDLE &&
        currentState != STATE_LOVE_METER &&
        currentState != STATE_DEAD &&
        currentState != STATE_DISCIPLINE_CALL &&
        currentState != STATE_FEED_MENU) {
        drawStatsBar();
        
        // Show battery warning if low
        if (lowBattery) {
            canvas.setTextColor(TFT_RED, TFT_BLACK);
            canvas.setCursor(5, 72);
            canvas.print("LOW BATTERY!");
        }
    }
    
    // === 7. PUSH CANVAS TO DISPLAY ===
    canvas.pushSprite(0, 0);
    
    // === 8. AUTO-SAVE TO EEPROM (batch writes to prevent flickering) ===
    static unsigned long lastSave = 0;
    if (now - lastSave > 60000) {
        EEPROM.write(0, boyfriend.sleeping);
        EEPROM.write(1, boyfriend.sleep);
        EEPROM.write(2, boyfriend.happiness);
        EEPROM.write(3, boyfriend.hunger);
        EEPROM.write(4, boyfriend.energy);
        EEPROM.write(5, boyfriend.age);
        EEPROM.write(6, boyfriend.mood);
        EEPROM.write(7, boyfriend.weight);
        EEPROM.write(8, boyfriend.discipline);
        EEPROM.write(9, boyfriend.careMistakes);
        EEPROM.write(10, boyfriend.poopCount);
        EEPROM.write(11, boyfriend.isSick);
        EEPROM.write(12, (int)boyfriend.stage);
        EEPROM.write(13, boyfriend.lightsOn);
        EEPROM.commit();
        lastSave = now;
    }
    
    // === 9. FRAME RATE CONTROL (consistent frame timing prevents flickering) ===
    delay(50);  // Smooth 20fps for responsive gameplay
}