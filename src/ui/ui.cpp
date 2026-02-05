#include <M5Unified.h>
#include "../ui/ui.h"

// implementation of the functions used throughout the project


// Function to draw a progress bar on the canvas
void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    canvas.drawRect(x, y, width, height, TFT_WHITE);
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        canvas.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}


// Function to draw the character at given position
void drawCharacter(int x, int y, bool heartEyes) {
    // Breathing animation
    int breathOffset = 0;
    switch (animationFrame % 4) {
        case 0: breathOffset = 0; break;
        case 1: breathOffset = 1; break;
        case 2: breathOffset = 2; break;
        case 3: breathOffset = 1; break;
    }
int radius = 20 + breathOffset;
    
    // Body (circle)
    canvas.fillCircle(x, y, radius, TFT_WHITE);
    
    // Eyes (expression based on happiness or heartEyes override)
    int eyeY = y - 5;
    if (heartEyes) {
        // Heart eyes! ♥
        canvas.setTextColor(TFT_RED, TFT_WHITE);
        canvas.setTextSize(1);
        canvas.setCursor(x - 8, eyeY - 3);
        canvas.print("♥");
        canvas.setCursor(x + 2, eyeY - 3);
        canvas.print("♥");
    } else if (boyfriend.happiness > 15) {
        // Happy eyes (arcs)
        canvas.drawLine(x - 8, eyeY, x - 4, eyeY - 2, TFT_BLACK);
        canvas.drawLine(x + 4, eyeY - 2, x + 8, eyeY, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Sad eyes
        canvas.fillCircle(x - 6, eyeY, 2, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 2, TFT_BLACK);
    } else {
        // Normal eyes
        canvas.fillCircle(x - 6, eyeY, 3, TFT_BLACK);
        canvas.fillCircle(x + 6, eyeY, 3, TFT_BLACK);
    }
    
    // Mouth
    int mouthY = y + 5;
    if (boyfriend.happiness > 15) {
        // Smile
        canvas.drawArc(x, mouthY - 2, 6, 4, 180, 360, TFT_BLACK);
    } else if (boyfriend.happiness < 5) {
        // Frown
        canvas.drawArc(x, mouthY + 4, 6, 4, 0, 180, TFT_BLACK);
    } else {
        // Neutral
        canvas.drawLine(x - 5, mouthY, x + 5, mouthY, TFT_BLACK);
    }
}


// Function to draw the stats bar on the canvas
void drawStats() {
  canvas.setTextSize(1);
    canvas.setTextColor(TFT_GREEN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Batt:%d%%", M5.Power.getBatteryLevel());
    
    drawProgressBar(5, 20, 80, 8, boyfriend.hunger, 24, (boyfriend.hunger > 18) ? TFT_RED : TFT_ORANGE);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(90, 22);
    canvas.printf("H:%d", boyfriend.hunger);
    
    drawProgressBar(5, 32, 80, 8, boyfriend.happiness, 24, TFT_YELLOW);
    canvas.setCursor(90, 34);
    canvas.printf("Hp:%d", boyfriend.happiness);
    
    drawProgressBar(5, 44, 80, 8, boyfriend.energy, 24, TFT_CYAN);
    canvas.setCursor(90, 46);
    canvas.printf("E:%d", boyfriend.energy);
    
    // Show status messages or special day banner
    if (currentSpecialDay >= 0) {
        // Special day banner!
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print(specialDays[currentSpecialDay].name);
    } else if (boyfriend.hunger > 18) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("HUNGRY!");
    } else if (boyfriend.happiness < 5) {
        canvas.setTextColor(TFT_BLUE, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("SAD :(");
    } else if (boyfriend.energy < 5) {
        canvas.setTextColor(TFT_PURPLE, TFT_BLACK);
        canvas.setCursor(5, 60);
        canvas.print("TIRED!");
    }
    
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Menu B:Feed");
}

