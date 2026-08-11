#ifndef HUD_RENDER_H
#define HUD_RENDER_H

#include <cstdint>

void drawStatsBar();
void drawHearts(int x, int y, int filled, int total, uint16_t color);
void drawSicknessIndicators(int x, int y);

#endif
