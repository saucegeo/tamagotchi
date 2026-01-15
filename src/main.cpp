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

// Game mode state machine
int gameMode = 0;  // 0=idle, 1=need attention, 2=feeding, 3=playing, etc.
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
    
    // 2. CRITICAL: Hold power on (required for M5StickC Plus 2!)
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    
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

// ===== MAIN LOOP =====
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
    
    // === 4. BUTTON INPUT (like TiMiNoo's checkButton) ===
    if (StickCP2.BtnA.wasPressed()) {
        boyfriend.updateHappiness(2);
        StickCP2.Speaker.tone(1000, 50);
    }
    
    if (StickCP2.BtnB.wasPressed()) {
        boyfriend.updateHunger(-2);
        if (boyfriend.hunger < 0) boyfriend.hunger = 0;
        StickCP2.Speaker.tone(1500, 50);
    }
    
    // === 5. DRAW EVERYTHING (like TiMiNoo's u8g.firstPage/nextPage loop) ===
    canvas.fillScreen(TFT_BLACK);
    
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
    
    // Character (center)
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    // Notifications (like TiMiNoo's speech bubbles)
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
    
    // === 6. FRAME RATE CONTROL ===
    delay(50);  // ~20 FPS
}
