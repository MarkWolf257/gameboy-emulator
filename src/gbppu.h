#ifndef GBPPU_H
#define GBPPU_H

#include <inttypes.h>

#define CYCLES_PER_LINE 114
#define VERTICAL_LINES 154


// Bitmap Header Types
typedef struct
{
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} bmp_header_t;

typedef struct
{
    uint32_t size;
    int32_t width, height;
    uint16_t planes, bits_per_pixel;
    uint32_t compression, image_size;
    int32_t x_resolution, y_resolution;
    uint32_t colors_used, colors_important;
} bmp_info_header_t;


// Graphic structs
typedef struct
{
    uint8_t blue, green, red;
} pixel;

typedef struct
{
    pixel pixels[8][8];
} tile;


#endif //GBPPU_H
