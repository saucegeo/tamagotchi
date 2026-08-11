#include "app/Game.h"



void handleSleepingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;

    canvas.fillCircle(centerX, centerY, 20, TFT_WHITE);
    canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 5, TFT_BLACK);
    canvas.drawLine(centerX + 4, centerY - 5, centerX + 8, centerY - 5, TFT_BLACK);
    canvas.drawLine(centerX - 3, centerY + 5, centerX + 3, centerY + 5, TFT_BLACK);

    int zOffset = (millis() - stateStartTime) / 500 % 3;
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(centerX + 25, centerY - 20 + zOffset * 5);
    canvas.print("Z");
    canvas.setCursor(centerX + 32, centerY - 15 + zOffset * 5);
    canvas.print("z");
    canvas.setCursor(centerX + 37, centerY - 10 + zOffset * 5);
    canvas.print("z");


    if (millis() - stateStartTime > 5000 ||
        M5.BtnA.wasPressed() ||
        M5.BtnB.wasPressed() ||
        !isFlat) {
        pet.updateEnergy(5);
        currentState = STATE_IDLE;
    }
}
