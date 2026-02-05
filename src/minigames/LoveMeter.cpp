#include "../GameState.h"

// ===== NEW MECHANIC: Love Meter (Button Mash) =====
void handleLoveMeter() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2;
    
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(10, 10);
    canvas.print("LOVE METER");
    canvas.setTextSize(1);
    
    // Heart bar
    drawProgressBar(10, 35, 120, 20, heartLevel, 100, TFT_RED);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(55, 40);
    canvas.printf("%d%%", heartLevel);
    
    // Character pumping hearts
    int pumpOffset = (millis() % 200) < 100 ? -3 : 3;
    drawCharacter(centerX, centerY + pumpOffset);
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 65);
    canvas.print("MASH BUTTONS!");
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.printf("Presses: %d", buttonPressCount);
    
    // Count button presses
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        buttonPressCount++;
        heartLevel += 2;
        if (heartLevel > 100) heartLevel = 100;
        M5.Speaker.tone(1000 + (heartLevel * 10), 30);
    }
    
    // Time limit: 10 seconds
    unsigned long timeLeft = 10000 - (millis() - loveMeterStartTime);
    if (timeLeft > 10000) timeLeft = 0;  // Handle overflow
    
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 20);
    canvas.printf("Time: %ld", timeLeft / 1000);
    
    // End game
    if (millis() - loveMeterStartTime > 10000) {
        currentState = STATE_MINIGAME_RESULT;
        gameScore = heartLevel;  // Use heart level as score
        stateStartTime = millis();
        boyfriend.updateHappiness(heartLevel / 20);  // Bonus happiness based on performance
    }
}