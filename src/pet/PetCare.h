#ifndef PET_CARE_H
#define PET_CARE_H

extern unsigned long lastSnackTime;
extern unsigned long lastMedicineTime;
extern unsigned long lastAttentionTime;
extern unsigned long lastAnimUpdate;
extern int consecutiveSnacks;
extern const unsigned long ATTENTION_COOLDOWN;
extern const unsigned long ATTENTION_TIMEOUT;
extern const unsigned long DEATH_THRESHOLD;
extern const unsigned long SICKNESS_DEATH_THRESHOLD;

void updateSicknessSystem();

#endif
