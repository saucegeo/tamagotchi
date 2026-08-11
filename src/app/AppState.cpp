#include "AppState.h"

GameState currentState = STATE_IDLE;
unsigned long stateStartTime = 0;
unsigned long lastAutoStateChange = 0;
