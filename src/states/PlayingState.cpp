#include "app/Game.h"



void handlePlayingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    int bounceOffset = (millis() - stateStartTime) % 500 < 250 ? -5 : 5;
    drawCharacter(centerX, centerY + bounceOffset);

    int heartOffset = (millis() - stateStartTime) / 100 % 20;
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(centerX - 25, centerY - heartOffset);
    canvas.print("♥");
    canvas.setCursor(centerX + 25, centerY - heartOffset - 5);
    canvas.print("♥");

    if (millis() - stateStartTime > 2000) {
        currentState = STATE_IDLE;
    }
}
