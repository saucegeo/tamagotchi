#include "app/Game.h"



void handleFeedMenu() {
    static int feedSelection = 0;

    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;


    drawCharacter(centerX, centerY, false);


    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(20, 10);
    canvas.print("FEED");
    canvas.setTextSize(1);


    bool mealAvailable = (pet.hunger < 4 && pet.canEat());
    bool snackAvailable = (pet.happiness < 4);
    bool medicineNeeded = (pet.isSick || pet.hasToothache);


    if (mealAvailable) {
        canvas.setTextColor(feedSelection == 0 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }
    canvas.setCursor(20, 35);
    canvas.print(feedSelection == 0 ? "> " : "  ");
    canvas.print("Meal (+H +1W)");
    if (!mealAvailable) {
        canvas.print(" FULL");
    }

    if (snackAvailable) {
        canvas.setTextColor(feedSelection == 1 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }
    canvas.setCursor(20, 47);
    canvas.print(feedSelection == 1 ? "> " : "  ");
    canvas.print("Snack (+♡ +2W)");
    if (!snackAvailable) {
        canvas.print(" FULL");
    };


    if (medicineNeeded) {
        canvas.setTextColor(feedSelection == 2 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }
    canvas.setCursor(20, 59);
    canvas.print(feedSelection == 2 ? "> " : "  ");
    canvas.print("Medicine");
    if (medicineNeeded) {
        canvas.printf(" x%d", pet.medicineNeeded);
    } else {
        canvas.print(" FULL");
    }

    canvas.setTextColor(feedSelection == 3 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    canvas.setCursor(20, 71);
    canvas.print(feedSelection == 3 ? "> " : "  ");
    canvas.print("Cancel");

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Select B:Next");


    if (M5.BtnA.wasPressed()) {
        if (feedSelection == 0) {

            if (!pet.canEat()) {

                M5.Speaker.tone(300, 150);
            } else if (pet.hunger < 4) {
                pet.updateHunger(1);
                pet.updateWeight(1);
                pet.resetSnackStreak();
                lastSnackTime = millis();
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
                feedSelection = 0;
            } else {

                M5.Speaker.tone(400, 100);
            }
        } else if (feedSelection == 1) {

            if (pet.isSulking) {
                pet.endSulking();
            }

            if (!pet.canEat()) {
                M5.Speaker.tone(300, 150);
            } else if (pet.happiness < 4) {
                pet.updateHappiness(1);
                pet.updateWeight(2);
                pet.eatSnack();
                lastSnackTime = millis();
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
                feedSelection = 0;
            } else {

                M5.Speaker.tone(400, 100);
            }
        } else if (feedSelection == 2) {

            if (!pet.isSick && !pet.hasToothache) {

                M5.Speaker.tone(400, 100);
            } else if (pet.isSick || pet.hasToothache) {
                pet.giveMedicine();
                lastMedicineTime = millis();

                M5.Speaker.tone(1800, 100);
                delay(100);
                M5.Speaker.tone(2000, 100);

                if (pet.medicineNeeded == 0) {

                    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
                    canvas.setCursor(40, 75);
                    canvas.print("CURED!");
                }

                feedSelection = 0;
            } else {
                M5.Speaker.tone(400, 100);
            }
        } else {

            currentState = STATE_IDLE;
            M5.Speaker.tone(800, 50);
            feedSelection = 0;
        }
    }


    if (M5.BtnB.wasPressed()) {
        feedSelection++;
        if (feedSelection > 3) feedSelection = 0;
        M5.Speaker.tone(1100, 30);
    }
}
