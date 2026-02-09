#include "../GameState.h"

/**
 * FEED MENU STATE: Choose between Meal or Snack
 * 
 * Meal: +1 Hunger heart, +1 Weight
 * Snack: +1 Happiness heart, +2 Weight
 * 
 * Controls: Button A = Select, Button B = Toggle option
 */

void handleFeedMenu() {
    static int feedSelection = 0;  // 0 = Meal, 1 = Snack, 2 = Medicine, 3 = Cancel
    
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw character
    drawCharacter(centerX, centerY, false);
    
    // Title
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(20, 10);
    canvas.print("FEED");
    canvas.setTextSize(1);
    
    // Check if options are available
    bool mealAvailable = (boyfriend.hunger < 4 && boyfriend.canEat());
    bool snackAvailable = (boyfriend.happiness < 4);
    bool medicineNeeded = (boyfriend.isSick || boyfriend.hasToothache);
    
    // Show options
    if (mealAvailable) {
        canvas.setTextColor(feedSelection == 0 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);  // Grayed out
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
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);  // Grayed out
    }
    canvas.setCursor(20, 47);
    canvas.print(feedSelection == 1 ? "> " : "  ");
    canvas.print("Snack (+♡ +2W)");
    if (!snackAvailable) {
        canvas.print(" FULL");
    };
    
    // Medicine option
    if (medicineNeeded) {
        canvas.setTextColor(feedSelection == 2 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    }
    canvas.setCursor(20, 59);
    canvas.print(feedSelection == 2 ? "> " : "  ");
    canvas.print("Medicine");
    if (medicineNeeded) {
        canvas.printf(" x%d", boyfriend.medicineNeeded);
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
    
    // Button A: Select option
    if (M5.BtnA.wasPressed()) {
        if (feedSelection == 0) {
            // Meal: +1 hunger, +1 weight
            if (!boyfriend.canEat()) {
                // Too sick to eat
                M5.Speaker.tone(300, 150);
            } else if (boyfriend.hunger < 4) {
                boyfriend.updateHunger(1);
                boyfriend.updateWeight(1);
                boyfriend.resetSnackStreak();  // Meal resets snack streak
                lastSnackTime = millis();  // Update last feed time
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
                feedSelection = 0;  // Reset selection
            } else {
                // Already full - play error sound
                M5.Speaker.tone(400, 100);
            }
        } else if (feedSelection == 1) {
            // Snack: +1 happiness, +2 weight, cures sulking
            if (boyfriend.isSulking) {
                boyfriend.endSulking();
            }
            
            if (!boyfriend.canEat()) {
                M5.Speaker.tone(300, 150);
            } else if (boyfriend.happiness < 4) {
                boyfriend.updateHappiness(1);
                boyfriend.updateWeight(2);
                boyfriend.eatSnack();  // Track snack for toothache
                lastSnackTime = millis();  // Update last snack time
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
                feedSelection = 0;  // Reset selection
            } else {
                // Already full happiness - play error sound
                M5.Speaker.tone(400, 100);
            }
        } else if (feedSelection == 2) {
            // Medicine - only allow if actually sick/has toothache
            if (!boyfriend.isSick && !boyfriend.hasToothache) {
                // Not sick - play error sound (like when meal/snack are full)
                M5.Speaker.tone(400, 100);
            } else if (boyfriend.isSick || boyfriend.hasToothache) {
                boyfriend.giveMedicine();
                lastMedicineTime = millis();
                
                M5.Speaker.tone(1800, 100);
                delay(100);
                M5.Speaker.tone(2000, 100);
                
                if (boyfriend.medicineNeeded == 0) {
                    // Show cured message briefly
                    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
                    canvas.setCursor(40, 75);
                    canvas.print("CURED!");
                }
                
                feedSelection = 0;
            } else {
                M5.Speaker.tone(400, 100);
            }
        } else {
            // Cancel
            currentState = STATE_IDLE;
            M5.Speaker.tone(800, 50);
            feedSelection = 0;  // Reset selection
        }
    }
    
    // Button B: Cycle through options
    if (M5.BtnB.wasPressed()) {
        feedSelection++;
        if (feedSelection > 3) feedSelection = 0;
        M5.Speaker.tone(1100, 30);  // Quick beep for menu navigation
    }
}
