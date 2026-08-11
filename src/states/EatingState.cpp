#include "app/Game.h"



void handleEatingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;

    unsigned long elapsed = millis() - stateStartTime;
    int frame = (elapsed / 200) % 10;


    drawCharacter(centerX, centerY, false);


    if (frame % 2 == 0 && elapsed < 1500) {

        canvas.fillCircle(centerX, centerY + 8, 6, TFT_BLACK);
    }


    if (elapsed < 1500) {
        int foodX = centerX - 30 + (frame * 2);
        int foodY = centerY - 10 + (frame / 2);


        if (pet.happiness > pet.hunger) {

            canvas.fillCircle(foodX, foodY, 5, TFT_PINK);
            canvas.fillCircle(foodX - 2, foodY - 1, 2, TFT_RED);
            canvas.fillCircle(foodX + 2, foodY + 1, 2, TFT_RED);
        } else {

            canvas.fillCircle(foodX, foodY, 6, TFT_RED);
            canvas.fillRect(foodX - 1, foodY - 8, 2, 3, TFT_GREEN);
        }
    }


    if (elapsed > 1500) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX - 35, centerY - 20);
        canvas.print("♥");
        canvas.setCursor(centerX + 20, centerY - 25);
        canvas.print("♥");

        canvas.setTextSize(1);
        canvas.setTextColor(TFT_GREEN, TFT_BLACK);
        canvas.setCursor(centerX - 20, centerY + 20);
        canvas.print("YUM!");
    }


    if (elapsed > 2000) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1800, 50);
    }
}
