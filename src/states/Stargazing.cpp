#include "../GameState.h"

/* Stargazing State:
    * 
    * Objective: Enjoy the stars and boost happiness
    * Controls: None - just tilt up to enter, tilt down to exit
    * Scoring: +2 happiness per second spent stargazing
    * 
    * Physics: None - it's a relaxing state with a starry background and a character looking up

*/

void handleStargazing() {
    // Draw stars background
    for (int i = 0; i < 30; i++) {
        int sx = (i * 37) % canvas.width();
        int sy = (i * 23) % canvas.height();
        int brightness = (millis() + i * 100) % 1000 < 500 ? TFT_WHITE : TFT_DARKGREY;
        canvas.fillCircle(sx, sy, 1, brightness);
    }
    
    // Draw character looking up
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 20;
    canvas.fillCircle(centerX, centerY, 20, TFT_WHITE);
    canvas.fillCircle(centerX - 6, centerY - 8, 3, TFT_BLACK);
    canvas.fillCircle(centerX + 6, centerY - 8, 3, TFT_BLACK);
    canvas.drawLine(centerX - 3, centerY + 2, centerX + 3, centerY + 2, TFT_BLACK);
    
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 10);
    canvas.print("So beautiful...");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Tilt down to stop");
    
    // Check if still tilted up
    if (!isTiltedUp) {
        boyfriend.updateHappiness(2);
        currentState = STATE_IDLE;
    }
}