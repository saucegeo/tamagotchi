/*
 * Boyfriend.h - Header file for Boyfriend virtual pet class.
 * Inspired by Alegotchi, adapted for Boyfriend Bot theme.
 * Created for Arduino Tamagotchi project.
 * Released into the public domain.
 */

#ifndef Boyfriend_h  // Include guard - prevents duplicate includes
#define Boyfriend_h

#include <EEPROM.h>  // For saving/loading pet state to non-volatile memory

class Boyfriend {
public:
  // Constructor: Initializes Boyfriend with starting values
  // Parameters match attributes below (use defaults if none provided)
  Boyfriend(bool _sleeping = false, int _sleep = 8, int _happiness = 12, int _hunger = 0, 
         int _energy = 12, int _age = 0, int _mood = 0);

  // Pet attributes (public so main code can read them easily; 0-24 scale like Alegotchi)
  bool sleeping;      // Is Boyfriend asleep? (true/false) - EEPROM address 0
  int sleep;          // Sleep needed (0-24 hours) - address 1
  int happiness;      // Happiness level (0-24) - address 2
  int hunger;         // Hunger level (0-24, increases over time) - address 3
  int energy;         // Energy level (0-24, drains from play) - address 4
  int age;            // Age in days (increments over time) - address 5
  int mood;           // Mood state (0=neutral, 1=happy, 2=excited, 3=sad) - address 6

  // Update methods: Change attributes and save to EEPROM
  void updateSleeping();  // Toggle sleep state and save
  void updateSleep(int sleepVal);  // Adjust sleep need (+/- value) and save
  void updateHappiness(int happinessVal);  // Adjust happiness and save
  void updateHunger(int hungerVal);  // Adjust hunger and save
  void updateEnergy(int energyVal);  // Adjust energy and save
  void updateAge();  // Increment age daily and save
  void updateMood(int newMood);  // Set mood (0-3) and save

  // Helper: Map attribute value to bar length for display (e.g., for OLED progress bars)
  int mapValue(int val, int barLength = 20);  // Returns pixels for bar (0-20 default)

  // Load/Save: Restore or backup all state from EEPROM
  void loadFromEEPROM();  // Load saved values on startup
  void saveToEEPROM();    // Save current state
};

#endif  // End include guard