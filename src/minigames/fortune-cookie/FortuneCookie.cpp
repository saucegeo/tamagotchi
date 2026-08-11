#include "FortuneCookie.h"
#include "FortuneMessages.h"
#include "app/GameContext.h"
#include "app/Game.h"



int fortuneIndex = 0;
void handleFortuneCookie() {
    static bool cookieCracked = false;
    static unsigned long crackTime = 0;

    int centerX = canvas.width() / 2;
    int centerY = canvas.height() / 2;
    unsigned long elapsed = millis() - stateStartTime;

    canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setCursor(10, 10);
    canvas.print("FORTUNE");
    canvas.setTextSize(1);

    if (!cookieCracked) {

        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 30);
        canvas.print("Press to crack!");


        int wobble = (millis() / 200) % 2;
        canvas.fillCircle(centerX, centerY + wobble, 12, TFT_ORANGE);
        canvas.fillCircle(centerX, centerY + wobble, 10, 0xFDA0);
        canvas.drawArc(centerX, centerY + wobble, 12, 10, 90, 270, TFT_BROWN);


        canvas.fillCircle(centerX - 4, centerY - 3 + wobble, 2, TFT_YELLOW);

        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        canvas.setCursor(10, canvas.height() - 10);
        canvas.print("Press any button");

        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
            cookieCracked = true;
            crackTime = millis();
            M5.Speaker.tone(1000, 80);
            delay(80);
            M5.Speaker.tone(1200, 80);
        }

    } else {
        unsigned long crackElapsed = millis() - crackTime;


        if (crackElapsed < 1000) {
            int separation = crackElapsed / 20;


            canvas.fillCircle(centerX - separation, centerY, 10, TFT_ORANGE);
            canvas.fillCircle(centerX + separation, centerY, 10, TFT_ORANGE);


            canvas.drawLine(centerX - separation + 8, centerY - 5, centerX - separation + 10, centerY + 5, TFT_BROWN);
            canvas.drawLine(centerX + separation - 8, centerY - 5, centerX + separation - 10, centerY + 5, TFT_BROWN);


            for (int i = 0; i < 5; i++) {
                int crumbY = centerY + (crackElapsed / 30) + (i * 3);
                canvas.fillCircle(centerX - 5 + i * 2, crumbY, 1, TFT_BROWN);
            }

        } else {

            canvas.setTextColor(TFT_CYAN, TFT_BLACK);
            canvas.setCursor(10, 30);


            const char* fortune = fortuneMessages[fortuneIndex];
            int lineY = 35;
            int lineWidth = 0;
            int wordStart = 0;

            for (int i = 0; i <= strlen(fortune); i++) {
                if (fortune[i] == ' ' || fortune[i] == '\0') {

                    char word[20];
                    strncpy(word, fortune + wordStart, i - wordStart);
                    word[i - wordStart] = '\0';

                    canvas.setCursor(10 + lineWidth * 6, lineY);
                    canvas.print(word);
                    canvas.print(" ");

                    lineWidth += strlen(word) + 1;


                    if (lineWidth > 17) {
                        lineY += 12;
                        lineWidth = 0;
                    }

                    wordStart = i + 1;
                }
            }


            if ((crackElapsed / 300) % 2 == 0) {
                canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
                canvas.setCursor(5, 25);
                canvas.print("*");
                canvas.setCursor(canvas.width() - 10, 28);
                canvas.print("*");
                canvas.setCursor(12, 65);
                canvas.print("*");
            }

            canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
            canvas.setCursor(10, canvas.height() - 10);
            canvas.print("Press any button");

            if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
                pet.updateHappiness(1);
                currentState = STATE_IDLE;
                cookieCracked = false;
                M5.Speaker.tone(1500, 50);
            }
        }
    }
}