#ifndef GBPPU_H
#define GBPPU_H

#include <SDL2/SDL.h>


// Bitmap Header Types
typedef struct
{
    Uint32 size;
    Uint16 reserved1;
    Uint16 reserved2;
    Uint32 offset;
} bmp_header_t;

typedef struct
{
    Uint32 size;
    Sint32 width, height;
    Uint16 planes, bits_per_pixel;
    Uint32 compression, image_size;
    Sint32 x_resolution, y_resolution;
    Uint32 colors_used, colors_important;
} bmp_info_header_t;


// Graphic structs
typedef struct
{
    Uint8 blue, green, red;
} pixel;

typedef struct
{
    pixel pixels[8][8];
} tile;


void init_gbppu();
void gbppu_process(const SDL_Surface *);


extern const int CYCLES_PER_LINE, VERTICAL_LINES, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT, WIDTH_IN_TILES, HEIGHT_IN_TILES;


#endif //GBPPU_H
