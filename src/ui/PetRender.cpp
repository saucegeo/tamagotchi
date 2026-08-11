#include "PetRender.h"
#include "app/GameContext.h"
#include <M5Unified.h>

int animationFrame = NULL;


void drawEgg(int x, int y) {

    canvas.fillEllipse(x, y, 18, 22, TFT_WHITE);
    canvas.drawEllipse(x, y, 18, 22, TFT_DARKGREY);


    canvas.fillCircle(x - 5, y - 8, 3, TFT_LIGHTGREY);
    canvas.fillCircle(x + 7, y - 3, 4, TFT_LIGHTGREY);
    canvas.fillCircle(x - 3, y + 8, 2, TFT_LIGHTGREY);
}

void drawPoop(int x, int y) {
    canvas.fillCircle(x, y + 3, 5, TFT_BROWN);
    canvas.fillCircle(x - 4, y + 2, 4, TFT_BROWN);
    canvas.fillCircle(x + 4, y + 2, 4, TFT_BROWN);
    canvas.fillCircle(x, y - 2, 4, TFT_BROWN);
    canvas.drawCircle(x, y + 3, 5, TFT_DARKGREY);
}


void drawCharacter(int x, int y, bool heartEyes) {


    int breathOffset = 0;
    int floatOffset = 0;
    switch (animationFrame % 8) {
        case 0: case 7: breathOffset = 0; floatOffset = 0; break;
        case 1: case 6: breathOffset = 1; floatOffset = -1; break;
        case 2: case 5: breathOffset = 2; floatOffset = -2; break;
        case 3: case 4: breathOffset = 1; floatOffset = -1; break;
    }

    y += floatOffset;
    int radius = 20 + breathOffset;


    uint16_t bodyColor = TFT_WHITE;
    if (pet.isSulking) {
        bodyColor = TFT_LIGHTGREY;
    } else if (pet.happiness == 4) {

        bodyColor = 0xFFF8;
    }
    canvas.fillCircle(x, y, radius, bodyColor);
    canvas.drawCircle(x, y, radius, TFT_DARKGREY);


    int eyeY = y - 5;
    if (heartEyes) {

        canvas.setTextColor(TFT_RED, bodyColor);
        canvas.setTextSize(1);
        canvas.setCursor(x - 10, eyeY - 3);
        canvas.print("♥");
        canvas.setCursor(x + 4, eyeY - 3);
        canvas.print("♥");
    } else if (pet.isSulking) {

        canvas.fillCircle(x - 8, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 4, eyeY, 2, TFT_BLACK);
    } else if (pet.isSick || pet.hasToothache) {

        canvas.drawLine(x - 8, eyeY - 2, x - 4, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x - 4, eyeY - 2, x - 8, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY - 2, x + 8, eyeY + 2, TFT_BLACK);
        canvas.drawLine(x + 8, eyeY - 2, x + 4, eyeY + 2, TFT_BLACK);
    } else if (pet.happiness >= 4) {

        canvas.fillCircle(x - 6, eyeY, 4, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 4, TFT_BLACK);
        canvas.fillCircle(x - 6, eyeY - 1, 1, TFT_WHITE);
        canvas.fillCircle(x + 6, eyeY - 1, 1, TFT_WHITE);
    } else if (pet.happiness == 3) {

        canvas.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    } else if (pet.happiness == 2) {

        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
    } else if (pet.happiness == 1) {

        canvas.drawLine(x - 8, eyeY - 1, x - 4, eyeY + 1, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY + 1, x + 8, eyeY - 1, TFT_BLACK);
        canvas.fillCircle(x - 6, eyeY + 1, 1, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY + 1, 1, TFT_BLACK);
    } else {

        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
        canvas.drawLine(x - 6, eyeY + 3, x - 6, eyeY + 8, TFT_CYAN);
        canvas.drawLine(x + 6, eyeY + 3, x + 6, eyeY + 8, TFT_CYAN);
    }


    int mouthY = y + 6;
    if (pet.isSick || pet.hasToothache) {

        canvas.drawLine(x - 6, mouthY, x - 2, mouthY + 2, TFT_BLACK);
        canvas.drawLine(x - 2, mouthY + 2, x + 2, mouthY, TFT_BLACK);
        canvas.drawLine(x + 2, mouthY, x + 6, mouthY + 2, TFT_BLACK);
    } else if (pet.isSulking) {

        canvas.drawLine(x - 5, mouthY + 2, x + 5, mouthY + 2, TFT_BLACK);
    } else if (pet.happiness >= 3) {

        canvas.drawArc(x, mouthY - 3, 8, 6, 180, 360, TFT_BLACK);

        if (pet.happiness == 4) {
            canvas.drawArc(x, mouthY - 1, 5, 4, 180, 360, TFT_BLACK);
        }
    } else if (pet.happiness == 2) {

        canvas.drawArc(x, mouthY - 1, 5, 3, 180, 360, TFT_BLACK);
    } else if (pet.happiness == 1) {

        canvas.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);
    } else {

        canvas.drawArc(x, mouthY + 5, 6, 4, 0, 180, TFT_BLACK);
    }


    if (pet.happiness >= 3 && !pet.isSick) {
        canvas.fillCircle(x - 15, y + 3, 3, TFT_PINK);
        canvas.fillCircle(x + 15, y + 3, 3, TFT_PINK);
    }
}


void drawSkull(int x, int y) {

    canvas.fillCircle(x, y, 22, TFT_WHITE);
    canvas.drawCircle(x, y, 22, TFT_DARKGREY);


    canvas.fillCircle(x - 8, y - 5, 5, TFT_BLACK);
    canvas.fillCircle(x + 8, y - 5, 5, TFT_BLACK);


    canvas.fillTriangle(x - 2, y + 2, x + 2, y + 2, x, y + 6, TFT_BLACK);


    canvas.fillRect(x - 12, y + 8, 24, 8, TFT_WHITE);
    canvas.drawRect(x - 12, y + 8, 24, 8, TFT_DARKGREY);


    for (int i = 0; i < 4; i++) {
        canvas.drawLine(x - 8 + (i * 5), y + 8, x - 8 + (i * 5), y + 15, TFT_DARKGREY);
    }
}
