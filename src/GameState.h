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
    STATE_LOVE_METER,
    STATE_DEAD,
    STATE_DISCIPLINE_CALL,
    STATE_FEED_MENU
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

// Death tracking
extern unsigned long hungerCriticalTime;
extern unsigned long happinessCriticalTime;
extern unsigned long energyCriticalTime;
extern bool isDead;

// Lifecycle tracking
extern unsigned long clockSetTime;
extern unsigned long hatchTime;
extern unsigned long lastPoopTime;
extern unsigned long attentionStartTime;
extern unsigned long nextDisciplineCall;
extern bool needsAttentionCare;
extern int roundsWon;
extern int roundsPlayed;
extern int playerGuess;
extern int computerChoice;

// Constants
extern const unsigned long ATTENTION_COOLDOWN;
extern const unsigned long DEATH_THRESHOLD;
extern const int MAX_AGE;
extern const unsigned long BABY_TO_CHILD_TIME;
extern const unsigned long ATTENTION_TIMEOUT;
extern const unsigned long POOP_INTERVAL_BABY;
extern const unsigned long POOP_INTERVAL_NORMAL;
extern const char* fortuneMessages[];
extern const SpecialDay specialDays[];
extern const int FORTUNE_MESSAGE_COUNT;
extern const int SPECIAL_DAY_COUNT;

// ===== HELPER FUNCTIONS (UI) =====
void drawCharacter(int x, int y, bool heartEyes = false);
void drawSkull(int x, int y);
void drawEgg(int x, int y);
void drawPoop(int x, int y);
void drawHearts(int x, int y, int filled, int total, uint16_t color);
void drawStatsBar();
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color);
int checkSpecialDay(int month, int day);
bool checkDeathConditions();
void resetPet();
void checkEvolution();
void schedulePoop();

// ===== STATE HANDLERS =====
// These are implemented in separate .cpp files
void handleIdleState();
void handleEatingState();
void handlePlayingState();
void handleSleepingState();
void handleAttentionNeededState();
void handleLoveNoteState();
void handleMinigameMenu();
void handleMinigameCatch();
void handleMinigameJump();
void handleFortuneCookie();
void handleBlowCandle();
void handleStargazing();
void handleMinigameResult();
void handleShakeClean();
void handleDanceMusic();
void handleLoveMeter();
void handleDeathState();
void handleDisciplineCall();
void handleFeedMenu();

#endif
