#ifndef GBPPU_H
#define GBPPU_H

#include <inttypes.h>

#define CYCLES_PER_LINE 114
#define VERTICAL_LINES 154


// Bitmap Header Types
typedef struct
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1, reserved2;
    uint32_t offset;
} bmp_header_t;

typedef struct
{
    uint32_t size, width, height;
    uint16_t planes, bits_per_pixel;
    uint32_t compression, image_size, x_resolution, y_resolution, colors_used, colors_important;
} bmp_info_header_t;


#endif //GBPPU_H
