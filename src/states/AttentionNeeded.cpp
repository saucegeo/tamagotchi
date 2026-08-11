#include "app/Game.h"
#include "app/GameContext.h"
#include "pet/PetCare.h"
#include "pet/Pet.h"
#include "ui/PetRender.h"
#include "ui/HudRender.h"



void handleAttentionNeededState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);


    if (pet.isSick || pet.hasToothache || pet.isSulking) {
        drawSicknessIndicators(centerX + 25, centerY - 15);
    } else {

        if ((millis() - stateStartTime) % 1000 < 500) {
            canvas.setTextColor(TFT_RED, TFT_BLACK);
            canvas.setTextSize(2);
            canvas.setCursor(centerX + 25, centerY - 20);
            canvas.print("!");
            canvas.setTextSize(1);
        }
    }


    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setTextSize(1);
    canvas.setCursor(10, canvas.height() - 30);

    if (pet.isSick || pet.hasToothache) {
        canvas.print("Needs medicine!");
    } else if (pet.isSulking) {
        canvas.print("Sulking...");
    } else if (pet.hunger == 0) {
        canvas.print("Hungry!");
    } else if (pet.happiness == 0) {
        canvas.print("Unhappy!");
    } else {
        canvas.print("Needs attention!");
    }

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");


    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
        return;
    }


    if (millis() - stateStartTime > 3000) {
        currentState = STATE_IDLE;
    }
}
