#include "../GameState.h"

    /**
    * MINIGAME: Dance Music
    * 
    * Objective: Make the boyfriend dance to music
    * Controls: None - just watch him dance! Button A or B to exit
    * Scoring: +3 happiness, -2 energy for dancing
    * 
    * Physics: Simple bouncing and rotating animation, simple RTTTL music playback
    */
   
// Simple RTTTL player - this is a basic template
void handleDanceMusic() {
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Animated dancing character
    int bounceOffset = (millis() - stateStartTime) % 400 < 200 ? -8 : 8;
    int rotateOffset = (millis() - stateStartTime) % 800 < 400 ? -5 : 5;
    
    canvas.fillCircle(centerX + rotateOffset, centerY + bounceOffset, 20, TFT_WHITE);
    canvas.fillCircle(centerX - 6, centerY + bounceOffset - 5, 3, TFT_BLACK);
    canvas.fillCircle(centerX + 6, centerY + bounceOffset - 5, 3, TFT_BLACK);
    canvas.drawArc(centerX, centerY + bounceOffset + 5, 8, 6, 180, 360, TFT_BLACK);
    
    // Music notes animation
    int noteY = centerY - ((millis() - stateStartTime) / 100 % 30);
    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(centerX - 30, noteY);
    canvas.print("♪");
    canvas.setCursor(centerX + 25, noteY - 10);
    canvas.print("♫");
    canvas.setTextSize(1);
    
    // Play a simple tone (real RTTTL would require a parser)
    // This is just a template - you'll implement the full RTTTL parser
    static unsigned long lastTone = 0;
    if (millis() - lastTone > 300) {
        int notes[] = {880, 1047, 1109, 1175, 1047, 1175};  // A5, C6, C#6, D6...
        int noteIndex = ((millis() - stateStartTime) / 300) % 6;
        M5.Speaker.tone(notes[noteIndex], 200);
        lastTone = millis();
    }
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(20, 20);
    canvas.print("DANCING!");
    
    // End after 10 seconds or button press
    if (millis() - stateStartTime > 10000 || M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        boyfriend.updateHappiness(3);
        boyfriend.updateEnergy(-2);
        currentState = STATE_IDLE;
    }
}