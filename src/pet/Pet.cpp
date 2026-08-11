#include "../pet/Pet.h"


Pet::Pet(bool _sleeping, int _sleep, int _happiness, int _hunger,
               int _energy, int _age, int _mood) {
  sleeping = _sleeping;
  sleep = _sleep;
  happiness = _happiness;
  hunger = _hunger;
  energy = _energy;
  age = _age;
  mood = _mood;


  weight = 10;
  discipline = 0;
  careMistakes = 0;
  poopCount = 0;
  isSick = false;
  stage = STAGE_BABY;
  lightsOn = true;


  hasToothache = false;
  isSulking = false;
  snackStreak = 0;
  medicineNeeded = 0;
  sulkStartTime = 0;
}


void Pet::updateSleeping() {
  sleeping = !sleeping;
}


void Pet::updateSleep(int sleepVal) {
  sleep += sleepVal;

  if (sleep < 0) sleep = 0;
  if (sleep > 24) sleep = 24;
}


void Pet::updateHappiness(int happinessVal) {
  happiness += happinessVal;

  if (happiness < 0) happiness = 0;
  if (happiness > 4) happiness = 4;
}


void Pet::updateHunger(int hungerVal) {
  hunger += hungerVal;

  if (hunger < 0) hunger = 0;
  if (hunger > 4) hunger = 4;
}


void Pet::updateEnergy(int energyVal) {
  energy += energyVal;

  if (energy < 0) energy = 0;
  if (energy > 24) energy = 24;
}


void Pet::updateAge() {
  age++;
}


void Pet::updateWeight(int weightVal) {
  weight += weightVal;
  if (weight < 5) weight = 5;
  if (weight > 99) weight = 99;
}


void Pet::updateDiscipline(int disciplineVal) {
  discipline += disciplineVal;
  if (discipline > 100) discipline = 100;
}


void Pet::incrementCareMistake() {
  careMistakes++;
}


void Pet::addPoop() {
  poopCount++;
  if (poopCount > 4) poopCount = 4;


  if (poopCount >= 4) {
    isSick = true;
  }
}


void Pet::cleanPoop() {
  poopCount = 0;
}


void Pet::toggleLights() {
  lightsOn = !lightsOn;
}


void Pet::evolve() {
  if (stage < STAGE_ADULT) {
    stage = (LifeStage)((int)stage + 1);


    if (stage == STAGE_CHILD) weight = 10;
    else if (stage == STAGE_TEEN) weight = 15;
    else if (stage == STAGE_ADULT) weight = 20;
  }
}


void Pet::updateMood(int newMood) {

  if (newMood < 0) newMood = 0;
  if (newMood > 3) newMood = 3;
  mood = newMood;
}


int Pet::mapValue(int val, int barLength) {

  return map(val, 0, 24, 0, barLength);
}


void Pet::loadFromEEPROM() {
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


  if (sleep > 24) sleep = 8;
  if (happiness > 4) happiness = 4;
  if (hunger > 4) hunger = 4;
  if (energy > 24) energy = 12;
  if (hasToothache > 1) hasToothache = false;
  if (isSulking > 1) isSulking = false;
  if (snackStreak > 30) snackStreak = 0;
  if (medicineNeeded > 5) medicineNeeded = 0;


  if (!isSick && !hasToothache && medicineNeeded > 0) {
    medicineNeeded = 0;
  }
  if (mood > 3) mood = 0;
  if (weight > 99) weight = 10;
  if (discipline > 100) discipline = 0;
  if (stage > STAGE_ADULT) stage = STAGE_BABY;
}


void Pet::saveToEEPROM() {
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




void Pet::eatSnack() {

  if (stage == STAGE_BABY || stage == STAGE_EGG) {
    return;
  }

  snackStreak++;
  checkToothache();
}


void Pet::resetSnackStreak() {
  snackStreak = 0;
}


void Pet::checkToothache() {
  if (snackStreak >= 20 && stage != STAGE_BABY && stage != STAGE_EGG) {
    hasToothache = true;
    medicineNeeded = 1;
    snackStreak = 0;
  }
}


void Pet::giveMedicine() {
  if (medicineNeeded > 0) {
    medicineNeeded--;


    if (medicineNeeded == 0) {
      isSick = false;
      hasToothache = false;
    }
  }
}


void Pet::startSulking() {

  if (stage == STAGE_ADULT || stage == STAGE_TEEN) {
    isSulking = true;
    sulkStartTime = millis();
  }
}


void Pet::endSulking() {
  isSulking = false;
  sulkStartTime = 0;
}


void Pet::checkSulking() {
  if (!isSulking) return;

  unsigned long sulkDuration = millis() - sulkStartTime;


  if (sulkDuration > 300000) {
    endSulking();
  }




  if (sulkDuration > 900000) {
    endSulking();

  }
}


bool Pet::canEat() {
  return !isSick;
}


bool Pet::canPlay() {
  return !isSick;
}


bool Pet::canUseItems() {
  return !isSick;
}