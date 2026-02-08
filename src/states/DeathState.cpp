#include "../GameState.h"

/**
 * DEATH STATE
 * 
 * Triggered when: age >= MAX_AGE, hunger = 0 for too long, 
 *                 happiness = 0 for too long, or sickness untreated
 * 
 * Reset: Press both buttons (A+B) together for 1 second
 * Effect: Displays cause of death and care mistakes
 */

void handleDeathState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw skull
    drawSkull(centerX, centerY);
    
    // Blinking "RIP" text
    if ((millis() - stateStartTime) % 1000 < 500) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(centerX - 15, centerY - 40);
        canvas.print("RIP");
        canvas.setTextSize(1);
    }
    
    // Death message based on cause
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, centerY + 35);
    
    if (boyfriend.age >= MAX_AGE) {
        canvas.print("Lived a full life");
    } else if (boyfriend.hunger == 0) {
        canvas.print("Died of starvation");
    } else if (boyfriend.happiness == 0) {
        canvas.print("Died of sadness");
    } else if (boyfriend.isSick) {
        canvas.print("Died from sickness");
    } else {
        canvas.print("Has passed away");
    }
    
    // Show care mistakes
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, centerY + 47);
    canvas.printf("Care: %d mistakes", boyfriend.careMistakes);
    
    // Instruction to reset
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 20);
    canvas.print("Press A+B together");
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("to start over");
    
    // Continuous beeping (but not too annoying)
    static unsigned long lastBeep = 0;
    if (millis() - lastBeep > 2000) {
        M5.Speaker.tone(400, 100);
        lastBeep = millis();
    }
    
    // Check for both buttons pressed together to reset
    static unsigned long bothPressedTime = 0;
    
    if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
        // Require both buttons held for 1 second to prevent accidental reset
        if (bothPressedTime == 0) {
            bothPressedTime = millis();
        } else if (millis() - bothPressedTime > 1000) {
            // Reset confirmed!
            resetPet();
            currentState = STATE_IDLE;
            stateStartTime = millis();
            bothPressedTime = 0;
        }
    } else {
        bothPressedTime = 0;  // Reset if buttons released
    }
}
