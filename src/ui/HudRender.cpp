#include "HudRender.h"
#include <M5Unified.h>
#include "app/GameContext.h"
#include "content/SpecialDays.h"


void drawStatsBar() {

    canvas.setTextSize(1);
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Batt:%d%%", M5.Power.getBatteryLevel());


    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(75, 5);
    canvas.printf("Age:%d", pet.age);


    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(5, 20);
    canvas.print("H:");
    drawHearts(20, 20, pet.hunger, 4, TFT_ORANGE);


    canvas.setCursor(5, 32);
    canvas.print("♡:");
    drawHearts(20, 32, pet.happiness, 4, TFT_YELLOW);


    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(80, 20);
    canvas.printf("Wt:%d", pet.weight);

    canvas.setTextColor(TFT_PURPLE, TFT_BLACK);
    canvas.setCursor(80, 32);
    canvas.printf("Ds:%d%%", pet.discipline);


    if (currentSpecialDay >= 0) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print(specialDays[currentSpecialDay].name);
    } else if (pet.hunger == 0) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("HUNGRY!");
    } else if (pet.happiness == 0) {
        canvas.setTextColor(TFT_BLUE, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("SAD :(");
    } else if (pet.isSick) {
        canvas.setTextColor(TFT_GREEN, TFT_BLACK);
        canvas.setCursor(5, 48);
        canvas.print("SICK!");
    }

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Menu B:Feed");
}


void drawHearts(int x, int y, int filled, int total, uint16_t color) {
    for (int i = 0; i < total; i++) {
        if (i < filled) {

            canvas.setTextColor(color, TFT_BLACK);
        } else {

            canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        }
        canvas.setCursor(x + (i * 12), y);
        canvas.print("♥");
    }
}

void drawSicknessIndicators(int x, int y) {
    if (pet.isSick) {
        canvas.fillCircle(x, y, 8, TFT_WHITE);
        canvas.drawCircle(x, y, 8, TFT_DARKGREY);
        canvas.fillCircle(x - 3, y - 2, 2, TFT_BLACK);
        canvas.fillCircle(x + 3, y - 2, 2, TFT_BLACK);
        canvas.drawLine(x - 3, y + 3, x + 3, y + 3, TFT_DARKGREY);
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(1);
        canvas.setCursor(x - 8, y + 12);
        canvas.printf("x%d", pet.medicineNeeded);
    }

    if (pet.hasToothache) {
        canvas.fillRoundRect(x - 4, y - 6, 8, 10, 2, TFT_WHITE);
        canvas.drawRoundRect(x - 4, y - 6, 8, 10, 2, TFT_DARKGREY);
        canvas.drawLine(x - 2, y - 2, x + 2, y, TFT_RED);
        canvas.drawLine(x + 2, y, x - 2, y + 2, TFT_RED);
        canvas.drawLine(x, y + 4, x, y + 8, TFT_DARKGREY);
    }

    if (pet.isSulking) {
        canvas.drawLine(x - 6, y - 8, x - 6, y - 2, TFT_BLUE);
        canvas.drawLine(x, y - 10, x, y - 2, TFT_DARKGREY);
        canvas.drawLine(x + 6, y - 8, x + 6, y - 2, TFT_BLUE);
    }
}
