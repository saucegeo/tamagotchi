#include "../GameState.h"

// Global constants needed by this state
extern unsigned long lastAttentionTime;
extern const unsigned long ATTENTION_COOLDOWN;

/**
 * IDLE STATE: Main screen where pet waits for interaction
 * 
 * Controls:
 * - Button A: Open minigame menu
 * - Button B: Feed the pet
 * 
 * Auto-transitions:
 * - If stats are bad → ATTENTION_NEEDED (with 10s cooldown)
 */

void handleIdleState() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw the character
    drawCharacter(centerX, centerY);
    
    // Draw poop if present
    if (boyfriend.poopCount > 0) {
        for (int i = 0; i < boyfriend.poopCount && i < 4; i++) {
            drawPoop(centerX - 30 + (i * 15), canvas.height() - 25);
        }
    }
    
    // Button B: Open feed menu
    if (M5.BtnB.wasPressed()) {
        currentState = STATE_FEED_MENU;
        stateStartTime = millis();
        M5.Speaker.tone(1000, 50);
    } 
    // Button A: Open menu
    else if (M5.BtnA.wasPressed()) {
        currentState = STATE_MINIGAME_MENU;
        stateStartTime = millis();
        minigameSelection = 0;  // Reset menu selection
        M5.Speaker.tone(1000, 50);
    }
    
    // Check if pet needs attention (with cooldown to prevent spam)
    unsigned long now = millis();
    if ((boyfriend.hunger == 0 || boyfriend.happiness == 0 || boyfriend.isSick) &&
        (now - lastAttentionTime > ATTENTION_COOLDOWN)) {
        currentState = STATE_ATTENTION_NEEDED;
        stateStartTime = now;
        lastAttentionTime = now;  // Set cooldown
    }
}
