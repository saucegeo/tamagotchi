/*
 * Boyfriend Bot - M5StickC Plus 2 Tamagotchi
 * Simplified single-file version inspired by TiMiNoo
 * Easy to debug, easy to extend!
 */

#include "M5StickCPlus2.h"
#include <EEPROM.h>
#include "sprites.h"

// Create the sprite object (our canvas) -> reduce flickering/smoother
M5Canvas canvas(&StickCP2.Display);

// ===== GLOBAL STATE (like TiMiNoo does) =====
Boyfriend boyfriend;

// ===== STATE MACHINE =====
enum GameState {
    STATE_IDLE,
    STATE_EATING,
    STATE_PLAYING,
    STATE_SLEEPING,
    STATE_LOVE_NOTE,
    STATE_SICK,
    STATE_ATTENTION_NEEDED
};

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

// ===== SETUP =====
void setup() {
    // 1. Hardware first
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    StickCP2.Display.setRotation(1); // Landscape mode
    
    // 3. Display setup
    StickCP2.Display.setBrightness(80);
    canvas.createSprite(StickCP2.Display.width(), StickCP2.Display.height());
    
    // 4. EEPROM initialization
    EEPROM.begin(512);
    
    // First boot detection
    uint8_t magicByte = EEPROM.read(100);
    if (magicByte != 0x42) {
        // First boot! Initialize defaults
        EEPROM.write(100, 0x42);  // Magic number
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
}

// ===== HELPER: Draw Progress Bar =====
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    // Outline
    StickCP2.Display.drawRect(x, y, width, height, TFT_WHITE);
    
    // Fill based on value
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        StickCP2.Display.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}

// ===== HELPER: Draw Character (Simple Animation) =====
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
    
    // Body (circle)
    StickCP2.Display.fillCircle(x, y, radius, TFT_WHITE);
    
    // Eyes (expression based on happiness)
    int eyeY = y - 5;
    if (boyfriend.happiness > 15) {
        // Happy eyes (arcs)
        StickCP2.Display.drawLine(x - 8, eyeY, x - 4, eyeY - 2, TFT_BLACK);
        StickCP2.Display.drawLine(x + 4, eyeY - 2, x + 8, eyeY, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Sad eyes
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
        // Smile
        StickCP2.Display.drawArc(x, mouthY - 2, 6, 4, 180, 360, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Frown
        StickCP2.Display.drawArc(x, mouthY + 4, 6, 4, 0, 180, TFT_BLACK);
    } else {
        // Neutral
        StickCP2.Display.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);
    }
}

// ===== HELPER: Draw Stats UI =====
void drawStats() {
    // Battery indicator (top left)
    canvas.setTextSize(1);
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Batt:%d%%", StickCP2.Power.getBatteryLevel());
    
    // Stats with progress bars
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
    
    // Notifications
    if (boyfriend.hunger > 18) {
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
    
    // Help text (bottom)
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Play B:Feed");
}

// ===== STATE HANDLERS =====
void handleIdleState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    // Check for state transitions
    if (StickCP2.BtnB.wasPressed()) {
        currentState = STATE_EATING;
        stateStartTime = millis();
        boyfriend.updateHunger(-5);
        StickCP2.Speaker.tone(1500, 50);
    } else if (StickCP2.BtnA.wasPressed()) {
        currentState = STATE_PLAYING;
        stateStartTime = millis();
        boyfriend.updateHappiness(3);
        StickCP2.Speaker.tone(1000, 50);
    }
    
    // Auto-transition to attention needed state
    if (boyfriend.hunger > 18 || boyfriend.happiness < 5 || boyfriend.energy < 5) {
        currentState = STATE_ATTENTION_NEEDED;
        stateStartTime = millis();
    }
}

void handleEatingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw eating animation (larger mouth/happy face)
    int breathOffset = 1;
    int radius = 20 + breathOffset;
    
    canvas.fillCircle(centerX, centerY, radius, TFT_WHITE);
    
    // Happy eyes
    canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 7, TFT_BLACK);
    canvas.drawLine(centerX + 4, centerY - 7, centerX + 8, centerY - 5, TFT_BLACK);
    
    // Big smile
    canvas.drawArc(centerX, centerY + 3, 8, 6, 180, 360, TFT_BLACK);
    
    // Food emoji/icon
    canvas.fillCircle(centerX - 25, centerY - 10, 5, TFT_RED);
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(centerX - 30, centerY + 15);
    canvas.print("YUM!");
    
    // Return to idle after 2 seconds
    if (millis() - stateStartTime > 2000) {
        currentState = STATE_IDLE;
    }
}

void handlePlayingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw playing animation (bouncing)
    int bounceOffset = (millis() - stateStartTime) % 500 < 250 ? -5 : 5;
    drawCharacter(centerX, centerY + bounceOffset);
    
    // Draw heart particles
    int heartOffset = (millis() - stateStartTime) / 100 % 20;
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(centerX - 25, centerY - heartOffset);
    canvas.print("♥");
    canvas.setCursor(centerX + 25, centerY - heartOffset - 5);
    canvas.print("♥");
    
    // Return to idle after 2 seconds
    if (millis() - stateStartTime > 2000) {
        currentState = STATE_IDLE;
    }
}

void handleSleepingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw sleeping character (closed eyes)
    int radius = 20;
    canvas.fillCircle(centerX, centerY, radius, TFT_WHITE);
    
    // Closed eyes (lines)
    canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 5, TFT_BLACK);
    canvas.drawLine(centerX + 4, centerY - 5, centerX + 8, centerY - 5, TFT_BLACK);
    
    // Small mouth
    canvas.drawLine(centerX - 3, centerY + 5, centerX + 3, centerY + 5, TFT_BLACK);
    
    // Zzz animation
    int zOffset = (millis() - stateStartTime) / 500 % 3;
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(centerX + 25, centerY - 20 + zOffset * 5);
    canvas.print("Z");
    canvas.setCursor(centerX + 32, centerY - 15 + zOffset * 5);
    canvas.print("z");
    canvas.setCursor(centerX + 37, centerY - 10 + zOffset * 5);
    canvas.print("z");
    
    // Wake up after 5 seconds or button press
    if (millis() - stateStartTime > 5000 || StickCP2.BtnA.wasPressed() || StickCP2.BtnB.wasPressed()) {
        boyfriend.updateEnergy(5);
        currentState = STATE_IDLE;
    }
}

void handleAttentionNeededState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw character with attention icon (blinking)
    drawCharacter(centerX, centerY);
    
    // Blinking exclamation mark
    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX + 25, centerY - 20);
        canvas.print("!");
        canvas.setTextSize(1);
    }
    
    // Return to idle after 3 seconds or when needs are met
    if (millis() - stateStartTime > 3000 || 
        (boyfriend.hunger <= 18 && boyfriend.happiness >= 5 && boyfriend.energy >= 5)) {
        currentState = STATE_IDLE;
    }
}

void handleLoveNoteState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    drawCharacter(centerX, centerY);
    
    // Draw love note/message
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(10, centerY - 25);
    canvas.print("I <3 U!");
    
    // Floating hearts
    int heartY = centerY - ((millis() - stateStartTime) / 50 % 30);
    canvas.setCursor(centerX - 30, heartY);
    canvas.print("♥");
    canvas.setCursor(centerX + 20, heartY - 5);
    canvas.print("♥");
    
    // Return to idle after 3 seconds
    if (millis() - stateStartTime > 3000) {
        currentState = STATE_IDLE;
    }
}


void loop() {
    unsigned long now = millis();
    
    // === 1. UPDATE BUTTONS ===
    StickCP2.update();
    
    // === 2. HANDLE STAT DECAY (like TiMiNoo's frame counter system) ===
    if (now - lastHungerUpdate > HUNGER_INTERVAL) {
        boyfriend.updateHunger(1);
        lastHungerUpdate = now;
    }
    
    if (now - lastHappinessUpdate > HAPPINESS_INTERVAL) {
        boyfriend.updateHappiness(-1);
        lastHappinessUpdate = now;
    }
    
    if (now - lastEnergyUpdate > ENERGY_INTERVAL) {
        boyfriend.updateEnergy(-1);
        lastEnergyUpdate = now;
    }
    
    // === 3. UPDATE ANIMATION ===
    if (now - lastAnimUpdate > 200) {  // 5 FPS animation
        animationFrame++;
        lastAnimUpdate = now;
    }
    
    // === 4. CLEAR CANVAS ===
    canvas.fillScreen(TFT_BLACK);
    
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
            
        case STATE_SICK:
            // TODO: Implement sick state
            drawCharacter(canvas.width() / 2 + 30, canvas.height() / 2 + 10);
            break;
    }
    
    // === 6. ALWAYS DRAW STATS ON TOP ===
    drawStats();
    
    // === 7. PUSH CANVAS TO DISPLAY ===
    canvas.pushSprite(0, 0);
    
    // === 8. AUTO-SAVE TO EEPROM (every 60 seconds) ===
    static unsigned long lastSave = 0;
    if (now - lastSave > 60000) {
        EEPROM.commit();
        lastSave = now;
    }
    
    // === 9. FRAME RATE CONTROL ===
    delay(50);  // ~20 FPS
}
