#include "GameState.h"

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
    
    // Button B: Feed
    if (M5.BtnB.wasPressed()) {
        currentState = STATE_EATING;
        stateStartTime = millis();
        boyfriend.updateHunger(-5);  // Reduce hunger by 5
        M5.Speaker.tone(1500, 50);   // Happy beep
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
    if ((boyfriend.hunger > 18 || boyfriend.happiness < 5 || boyfriend.energy < 5) &&
        (now - lastAttentionTime > ATTENTION_COOLDOWN)) {
        currentState = STATE_ATTENTION_NEEDED;
        stateStartTime = now;
        lastAttentionTime = now;  // Set cooldown
    }
}
