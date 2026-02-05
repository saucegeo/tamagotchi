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

// ===== STATE MACHINE =====
GameState currentState = STATE_IDLE;
unsigned long stateStartTime = 0;
int animationFrame = 0;
unsigned long lastAnimUpdate = 0;

// Timers for stat decay (like TiMiNoo's frame counters)
unsigned long lastHungerUpdate = 0;
unsigned long lastHappinessUpdate = 0;
unsigned long lastEnergyUpdate = 0;

// Intervals (in milliseconds)
const unsigned long HUNGER_INTERVAL = 30000;     // 30 seconds
const unsigned long HAPPINESS_INTERVAL = 45000;  // 45 seconds
const unsigned long ENERGY_INTERVAL = 60000;     // 60 seconds

// Minigame state
int minigameSelection = 0;  // 0 = catch, 1 = jump, 2 = more options
int playerX = 60;           // Player position
int objectX = 0;            // Falling/moving object position
int objectY = 0;
int gameScore = 0;
bool gameActive = false;

// IMU/Gyro state for motion mechanics
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
bool isFlat = false;
bool isTiltedUp = false;
int shakeCount = 0;
unsigned long lastShakeTime = 0;
int stepCount = 0;
unsigned long lastStepTime = 0;

// Prevent immediate auto-sleep when just starting
unsigned long lastAutoStateChange = 0;
const unsigned long AUTO_STATE_COOLDOWN = 3000;  // 3 seconds before auto-sleep/stargazing can trigger

// Prevent attention state from spamming
unsigned long lastAttentionTime = 0;
const unsigned long ATTENTION_COOLDOWN = 10000;  // 10 seconds before attention can trigger again

// Room cleanliness state
int roomMess = 0;  // 0-10 scale

// Fortune cookie messages
const char* fortuneMessages[] = {
    "You look pretty today",
    "Your smile is magic",
    "I'm lucky to have you",
    "You make me happy",
    "You're amazing!",
    "Today will be great",
    "I believe in you",
    "You're my sunshine"
};
int fortuneIndex = 0;

// RTTTL music player state
const char* evangelionRTTTL = "Evangelion:d=8,o=3,b=120:8a5,8p,8c6,8p,32c#6,8d6,32p,6c6,4d6,8d6,8g6,8f6,16e6,16d6,16p,4e6";
bool isMusicPlaying = false;
int musicNoteIndex = 0;



// Microphone state
int micLevel = 0;
int blowThreshold = 500;  // Adjust based on testing


// Love meter state
int heartLevel = 0;
int buttonPressCount = 0;
unsigned long loveMeterStartTime = 0;

const SpecialDay specialDays[] = {
    {2, 14, "Valentine's", "Happy Valentine's Day! I love you! ❤"},
    {2, 20, "Birthday", "Happy Birthday, my love! You're amazing!"},
    {6, 7, "Birthday", "It's my birthday! Thanks for being here!"}
};

int currentSpecialDay = -1;  // -1 = no special day, else index into specialDays[]

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

// ===== HELPER:  Draw Progress Bar =====
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    canvas.drawRect(x, y, width, height, TFT_WHITE);
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        canvas.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}

