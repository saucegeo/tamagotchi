/*
 * Boyfriend.cpp - Implementation file for Boyfriend virtual pet class
 * Inspired by Alegotchi project
 */

#include "Boyfriend.h"

// Default constructor
Boyfriend::Boyfriend(bool _sleeping, int _sleep, int _happiness, int _hunger, 
               int _energy, int _age, int _mood) {
  sleeping = _sleeping;
  sleep = _sleep;
  happiness = _happiness;  // Now 0-4 hearts
  hunger = _hunger;        // Now 0-4 hearts
  energy = _energy;
  age = _age;
  mood = _mood;
  
  // Initialize new Tamagotchi mechanics
  weight = 10;          // Starting weight
  discipline = 0;       // Starting discipline %
  careMistakes = 0;     // No mistakes yet
  poopCount = 0;        // No poop yet
  isSick = false;       // Healthy start
  stage = STAGE_BABY;   // Start as baby
  lightsOn = true;      // Lights on by default
  
  // Initialize sickness system
  hasToothache = false; // No toothache
  isSulking = false;    // Not sulking
  snackStreak = 0;      // No snacks eaten yet
  medicineNeeded = 0;   // No medicine needed
  sulkStartTime = 0;    // Not sulking
}

// Toggle sleep state (no immediate EEPROM write to prevent flickering)
void Boyfriend::updateSleeping() {
  sleeping = !sleeping;
}

// Update sleep need (add/subtract value) - batched save in main loop
void Boyfriend::updateSleep(int sleepVal) {
  sleep += sleepVal;
  // Constrain to 0-24 range
  if (sleep < 0) sleep = 0;
  if (sleep > 24) sleep = 24;
}

// Update happiness level (0-4 hearts)
void Boyfriend::updateHappiness(int happinessVal) {
  happiness += happinessVal;
  // Constrain to 0-4 range
  if (happiness < 0) happiness = 0;
  if (happiness > 4) happiness = 4;
}

// Update hunger level (0-4 hearts)
void Boyfriend::updateHunger(int hungerVal) {
  hunger += hungerVal;
  // Constrain to 0-4 range
  if (hunger < 0) hunger = 0;
  if (hunger > 4) hunger = 4;
}

// Update energy level - batched save in main loop
void Boyfriend::updateEnergy(int energyVal) {
  energy += energyVal;
  // Constrain to 0-24 range
  if (energy < 0) energy = 0;
  if (energy > 24) energy = 24;
}

// Increment age - batched save in main loop
void Boyfriend::updateAge() {
  age++;
}

// Update weight
void Boyfriend::updateWeight(int weightVal) {
  weight += weightVal;
  if (weight < 5) weight = 5;   // Minimum weight
  if (weight > 99) weight = 99; // Maximum weight
}

// Add to discipline (increments by 25%)
void Boyfriend::updateDiscipline(int disciplineVal) {
  discipline += disciplineVal;
  if (discipline > 100) discipline = 100;
}

// Increment care mistake counter
void Boyfriend::incrementCareMistake() {
  careMistakes++;
}

// Add poop to screen
void Boyfriend::addPoop() {
  poopCount++;
  if (poopCount > 4) poopCount = 4;
  
  // Get sick if too much poop
  if (poopCount >= 4) {
    isSick = true;
  }
}

// Clean all poop
void Boyfriend::cleanPoop() {
  poopCount = 0;
}

// Toggle lights on/off for sleep
void Boyfriend::toggleLights() {
  lightsOn = !lightsOn;
}

// Evolve to next stage
void Boyfriend::evolve() {
  if (stage < STAGE_ADULT) {
    stage = (LifeStage)((int)stage + 1);
    
    // Reset weight on evolution
    if (stage == STAGE_CHILD) weight = 10;
    else if (stage == STAGE_TEEN) weight = 15;
    else if (stage == STAGE_ADULT) weight = 20;
  }
}

// Set mood - batched save in main loop
void Boyfriend::updateMood(int newMood) {
  // Constrain to 0-3 range (0=neutral, 1=happy, 2=excited, 3=sad)
  if (newMood < 0) newMood = 0;
  if (newMood > 3) newMood = 3;
  mood = newMood;
}

// Map attribute value to bar length for display
int Boyfriend::mapValue(int val, int barLength) {
  // Map 0-24 range to 0-barLength range
  return map(val, 0, 24, 0, barLength);
}

