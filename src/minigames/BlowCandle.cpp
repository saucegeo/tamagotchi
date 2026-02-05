#include "../GameState.h"

 /**
  * MINIGAME: Blow Candle
  * 
  * Objective: Blow out the candle by making a loud noise into the mic
  * Controls: Blow into the mic, Button B to return to idle -> need to blow with mic level (set at 500) to succeed
  * Scoring: +4 happiness for blowing out the candle
  * 
  * Physics: None - it just detects mic input and updates the display accordingly
  */

void handleBlowCandle() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    int micLevel = 0;
    int blowThreshold = 500; // Adjust this threshold based on testing with the mic input
    
    // Draw character
    drawCharacter(centerX - 20, centerY + 10);
    
    // Draw candle flame (flickers)
    int flameFlicker = (millis() - stateStartTime) % 200 < 100 ? 2 : 0;
    canvas.fillRect(centerX + 20, centerY, 8, 20, TFT_BLUE);  // Candle body
    canvas.fillCircle(centerX + 24, centerY - 5 - flameFlicker, 6, TFT_ORANGE);  // Flame
    canvas.fillCircle(centerX + 24, centerY - 7 - flameFlicker, 3, TFT_YELLOW);  // Inner flame
    
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(10, 10);
    canvas.print("BLOW THE CANDLE!");
    
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, 25);
    canvas.printf("Mic: %d", micLevel);
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("B:Back");
    
    if (millis() - stateStartTime < 1000) {
        return;  // Give player a moment before detecting
    }

    // Detect blowing (high mic level)
    if (micLevel > blowThreshold) {
        // Success! Candle blown out
        canvas.fillScreen(TFT_BLACK);
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(20, 30);
        canvas.print("WISH MADE!");
        canvas.setTextSize(1);
        canvas.pushSprite(0, 0);
        
        boyfriend.updateHappiness(4);
        M5.Speaker.tone(2000, 200);
        delay(2000);
        currentState = STATE_IDLE;
    }
    
    if (M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
    }
}