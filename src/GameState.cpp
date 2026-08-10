#include "GameState.h"

// State machine to track the game states
int minigameSelection = 0;
int playerX = 60;
int objectX = 0;
int objectY = 0;
int gameScore = 0;
bool gameActive = false;

// Sensor state of the M5Stack ESP-32 S3 board
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
bool isFlat = false;
bool isTiltedUp = false;
int shakeCount = 0;
int micLevel = 0;

// Special features state for minigames
int roomMess = 0;
int fortuneIndex = 0;
int heartLevel = 0;
int buttonPressCount = 0;
unsigned long loveMeterStartTime = 0;
unsigned long lastAttentionTime = 0;
int currentSpecialDay = -1;

// Death tracking
unsigned long hungerCriticalTime = 0;
unsigned long happinessCriticalTime = 0;
unsigned long energyCriticalTime = 0;
bool isDead = false;

// Lifecycle tracking
unsigned long clockSetTime = 0;
unsigned long hatchTime = 0;
unsigned long lastPoopTime = 0;
unsigned long attentionStartTime = 0;
unsigned long nextDisciplineCall = 0;
bool needsAttentionCare = false;
int roundsWon = 0;
int roundsPlayed = 0;
int playerGuess = 0;
int computerChoice = 0;

// Sickness tracking
unsigned long lastSnackTime = 0;
unsigned long lastMedicineTime = 0;
int consecutiveSnacks = 0;

// Constants
const unsigned long ATTENTION_COOLDOWN = 10000;  // 10 seconds
const unsigned long DEATH_THRESHOLD = 7200000;    // 2 hours in critical condition = death
const unsigned long SICKNESS_DEATH_THRESHOLD = 5400000; // 90 minutes sick without medicine = death
const int MAX_AGE = 100;                          // Maximum age before death from old age
const unsigned long BABY_TO_CHILD_TIME = 3900000; // 65 minutes
const unsigned long ATTENTION_TIMEOUT = 900000;  // 15 minutes before care mistake
const unsigned long POOP_INTERVAL_BABY = 900000; // 15 minutes for baby first poop
const unsigned long POOP_INTERVAL_NORMAL = 10800000; // 3 hours

// Fortune cookie custom messages
// Board reads list of character until we reach \0 (end of line)
const char* fortuneMessages[] = {
    "Believe you can and you're halfway there. \n - Theodore Roosevelt",
    "Dont count the days, make the days count. \n - Muhammad Ali",
    "To begin, begin. \n - William Wordsworth",
    "Do the best you can until you know better. Then when you know better, do better. \n - Maya Angelou",
    "A year from now you may wish you had started today. \n - Karen Lamb"
};
const int FORTUNE_MESSAGE_COUNT = 8;

// Special days - defined here
const int SPECIAL_DAY_COUNT = 3;

// ===== HELPER FUNCTIONS =====

// Check if today is a special day
int checkSpecialDay(int month, int day) {
    for (int i = 0; i < SPECIAL_DAY_COUNT; i++) {
        if (specialDays[i].month == month && specialDays[i].day == day) {
            return i;
        }
    }
    return -1;
}

// Draw progress bar helper
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    canvas.drawRect(x, y, width, height, TFT_WHITE);
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        canvas.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}

