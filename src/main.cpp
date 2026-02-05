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
        // First boot!  Initialize defaults
        EEPROM.write(100, 0x42);
        EEPROM.write(0, 0);   // sleeping = false
        EEPROM.write(1, 8);   // sleep = 8
        EEPROM.write(2, 12);  // happiness = 12
        EEPROM.write(3, 0);   // hunger = 0
        EEPROM.write(4, 12);  // energy = 12
        EEPROM.write(5, 0);   // age = 0
        EEPROM.write(6, 0);   // mood = neutral
        EEPROM.commit();
    }
    
    // Load pet data
    boyfriend.loadFromEEPROM();
    
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

void handlePlayingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    int bounceOffset = (millis() - stateStartTime) % 500 < 250 ? -5 : 5;
    drawCharacter(centerX, centerY + bounceOffset);
    
    int heartOffset = (millis() - stateStartTime) / 100 % 20;
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(centerX - 25, centerY - heartOffset);
    canvas.print("♥");
    canvas.setCursor(centerX + 25, centerY - heartOffset - 5);
    canvas.print("♥");
    
    if (millis() - stateStartTime > 2000) {
        currentState = STATE_IDLE;
    }
}

void handleSleepingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    canvas.fillCircle(centerX, centerY, 20, TFT_WHITE);
    canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 5, TFT_BLACK);
    canvas.drawLine(centerX + 4, centerY - 5, centerX + 8, centerY - 5, TFT_BLACK);
    canvas.drawLine(centerX - 3, centerY + 5, centerX + 3, centerY + 5, TFT_BLACK);
    
    int zOffset = (millis() - stateStartTime) / 500 % 3;
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(centerX + 25, centerY - 20 + zOffset * 5);
    canvas.print("Z");
    canvas.setCursor(centerX + 32, centerY - 15 + zOffset * 5);
    canvas.print("z");
    canvas.setCursor(centerX + 37, centerY - 10 + zOffset * 5);
    canvas.print("z");
    
    // Wake conditions: timeout OR buttons OR device picked up (not flat)
    if (millis() - stateStartTime > 5000 || 
        M5.BtnA.wasPressed() || 
        M5.BtnB.wasPressed() ||
        !isFlat) {  // Add this: wake when device picked up!
        boyfriend.updateEnergy(5);
        currentState = STATE_IDLE;
    }
}

void handleAttentionNeededState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX + 25, centerY - 20);
        canvas.print("!");
        canvas.setTextSize(1);
    }
    
    // Show prompt
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    // Allow button press to exit immediately
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
        return;
    }
    
    if (millis() - stateStartTime > 3000 || 
        (boyfriend.hunger <= 18 && boyfriend.happiness >= 5 && boyfriend.energy >= 5)) {
        currentState = STATE_IDLE;
    }
}

void handleLoveNoteState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(10, centerY - 25);
    canvas.print("I <3 U!");
    
    int heartY = centerY - ((millis() - stateStartTime) / 50 % 30);
    canvas.setCursor(centerX - 30, heartY);
    canvas.print("♥");
    canvas.setCursor(centerX + 20, heartY - 5);
    canvas.print("♥");
    
    if (millis() - stateStartTime > 3000) {
        currentState = STATE_IDLE;
    }
}

// ===== MINIGAME: Menu =====
void handleMinigameMenu() {
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(20, 10);
    canvas.print("MENU");
    canvas.setTextSize(1);
    
    // Display options based on scroll position
    const char* menuOptions[] = {
        "CATCH", "JUMP", "FORTUNE", "DANCE", 
        "SHAKE", "CANDLE", "LOVE", "EXIT"
    };
    int numOptions = 8;
    
    // Show 3 options at a time
    for (int i = 0; i < 3 && (minigameSelection - 1 + i) < numOptions; i++) {
        int optionIndex = minigameSelection - 1 + i;
        if (optionIndex < 0) optionIndex = 0;
        
        canvas.setTextColor(i == 1 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
        canvas.setCursor(20, 35 + (i * 15));
        canvas.print(i == 1 ? "> " : "  ");
        canvas.print(menuOptions[optionIndex]);
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Select B:Next");
    
    if (M5.BtnA.wasPressed()) {
        // Select current option
        stateStartTime = millis();
        switch(minigameSelection) {
            case 0:  // Catch
                currentState = STATE_MINIGAME_CATCH;
                objectX = random(20, 120);
                objectY = 10;
                playerX = 60;
                gameScore = 0;
                gameActive = true;
                break;
            case 1:  // Jump
                currentState = STATE_MINIGAME_JUMP;
                objectX = 0;
                playerX = 30;
                gameScore = 0;
                gameActive = true;
                break;
            case 2:  // Fortune
                currentState = STATE_FORTUNE_COOKIE;
                fortuneIndex = random(0, 8);
                break;
            case 3:  // Dance
                currentState = STATE_DANCE_MUSIC;
                break;
            case 4:  // Shake Clean
                currentState = STATE_SHAKE_CLEAN;
                roomMess = 20;  // Start with messy room
                shakeCount = 0;
                break;
            case 5:  // Blow Candle
                currentState = STATE_BLOW_CANDLE;
                break;
            case 6:  // Love Meter
                currentState = STATE_LOVE_METER;
                heartLevel = 0;
                buttonPressCount = 0;
                loveMeterStartTime = millis();
                break;
            case 7:  // Exit
                currentState = STATE_IDLE;
                minigameSelection = 0;
                M5.Speaker.tone(800, 50);
                break;
        }
        M5.Speaker.tone(1200, 50);
    } else if (M5.BtnB.wasPressed()) {
        // Cycle through options
        minigameSelection++;
        if (minigameSelection >= numOptions) minigameSelection = 0;
        M5.Speaker.tone(1000, 30);
    }
    
    // Long press B to go back (alternative quick exit)
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
        minigameSelection = 0;
        M5.Speaker.tone(800, 50);
    }
}



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
    if (now - lastHungerUpdate > HUNGER_INTERVAL) {
        boyfriend.hunger++;
        if (boyfriend.hunger > 24) boyfriend.hunger = 24;
        lastHungerUpdate = now;
    }
    
    if (now - lastHappinessUpdate > HAPPINESS_INTERVAL) {
        boyfriend.happiness--;
        if (boyfriend.happiness < 0) boyfriend.happiness = 0;
        lastHappinessUpdate = now;
    }
    
    if (now - lastEnergyUpdate > ENERGY_INTERVAL) {
        boyfriend.energy--;
        if (boyfriend.energy < 0) boyfriend.energy = 0;
        lastEnergyUpdate = now;
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
        currentState != STATE_LOVE_METER) {
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
        EEPROM.commit();
        lastSave = now;
    }
    
    // === 9. FRAME RATE CONTROL (consistent frame timing prevents flickering) ===
    delay(50);  // Smooth 20fps for responsive gameplay
}