#include "../GameState.h"

 /**
  * MINIGAME: Blow Candle
  * 
  * Objective: Blow out the candle by making noise into the mic
  * Controls: Blow into the mic, Button B to return
  * Scoring: +4 happiness for blowing out the candle
  */

void handleBlowCandle() {
    const int BLOW_THRESHOLD = 500;
    static int blowProgress = 0;
    static bool candleBlownOut = false;
    static unsigned long blowOutTime = 0;
    
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    unsigned long elapsed = millis() - stateStartTime;
    
    // Instructions phase
    if (elapsed < 2000) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(25, 20);
        canvas.print("BLOW!");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Blow out candle!");
        
        // Draw candle preview
        int flicker = (millis() / 100) % 3;
        canvas.fillRect(centerX, centerY + 10, 6, 15, TFT_CYAN);
        canvas.fillCircle(centerX + 3, centerY + 7 - flicker, 4, TFT_ORANGE);
        canvas.fillCircle(centerX + 3, centerY + 5 - flicker, 2, TFT_YELLOW);
        return;
    }
    
    // Candle blown out - celebration animation
    if (candleBlownOut) {
        unsigned long celebTime = millis() - blowOutTime;
        
        // Smoke particles rising
        for (int i = 0; i < 5; i++) {
            int smokeY = centerY - ((celebTime / 50) % 30) - (i * 8);
            canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
            canvas.setCursor(centerX + 20 + (i % 3) * 3, smokeY);
            canvas.print("o");
        }
        
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(15, 30);
        canvas.print("WISH MADE!");
        canvas.setTextSize(1);
        
        // Sparkles
        if (celebTime < 2000) {
            int sparkle = (celebTime / 100) % 4;
            canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
            canvas.setCursor(10 + sparkle * 30, 50 - (sparkle * 5));
            canvas.print("*");
        }
        
        if (celebTime > 2500) {
            boyfriend.updateHappiness(4);
            currentState = STATE_IDLE;
        }
        return;
    }
    
    // Draw character watching candle
    drawCharacter(centerX - 25, centerY + 10);
    
    // Draw candle with flame intensity based on blow progress
    int flameSize = 6 - (blowProgress / 5);
    if (flameSize < 1) flameSize = 1;
    int flameFlicker = (millis() / (100 + blowProgress * 5)) % 3;
    
    canvas.fillRect(centerX + 20, centerY + 5, 6, 15, TFT_CYAN);
    
    if (flameSize > 1) {
        // Flame gets smaller as you blow
        canvas.fillCircle(centerX + 23, centerY + 2 - flameFlicker, flameSize, TFT_ORANGE);
        canvas.fillCircle(centerX + 23, centerY - flameFlicker, flameSize - 2, TFT_YELLOW);
        
        // Wind effect lines when blowing
        if (micLevel > BLOW_THRESHOLD) {
            canvas.setTextColor(TFT_CYAN, TFT_BLACK);
            for (int i = 0; i < 3; i++) {
                canvas.drawLine(centerX - 10, centerY + i * 3, centerX + 15, centerY + i * 3 - 2, TFT_CYAN);
            }
        }
    }
    
    // Title
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 5);
    canvas.print("BLOW CANDLE");
    
    // Mic level bar (visual feedback)
    int barWidth = (micLevel * 100) / 1000;
    if (barWidth > 100) barWidth = 100;
    canvas.fillRect(10, 20, barWidth, 5, micLevel > BLOW_THRESHOLD ? TFT_GREEN : TFT_ORANGE);
    canvas.drawRect(10, 20, 100, 5, TFT_WHITE);
    
    // Blow progress indicator
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, 30);
    canvas.printf("Progress: %d%%", (blowProgress * 100) / 25);
    
    // Detect blowing
    if (elapsed > 1000 && micLevel > BLOW_THRESHOLD) {
        blowProgress++;
        if (blowProgress % 3 == 0) {
            M5.Speaker.tone(800 + (blowProgress * 20), 30);
        }
        
        // Success!
        if (blowProgress >= 25) {
            candleBlownOut = true;
            blowOutTime = millis();
            M5.Speaker.tone(2000, 200);
        }
    } else {
        // Progress decays slowly if not blowing
        if (blowProgress > 0 && millis() % 100 == 0) {
            blowProgress--;
        }
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("B:Back");
    
    if (M5.BtnB.wasPressed()) {
        currentState = STATE_IDLE;
    }
}