// Draw the character with expressions based on happiness
void drawCharacter(int x, int y, bool heartEyes) {
    // Subtle floating/breathing animation for more life
    int breathOffset = 0;
    int floatOffset = 0;
    switch (animationFrame % 8) {
        case 0: case 7: breathOffset = 0; floatOffset = 0; break;
        case 1: case 6: breathOffset = 1; floatOffset = -1; break;
        case 2: case 5: breathOffset = 2; floatOffset = -2; break;
        case 3: case 4: breathOffset = 1; floatOffset = -1; break;
    }
    
    y += floatOffset;  // Add floating effect
    int radius = 20 + breathOffset;
    
    // Body (circle) - color based on happiness
    uint16_t bodyColor = TFT_WHITE;
    if (boyfriend.isSulking) {
        bodyColor = TFT_LIGHTGREY;  // Darker when sulking
    } else if (boyfriend.happiness == 4) {
        // Very happy - slight pink tint
        bodyColor = 0xFFF8;  // Light pink
    }
    canvas.fillCircle(x, y, radius, bodyColor);
    canvas.drawCircle(x, y, radius, TFT_DARKGREY);  // Outline
    
    // Eyes (expression based on 0-4 happiness scale)
    int eyeY = y - 5;
    if (heartEyes) {
        // Heart eyes! (when fed snacks or playing)
        canvas.setTextColor(TFT_RED, bodyColor);
        canvas.setTextSize(1);
        canvas.setCursor(x - 10, eyeY - 3);
        canvas.print("♥");
        canvas.setCursor(x + 4, eyeY - 3);
        canvas.print("♥");
    } else if (boyfriend.isSulking) {
        // Sulking - looking away
        canvas.fillCircle(x - 8, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 4, eyeY, 2, TFT_BLACK);
    } else if (boyfriend.isSick || boyfriend.hasToothache) {
        // Sick eyes - X_X
        canvas.drawLine(x - 8, eyeY - 2, x - 4, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x - 4, eyeY - 2, x - 8, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY - 2, x + 8, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x + 8, eyeY - 2, x + 4, eyeY + 2, TFT_BLACK);
    } else if (boyfriend.happiness >= 4) {
        // Very happy - big sparkle eyes with stars
        canvas.fillCircle(x - 6, eyeY, 4, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 4, TFT_BLACK);
        canvas.fillCircle(x - 6, eyeY - 1, 1, TFT_WHITE);  // Sparkle
        canvas.fillCircle(x + 6, eyeY - 1, 1, TFT_WHITE);
    } else if (boyfriend.happiness == 3) {
        // Happy - normal big eyes
        canvas.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    } else if (boyfriend.happiness == 2) {
        // OK - smaller eyes
        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
    } else if (boyfriend.happiness == 1) {
        // Sad - droopy eyes
        canvas.drawLine(x - 8, eyeY - 1, x - 4, eyeY + 1, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY + 1, x + 8, eyeY - 1, TFT_BLACK);
        canvas.fillCircle(x - 6, eyeY + 1, 1, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY + 1, 1, TFT_BLACK);
    } else {
        // Very sad (0) - tearful
        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
        canvas.drawLine(x - 6, eyeY + 3, x - 6, eyeY + 8, TFT_CYAN);  // Tear
        canvas.drawLine(x + 6, eyeY + 3, x + 6, eyeY + 8, TFT_CYAN);
    }
    
    // Mouth - based on happiness
    int mouthY = y + 6;
    if (boyfriend.isSick || boyfriend.hasToothache) {
        // Sick mouth - wavy line
        canvas.drawLine(x - 6, mouthY, x - 2, mouthY + 2, TFT_BLACK);
        canvas.drawLine(x - 2, mouthY + 2, x + 2, mouthY, TFT_BLACK);
        canvas.drawLine(x + 2, mouthY, x + 6, mouthY + 2, TFT_BLACK);
    } else if (boyfriend.isSulking) {
        // Pouting
        canvas.drawLine(x - 5, mouthY + 2, x + 5, mouthY + 2, TFT_BLACK);
    } else if (boyfriend.happiness >= 3) {
        // Big smile
        canvas.drawArc(x, mouthY - 3, 8, 6, 180, 360, TFT_BLACK);
        // Open mouth for extra happiness
        if (boyfriend.happiness == 4) {
            canvas.drawArc(x, mouthY - 1, 5, 4, 180, 360, TFT_BLACK);
        }
    } else if (boyfriend.happiness == 2) {
        // Small smile
        canvas.drawArc(x, mouthY - 1, 5, 3, 180, 360, TFT_BLACK);
    } else if (boyfriend.happiness == 1) {
        // Neutral/flat
        canvas.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);
    } else {
        // Frown (0 happiness)
        canvas.drawArc(x, mouthY + 5, 6, 4, 0, 180, TFT_BLACK);
    }
    
    // Add blush when very happy
    if (boyfriend.happiness >= 3 && !boyfriend.isSick) {
        canvas.fillCircle(x - 15, y + 3, 3, TFT_PINK);
        canvas.fillCircle(x + 15, y + 3, 3, TFT_PINK);
    }
}

