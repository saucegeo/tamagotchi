#include "../GameState.h"

/**
 * MINIGAME: Fortune Cookie
 * 
 * Objective: Read prewritten messages of love and encouragement
 * Controls: Button A or B to return to idle
 * Scoring: +1 happiness when reading a fortune
 * 
 * Physics: None - it just displays a message and waits for the user to press a button to return to idle state
 */

void handleFortuneCookie() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(10, 20);
    canvas.print("FORTUNE");
    canvas.setTextSize(1);
    
    // Display fortune message (word wrap)
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, 45);
    canvas.print(fortuneMessages[fortuneIndex]);
    
    // Draw cookie emoji or simple icon
    canvas.setTextColor(TFT_ORANGE, TFT_BLACK);
    canvas.setTextSize(3);
    canvas.setCursor(centerX, centerY + 10);
    canvas.print("O");  // Simple cookie representation
    canvas.setTextSize(1);
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        boyfriend.updateHappiness(1);
        currentState = STATE_IDLE;
        M5.Speaker.tone(1200, 50);
    }
}