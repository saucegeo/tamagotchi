#include "../GameState.h"

/**
 * LOVE NOTE STATE
 * 
 * Duration: 3 seconds
 * Animation: Character with "I <3 U!" message and floating hearts
 * Auto-transition: Returns to IDLE after 3s
 */

void handleLoveNoteState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(10, centerY - 25);
    canvas.print("I <3 U!");
    
    int heartY = centerY - ((millis() - stateStartTime) / 50 % 30);
    canvas.setCursor(centerX - 30, heartY);
    canvas.print("♥");
    canvas.setCursor(centerX + 20, heartY - 5);
    canvas.print("♥");
    
    if (millis() - stateStartTime > 3000) {
        currentState = STATE_IDLE;
    }
}
