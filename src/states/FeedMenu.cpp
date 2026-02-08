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
    static int feedSelection = 0;  // 0 = Meal, 1 = Snack, 2 = Cancel
    
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
    
    // Show options
    canvas.setTextColor(feedSelection == 0 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    canvas.setCursor(20, 35);
    canvas.print(feedSelection == 0 ? "> " : "  ");
    canvas.print("Meal (+H +1W)");
    
    canvas.setTextColor(feedSelection == 1 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    canvas.setCursor(20, 47);
    canvas.print(feedSelection == 1 ? "> " : "  ");
    canvas.print("Snack (+♡ +2W)");
    
    canvas.setTextColor(feedSelection == 2 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
    canvas.setCursor(20, 59);
    canvas.print(feedSelection == 2 ? "> " : "  ");
    canvas.print("Cancel");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Select B:Next");
    
    // Button A: Select option
    if (M5.BtnA.wasPressed()) {
        if (feedSelection == 0) {
            // Meal: +1 hunger, +1 weight
            if (boyfriend.hunger < 4) {
                boyfriend.updateHunger(1);
                boyfriend.updateWeight(1);
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
            } else {
                // Already full
                M5.Speaker.tone(400, 100);
            }
        } else if (feedSelection == 1) {
            // Snack: +1 happiness, +2 weight
            if (boyfriend.happiness < 4) {
                boyfriend.updateHappiness(1);
                boyfriend.updateWeight(2);
                currentState = STATE_EATING;
                stateStartTime = millis();
                M5.Speaker.tone(1500, 50);
            } else {
                // Already full happiness
                M5.Speaker.tone(400, 100);
            }
        } else {
            // Cancel
            currentState = STATE_IDLE;
            M5.Speaker.tone(800, 50);
        }
        feedSelection = 0;  // Reset selection
    }
    
    // Button B: Cycle through options
    if (M5.BtnB.wasPressed()) {
        feedSelection++;
        if (feedSelection > 2) feedSelection = 0;
        M5.Speaker.tone(1000, 30);
    }
}
