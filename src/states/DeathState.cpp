#include "app/Game.h"



void handleDeathState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;


    drawSkull(centerX, centerY);


    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX - 15, centerY - 40);
        canvas.print("RIP");
        canvas.setTextSize(1);
    }


    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, centerY + 35);

    if (pet.age >= MAX_AGE) {
        canvas.print("Lived a full life");
    } else if (pet.hunger == 0) {
        canvas.print("Died of starvation");
    } else if (pet.happiness == 0) {
        canvas.print("Died of sadness");
    } else if (pet.isSick) {
        canvas.print("Died from sickness");
    } else {
        canvas.print("Has passed away");
    }


    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, centerY + 47);
    canvas.printf("Care: %d mistakes", pet.careMistakes);


    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 20);
    canvas.print("Press A+B together");
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("to start over");


    static unsigned long lastBeep = 0;
    if (millis() - lastBeep > 2000) {
        M5.Speaker.tone(400, 100);
        lastBeep = millis();
    }


    static unsigned long bothPressedTime = 0;

    if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {

        if (bothPressedTime == 0) {
            bothPressedTime = millis();
        } else if (millis() - bothPressedTime > 1000) {

            resetPet();
            currentState = STATE_IDLE;
            stateStartTime = millis();
            bothPressedTime = 0;
        }
    } else {
        bothPressedTime = 0;
    }
}
