#include "../GameState.h"

/**
 * MINIGAME MENU STATE
 * 
 * Controls: 
 * - Button A: Select current option
 * - Button B: Cycle through options
 * - Long press B: Quick exit to IDLE
 * 
 * Menu options: Catch, Jump, Fortune, Dance, Shake, Candle, Love, Exit
 */

void handleMinigameMenu() {
    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(20, 10);
    canvas.print("MENU");
    canvas.setTextSize(1);
    
    // Show warning if sick (but still allow playing)
    if (!boyfriend.canPlay()) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(80, 12);
        canvas.print("SICK!");
    }
    
    // Display options based on scroll position
    const char* menuOptions[] = {
        "CATCH", "JUMP", "FORTUNE", 
        "DANCE", "SHAKE", "CANDLE", "LOVE", "EXIT"
    };
    int numOptions = 8;
    
    // Show 3 options at a time
    for (int i = 0; i < 3 && (minigameSelection - 1 + i) < numOptions; i++) {
        int optionIndex = minigameSelection - 1 + i;
        if (optionIndex < 0) optionIndex = 0;
        
        canvas.setTextColor(i == 1 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
        canvas.setCursor(20, 35 + (i * 15));
        canvas.print(i == 1 ? "> " : "  ");
        canvas.print(menuOptions[optionIndex]);
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A:Select B:Next");
    
    if (M5.BtnA.wasPressed()) {
        // Select current option
        stateStartTime = millis();
        switch(minigameSelection) {
            case 0:  // Catch
                currentState = STATE_MINIGAME_CATCH;
                objectX = random(20, 120);
                objectY = 10;
                playerX = 60;
                gameScore = 0;
                gameActive = true;
                break;
            case 1:  // Jump
                currentState = STATE_MINIGAME_JUMP;
                objectX = 0;
                playerX = 30;
                gameScore = 0;
                gameActive = true;
                break;
            case 2:  // Fortune
                currentState = STATE_FORTUNE_COOKIE;
                fortuneIndex = random(0, 8);
                break;
            case 3:  // Dance
                currentState = STATE_DANCE_MUSIC;
                break;
            case 4:  // Shake Clean
                currentState = STATE_SHAKE_CLEAN;
                roomMess = 20;  // Start with messy room
                shakeCount = 0;
                break;
            case 5:  // Blow Candle
                currentState = STATE_BLOW_CANDLE;
                break;
            case 6:  // Love Meter
                currentState = STATE_LOVE_METER;
                heartLevel = 0;
                buttonPressCount = 0;
                loveMeterStartTime = millis();
                break;
            case 7:  // Exit
                currentState = STATE_IDLE;
                minigameSelection = 0;
                M5.Speaker.tone(800, 50);
                break;
        }
        M5.Speaker.tone(1600, 50);  // Exciting sound for starting game
    } else if (M5.BtnB.wasPressed()) {
        // Cycle through options
        minigameSelection++;
        if (minigameSelection >= numOptions) minigameSelection = 0;
        M5.Speaker.tone(1100, 30);  // Menu navigation beep
    }
    
    // Long press B to go back (alternative quick exit)
    if (M5.BtnB.pressedFor(1000)) {
        currentState = STATE_IDLE;
        minigameSelection = 0;
        M5.Speaker.tone(800, 50);
    }
}
