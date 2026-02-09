#include "../GameState.h"

/**
 * MINIGAME: Jump Over Obstacles
 * 
 * Objective: Time your jumps to avoid obstacles
 * Controls: Button A or B = Jump
 * Game Over: If you hit an obstacle 3 times
 * Scoring: +1 point per obstacle passed
 */

void handleMinigameJump() {
    static bool isJumping = false;
    static int jumpProgress = 0;
    static int lives = 3;
    static int hitAnimTimer = 0;
    static bool showInstructions = true;
    static int speedIncrease = 0;
    
    unsigned long elapsed = millis() - stateStartTime;
    
    // Show instructions for first 3 seconds
    if (elapsed < 3000 && showInstructions) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(25, 25);
        canvas.print("JUMP!");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Avoid the spikes!");
        canvas.setCursor(10, canvas.height() - 15);
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        canvas.print("A or B:Jump");
        return;
    }
    showInstructions = false;
    
    // Display score & lives
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);
    
    // Draw lives as hearts
    for (int i = 0; i < lives; i++) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(canvas.width() - 30 + (i * 10), 5);
        canvas.print("\x03");
    }
    
    int groundY = canvas.height() - 20;
    int jumpHeight = 28;
    int playerY = groundY;
    
    // Calculate smooth jump arc (parabolic)
    if (isJumping) {
        jumpProgress++;
        float t = jumpProgress / 20.0;  // Normalize to 0-1
        // Parabolic arc: y = -4h(t)(t-1)
        playerY = groundY - (int)(4 * jumpHeight * t * (1 - t));
        
        if (jumpProgress >= 20) {
            isJumping = false;
            jumpProgress = 0;
        }
    }
    
    // Draw ground line
    canvas.drawLine(0, groundY + 5, canvas.width(), groundY + 5, TFT_GREEN);
    
    // Draw player character (animated)
    int rotation = isJumping ? (jumpProgress * 18) : 0;  // Rotate while jumping
    canvas.fillCircle(playerX, playerY, 4, TFT_WHITE);
    canvas.fillCircle(playerX - 2, playerY - 1, 1, TFT_BLACK);  // Eye
    canvas.fillCircle(playerX + 2, playerY - 1, 1, TFT_BLACK);
    
    // Running animation (legs)
    if (!isJumping) {
        int legAnim = (millis() / 100) % 4;
        canvas.drawLine(playerX - 2, playerY + 4, playerX - 3, playerY + 7, TFT_WHITE);
        canvas.drawLine(playerX + 2, playerY + 4, playerX + 3, playerY + 7, TFT_WHITE);
    }
    
    // Draw spiky obstacle (animated)
    int spikeColor = (hitAnimTimer > 0) ? TFT_ORANGE : TFT_RED;
    canvas.fillRect(objectX, groundY - 6, 8, 8, spikeColor);
    // Draw spikes on top
    canvas.fillTriangle(objectX, groundY - 6, objectX + 4, groundY - 10, objectX + 8, groundY - 6, spikeColor);
    
    // Move obstacle (speed increases over time)
    speedIncrease = gameScore / 3;  // Gets faster every 3 points
    objectX -= (2 + speedIncrease);
    
    // Collision detection (more forgiving hitbox)
    if (objectX > playerX - 6 && objectX < playerX + 2) {
        if (playerY >= groundY - 8) {
            // Hit!
            lives--;
            hitAnimTimer = 20;
            M5.Speaker.tone(300, 100);
            
            // Reset obstacle position
            objectX = canvas.width() + 20;
            
            if (lives <= 0) {
                // Game over
                currentState = STATE_MINIGAME_RESULT;
                stateStartTime = millis();
                gameActive = false;
                return;
            }
        }
    }
    
    // Obstacle passed successfully
    if (objectX < -10) {
        objectX = canvas.width() + 20;
        gameScore++;
        boyfriend.updateHappiness(1);
        M5.Speaker.tone(1200 + (gameScore * 50), 40);
    }
    
    // Hit animation (flash)
    if (hitAnimTimer > 0) {
        if (hitAnimTimer % 4 < 2) {
            canvas.fillCircle(playerX, playerY, 5, TFT_RED);
        }
        hitAnimTimer--;
    }
    
    // Jump input
    if ((M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) && !isJumping && playerY >= groundY - 2) {
        isJumping = true;
        jumpProgress = 0;
        M5.Speaker.tone(1000, 50);
    }
    
    // Show controls
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A/B:Jump");
}
