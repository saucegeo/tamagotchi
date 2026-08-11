#include "Widgets.h"
#include "app/GameContext.h"


void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, uint16_t color) {
    canvas.drawRect(x, y, width, height, TFT_WHITE);
    int fillWidth = (value * (width - 2)) / maxValue;
    if (fillWidth > 0) {
        canvas.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }
}