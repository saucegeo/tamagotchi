#include "../GameState.h"

/**
 * ATTENTION NEEDED STATE
 * 
 * Duration: 3 seconds or until button pressed
 * Trigger: When stats are low
 * Animation: Character with blinking exclamation mark
 * Interaction: Press any button to acknowledge
 */

void handleAttentionNeededState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX + 25, centerY - 20);
        canvas.print("!");
        canvas.setTextSize(1);
    }
    
    // Show prompt
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    // Allow button press to exit immediately
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
        return;
    }
    
    if (millis() - stateStartTime > 3000 || 
        (boyfriend.hunger <= 18 && boyfriend.happiness >= 5 && boyfriend.energy >= 5)) {
        currentState = STATE_IDLE;
    }
}
