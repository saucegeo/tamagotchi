#include <M5StickCPlus2.h> // REQUIRED for the screen
#include <EEPROM.h>
#include "Teddie.h"
#include "sprites.h"       // Your new image file

Teddie myTeddie; 

void setup() {
    // 1. Initialize M5StickC Plus 2
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    
    // 2. Rotate Screen (1 = Landscape)
    StickCP2.Display.setRotation(1);
    
    // 3. Initialize EEPROM (Must specify size for ESP32)
    EEPROM.begin(512); 
    
    // 4. Load Pet
    myTeddie.loadFromEEPROM();
    
    // Debug info to Serial Monitor
    Serial.println("Teddie Started!");
}

void loop() {
    // --- LOGIC ---
    StickCP2.update(); // Update button states
    
    static unsigned long lastTimer = 0;
    if (millis() - lastTimer > 5000) {
        myTeddie.updateHunger(1);
        myTeddie.saveToEEPROM();
        lastTimer = millis();
    }

    // --- DISPLAY ---
    // If hunger is high, show the "Eating" sprite, otherwise "Idle"
    if (myTeddie.hunger > 10) {
         // pushImage(x, y, width, height, data_array)
         StickCP2.Display.pushImage(40, 30, 64, 64, teddie_eating);
    } else {
         StickCP2.Display.pushImage(40, 30, 64, 64, teddie_idle);
    }
    
    // Draw Stats Text
    StickCP2.Display.setTextColor(WHITE, BLACK);
    StickCP2.Display.setCursor(0, 0);
    StickCP2.Display.printf("Hunger: %d  ", myTeddie.hunger);
    
    delay(100);
}