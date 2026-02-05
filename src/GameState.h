#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <M5Unified.h>
#include "sprites.h"

// ===== GAME STATES =====
enum GameState {
    STATE_IDLE,
    STATE_EATING,
    STATE_PLAYING,
    STATE_SLEEPING,
    STATE_LOVE_NOTE,
    STATE_SICK,
    STATE_ATTENTION_NEEDED,
    STATE_MINIGAME_MENU,
    STATE_MINIGAME_CATCH,
    STATE_MINIGAME_JUMP,
    STATE_MINIGAME_RESULT,
    STATE_FORTUNE_COOKIE,
    STATE_DANCE_MUSIC,
    STATE_STARGAZING,
    STATE_SHAKE_CLEAN,
    STATE_BLOW_CANDLE,
    STATE_LOVE_METER
};

// ===== SPECIAL DAYS =====
struct SpecialDay {
    int month;
    int day;
    const char* name;
    const char* message;
};

// ===== GLOBAL STATE (shared across files) =====
// These need to be declared as "extern" so multiple files can access them
extern M5Canvas canvas;
extern Boyfriend boyfriend;
extern GameState currentState;
extern unsigned long stateStartTime;
extern int animationFrame;

// Minigame state
extern int minigameSelection;
extern int playerX;
extern int objectX;
extern int objectY;
extern int gameScore;
extern bool gameActive;

// Sensor state
extern float accelX, accelY, accelZ;
extern float gyroX, gyroY, gyroZ;
extern bool isFlat;
extern bool isTiltedUp;
extern int shakeCount;
extern int micLevel;

// Special features state
extern int roomMess;
extern int fortuneIndex;
extern int heartLevel;
extern int buttonPressCount;
extern unsigned long loveMeterStartTime;
extern unsigned long lastAttentionTime;
extern int currentSpecialDay;

// Constants
extern const unsigned long ATTENTION_COOLDOWN;
extern const char* fortuneMessages[];
extern const SpecialDay specialDays[];
extern const int FORTUNE_MESSAGE_COUNT;
extern const int SPECIAL_DAY_COUNT;

// ===== HELPER FUNCTIONS (UI) =====
void drawCharacter(int x, int y, bool heartEyes = false);
void drawStatsBar();
int checkSpecialDay(int month, int day);

// ===== STATE HANDLERS =====
// These are implemented in separate .cpp files
void handleIdleState();
void handleEatingState();
void handleMinigameCatch();
void handleMinigameJump();
void handleFortuneCookie();
void handleBlowCandle();
void handleStargazing();

#endif
