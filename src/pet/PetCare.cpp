#include "PetCare.h"
#include "app/GameContext.h"


unsigned long lastSnackTime = 0;
unsigned long lastMedicineTime = 0;
int consecutiveSnacks = 0;
unsigned long lastAttentionTime = 0;
unsigned long lastAnimUpdate = 0;
const unsigned long ATTENTION_COOLDOWN = 10000;
const unsigned long ATTENTION_TIMEOUT = 900000;
const unsigned long DEATH_THRESHOLD = 7200000;
const unsigned long SICKNESS_DEATH_THRESHOLD = 5400000;


void updateSicknessSystem() {
    unsigned long now = millis();


    if (pet.isSulking) {
        pet.checkSulking();
    }


    if (lastSnackTime > 0 && (now - lastSnackTime > 3600000)) {
        pet.resetSnackStreak();
        lastSnackTime = 0;
    }


    if ((pet.isSick || pet.hasToothache) && pet.medicineNeeded == 0) {
        pet.medicineNeeded = (pet.hasToothache ? 1 : 2);
    }
}
