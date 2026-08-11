#include "app/Game.h"


extern unsigned long lastAttentionTime;
extern const unsigned long ATTENTION_COOLDOWN;



void handleIdleState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;


    drawCharacter(centerX, centerY);


    if (pet.isSick || pet.hasToothache || pet.isSulking) {
        drawSicknessIndicators(centerX + 25, centerY - 15);
    }


    if (pet.poopCount > 0) {
        for (int i = 0; i < pet.poopCount && i < 4; i++) {
            drawPoop(centerX - 30 + (i * 15), canvas.height() - 25);
        }
    }


    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setTextSize(1);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Menu B:Feed");


    if (M5.BtnB.wasPressed()) {
        currentState = STATE_FEED_MENU;
        stateStartTime = millis();
        M5.Speaker.tone(1200, 50);
    }

    else if (M5.BtnA.wasPressed()) {
        currentState = STATE_MINIGAME_MENU;
        stateStartTime = millis();
        minigameSelection = 0;
        M5.Speaker.tone(1500, 50);
    }


    unsigned long now = millis();
    if ((pet.hunger == 0 || pet.happiness == 0 || pet.isSick || pet.hasToothache) &&
        (now - lastAttentionTime > ATTENTION_COOLDOWN)) {
        currentState = STATE_ATTENTION_NEEDED;
        stateStartTime = now;
        lastAttentionTime = now;
    }
}
