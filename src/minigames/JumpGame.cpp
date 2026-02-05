#include "../GameState.h"

/**
 * MINIGAME: Jump Over Obstacles
 * 
 * Objective: Time your jumps to avoid obstacles moving from right to left
 * Controls: Button A or B = Jump
 * Game Over: If you hit an obstacle
 * Scoring: +1 point per obstacle passed, +1 happiness per obstacle
 * 
 * Physics:
 * - Jump arc: 25 pixels high, 20 frames total (10 up, 10 down)
 * - Obstacle speed: 3 pixels/frame
 * - Collision: if player Y >= ground-10 when obstacle passes
 */

void handleMinigameJump() {
    // Display score
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);
    
    // Ground level and jump parameters
    int groundY = canvas.height() - 20;
    int jumpHeight = 25;
    static bool isJumping = false;
    static int jumpProgress = 0;  // 0-20: jump animation progress
    int playerY = groundY;
    
    // Calculate jump arc (parabolic motion)
    if (isJumping) {
        jumpProgress++;
        if (jumpProgress < 10) {
            // Ascending: linear increase
            playerY = groundY - (jumpProgress * jumpHeight / 10);
        } else if (jumpProgress < 20) {
            // Descending: linear decrease
            playerY = groundY - ((20 - jumpProgress) * jumpHeight / 10);
        } else {
            // Jump complete
            isJumping = false;
            jumpProgress = 0;
        }
    }
    
    // Draw player (green square)
    canvas.fillRect(playerX - 2, playerY - 5, 5, 5, TFT_GREEN);
    
    // Draw obstacle (red rectangle moving left)
    canvas.fillRect(objectX, groundY - 8, 6, 8, TFT_RED);
    
    // Move obstacle left
    objectX += 3;
    
    // Collision detection
    if (objectX > playerX - 8 && objectX < playerX + 8) {
        if (playerY >= groundY - 10) {
            // Hit! Game over
            currentState = STATE_MINIGAME_RESULT;
            stateStartTime = millis();
            gameActive = false;
            M5.Speaker.tone(300, 100);  // Sad beep
            return;
        }
    }
    
    // Obstacle passed successfully
    if (objectX > canvas.width()) {
        objectX = 0;  // Reset to left side
        gameScore++;
        boyfriend.updateHappiness(1);
        M5.Speaker.tone(1200, 30);  // Success beep
    }
    
    // Jump input (either button works)
    if ((M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) && !isJumping) {
        isJumping = true;
        jumpProgress = 0;
        M5.Speaker.tone(1000, 50);  // Jump sound
    }
    
    // Show controls
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A/B:Jump");
}
