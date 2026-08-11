#ifndef PET_H
#define PET_H

#include <EEPROM.h>


enum LifeStage {
  STAGE_EGG = 0,
  STAGE_BABY = 1,
  STAGE_CHILD = 2,
  STAGE_TEEN = 3,
  STAGE_ADULT = 4,
  STAGE_GRANDPA = 5
};

enum CharacterOutfit {
  OUTFIT_CASUAL = 0,
  OUTFIT_FORMAL = 1,
  OUTFIT_SPORTS = 2,
  OUTFIT_SLEEPWEAR = 3
};

class Pet {

public:


  bool sleeping;
  int sleep;
  int happiness;
  int hunger;
  int energy;
  int age;
  int mood;


  int weight;
  int discipline;
  int careMistakes;
  int poopCount;
  LifeStage stage;
  bool lightsOn;


  bool hasToothache;
  bool isSulking;
  int snackStreak;
  int medicineNeeded;
  unsigned long sulkStartTime;

public:

  Pet(bool _sleeping = false, int _sleep = 8, int _happiness = 4, int _hunger = 4,
         int _energy = 12, int _age = 0, int _mood = 0);

  bool isSick;


  void updateSleeping();
  void updateSleep(int sleepVal);
  void updateHappiness(int happinessVal);
  void updateHunger(int hungerVal);
  void updateEnergy(int energyVal);
  void updateAge();
  void updateMood(int newMood);
  void updateWeight(int weightVal);
  void updateDiscipline(int disciplineVal);
  void incrementCareMistake();
  void addPoop();
  void cleanPoop();
  void toggleLights();
  void evolve();


  void eatSnack();
  void resetSnackStreak();
  void checkToothache();
  void giveMedicine();
  void startSulking();
  void endSulking();
  void checkSulking();
  bool canEat();
  bool canPlay();
  bool canUseItems();


  int mapValue(int val, int barLength = 20);


  void loadFromEEPROM();
  void saveToEEPROM();

  CharacterOutfit currOutfit;
  void nextOutfit();
};


#endif
