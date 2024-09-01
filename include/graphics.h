#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144

typedef struct line_log {
    Uint8 lcdc, scy, scx, bgp, obp0, obp1, wy, wx;
} line_log_t;

void read_vram();
void render_frame(const SDL_Surface *surface, line_log_t scroll_logs[GB_SCREEN_HEIGHT]);

#endif //GRAPHICS_H
