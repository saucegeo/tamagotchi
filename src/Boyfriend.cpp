/*
 * Boyfriend.cpp - Implementation file for Boyfriend virtual pet class.
 * Inspired by Alegotchi, adapted for Boyfriend Bot theme.
 * Created for Arduino Tamagotchi project.
 * Released into the public domain.
 */

#include "sprites.h"

// Constructor: Initialize Boyfriend with default or provided values
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
  
  // Validate loaded values and set defaults if corrupted
  if (sleep > 24) sleep = 8;
  if (happiness > 4) happiness = 4;
  if (hunger > 4) hunger = 4;
  if (energy > 24) energy = 12;
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

  EEPROM.commit();
}