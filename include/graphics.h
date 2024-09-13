#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

// Constants for the SDL window
#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144
#define SCALING_FACTOR 2

// Structure for logging lines
typedef struct line_log {
    Uint8 lcdc, scy, scx, bgp, obp0, obp1, wy, wx;
} line_log_t;

// Graphics functions
void read_vram();
void render_frame(const SDL_Surface *surface, line_log_t line_logs[GB_SCREEN_HEIGHT]);

#endif //GRAPHICS_H
