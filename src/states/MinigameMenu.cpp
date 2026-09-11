#include "app/Game.h"
#include "app/GameContext.h"
#include <M5Unified.h>



int minigameSelection = 0;

void handleMinigameMenu() {
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(20, 10);
    canvas.print("MENU");
    canvas.setTextSize(1);


    if (!pet.canPlay()) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(80, 12);
        canvas.print("SICK!");
    }


    const char* menuOptions[] = {
        "CATCH", "JUMP", "FORTUNE",
        "DANCE", "SHAKE", "CANDLE", "LOVE", "EXIT"
    };
    int numOptions = 8;


    for (int i = 0; i < 3 && (minigameSelection - 1 + i) < numOptions; i++) {
        int optionIndex = minigameSelection - 1 + i;
        if (optionIndex < 0) optionIndex = 0;

        canvas.setTextColor(i == 1 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
        canvas.setCursor(20, 35 + (i * 15));
        canvas.print(i == 1 ? "> " : "  ");
        canvas.print(menuOptions[optionIndex]);
    }

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Select B:Next");

    if (M5.BtnA.wasPressed()) {

        stateStartTime = millis();
        switch(minigameSelection) {
            case 0:
                currentState = STATE_MINIGAME_CATCH;
                objectX = random(20, 120);
                objectY = 10;
                playerX = 60;
                gameScore = 0;
                gameActive = true;
                break;
            case 1:
                currentState = STATE_MINIGAME_JUMP;
                objectX = 0;
                playerX = 30;
                gameScore = 0;
                gameActive = true;
                break;
            case 2:
                currentState = STATE_FORTUNE_COOKIE;
                // Choose one message randomly and output it on screen
                randomFortune = random(0, totalFortunes);
                break;
            case 3:
                currentState = STATE_DANCE_MUSIC;
                break;
            case 4:
                currentState = STATE_SHAKE_CLEAN;
                roomMess = 20;
                shakeCount = 0;
                break;
            case 5:
                currentState = STATE_BLOW_CANDLE;
                break;
            case 6:
                currentState = STATE_LOVE_METER;
                heartLevel = 0;
                buttonPressCount = 0;
                loveMeterStartTime = millis();
                break;
            case 7:
                currentState = STATE_IDLE;
                minigameSelection = 0;
                M5.Speaker.tone(800, 50);
                break;
        }

        M5.Speaker.tone(1600, 50);
    } else if (M5.BtnB.wasPressed()) {

        minigameSelection++;
        if (minigameSelection >= numOptions) minigameSelection = 0;
        M5.Speaker.tone(1100, 30);
    }


    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
        minigameSelection = 0;
        M5.Speaker.tone(800, 50);
    }
};
