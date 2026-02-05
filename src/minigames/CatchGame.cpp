#include "../GameState.h"

/**
 * MINIGAME: Catch Falling Objects
 * 
 * Objective: Move left/right to catch falling objects
 * Controls: Button A = Left, Button B = Right
 * Duration: 20 seconds
 * Scoring: +1 point per catch, +1 happiness per catch
 * 
 * Physics:
 * - Object falls at 3 pixels/frame
 * - Player moves at 3 pixels/frame
 * - Collision detection: within 8 pixels horizontally
 */

void handleMinigameCatch() {
    // Display current score
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);
    
    // Draw player (green square at bottom of screen)
    canvas.fillRect(playerX - 2, canvas.height() - 20, 5, 5, TFT_GREEN);
    
    // Draw falling object (red square)
    canvas.fillRect(objectX - 2, objectY, 4, 4, TFT_RED);
    
    // Update object position (gravity)
    objectY += 3;  // Fall speed: 3 pixels per frame
    
    // Collision detection (check if object reached player height)
    if (objectY >= canvas.height() - 25 && objectY <= canvas.height() - 15) {
        // Check horizontal alignment
        if (abs(objectX - playerX) < 8) {
            // Success! Caught the object
            gameScore++;
            boyfriend.updateHappiness(1);
            M5.Speaker.tone(1500, 30);  // Success beep
            
            // Spawn new object at top
            objectX = random(20, 120);  // Random X position
            objectY = 10;               // Reset to top
        }
    }
    
    // Object missed (hit bottom)
    if (objectY > canvas.height() - 10) {
        // Respawn at top without scoring
        objectX = random(20, 120);
        objectY = 10;
    }
    
    // Player movement
    if (M5.BtnA.isPressed() && playerX > 10) {
        playerX -= 3;  // Move left
    }
    if (M5.BtnB.isPressed() && playerX < canvas.width() - 10) {
        playerX += 3;  // Move right
    }
    
    // Game ends after 20 seconds
    if (millis() - stateStartTime > 20000) {
        currentState = STATE_MINIGAME_RESULT;
        stateStartTime = millis();
        gameActive = false;
    }
    
    // Show controls
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Left B:Right");
}
