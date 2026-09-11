#include "app/Game.h"

// Handle the stargazing state
void handleStargazing() {

    // Create the starfield backgroud
    // 30 stars per frame
    for (int i = 0; i < 30; i++) {
        // Scatters the stars on the screen to look random
        int sx = (i * 37) % canvas.width(); 
        int sy = (i * 23) % canvas.height();

        // For the first half second is bright white
        // For the second half second becomes grey
        // This create the blinking effect
        int brightness = (millis() + i * 100) % 1000 < 500 ? TFT_WHITE : TFT_DARKGREY;
        canvas.fillCircle(sx, sy, 1, brightness);
    }

    // Draw the character
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 20;
    canvas.fillCircle(centerX, centerY, 20, TFT_WHITE);
    canvas.fillCircle(centerX - 6, centerY - 8, 3, TFT_BLACK);
    canvas.fillCircle(centerX + 6, centerY - 8, 3, TFT_BLACK);
    canvas.drawLine(centerX - 3, centerY + 2, centerX + 3, centerY + 2, TFT_BLACK);

    // Text showed at the top left of the screen
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 10);
    canvas.print("So beautiful...");

    // Text showed at the bottom
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Tilt down to stop");

    // Go back to the idle state when not tilted
    if (!isTiltedDown) {
        pet.updateHappiness(2);
        currentState = STATE_IDLE;
    }
}