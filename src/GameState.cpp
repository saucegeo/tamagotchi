#include "GameState.h"

// ===== GLOBAL VARIABLE DEFINITIONS =====
// These are declared as "extern" in GameState.h, now we allocate memory for them

// Core game objects (canvas is defined in main.cpp, boyfriend in main.cpp)
// Note: canvas and boyfriend are defined in main.cpp to keep hardware init there

// State machine
int minigameSelection = 0;
int playerX = 60;
int objectX = 0;
int objectY = 0;
int gameScore = 0;
bool gameActive = false;

// Sensor state
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
bool isFlat = false;
bool isTiltedUp = false;
int shakeCount = 0;
int micLevel = 0;

// Special features state
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

// Constants
const unsigned long ATTENTION_COOLDOWN = 10000;  // 10 seconds
const unsigned long DEATH_THRESHOLD = 120000;    // 2 minutes in critical condition = death
const int MAX_AGE = 99;                          // Maximum age before death from old age
const unsigned long BABY_TO_CHILD_TIME = 3900000; // 65 minutes
const unsigned long ATTENTION_TIMEOUT = 900000;  // 15 minutes
const unsigned long POOP_INTERVAL_BABY = 900000; // 15 minutes for baby first poop
const unsigned long POOP_INTERVAL_NORMAL = 10800000; // 3 hours

// Fortune cookie messages
const char* fortuneMessages[] = {
    "Love grows when you listen",
    "Small gestures mean everything",
    "Your patience will be rewarded",
    "Adventure awaits together",
    "Laughter is your best gift",
    "Trust deepens with time",
    "Kindness returns tenfold",
    "Your presence is enough"
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
        // Heart eyes!
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
            needsAttentionCare = false;
        }
    } else {
        happinessCriticalTime = 0;  // Reset if recovered
        if (boyfriend.happiness > 0 && needsAttentionCare) {
            needsAttentionCare = false;  // Made happy in time!
        }
    }
    
    // Sick from too much poop
    if (boyfriend.isSick) {
        if (energyCriticalTime == 0) {
            energyCriticalTime = now;
        } else if (now - energyCriticalTime > DEATH_THRESHOLD) {
            return true;  // Died from sickness
        }
    } else {
        energyCriticalTime = 0;
    }
    
    return false;
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
