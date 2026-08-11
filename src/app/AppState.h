#ifndef APP_STATE_H
#define APP_STATE_H

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

extern GameState currentState;
extern unsigned long stateStartTime;
extern unsigned long lastAutoStateChange;
extern int minigameSelection;

void handleIdleState();
void handleEatingState();
void handlePlayingState();
void handleSleepingState();
void handleAttentionNeededState();
void handleLoveNoteState();
void handleMinigameMenu();
void handleMinigameCatch();
void handleMinigameJump();
void handleMinigameResult();
void handleFortuneCookie();
void handleBlowCandle();
void handleStargazing();
void handleShakeClean();
void handleDanceMusic();
void handleLoveMeter();
void handleDeathState();
void handleDisciplineCall();
void handleFeedMenu();

#endif
