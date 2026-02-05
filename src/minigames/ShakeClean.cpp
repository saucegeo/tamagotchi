#include "../GameState.h"

/**
 * MINIGAME: Shake to Clean
 * 
 * Objective: Shake the device to clean the room
 * Controls: Shake the device, Button B to return to idle
 * Scoring: +3 happiness for cleaning the room
 * 
 * Physics: Detect shakes using the IMU and reduce mess level accordingly
 */

// ===== NEW MECHANIC: Shake to Clean =====
void handleShakeClean() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Draw messy room (X marks)
    canvas.setTextColor(TFT_BROWN, TFT_BLACK);
    for (int i = 0; i < roomMess; i++) {
        int mx = 10 + (i * 15) % 100;
        int my = 20 + (i * 7) % 40;
        canvas.setCursor(mx, my);
        canvas.print("x");
    }
    
    // Progress bar
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, 5);
    canvas.printf("Clean: %d/20", 20 - roomMess);
    drawProgressBar(10, 15, 100, 8, 20 - roomMess, 20, TFT_GREEN);
    
    // Character cleaning
    drawCharacter(centerX, centerY);
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 70);
    canvas.print("SHAKE TO CLEAN!");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("Shake count: ");
    canvas.print(shakeCount);
    
    // Decrease mess with each shake
    if (shakeCount > 0) {
        roomMess -= shakeCount;
        shakeCount = 0;  // Reset after processing
        if (roomMess < 0) roomMess = 0;
    }
    
    // Complete when room is clean
    if (roomMess <= 0) {
        boyfriend.updateHappiness(3);
        M5.Speaker.tone(1500, 100);
        currentState = STATE_IDLE;
    }
    
    // Exit button
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
    }
}