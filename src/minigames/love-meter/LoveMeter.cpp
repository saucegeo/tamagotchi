#include "app/Game.h"

int heartLevel = 0;
int buttonPressCount = 0;
unsigned long loveMeterStartTime = 0;

void handleLoveMeter() {
    static bool showInstructions = true;
    static int particles[10] = {0};

    int centerX = canvas.width() / 2;
    int centerY = canvas.height() / 2;
    unsigned long elapsed = millis() - loveMeterStartTime;
    unsigned long timeLeft = 10000 - elapsed;


    if (elapsed < 2000 && showInstructions) {
        canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(10, 20);
        canvas.print("LOVE METER");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Mash A+B fast!");


        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(3);
        canvas.setCursor(centerX - 10, centerY + 5);
        canvas.printf("%d", 3 - (elapsed / 1000));
        canvas.setTextSize(1);
        return;
    }
    showInstructions = false;

    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(5, 5);
    canvas.print("LOVE");
    canvas.setTextSize(1);


    int barColor = TFT_RED;
    if (heartLevel > 70) barColor = TFT_MAGENTA;
    else if (heartLevel > 40) barColor = TFT_PINK;

    drawProgressBar(10, 25, 110, 15, heartLevel, 100, barColor);


    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setTextSize(1);
    canvas.setCursor(48, 30);
    canvas.printf("%d%%", heartLevel);


    static int pumpAnim = 0;
    if (M5.BtnA.isPressed() || M5.BtnB.isPressed()) {
        pumpAnim = 8;
    } else if (pumpAnim > 0) {
        pumpAnim--;
    }
    int pumpOffset = pumpAnim > 0 ? -pumpAnim / 2 : 0;

    drawCharacter(centerX + 30, centerY + 15 + pumpOffset, heartLevel > 80);


    for (int i = 0; i < 10; i++) {
        if (particles[i] > 0) {
            canvas.setTextColor(TFT_RED, TFT_BLACK);
            canvas.setCursor(10 + (i * 12), canvas.height() - particles[i]);
            canvas.print("\x03");
            particles[i] += 2;
            if (particles[i] > canvas.height()) particles[i] = 0;
        }
    }


    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setCursor(10, 50);
    if (heartLevel < 30) {
        canvas.print("Keep going!");
    } else if (heartLevel < 70) {
        canvas.print("Great!");
    } else if (heartLevel < 100) {
        canvas.print("Amazing!!");
    } else {
        canvas.print("MAX LOVE!");
    }


    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        buttonPressCount++;
        heartLevel += 2;
        if (heartLevel > 100) heartLevel = 100;


        for (int i = 0; i < 10; i++) {
            if (particles[i] == 0) {
                particles[i] = 1;
                break;
            }
        }


        M5.Speaker.tone(800 + (heartLevel * 10), 25);
    }


    if (timeLeft < 3000) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
    } else {
        canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    }
    canvas.setCursor(10, 65);
    canvas.printf("Time: %ld.%ld", timeLeft / 1000, (timeLeft % 1000) / 100);

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.printf("Taps: %d", buttonPressCount);


    if (elapsed > 10000) {
        currentState = STATE_MINIGAME_RESULT;
        gameScore = heartLevel / 10;
        stateStartTime = millis();
        pet.updateHappiness(heartLevel / 20);


        for (int i = 0; i < 10; i++) particles[i] = 0;
    }
}
