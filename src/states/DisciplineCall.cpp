#include "app/Game.h"



void handleDisciplineCall() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;


    drawCharacter(centerX, centerY, false);


    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(3);
        canvas.setCursor(centerX + 25, centerY - 25);
        canvas.print("!");
        canvas.setTextSize(1);
    }


    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 10);
    canvas.print("Needs Discipline!");

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Scold  B:Ignore");


    if (M5.BtnA.wasPressed()) {
        pet.updateDiscipline(25);
        currentState = STATE_IDLE;
        M5.Speaker.tone(1200, 100);
        delay(100);
        M5.Speaker.tone(1500, 100);
        return;
    }


    if (M5.BtnB.wasPressed() || millis() - stateStartTime > 10000) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(800, 50);
    }
}
