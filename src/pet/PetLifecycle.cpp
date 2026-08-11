#include "PetLifecycle.h"
#include <EEPROM.h>
#include <M5Unified.h>
#include "app/GameContext.h"
#include "pet/PetCare.h"


unsigned long clockSetTime = 0;
unsigned long hatchTime = 0;
unsigned long attentionStartTime = 0;
unsigned long nextDisciplineCall = 0;
bool needsAttentionCare = false;
bool isDead = false;
unsigned long hungerCriticalTime = 0;
unsigned long happinessCriticalTime = 0;
unsigned long energyCriticalTime = 0;
unsigned long lastPoopTime = 0;
const unsigned long BABY_TO_CHILD_TIME = 3900000;
const int MAX_AGE = 100;
unsigned long lastHungerUpdate = 0;
unsigned long lastHappinessUpdate = 0;
unsigned long lastEnergyUpdate = 0;

const unsigned long HUNGER_INTERVAL = 1200000;
const unsigned long HAPPINESS_INTERVAL = 1800000;
const unsigned long ENERGY_INTERVAL = 3600000;
const unsigned long POOP_INTERVAL_BABY = 900000;
const unsigned long POOP_INTERVAL_NORMAL = 10800000;


void checkEvolution() {

    unsigned long now = millis();


    if (pet.stage == STAGE_BABY && hatchTime > 0) {
        if (now - hatchTime >= BABY_TO_CHILD_TIME) {
            pet.evolve();
            M5.Speaker.tone(1500, 200);
            delay(200);
            M5.Speaker.tone(1800, 200);
        }
    }


    if (pet.stage == STAGE_CHILD && pet.age >= 3) {
        pet.evolve();
        M5.Speaker.tone(1500, 200);
        delay(200);
        M5.Speaker.tone(1800, 200);
    }


    if (pet.stage == STAGE_TEEN && pet.age >= 6) {
        pet.evolve();
        M5.Speaker.tone(1500, 200);
        delay(200);
        M5.Speaker.tone(1800, 200);
    }
}


bool checkDeathConditions() {
    unsigned long now = millis();


    if (pet.age >= MAX_AGE) {
        return true;
    }



    if (pet.hunger == 0) {
        if (hungerCriticalTime == 0) {
            hungerCriticalTime = now;
            needsAttentionCare = true;
            attentionStartTime = now;
        } else if (now - hungerCriticalTime > DEATH_THRESHOLD) {
            return true;
        }


        if (now - attentionStartTime > ATTENTION_TIMEOUT && needsAttentionCare) {
            pet.incrementCareMistake();


            if (pet.stage == STAGE_ADULT || pet.stage == STAGE_TEEN) {
                pet.startSulking();
            }

            needsAttentionCare = false;
        }
    } else {
        hungerCriticalTime = 0;
        if (pet.hunger > 0 && needsAttentionCare) {
            needsAttentionCare = false;
        }
    }


    if (pet.happiness == 0) {
        if (happinessCriticalTime == 0) {
            happinessCriticalTime = now;
            needsAttentionCare = true;
            attentionStartTime = now;
        } else if (now - happinessCriticalTime > DEATH_THRESHOLD) {
            return true;
        }


        if (now - attentionStartTime > ATTENTION_TIMEOUT && needsAttentionCare) {
            pet.incrementCareMistake();


            if (pet.stage == STAGE_ADULT || pet.stage == STAGE_TEEN) {
                pet.startSulking();
            }

            needsAttentionCare = false;
        }
    } else {
        happinessCriticalTime = 0;
        if (pet.happiness > 0 && needsAttentionCare) {
            needsAttentionCare = false;
        }
    }


    if (pet.isSick || pet.hasToothache) {
        if (energyCriticalTime == 0) {
            energyCriticalTime = now;
        } else if (now - energyCriticalTime > SICKNESS_DEATH_THRESHOLD) {
            return true;
        }
    } else {
        energyCriticalTime = 0;
    }

    return false;
}


void resetPet() {

    pet.sleeping = false;
    pet.sleep = 8;
    pet.happiness = 4;
    pet.hunger = 4;
    pet.energy = 12;
    pet.age = 0;
    pet.mood = 0;
    pet.weight = 10;
    pet.discipline = 0;
    pet.careMistakes = 0;
    pet.poopCount = 0;
    pet.isSick = false;
    pet.stage = STAGE_BABY;
    pet.lightsOn = true;


    pet.hasToothache = false;
    pet.isSulking = false;
    pet.snackStreak = 0;
    pet.medicineNeeded = 0;
    pet.sulkStartTime = 0;
    lastSnackTime = 0;
    lastMedicineTime = 0;


    hungerCriticalTime = 0;
    happinessCriticalTime = 0;
    energyCriticalTime = 0;
    isDead = false;
    needsAttentionCare = false;


    clockSetTime = millis();
    hatchTime = millis();
    lastPoopTime = millis();
    attentionStartTime = 0;
    nextDisciplineCall = 0;


    EEPROM.write(0, 0);
    EEPROM.write(1, 8);
    EEPROM.write(2, 4);
    EEPROM.write(3, 4);
    EEPROM.write(4, 12);
    EEPROM.write(5, 0);
    EEPROM.write(6, 0);
    EEPROM.write(7, 10);
    EEPROM.write(8, 0);
    EEPROM.write(9, 0);
    EEPROM.write(10, 0);
    EEPROM.write(11, 0);
    EEPROM.write(12, 1);
    EEPROM.write(13, 1);
    EEPROM.commit();


    M5.Speaker.tone(800, 100);
    delay(100);
    M5.Speaker.tone(1000, 100);
    delay(100);
    M5.Speaker.tone(1200, 200);
}

void schedulePoop() {
    unsigned long now = millis();

    if (pet.stage == STAGE_BABY) {
        if (hatchTime > 0) {
            unsigned long timeSinceHatch = now - hatchTime;
            if (timeSinceHatch >= POOP_INTERVAL_BABY && timeSinceHatch < POOP_INTERVAL_BABY + 60000) {
                if (lastPoopTime == 0 || now - lastPoopTime > 60000) {
                    pet.addPoop();
                    lastPoopTime = now;
                }
            }
            else if (timeSinceHatch >= 2400000 && timeSinceHatch < 2700000) {
                if (now - lastPoopTime > 60000) {
                    pet.addPoop();
                    lastPoopTime = now;
                }
            }
        }
    } else if (pet.stage >= STAGE_CHILD) {
        if (lastPoopTime == 0) lastPoopTime = now;
        if (now - lastPoopTime >= POOP_INTERVAL_NORMAL) {
            pet.addPoop();
            lastPoopTime = now;
        }
    }
}