// Draw stats bar with battery, hunger, happiness, energy
void drawStatsBar() {
    canvas.setTextSize(1);
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Batt:%d%%", M5.Power.getBatteryLevel());
    
    // Age display
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(75, 5);
    canvas.printf("Age:%d", boyfriend.age);
    
    // Hunger hearts (4 hearts)
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(5, 20);
    canvas.print("H:");
    drawHearts(20, 20, boyfriend.hunger, 4, TFT_ORANGE);
    
    // Happiness hearts (4 hearts)
    canvas.setCursor(5, 32);
    canvas.print("♡:");
    drawHearts(20, 32, boyfriend.happiness, 4, TFT_YELLOW);
    
    // Weight & Discipline
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(80, 20);
    canvas.printf("Wt:%d", boyfriend.weight);
    
    canvas.setTextColor(TFT_PURPLE, TFT_BLACK);
    canvas.setCursor(80, 32);
    canvas.printf("Ds:%d%%", boyfriend.discipline);
    
    // Show status messages or special day banner
    if (currentSpecialDay >= 0) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print(specialDays[currentSpecialDay].name);
    } else if (boyfriend.hunger == 0) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("HUNGRY!");
    } else if (boyfriend.happiness == 0) {
        canvas.setTextColor(TFT_BLUE, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("SAD :(");
    } else if (boyfriend.isSick) {
        canvas.setTextColor(TFT_GREEN, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("SICK!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Menu B:Feed");
}

// Draw hearts for hunger/happiness display
void drawHearts(int x, int y, int filled, int total, uint16_t color) {
    for (int i = 0; i < total; i++) {
        if (i < filled) {
            // Filled heart
            canvas.setTextColor(color, TFT_BLACK);
        } else {
            // Empty heart
            canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        }
        canvas.setCursor(x + (i * 12), y);
        canvas.print("♥");
    }
}

// Draw egg sprite
void drawEgg(int x, int y) {
    // Egg body (oval)
    canvas.fillEllipse(x, y, 18, 22, TFT_WHITE);
    canvas.drawEllipse(x, y, 18, 22, TFT_DARKGREY);
    
    // Spots on egg
    canvas.fillCircle(x - 5, y - 8, 3, TFT_LIGHTGREY);
    canvas.fillCircle(x + 7, y - 3, 4, TFT_LIGHTGREY);
    canvas.fillCircle(x - 3, y + 8, 2, TFT_LIGHTGREY);
}

// Draw poop sprite
void drawPoop(int x, int y) {
    // Brown poop pile (multiple circles)
    canvas.fillCircle(x, y + 3, 5, TFT_BROWN);
    canvas.fillCircle(x - 4, y + 2, 4, TFT_BROWN);
    canvas.fillCircle(x + 4, y + 2, 4, TFT_BROWN);
    canvas.fillCircle(x, y - 2, 4, TFT_BROWN);
    
    // Dark outline
    canvas.drawCircle(x, y + 3, 5, TFT_DARKGREY);
}

// Draw skull icon for death state
void drawSkull(int x, int y) {
    // Skull head (circle)
    canvas.fillCircle(x, y, 22, TFT_WHITE);
    canvas.drawCircle(x, y, 22, TFT_DARKGREY);
    
    // Eye sockets (hollow black circles)
    canvas.fillCircle(x - 8, y - 5, 5, TFT_BLACK);
    canvas.fillCircle(x + 8, y - 5, 5, TFT_BLACK);
    
    // Nose (small triangle)
    canvas.fillTriangle(x - 2, y + 2, x + 2, y + 2, x, y + 6, TFT_BLACK);
    
    // Jaw (bottom part)
    canvas.fillRect(x - 12, y + 8, 24, 8, TFT_WHITE);
    canvas.drawRect(x - 12, y + 8, 24, 8, TFT_DARKGREY);
    
    // Teeth (vertical lines)
    for (int i = 0; i < 4; i++) {
        canvas.drawLine(x - 8 + (i * 5), y + 8, x - 8 + (i * 5), y + 15, TFT_DARKGREY);
    }
}

// Draw sickness indicators next to character
void drawSicknessIndicators(int x, int y) {
    // Draw skull for standard sickness
    if (boyfriend.isSick) {
        // Small skull icon
        canvas.fillCircle(x, y, 8, TFT_WHITE);
        canvas.drawCircle(x, y, 8, TFT_DARKGREY);
        canvas.fillCircle(x - 3, y - 2, 2, TFT_BLACK);  // Left eye
        canvas.fillCircle(x + 3, y - 2, 2, TFT_BLACK);  // Right eye
        canvas.drawLine(x - 3, y + 3, x + 3, y + 3, TFT_DARKGREY);  // Mouth
        
        // Show medicine needed count
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(1);
        canvas.setCursor(x - 8, y + 12);
        canvas.printf("x%d", boyfriend.medicineNeeded);
    }
    
    // Draw broken tooth for toothache
    if (boyfriend.hasToothache) {
        // Tooth shape
        canvas.fillRoundRect(x - 4, y - 6, 8, 10, 2, TFT_WHITE);
        canvas.drawRoundRect(x - 4, y - 6, 8, 10, 2, TFT_DARKGREY);
        
        // Crack/break in tooth (red zigzag)
        canvas.drawLine(x - 2, y - 2, x + 2, y, TFT_RED);
        canvas.drawLine(x + 2, y, x - 2, y + 2, TFT_RED);
        
        // Root
        canvas.drawLine(x, y + 4, x, y + 8, TFT_DARKGREY);
    }
    
    // Draw sulking lines
    if (boyfriend.isSulking) {
        // Three vertical lines (one longer)
        canvas.drawLine(x - 6, y - 8, x - 6, y - 2, TFT_BLUE);
        canvas.drawLine(x, y - 10, x, y - 2, TFT_DARKGREY);  // Longer middle line
        canvas.drawLine(x + 6, y - 8, x + 6, y - 2, TFT_BLUE);
    }
}

// Check if pet should die from neglect or old age
bool checkDeathConditions() {
    unsigned long now = millis();
    
    // Check for old age death
    if (boyfriend.age >= MAX_AGE) {
        return true;
    }
    
    // Track how long pet has been in critical condition (0 hearts)
    // Critical hunger (empty - 0 hearts)
    if (boyfriend.hunger == 0) {
        if (hungerCriticalTime == 0) {
            hungerCriticalTime = now;
            needsAttentionCare = true;
            attentionStartTime = now;
        } else if (now - hungerCriticalTime > DEATH_THRESHOLD) {
            return true;  // Starved to death
        }
        
        // Check 15-minute rule for care mistake
        if (now - attentionStartTime > ATTENTION_TIMEOUT && needsAttentionCare) {
            boyfriend.incrementCareMistake();
            
            // Adults and teens sulk instead of getting sick from care mistakes
            if (boyfriend.stage == STAGE_ADULT || boyfriend.stage == STAGE_TEEN) {
                boyfriend.startSulking();
            }
            
            needsAttentionCare = false;
        }
    } else {
        hungerCriticalTime = 0;  // Reset if recovered
        if (boyfriend.hunger > 0 && needsAttentionCare) {
            needsAttentionCare = false;  // Fed in time!
        }
    }
    
    // Critical happiness (empty - 0 hearts)
    if (boyfriend.happiness == 0) {
        if (happinessCriticalTime == 0) {
            happinessCriticalTime = now;
            needsAttentionCare = true;
            attentionStartTime = now;
        } else if (now - happinessCriticalTime > DEATH_THRESHOLD) {
            return true;  // Died of sadness
        }
        
        // Check 15-minute rule for care mistake
        if (now - attentionStartTime > ATTENTION_TIMEOUT && needsAttentionCare) {
            boyfriend.incrementCareMistake();
            
            // Adults and teens sulk instead of getting sick from care mistakes
            if (boyfriend.stage == STAGE_ADULT || boyfriend.stage == STAGE_TEEN) {
                boyfriend.startSulking();
            }
            
            needsAttentionCare = false;
        }
    } else {
        happinessCriticalTime = 0;  // Reset if recovered
        if (boyfriend.happiness > 0 && needsAttentionCare) {
            needsAttentionCare = false;  // Made happy in time!
        }
    }
    
    // Sick from too much poop or toothache
    if (boyfriend.isSick || boyfriend.hasToothache) {
        if (energyCriticalTime == 0) {
            energyCriticalTime = now;
        } else if (now - energyCriticalTime > SICKNESS_DEATH_THRESHOLD) {
            return true;  // Died from untreated sickness (90 minutes)
        }
    } else {
        energyCriticalTime = 0;
    }
    
    return false;
}

// Update sickness system - check sulking, toothache, snack streaks
void updateSicknessSystem() {
    unsigned long now = millis();
    
    // Check sulking status (auto-recovery or running away)
    if (boyfriend.isSulking) {
        boyfriend.checkSulking();
    }
    
    // Reset snack streak if enough time has passed (1 hour without eating)
    if (lastSnackTime > 0 && (now - lastSnackTime > 3600000)) {  // 1 hour
        boyfriend.resetSnackStreak();
        lastSnackTime = 0;
    }
    
    // If sick or has toothache, set medicineNeeded if not already set
    if ((boyfriend.isSick || boyfriend.hasToothache) && boyfriend.medicineNeeded == 0) {
        boyfriend.medicineNeeded = (boyfriend.hasToothache ? 1 : 2);  // Toothache=1 dose, Sick=2 doses
    }
}

// Reset pet to initial state
void resetPet() {
    // Reset all stats to defaults (4-heart system)
    boyfriend.sleeping = false;
    boyfriend.sleep = 8;
    boyfriend.happiness = 4;  // Full happiness
    boyfriend.hunger = 4;     // Full hunger
    boyfriend.energy = 12;
    boyfriend.age = 0;
    boyfriend.mood = 0;
    boyfriend.weight = 10;
    boyfriend.discipline = 0;
    boyfriend.careMistakes = 0;
    boyfriend.poopCount = 0;
    boyfriend.isSick = false;
    boyfriend.stage = STAGE_BABY;
    boyfriend.lightsOn = true;
    
    // Reset sickness system
    boyfriend.hasToothache = false;
    boyfriend.isSulking = false;
    boyfriend.snackStreak = 0;
    boyfriend.medicineNeeded = 0;
    boyfriend.sulkStartTime = 0;
    lastSnackTime = 0;
    lastMedicineTime = 0;
    
    // Clear death tracking
    hungerCriticalTime = 0;
    happinessCriticalTime = 0;
    energyCriticalTime = 0;
    isDead = false;
    needsAttentionCare = false;
    
    // Reset lifecycle timers
    clockSetTime = millis();
    hatchTime = millis();  // Set for evolution tracking
    lastPoopTime = millis();
    attentionStartTime = 0;
    nextDisciplineCall = 0;
    
    // Reset EEPROM
    EEPROM.write(0, 0);   // sleeping = false
    EEPROM.write(1, 8);   // sleep = 8
    EEPROM.write(2, 4);   // happiness = 4
    EEPROM.write(3, 4);   // hunger = 4
    EEPROM.write(4, 12);  // energy = 12
    EEPROM.write(5, 0);   // age = 0
    EEPROM.write(6, 0);   // mood = neutral
    EEPROM.write(7, 10);  // weight = 10
    EEPROM.write(8, 0);   // discipline = 0
    EEPROM.write(9, 0);   // careMistakes = 0
    EEPROM.write(10, 0);  // poopCount = 0
    EEPROM.write(11, 0);  // isSick = false
    EEPROM.write(12, 1);  // stage = BABY
    EEPROM.write(13, 1);  // lightsOn = true
    EEPROM.commit();
    
    // Play reset sound
    M5.Speaker.tone(800, 100);
    delay(100);
    M5.Speaker.tone(1000, 100);
    delay(100);
    M5.Speaker.tone(1200, 200);
}

// Check if pet should evolve based on time and age
void checkEvolution() {
    unsigned long now = millis();
    
    // Baby → Child: 65 minutes after hatching
    if (boyfriend.stage == STAGE_BABY && hatchTime > 0) {
        if (now - hatchTime >= BABY_TO_CHILD_TIME) {
            boyfriend.evolve();
            M5.Speaker.tone(1500, 200);
            delay(200);
            M5.Speaker.tone(1800, 200);
        }
    }
    
    // Child → Teen: At age 3 (72 hours = 3 days)
    if (boyfriend.stage == STAGE_CHILD && boyfriend.age >= 3) {
        boyfriend.evolve();
        M5.Speaker.tone(1500, 200);
        delay(200);
        M5.Speaker.tone(1800, 200);
    }
    
    // Teen → Adult: At age 6 (144 hours = 6 days)
    if (boyfriend.stage == STAGE_TEEN && boyfriend.age >= 6) {
        boyfriend.evolve();
        M5.Speaker.tone(1500, 200);
        delay(200);
        M5.Speaker.tone(1800, 200);
    }
}

// Schedule poop based on stage
void schedulePoop() {
    unsigned long now = millis();
    
    if (boyfriend.stage == STAGE_BABY) {
        // Baby poops at 15 and 40-45 minutes
        if (hatchTime > 0) {
            unsigned long timeSinceHatch = now - hatchTime;
            // First poop at 15 min
            if (timeSinceHatch >= POOP_INTERVAL_BABY && timeSinceHatch < POOP_INTERVAL_BABY + 60000) {
                if (lastPoopTime == 0 || now - lastPoopTime > 60000) {
                    boyfriend.addPoop();
                    lastPoopTime = now;
                }
            }
            // Second poop at 40-45 min
            else if (timeSinceHatch >= 2400000 && timeSinceHatch < 2700000) {
                if (now - lastPoopTime > 60000) {
                    boyfriend.addPoop();
                    lastPoopTime = now;
                }
            }
        }
    } else if (boyfriend.stage >= STAGE_CHILD) {
        // Child/Teen/Adult: Poop every 3 hours
        if (lastPoopTime == 0) lastPoopTime = now;
        if (now - lastPoopTime >= POOP_INTERVAL_NORMAL) {
            boyfriend.addPoop();
            lastPoopTime = now;
        }
    }
}
