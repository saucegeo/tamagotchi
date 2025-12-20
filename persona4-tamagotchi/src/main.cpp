#include <M5StickCPlus2.h> // REQUIRED for the screen
#include <EEPROM.h>
#include "Boyfriend.h"     // Your Boyfriend pet class
#include "Sprites.h"       // Your new image file
#include "DeviceManager.h" // Hardware abstraction and game state management
#include "MechanicsManager.h" // Mini-games and hardware interactions

Boyfriend boyfriend;
// Create the manager and pass a pointer to your pet.
// WHY: Manager updates happiness, checks sensors, etc., without global coupling.
DeviceManager manager(&boyfriend);
MechanicsManager mechanics(&boyfriend);  // NEW: Mechanics manager for mini-games 

void setup() {
    // 1. Initialize M5StickC Plus 2
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    
    // 2. Rotate Screen (1 = Landscape)
    StickCP2.Display.setRotation(1);
    
    // 3. Initialize EEPROM (Must specify size for ESP32)
    EEPROM.begin(512); 
    
    // 4. Set RTC Date and Time
    // IMPORTANT: Set this to current date/time on first upload!
    // Format: setDateTime({{year, month, day}, {hour, minute, second}})
    // Example: December 20, 2025, 10:30:00 AM
    // StickCP2.Rtc.setDateTime({{2025, 12, 20}, {10, 30, 0}});
    
    // Print current RTC date/time for debugging
    auto rtcDate = StickCP2.Rtc.getDate();
    auto rtcTime = StickCP2.Rtc.getTime();
    Serial.printf("Current Date: %04d-%02d-%02d\n", 
                  rtcDate.year, rtcDate.month, rtcDate.date);
    Serial.printf("Current Time: %02d:%02d:%02d\n", 
                  rtcTime.hours, rtcTime.minutes, rtcTime.seconds);
    
    // 5. Load Pet
    boyfriend.loadFromEEPROM();
    
    // 6. Initialize DeviceManager (hardware abstraction)
    manager.begin();
    manager.setState(STATE_HOME);
    
    // 7. Initialize MechanicsManager (mini-games and mechanics)
    mechanics.begin();
    
    // 8. Set special dates (Birthday: March 15, Anniversary: July 20)
    // CUSTOMIZE: Change these to your actual dates!
    mechanics.setSpecialDates(3, 15, 7, 20);  // (birthMonth, birthDay, anniMonth, anniDay)
    CHECK SPECIAL EVENTS (Date-Based) ---
    static EventType lastEvent = EVENT_NONE;
    static unsigned long lastEventCheck = 0;
    
    // Check for special events once per minute (avoid constant RTC reads)
    if (millis() - lastEventCheck > 60000) {
        EventType currentEvent = mechanics.checkSpecialEvents();
        
        if (currentEvent != EVENT_NONE && currentEvent != lastEvent) {
            // New special event detected!
            StickCP2.Display.fillScreen(BLACK);
            StickCP2.Display.setTextColor(MAGENTA, BLACK);
            StickCP2.Display.setCursor(10, 30);
            
            switch (currentEvent) {
                case EVENT_BIRTHDAY:
                    StickCP2.Display.printf("HAPPY BIRTHDAY!");
                    mechanics.playTune(MechanicsManager::SONG_BIRTHDAY);
                    boyfriend.updateHappiness(10);
                    break;
                    
                case EVENT_ANNIVERSARY:
                    StickCP2.Display.printf("ANNIVERSARY!");
                    mechanics.playTune(MechanicsManager::SONG_LOVE);
                    boyfriend.updateHappiness(15);
                    break;
                    
                case EVENT_VALENTINES:
                    StickCP2.Display.printf("VALENTINE'S DAY!");
                    mechanics.playTune(MechanicsManager::SONG_LOVE);
                    boyfriend.updateHappiness(12);
                    break;
                    
                default:
                    break;
            }
            
            delay(3000);
            StickCP2.Display.fillScreen(BLACK);
            lastEvent = currentEvent;
        }
        
        lastEventCheck = millis();
    }
    
    // --- 
    // Debug info to Serial Monitor
    Serial.println("Boyfriend Bot Started!");
}

