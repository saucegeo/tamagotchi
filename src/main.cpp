#include "M5StickCPlus2.h"
#include <EEPROM.h>
#include "Sprites.h"
#include "DeviceManager.h"
#include "MechanicsManager.h"

// Global Objects
Boyfriend boyfriend;
DeviceManager manager(&boyfriend);
MechanicsManager mechanics(&boyfriend);

// M5GFX Sprite for Flicker-Free Drawing
M5Canvas canvas(&StickCP2.Display);

void setup() {
    // 1. Initialize M5StickC Plus 2
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    
    // 1.5. Configure power management to prevent flickering
    // Set LCD brightness to stable level (0-255)
    StickCP2.Display.setBrightness(128);
    
    // Configure power settings
    StickCP2.Power.setChargeCurrent(450); // Set charging current to 450mA
    
    // Disable power saving features that can cause flickering
    // Keep display always powered
    StickCP2.Display.wakeup();
    StickCP2.Display.powerSaveOff();
    
    // 2. Rotate Screen (1 = Landscape)
    StickCP2.Display.setRotation(1);
    
    // 3. Initialize Sprite
    // Allocate memory for the full screen to prevent flickering
    canvas.createSprite(StickCP2.Display.width(), StickCP2.Display.height());
    canvas.setColorDepth(16); // 16-bit color for better performance
    
    // Clear display completely before first use
    StickCP2.Display.fillScreen(BLACK);
    StickCP2.Display.display();
    
    // 4. Initialize EEPROM
    EEPROM.begin(512); 
    
    // 5. Load Pet
    boyfriend.loadFromEEPROM();
    
    // 6. Initialize Managers
    manager.begin();
    manager.setState(STATE_HOME);
    mechanics.begin();
    mechanics.setSpecialDates(3, 15, 7, 20); // Customize your dates here
    
    // Initial Clear
    canvas.fillScreen(BLACK);
    canvas.pushSprite(0, 0);
    
    Serial.begin(115200);
    Serial.println("Boyfriend Bot Started with Sprite Buffer");
}

/* void loop() {
    // ====================================================================
    // 1. UPDATE INPUTS
    // ====================================================================
    StickCP2.update(); // Update button states
    
    // Simple timer for hunger (every 30 seconds)
    static unsigned long lastTimer = 0;
    if (millis() - lastTimer > 30000) {
        boyfriend.updateHunger(1);
        lastTimer = millis();
    }
    
    // ====================================================================
    // 2. HANDLE ACTIONS
    // ====================================================================
    
    // Button A: Happiness
    if (StickCP2.BtnA.wasPressed()) {
        boyfriend.updateHappiness(1);
        StickCP2.Speaker.tone(1000, 50);
    }
    
    // Button B: Feed
    if (StickCP2.BtnB.wasPressed()) {
        boyfriend.updateHunger(-1);
        if (boyfriend.hunger < 0) boyfriend.hunger = 0;
        StickCP2.Speaker.tone(1500, 50);
    }
    
    // ====================================================================
    // 3. DRAW TO SPRITE (Buffer)
    // ====================================================================
    
    // always clear the sprite buffer first
    canvas.fillScreen(BLACK);
    
    // Center coordinates
    int centerX = canvas.width() / 2;
    int centerY = canvas.height() / 2;
    
    // --- DRAW STATS ---
    canvas.setTextSize(1);
    canvas.setTextColor(GREEN, BLACK);
    canvas.setCursor(5, 5);
    int battPct = StickCP2.Power.getBatteryLevel();
    canvas.printf("Batt: %d%%", battPct);
    
    canvas.setTextColor(WHITE, BLACK);
    canvas.setCursor(5, 20);
    canvas.printf("Hunger: %d", boyfriend.hunger);
    canvas.setCursor(5, 30);
    canvas.printf("Happy:  %d", boyfriend.happiness);
    
    // --- DRAW CHARACTER ---
    // Make it bigger (radius 15 minimum) so it's clearly visible
    // "Breathing" animation for lifelike feel
    static float breath = 0;
    breath += 0.1;
    int currentRadius = 15 + sin(breath) * 2; // radius fluctuates between 13 and 17
    
    canvas.fillCircle(centerX, centerY, currentRadius, WHITE);
    
    // --- DRAW STATUS INDICATORS ---
    if (boyfriend.sleeping) {
        canvas.setTextColor(CYAN, BLACK);
        canvas.setCursor(centerX + 20, centerY - 20);
        canvas.printf("Zzz...");
    } else if (boyfriend.hunger > 10) {
        // Warning outline if hungry
        canvas.drawCircle(centerX, centerY, currentRadius + 3, RED);
        canvas.drawCircle(centerX, centerY, currentRadius + 4, RED);
    }
    
    // --- DRAW HELP ---
    canvas.setTextDatum(bottom_center);
    canvas.drawString("A: Happy  B: Feed", centerX, canvas.height() - 5);
    
    // ====================================================================
    // 4. PUSH TO SCREEN
    // ====================================================================
    canvas.pushSprite(0, 0);
    
    // Optimized delay to prevent flickering and power issues
    // 50ms = 20fps which is smooth and prevents brownout
    delay(50);
    
    // Feed the watchdog timer to prevent reset
    yield();
}*/
