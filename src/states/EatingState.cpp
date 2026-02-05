#include "../GameState.h"

/**
 * EATING STATE: Shows animation of pet eating food
 * 
 * Duration: 2 seconds
 * Auto-transition: Returns to IDLE after animation completes
 */

void handleEatingState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw happy eating face
    canvas.fillCircle(centerX, centerY, 21, TFT_WHITE);
    canvas.drawLine(centerX - 8, centerY - 5, centerX - 4, centerY - 7, TFT_BLACK);  // Left eye
    canvas.drawLine(centerX + 4, centerY - 7, centerX + 8, centerY - 5, TFT_BLACK);  // Right eye
    canvas.drawArc(centerX, centerY + 3, 8, 6, 180, 360, TFT_BLACK);                // Smile
    
    // Draw food (apple)
    canvas.fillCircle(centerX - 25, centerY - 10, 5, TFT_RED);
    
    // "YUM!" text
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(centerX - 30, centerY + 15);
    canvas.print("YUM!");
    
    // Return to idle after 2 seconds
    if (millis() - stateStartTime > 2000) {
        currentState = STATE_IDLE;
    }
}