// ===== HELPER:  Draw Stats UI =====
void drawStats() {
    canvas.setTextSize(1);
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Batt:%d%%", M5.Power.getBatteryLevel());
    
    drawProgressBar(5, 20, 80, 8, boyfriend.hunger, 24, (boyfriend.hunger > 18) ? TFT_RED : TFT_ORANGE);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(90, 22);
    canvas.printf("H:%d", boyfriend.hunger);
    
    drawProgressBar(5, 32, 80, 8, boyfriend.happiness, 24, TFT_YELLOW);
    canvas.setCursor(90, 34);
    canvas.printf("Hp:%d", boyfriend.happiness);
    
    drawProgressBar(5, 44, 80, 8, boyfriend.energy, 24, TFT_CYAN);
    canvas.setCursor(90, 46);
    canvas.printf("E:%d", boyfriend.energy);
    
    // Show status messages or special day banner
    if (currentSpecialDay >= 0) {
        // Special day banner!
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print(specialDays[currentSpecialDay].name);
    } else if (boyfriend.hunger > 18) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("HUNGRY!");
    } else if (boyfriend.happiness < 5) {
        canvas.setTextColor(TFT_BLUE, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("SAD :(");
    } else if (boyfriend.energy < 5) {
        canvas.setTextColor(TFT_PURPLE, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("TIRED!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Menu B:Feed");
}

// ===== HELPER: Draw Character =====
void drawCharacter(int x, int y, bool heartEyes) {
    // Breathing animation
    int breathOffset = 0;
    switch (animationFrame % 4) {
        case 0: breathOffset = 0; break;
        case 1: breathOffset = 1; break;
        case 2: breathOffset = 2; break;
        case 3: breathOffset = 1; break;
    }
    
    int radius = 20 + breathOffset;
    
    // Body (circle)
    canvas.fillCircle(x, y, radius, TFT_WHITE);
    
    // Eyes (expression based on happiness or heartEyes override)
    int eyeY = y - 5;
    if (heartEyes) {
        // Heart eyes! ♥
        canvas.setTextColor(TFT_RED, TFT_WHITE);
        canvas.setTextSize(1);
        canvas.setCursor(x - 8, eyeY - 3);
        canvas.print("♥");
        canvas.setCursor(x + 2, eyeY - 3);
        canvas.print("♥");
    } else if (boyfriend.happiness > 15) {
        // Happy eyes (arcs)
        canvas.drawLine(x - 8, eyeY, x - 4, eyeY - 2, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY - 2, x + 8, eyeY, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Sad eyes
        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
    } else {
        // Normal eyes
        canvas.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    }
    
    // Mouth
    int mouthY = y + 5;
    if (boyfriend.happiness > 15) {
        // Smile
        canvas.drawArc(x, mouthY - 2, 6, 4, 180, 360, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Frown
        canvas.drawArc(x, mouthY + 4, 6, 4, 0, 180, TFT_BLACK);
    } else {
        // Neutral
        canvas.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);
    }
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
        "SHAKE", "CANDLE", "LOVE"
    };
    int numOptions = 7;
    
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
    canvas.print("A:OK B:Next");
    
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
        }
        M5.Speaker.tone(1200, 50);
    } else if (M5.BtnB.wasPressed()) {
        // Cycle through options
        minigameSelection++;
        if (minigameSelection >= numOptions) minigameSelection = 0;
        M5.Speaker.tone(1000, 30);
    }
    
    // Long press B to go back
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
        minigameSelection = 0;
        M5.Speaker.tone(800, 50);
    }
}

// ===== NEW MECHANIC: Dance with Anime Music =====
// Simple RTTTL player - this is a basic template
void handleDanceMusic() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Animated dancing character
    int bounceOffset = (millis() - stateStartTime) % 400 < 200 ? -8 : 8;
    int rotateOffset = (millis() - stateStartTime) % 800 < 400 ? -5 : 5;
    
    canvas.fillCircle(centerX + rotateOffset, centerY + bounceOffset, 20, TFT_WHITE);
    canvas.fillCircle(centerX - 6, centerY + bounceOffset - 5, 3, TFT_BLACK);
    canvas.fillCircle(centerX + 6, centerY + bounceOffset - 5, 3, TFT_BLACK);
    canvas.drawArc(centerX, centerY + bounceOffset + 5, 8, 6, 180, 360, TFT_BLACK);
    
    // Music notes animation
    int noteY = centerY - ((millis() - stateStartTime) / 100 % 30);
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(centerX - 30, noteY);
    canvas.print("♪");
    canvas.setCursor(centerX + 25, noteY - 10);
    canvas.print("♫");
    canvas.setTextSize(1);
    
    // Play a simple tone (real RTTTL would require a parser)
    // This is just a template - you'll implement the full RTTTL parser
    static unsigned long lastTone = 0;
    if (millis() - lastTone > 300) {
        int notes[] = {880, 1047, 1109, 1175, 1047, 1175};  // A5, C6, C#6, D6...
        int noteIndex = ((millis() - stateStartTime) / 300) % 6;
        M5.Speaker.tone(notes[noteIndex], 200);
        lastTone = millis();
    }
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(20, 20);
    canvas.print("DANCING!");
    
    // End after 10 seconds or button press
    if (millis() - stateStartTime > 10000 || M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        boyfriend.updateHappiness(3);
        boyfriend.updateEnergy(-2);
        currentState = STATE_IDLE;
    }
}