void loop() {
    // --- LOGIC ---
    StickCP2.update(); // Update button states
    manager.update();  // Central place for future periodic hardware tasks
    mechanics.update(); // NEW: Update continuous mechanics (pedometer, sleepy head)
    
    static unsigned long lastTimer = 0;
    if (millis() - lastTimer > 5000) {
        boyfriend.updateHunger(1);
        boyfriend.saveToEEPROM();
        lastTimer = millis();
    }
    
    // --- BUTTON CONTROLS ---
    // Button A: Play Love Meter mini-game
    if (StickCP2.BtnA.wasPressed()) {
        if (mechanics.playLoveMeter(5000)) {
            // User won! Boyfriend got +10 happiness automatically
            StickCP2.Display.fillScreen(BLACK);
        }
    }
    
    // Button B (PWR): Mechanics menu
    if (StickCP2.BtnPWR.wasPressed()) {
        // Example: Play anime songs!
        // Cycle through different songs on each press
        static int songIndex = 0;
        
        StickCP2.Display.fillScreen(BLACK);
        StickCP2.Display.setTextColor(CYAN, BLACK);
        StickCP2.Display.setCursor(10, 30);
        
        switch (songIndex) {
            case 0:
                StickCP2.Display.printf("Playing:\nEvangelion!");
                mechanics.playTune(MechanicsManager::SONG_EVA);
                break;
            case 1:
                StickCP2.Display.printf("Playing:\nPokemon!");
                mechanics.playTune(MechanicsManager::SONG_POKEMON);
                break;
            case 2:
                StickCP2.Display.printf("Playing:\nTotoro!");
                mechanics.playTune(MechanicsManager::SONG_TOTORO);
                break;
        }
        
        songIndex = (songIndex + 1) % 3;
        StickCP2.Display.fillScreen(BLACK);
    }
    
    // Long press PWR: Get Fortune Cookie
    static unsigned long pwrPressStart = 0;
    if (StickCP2.BtnPWR.wasPressed()) {
        pwrPressStart = millis();
    }
    if (StickCP2.BtnPWR.wasReleased()) {
        if (millis() - pwrPressStart > 1000) {  // 1 second long-press
            // Check Sleepy Head mode
            if (mechanics.checkSleepyHead(3.0f)) {
                boyfriend.updateSleeping();
                StickCP2.Display.fillScreen(BLACK);
                StickCP2.Display.setCursor(20, 50);
                StickCP2.Display.printf("Zzz... Sleeping");
                delay(2000);
            }
            
            // Get Fortune Cookie
            const char* affection = mechanics.getRandomAffection();
            StickCP2.Display.fillScreen(BLACK);
            StickCP2.Display.setCursor(5, 30);
            StickCP2.Display.setTextColor(MAGENTA, BLACK);
            StickCP2.Display.printf("%s", affection);
            StickCP2.Speaker.tone(1500, 100);
            delay(2000);
            StickCP2.Display.fillScreen(BLACK);
        }
    }
    
    // Continuous: Shake to Clean detection
    if (mechanics.checkShakeToClean()) {
        boyfriend.updateHappiness(2);
        StickCP2.Display.fillScreen(BLACK);
        StickCP2.Display.setCursor(20, 50);
        StickCP2.Display.setTextColor(CYAN, BLACK);
        StickCP2.Display.printf("Cleaned! +2 ♡");
        delay(1000);
        StickCP2.Display.fillScreen(BLACK);
    }

    // --- DISPLAY ---
    // If hunger is high, show the "Eating" sprite, otherwise "Idle"
    if (boyfriend.hunger > 10) {
         // pushImage(x, y, width, height, data_array)
         StickCP2.Display.pushImage(40, 30, 64, 64, teddie_eating);
    } else {
         StickCP2.Display.pushImage(40, 30, 64, 64, teddie_idle);
    }
    
    // Draw Stats Text
    StickCP2.Display.setTextColor(WHITE, BLACK);
    StickCP2.Display.setCursor(0, 0);
    StickCP2.Display.printf("Hunger: %d  ", boyfriend.hunger);
    
    // Show battery percent with hangover check
    bool hungover = false;
    int battPct = mechanics.checkBatteryHangover(&hungover);
    StickCP2.Display.setCursor(0, 10);
    if (hungover) {
        StickCP2.Display.setTextColor(RED, BLACK);
        StickCP2.Display.printf("Batt: %d%% LOW!", battPct);
    } else {
        StickCP2.Display.setTextColor(WHITE, BLACK);
        StickCP2.Display.printf("Batt: %d%%   ", battPct);
    }
    
    // Show pedometer steps
    StickCP2.Display.setTextColor(WHITE, BLACK);
    StickCP2.Display.setCursor(0, 110);
    StickCP2.Display.printf("Steps: %d  ", mechanics.getPedometer());
    
    delay(100);
}