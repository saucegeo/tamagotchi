#include "../GameState.h"

/**
 * ATTENTION NEEDED STATE
 * 
 * Duration: 3 seconds or until button pressed
 * Trigger: When stats are low or sick
 * Animation: Character with blinking exclamation mark (or sickness indicator)
 * Interaction: Press any button to go to feed menu
 */

void handleAttentionNeededState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    drawCharacter(centerX, centerY);
    
    // Draw sickness indicators if sick
    if (boyfriend.isSick || boyfriend.hasToothache || boyfriend.isSulking) {
        drawSicknessIndicators(centerX + 25, centerY - 15);
    } else {
        // Regular attention mark (blinking)
        if ((millis() - stateStartTime) % 1000 < 500) {
            canvas.setTextColor(TFT_RED, TFT_BLACK);
            canvas.setTextSize(2);
            canvas.setCursor(centerX + 25, centerY - 20);
            canvas.print("!");
            canvas.setTextSize(1);
        }
    }
    
    // Show appropriate prompt based on condition
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setTextSize(1);
    canvas.setCursor(10, canvas.height() - 30);
    
    if (boyfriend.isSick || boyfriend.hasToothache) {
        canvas.print("Needs medicine!");
    } else if (boyfriend.isSulking) {
        canvas.print("Sulking...");
    } else if (boyfriend.hunger == 0) {
        canvas.print("Hungry!");
    } else if (boyfriend.happiness == 0) {
        canvas.print("Unhappy!");
    } else {
        canvas.print("Needs attention!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Press any button");
    
    // Any button exits
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(1000, 50);
        return;
    }
    
    // Auto-exit after 3 seconds
    if (millis() - stateStartTime > 3000) {
        currentState = STATE_IDLE;
    }
}
