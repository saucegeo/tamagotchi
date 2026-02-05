#include "../GameState.h"

/**
 * MINIGAME RESULT STATE:
 * 
 * Objective: Display the results of the minigame played
 * Controls: Any button to return to idle
 * 
 * Physics: None - just a static results screen
 */

void handleMinigameResult() {
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(30, 30);
    canvas.print("SCORE:");
    canvas.setCursor(50, 50);
    canvas.printf("%d", gameScore);
    canvas.setTextSize(1);
    
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(20, 75);
    if (gameScore > 5) {
        canvas.print("Great job!");
    } else if (gameScore > 2) {
        canvas.print("Nice try!");
    } else {
        canvas.print("Keep practicing!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
    }
}