#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

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

int main() {
    printf("%d %d", sizeof(bmp_header_t), sizeof(bmp_info_header_t));
    FILE *f = fopen("output.bmp", "wb");
    if (!f) {
        printf("Failed to open file!\n");
        return -1;
    }

    uint16_t type = 0x4D42;
    bmp_header_t header = { 54 + 3*4*4, 0, 0, 54 };
    bmp_info_header_t info_header = { 40, 4, 4, 1, 24, 0, 3*4*4, 2835, 2835, 0, 0 };

    fwrite(&type, sizeof(type), 1, f);
    fwrite(&header, sizeof(header), 1, f);
    fwrite(&info_header, sizeof(info_header), 1, f);

    
    for (int i = 0; i < 4*4; i++) {
        unsigned char pixel[3] = { 15 * i, 15 * i, 15 * i };  //BGR
        fwrite(pixel, sizeof(pixel), 1, f);
    }

    fclose(f);
    return 0;
}