// Load all attributes from EEPROM
void Boyfriend::loadFromEEPROM() {
  sleeping = EEPROM.read(0);
  sleep = EEPROM.read(1);
  happiness = EEPROM.read(2);
  hunger = EEPROM.read(3);
  energy = EEPROM.read(4);
  age = EEPROM.read(5);
  mood = EEPROM.read(6);
  weight = EEPROM.read(7);
  discipline = EEPROM.read(8);
  careMistakes = EEPROM.read(9);
  poopCount = EEPROM.read(10);
  isSick = EEPROM.read(11);
  stage = (LifeStage)EEPROM.read(12);
  lightsOn = EEPROM.read(13);
  hasToothache = EEPROM.read(14);
  isSulking = EEPROM.read(15);
  snackStreak = EEPROM.read(16);
  medicineNeeded = EEPROM.read(17);
  
  // Validate loaded values and set defaults if corrupted
  if (sleep > 24) sleep = 8;
  if (happiness > 4) happiness = 4;
  if (hunger > 4) hunger = 4;
  if (energy > 24) energy = 12;
  if (hasToothache > 1) hasToothache = false;  // Validate boolean
  if (isSulking > 1) isSulking = false;        // Validate boolean
  if (snackStreak > 30) snackStreak = 0;       // Cap snack streak
  if (medicineNeeded > 5) medicineNeeded = 0;  // Cap at 5 doses, reset if corrupted
  
  // If sickness flags are clear but medicineNeeded is set, reset it
  if (!isSick && !hasToothache && medicineNeeded > 0) {
    medicineNeeded = 0;
  }
  if (mood > 3) mood = 0;
  if (weight > 99) weight = 10;
  if (discipline > 100) discipline = 0;
  if (stage > STAGE_ADULT) stage = STAGE_BABY;
}

// Save all attributes to EEPROM
void Boyfriend::saveToEEPROM() {
  EEPROM.write(0, sleeping);
  EEPROM.write(1, sleep);
  EEPROM.write(2, happiness);
  EEPROM.write(3, hunger);
  EEPROM.write(4, energy);
  EEPROM.write(5, age);
  EEPROM.write(6, mood);
  EEPROM.write(7, weight);
  EEPROM.write(8, discipline);
  EEPROM.write(9, careMistakes);
  EEPROM.write(10, poopCount);
  EEPROM.write(11, isSick);
  EEPROM.write(12, (int)stage);
  EEPROM.write(13, lightsOn);
  EEPROM.write(14, hasToothache);
  EEPROM.write(15, isSulking);
  EEPROM.write(16, snackStreak);
  EEPROM.write(17, medicineNeeded);

  EEPROM.commit();
}

// ===== SICKNESS SYSTEM METHODS =====

// Track snack eating for toothache (15 consecutive = toothache)
void Boyfriend::eatSnack() {
  // Don't track for babies (they can't get toothaches)
  if (stage == STAGE_BABY || stage == STAGE_EGG) {
    return;
  }
  
  snackStreak++;
  checkToothache();
}

// Reset snack streak (called after time passes without snacks)
void Boyfriend::resetSnackStreak() {
  snackStreak = 0;
}

// Check if should get toothache from too many snacks (increased to 20 for less frequent)
void Boyfriend::checkToothache() {
  if (snackStreak >= 20 && stage != STAGE_BABY && stage != STAGE_EGG) {
    hasToothache = true;
    medicineNeeded = 1;  // Toothache needs 1 dose to cure
    snackStreak = 0;  // Reset counter
  }
}

// Give one dose of medicine to cure sickness
void Boyfriend::giveMedicine() {
  if (medicineNeeded > 0) {
    medicineNeeded--;
    
    // If all doses given, cure the sickness
    if (medicineNeeded == 0) {
      isSick = false;
      hasToothache = false;
    }
  }
}

// Start sulking from care mistake (adults only on most releases)
void Boyfriend::startSulking() {
  // Only adults can sulk (or children on 4U/4U+, but we'll keep it adult-only)
  if (stage == STAGE_ADULT || stage == STAGE_TEEN) {
    isSulking = true;
    sulkStartTime = millis();
  }
}

// Stop sulking
void Boyfriend::endSulking() {
  isSulking = false;
  sulkStartTime = 0;
}

// Check sulking status (auto-recovery or running away)
void Boyfriend::checkSulking() {
  if (!isSulking) return;
  
  unsigned long sulkDuration = millis() - sulkStartTime;
  
  // Auto-recovery after 5 minutes
  if (sulkDuration > 300000) {  // 5 minutes
    endSulking();
  }
  
  // Run away if sulking for too long (15 minutes)
  // This should trigger death/run away state in main game loop
  // For now we'll just end sulking to prevent permanent lock
  if (sulkDuration > 900000) {  // 15 minutes
    endSulking();
    // Game logic should handle running away
  }
}

// Check if can eat (only seriously sick blocks eating)
bool Boyfriend::canEat() {
  return !isSick;  // Only serious sickness blocks eating
}

// Check if can play games (only seriously sick blocks playing)
bool Boyfriend::canPlay() {
  return !isSick;  // Only serious sickness blocks playing
}

// Check if can use items (only seriously sick blocks items)
bool Boyfriend::canUseItems() {
  return !isSick;  // Only serious sickness blocks items
}