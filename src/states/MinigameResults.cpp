#include "app/Game.h"



void handleMinigameResult() {

    unsigned long elapsed = millis() - stateStartTime;
    bool scoreRevealed = elapsed > 500;

    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(30, 20);
    canvas.print("SCORE:");


    if (scoreRevealed) {
        canvas.setCursor(50, 40);
        canvas.printf("%d", gameScore);
        canvas.setTextSize(1);


        if (gameScore > 5 && (elapsed / 200) % 2 == 0) {
            canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
            canvas.setCursor(30, 40);
            canvas.print("*");
            canvas.setCursor(100, 40);
            canvas.print("*");
        }
    }

    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setTextSize(1);
    canvas.setCursor(20, 65);
    if (gameScore > 5) {
        canvas.print("Great job!");

        if (elapsed > 1000 && elapsed < 1100) {
            M5.Speaker.tone(1500, 100);
        } else if (elapsed > 1100 && elapsed < 1200) {
            M5.Speaker.tone(1800, 100);
        }

        if (pet.isSulking) {
            pet.endSulking();
            canvas.setCursor(20, 80);
            canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
            canvas.print("Cheered up!");
        }
    } else if (gameScore > 2) {
        canvas.print("Nice try!");

        if (pet.isSulking) {
            pet.endSulking();
            canvas.setCursor(20, 80);
            canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
            canvas.print("Feeling better!");
        }
    } else {
        canvas.print("Keep practicing!");
    }

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");

    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1400, 50);
    }
}