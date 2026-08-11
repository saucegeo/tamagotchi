#ifndef PET_LIFECYCLE_H
#define PET_LIFECYCLE_H

extern unsigned long clockSetTime;
extern unsigned long hatchTime;
extern unsigned long lastPoopTime;
extern unsigned long attentionStartTime;
extern unsigned long nextDisciplineCall;
extern bool needsAttentionCare;
extern bool isDead;
extern unsigned long hungerCriticalTime;
extern unsigned long happinessCriticalTime;
extern unsigned long energyCriticalTime;
extern unsigned long lastHungerUpdate;
extern unsigned long lastHappinessUpdate;
extern unsigned long lastEnergyUpdate;
extern const int MAX_AGE;
extern const unsigned long BABY_TO_CHILD_TIME;
extern const unsigned long HUNGER_INTERVAL;
extern const unsigned long HAPPINESS_INTERVAL;
extern const unsigned long ENERGY_INTERVAL;
extern const unsigned long POOP_INTERVAL_BABY;
extern const unsigned long POOP_INTERVAL_NORMAL;

void checkEvolution();
bool checkDeathConditions();
void resetPet();
void schedulePoop();

#endif