// ===== NEW MECHANIC: Shake to Clean =====
void handleShakeClean() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw messy room (X marks)
    canvas.setTextColor(TFT_BROWN, TFT_BLACK);
    for (int i = 0; i < roomMess; i++) {
        int mx = 10 + (i * 15) % 100;
        int my = 20 + (i * 7) % 40;
        canvas.setCursor(mx, my);
        canvas.print("x");
    }
    
    // Progress bar
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, 5);
    canvas.printf("Clean: %d/20", 20 - roomMess);
    drawProgressBar(10, 15, 100, 8, 20 - roomMess, 20, TFT_GREEN);
    
    // Character cleaning
    drawCharacter(centerX, centerY);
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 70);
    canvas.print("SHAKE TO CLEAN!");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Shake count: ");
    canvas.print(shakeCount);
    
    // Decrease mess with each shake
    if (shakeCount > 0) {
        roomMess -= shakeCount;
        shakeCount = 0;  // Reset after processing
        if (roomMess < 0) roomMess = 0;
    }
    
    // Complete when room is clean
    if (roomMess <= 0) {
        boyfriend.updateHappiness(3);
        M5.Speaker.tone(1500, 100);
        currentState = STATE_IDLE;
    }
    
    // Exit button
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
    }
}

// ===== NEW MECHANIC: Love Meter (Button Mash) =====
void handleLoveMeter() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2;
    
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(10, 10);
    canvas.print("LOVE METER");
    canvas.setTextSize(1);
    
    // Heart bar
    drawProgressBar(10, 35, 120, 20, heartLevel, 100, TFT_RED);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(55, 40);
    canvas.printf("%d%%", heartLevel);
    
    // Character pumping hearts
    int pumpOffset = (millis() % 200) < 100 ? -3 : 3;
    drawCharacter(centerX, centerY + pumpOffset);
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 65);
    canvas.print("MASH BUTTONS!");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.printf("Presses: %d", buttonPressCount);
    
    // Count button presses
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        buttonPressCount++;
        heartLevel += 2;
        if (heartLevel > 100) heartLevel = 100;
        M5.Speaker.tone(1000 + (heartLevel * 10), 30);
    }
    
    // Time limit: 10 seconds
    unsigned long timeLeft = 10000 - (millis() - loveMeterStartTime);
    if (timeLeft > 10000) timeLeft = 0;  // Handle overflow
    
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 20);
    canvas.printf("Time: %ld", timeLeft / 1000);
    
    // End game
    if (millis() - loveMeterStartTime > 10000) {
        currentState = STATE_MINIGAME_RESULT;
        gameScore = heartLevel;  // Use heart level as score
        stateStartTime = millis();
        boyfriend.updateHappiness(heartLevel / 20);  // Bonus happiness based on performance
    }
}

// ===== MINIGAME: Result Screen =====
void handleMinigameResult() {
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(30, 30);
    canvas.print("SCORE:");
    canvas.setCursor(50, 50);
    canvas.printf("%d", gameScore);
    canvas.setTextSize(1);
    
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(20, 75);
    if (gameScore > 5) {
        canvas.print("Great job!");
    } else if (gameScore > 2) {
        canvas.print("Nice try!");
    } else {
        canvas.print("Keep practicing!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
    }
}

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
        drawStats();
        
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