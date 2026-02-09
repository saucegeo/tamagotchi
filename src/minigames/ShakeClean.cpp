#include "../GameState.h"

/**
 * MINIGAME: Shake to Clean
 * 
 * Objective: Shake the device to clean the room
 * Controls: Shake the device, Button B to exit
 * Scoring: +3 happiness for cleaning
 */

void handleShakeClean() {
    static bool showInstructions = true;
    static int cleanParticles[15] = {0};  // Cleaning sparkle particles
    static int totalShakes = 0;
    
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2;
    unsigned long elapsed = millis() - stateStartTime;
    
    // Instructions for first 2 seconds
    if (elapsed < 2000 && showInstructions) {
        canvas.setTextColor(TFT_GREEN, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(20, 20);
        canvas.print("CLEAN!");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Shake to clean!");
        
        // Draw broom
        canvas.drawLine(centerX, centerY + 10, centerX + 5, centerY + 25, TFT_BROWN);
        canvas.fillTriangle(centerX - 3, centerY + 25, centerX + 8, centerY + 25,
                           centerX + 2, centerY + 30, TFT_YELLOW);
        return;
    }
    showInstructions = false;
    
    // Draw dirt piles (animated)
    canvas.setTextColor(TFT_BROWN, TFT_BLACK);
    for (int i = 0; i < roomMess; i++) {
        int mx = 10 + (i * 13) % 100;
        int my = 25 + (i * 11) % 35;
        // Multiple dots to make it look like a pile
        canvas.fillCircle(mx, my, 2, TFT_BROWN);
        canvas.fillCircle(mx + 2, my + 1, 1, 0x6180);  // Dark brown
        canvas.fillCircle(mx - 2, my + 1, 1, 0x6180);
    }
    
    // Progress bar
    int cleanPercent = ((20 - roomMess) * 100) / 20;
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Clean: %d%%", cleanPercent);
    drawProgressBar(5, 15, 110, 8, 20 - roomMess, 20, TFT_GREEN);
    
    // Character cleaning (animated when shaking)
    static int cleanAnim = 0;
    if (shakeCount > 0) {
        cleanAnim = 15;
    } else if (cleanAnim > 0) {
        cleanAnim--;
    }
    
    int wobble = cleanAnim > 0 ? (cleanAnim % 4) - 2 : 0;
    drawCharacter(centerX, centerY + 10 + wobble);
    
    // Cleaning sparkles
    for (int i = 0; i < 15; i++) {
        if (cleanParticles[i] > 0) {
            int sparkleLife = cleanParticles[i];
            canvas.setTextColor(sparkleLife > 5 ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
            canvas.setCursor(20 + (i * 8) % 90, 25 + (i * 7) % 30);
            canvas.print(sparkleLife > 5 ? "*" : ".");
            cleanParticles[i]--;
        }
    }
    
    // Encouragement messages
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 65);
    if (roomMess > 15) {
        canvas.print("SHAKE SHAKE!");
    } else if (roomMess > 10) {
        canvas.print("Keep going!");
    } else if (roomMess > 5) {
        canvas.print("Almost there!");
    } else {
        canvas.print("Last bit!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.printf("Shakes: %d", totalShakes);
    
    // Process shakes with visual feedback
    if (shakeCount > 0) {
        int shakes = shakeCount;
        roomMess -= shakes;
        totalShakes += shakes;
        
        // Spawn sparkle particles
        for (int i = 0; i < shakes && i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                if (cleanParticles[j] == 0) {
                    cleanParticles[j] = 10;
                    break;
                }
            }
        }
        
        // Sound effect pitch increases as room gets cleaner
        M5.Speaker.tone(800 + ((20 - roomMess) * 40), 40);
        
        shakeCount = 0;
        if (roomMess < 0) roomMess = 0;
    }
    
    // Complete with celebration
    if (roomMess <= 0) {
        static unsigned long celebStartTime = 0;
        if (celebStartTime == 0) {
            celebStartTime = millis();
            M5.Speaker.tone(1500, 100);
            delay(100);
            M5.Speaker.tone(1800, 100);
        }
        
        unsigned long celebTime = millis() - celebStartTime;
        
        // Sparkles everywhere!
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(15, 30);
        canvas.print("SPOTLESS!");
        canvas.setTextSize(1);
        
        if (celebTime < 2000) {
            for (int i = 0; i < 8; i++) {
                int sparkX = 10 + (i * 15);
                int sparkY = 50 - ((celebTime / 100 + i * 2) % 30);
                canvas.setTextColor((i % 2) ? TFT_YELLOW : TFT_CYAN, TFT_BLACK);
                canvas.setCursor(sparkX, sparkY);
                canvas.print("*");
            }
        }
        
        if (celebTime > 2500) {
            boyfriend.updateHappiness(3);
            currentState = STATE_IDLE;
            celebStartTime = 0;  // Reset
        }
        return;
    }
    
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
    }
}