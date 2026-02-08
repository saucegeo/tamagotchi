#include "../GameState.h"

/**
 * DISCIPLINE CALL STATE: Random attention call for discipline training
 * 
 * Trigger: Random timer while pet is idle
 * Controls: Button A = Discipline (correct), Button B = Ignore/Cancel
 * Result: +25% discipline if button A pressed
 */

void handleDisciplineCall() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw character calling for attention
    drawCharacter(centerX, centerY, false);
    
    // Flashing exclamation mark
    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(3);
        canvas.setCursor(centerX + 25, centerY - 25);
        canvas.print("!");
        canvas.setTextSize(1);
    }
    
    // Show prompt
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 10);
    canvas.print("Needs Discipline!");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Scold  B:Ignore");
    
    // Button A: Discipline (correct action)
    if (M5.BtnA.wasPressed()) {
        boyfriend.updateDiscipline(25);  // Add 25% discipline
        currentState = STATE_IDLE;
        M5.Speaker.tone(1200, 100);
        delay(100);
        M5.Speaker.tone(1500, 100);
        return;
    }
    
    // Button B or timeout: Ignore (no penalty, but no discipline gained)
    if (M5.BtnB.wasPressed() || millis() - stateStartTime > 10000) {
        currentState = STATE_IDLE;
        M5.Speaker.tone(800, 50);
    }
}
