#include "MotionInput.h"

unsigned long lastShakeTime = 0;
int stepCount = 0;
unsigned long lastStepTime = 0;
bool isFlat = false;
bool isTiltedUp = false;
bool isTiltedDown = false;
int shakeCount = 0;
