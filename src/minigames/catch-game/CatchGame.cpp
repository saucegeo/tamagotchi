#include "app/Game.h"





int playerX = 60;
int objectX = 0;
int objectY = 0;
int gameScore = 0;
bool gameActive = false;

void handleMinigameCatch() {
    static int catchAnimTimer = 0;
    static int missAnimTimer = 0;
    static bool showInstructions = true;

    unsigned long elapsed = millis() - stateStartTime;
    unsigned long timeLeft = 20000 - elapsed;


    if (elapsed < 3000 && showInstructions) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(20, 25);
        canvas.print("CATCH!");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Catch falling hearts");
        canvas.setCursor(10, canvas.height() - 15);
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        canvas.print("A:Left  B:Right");
        return;
    }
    showInstructions = false;


    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);


    int timerWidth = (timeLeft * 110) / 20000;
    canvas.fillRect(5, 15, timerWidth, 3, TFT_GREEN);
    canvas.drawRect(5, 15, 110, 3, TFT_WHITE);


    int basketBounce = (M5.BtnA.isPressed() || M5.BtnB.isPressed()) ? -2 : 0;
    canvas.fillRect(playerX - 8, canvas.height() - 18 + basketBounce, 16, 4, TFT_ORANGE);
    canvas.fillRect(playerX - 6, canvas.height() - 14 + basketBounce, 12, 2, TFT_BROWN);
    canvas.drawRect(playerX - 8, canvas.height() - 18 + basketBounce, 16, 8, TFT_DARKGREY);


    int heartPulse = (millis() / 150) % 2;
    canvas.setTextColor(TFT_RED, TFT_WHITE);
    canvas.setTextSize(2 + heartPulse);
    canvas.setCursor(objectX - 5, objectY);
    canvas.print("<3");
    canvas.setTextSize(1);


    objectY += 2 + (elapsed / 5000);


    if (objectY >= canvas.height() - 25 && objectY <= canvas.height() - 15) {
        if (abs(objectX - playerX) < 12) {

            gameScore++;
            pet.updateHappiness(1);
            catchAnimTimer = 15;


            M5.Speaker.tone(1200 + (gameScore * 50), 50);


            objectX = random(20, canvas.width() - 20);
            objectY = 10;
        }
    }


    if (objectY > canvas.height() - 10) {
        missAnimTimer = 10;
        M5.Speaker.tone(300, 80);
        objectX = random(20, canvas.width() - 20);
        objectY = 10;
    }


    if (catchAnimTimer > 0) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setCursor(playerX - 10, canvas.height() - 30);
        canvas.print("*");
        canvas.setCursor(playerX + 5, canvas.height() - 32);
        canvas.print("*");
        catchAnimTimer--;
    }


    if (missAnimTimer > 0) {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        canvas.setCursor(playerX - 3, canvas.height() - 25);
        canvas.print("X");
        missAnimTimer--;
    }


    if (M5.BtnA.isPressed() && playerX > 12) {
        playerX -= 4;
    }
    if (M5.BtnB.isPressed() && playerX < canvas.width() - 12) {
        playerX += 4;
    }


    if (timeLeft < 3000 && (millis() / 500) % 2 == 0) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(canvas.width() - 15, 5);
        canvas.printf("%ld", timeLeft / 1000 + 1);
    }


    if (elapsed > 20000) {
        currentState = STATE_MINIGAME_RESULT;
        stateStartTime = millis();
        gameActive = false;
        M5.Speaker.tone(1800, 100);
    }


    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Left B:Right");
}
