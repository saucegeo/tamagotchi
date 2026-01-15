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
  happiness = _happiness;
  hunger = _hunger;
  energy = _energy;
  age = _age;
  mood = _mood;
}

// Toggle sleep state and save to EEPROM
void Boyfriend::updateSleeping() {
  sleeping = !sleeping;
  EEPROM.write(0, sleeping);
}

// Update sleep need (add/subtract value) and save to EEPROM
void Boyfriend::updateSleep(int sleepVal) {
  sleep += sleepVal;
  // Constrain to 0-24 range
  if (sleep < 0) sleep = 0;
  if (sleep > 24) sleep = 24;
  EEPROM.write(1, sleep);
}

// Update happiness level and save to EEPROM
void Boyfriend::updateHappiness(int happinessVal) {
  happiness += happinessVal;
  // Constrain to 0-24 range
  if (happiness < 0) happiness = 0;
  if (happiness > 24) happiness = 24;
  EEPROM.write(2, happiness);
}

// Update hunger level and save to EEPROM
void Boyfriend::updateHunger(int hungerVal) {
  hunger += hungerVal;
  // Constrain to 0-24 range
  if (hunger < 0) hunger = 0;
  if (hunger > 24) hunger = 24;
  EEPROM.write(3, hunger);
}

// Update energy level and save to EEPROM
void Boyfriend::updateEnergy(int energyVal) {
  energy += energyVal;
  // Constrain to 0-24 range
  if (energy < 0) energy = 0;
  if (energy > 24) energy = 24;
  EEPROM.write(4, energy);
}

// Increment age and save to EEPROM
void Boyfriend::updateAge() {
  age++;
  EEPROM.write(5, age);
}

// Set mood and save to EEPROM
void Boyfriend::updateMood(int newMood) {
  // Constrain to 0-3 range (0=neutral, 1=happy, 2=excited, 3=sad)
  if (newMood < 0) newMood = 0;
  if (newMood > 3) newMood = 3;
  mood = newMood;
  EEPROM.write(6, mood);
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
  
  // Validate loaded values and set defaults if corrupted
  if (sleep > 24) sleep = 8;
  if (happiness > 24) happiness = 12;
  if (hunger > 24) hunger = 0;
  if (energy > 24) energy = 12;
  if (mood > 3) mood = 0;
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

  EEPROM.commit();
}