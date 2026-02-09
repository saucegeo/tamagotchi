/*
 * Boyfriend.h - Header file for Boyfriend virtual pet class.
 * Inspired by Alegotchi, adapted for Boyfriend Bot theme.
 * Created for Arduino Tamagotchi project.
 * Released into the public domain.
 */

#ifndef Boyfriend_h  // Include guard - prevents duplicate includes
#define Boyfriend_h

#include <EEPROM.h>  // For saving/loading pet state to non-volatile memory

// Evolution stages
enum LifeStage {
  STAGE_EGG = 0,
  STAGE_BABY = 1,
  STAGE_CHILD = 2,
  STAGE_TEEN = 3,
  STAGE_ADULT = 4
};

class Boyfriend {
public:
  // Constructor: Initializes Boyfriend with starting values
  Boyfriend(bool _sleeping = false, int _sleep = 8, int _happiness = 4, int _hunger = 4, 
         int _energy = 12, int _age = 0, int _mood = 0);

  // Pet attributes (authentic Tamagotchi style)
  bool sleeping;      // Is Boyfriend asleep? (true/false) - EEPROM address 0
  int sleep;          // Sleep needed (0-24 hours) - address 1
  int happiness;      // Happiness hearts (0-4) - address 2
  int hunger;         // Hunger hearts (0-4, 4=full) - address 3
  int energy;         // Energy level (0-24, drains from play) - address 4
  int age;            // Age in days (increments on wake) - address 5
  int mood;           // Mood state (0=neutral, 1=happy, 2=excited, 3=sad) - address 6
  
  // New Tamagotchi mechanics
  int weight;         // Weight in units (starts at 10) - address 7
  int discipline;     // Discipline % (0-100, increments by 25) - address 8
  int careMistakes;   // Care mistake counter - address 9
  int poopCount;      // Number of poops on screen (0-4) - address 10
  bool isSick;        // Is pet sick from neglect? - address 11
  LifeStage stage;    // Evolution stage - address 12
  bool lightsOn;      // Are lights on/off - address 13
  
  // Sickness system
  bool hasToothache;  // Has toothache from too many snacks - address 14
  bool isSulking;     // Is sulking from care mistake - address 15
  int snackStreak;    // Consecutive snacks eaten (resets after time) - address 16
  int medicineNeeded; // Doses of medicine needed to cure - address 17
  unsigned long sulkStartTime; // When sulking started (not saved to EEPROM)

  // Update methods: Change attributes and save to EEPROM
  void updateSleeping();  // Toggle sleep state and save
  void updateSleep(int sleepVal);  // Adjust sleep need (+/- value) and save
  void updateHappiness(int happinessVal);  // Adjust happiness hearts (0-4)
  void updateHunger(int hungerVal);  // Adjust hunger hearts (0-4)
  void updateEnergy(int energyVal);  // Adjust energy and save
  void updateAge();  // Increment age on wake and save
  void updateMood(int newMood);  // Set mood (0-3) and save
  void updateWeight(int weightVal);  // Adjust weight
  void updateDiscipline(int disciplineVal);  // Add to discipline
  void incrementCareMistake();  // Add care mistake
  void addPoop();  // Add poop to screen
  void cleanPoop();  // Remove all poop
  void toggleLights();  // Turn lights on/off
  void evolve();  // Evolve to next stage
  
  // Sickness system methods
  void eatSnack();  // Track snack eating for toothache
  void resetSnackStreak();  // Reset snack counter after time
  void checkToothache();  // Check if should get toothache
  void giveMedicine();  // Give one dose of medicine
  void startSulking();  // Begin sulking state
  void endSulking();  // Stop sulking
  void checkSulking();  // Check if should stop sulking or run away
  bool canEat();  // Check if can eat (not sick/sulking)
  bool canPlay();  // Check if can play games (not sick)
  bool canUseItems();  // Check if can use items (not sick)

  // Helper: Map attribute value to bar length for display (e.g., for OLED progress bars)
  int mapValue(int val, int barLength = 20);  // Returns pixels for bar (0-20 default)

  // Load/Save: Restore or backup all state from EEPROM
  void loadFromEEPROM();  // Load saved values on startup
  void saveToEEPROM();    // Save current state
};

#endif  // End include guard