#include "../GameState.h"
#include "../music/RTTTL.h"
#include "../music/songs.h"

/**
 * MINIGAME: Dance Music
 * 
 * Objective: Make the boyfriend dance to music
 * Controls: None - just watch him dance! Button A or B to exit
 * Scoring: +3 happiness, -2 energy for dancing
 * 
 * Physics: Simple bouncing and rotating animation, RTTTL music playback
 */

void handleDanceMusic() {
    static RTTTL rtttlPlayer;
    static Note currentNote = {0, 0};  // Initialize with zero frequency and duration
    static unsigned long noteStartTime = 0;
    static bool noteActive = false;
    static int currentSongIndex = -1;
    static bool initialized = false;
    
    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    
    // Initialize song on first entry
    if (!initialized) {
        currentSongIndex = random(0, DANCE_SONG_COUNT);
        rtttlPlayer.begin(DANCE_SONGS[currentSongIndex]);
        noteActive = false;
        noteStartTime = millis();
        initialized = true;
    }
    
    // Handle music playback
    unsigned long now = millis();
    if (!noteActive || (now - noteStartTime >= (unsigned long)currentNote.duration)) {
        // Get next note
        if (rtttlPlayer.getNextNote(currentNote)) {
            if (currentNote.frequency > 0) {
                M5.Speaker.tone(currentNote.frequency, currentNote.duration);
            } else {
                M5.Speaker.stop();
            }
            noteActive = true;
            noteStartTime = now;
        } else {
            // Song finished, restart it
            rtttlPlayer.reset();
            noteActive = false;
        }
    }
    
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
    
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(20, 20);
    canvas.print("DANCING!");
    
    // End after 15 seconds or button press
    if (millis() - stateStartTime > 15000 || M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        M5.Speaker.stop();
        initialized = false;  // Reset for next time
        boyfriend.updateHappiness(3);
        boyfriend.updateEnergy(-2);
        currentState = STATE_IDLE;
    }
}