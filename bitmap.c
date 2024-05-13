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
    FILE *f = fopen("output.bmp", "wb");
    if (!f) {
        printf("Failed to open file!\n");
        return -1;
    }

    uint16_t type = 0x4D42;
    bmp_header_t header = { 54 + 3*8*800, 0, 0, 54 };
    bmp_info_header_t info_header = { 40, 8, 800, 1, 24, 0, 3*8*800, 2835, 2835, 0, 0 };

    fwrite(&type, sizeof(type), 1, f);
    fwrite(&header, sizeof(header), 1, f);
    fwrite(&info_header, sizeof(info_header), 1, f);


    FILE *rom = fopen("rom.gb", "rb");
    if (!rom) {
      printf("Unable to open rom!\n");
      return -1;
    }

    uint8_t data[800][2];
    fseek(rom, 0x64000, SEEK_SET);
    fread(&data, sizeof(data), 1, rom);
    fclose(rom);

    for (int i = 0; i < 8; i++) {
      printf("%02X %02X\n", data[i][0], data[i][1]);
    }

    for (int i = 799; i >= 0; i--) {
      for (int j = 7; j >= 0; j--) {
        uint8_t bit = ~(((data[i][0] >> j) & 1) | (((data[i][1] >> j) & 1) << 1)) & 3;
        uint8_t pixel[3] = { 85 * bit, 85 * bit, 85 * bit };
        fwrite(pixel, sizeof(pixel), 1, f);
      }
    }

    
    // for (int i = 0; i < 4*4; i++) {
    //     unsigned char pixel[3] = { 15 * i, 15 * i, 15 * i };  //BGR
    //     fwrite(pixel, sizeof(pixel), 1, f);
    // }

    fclose(f);
    return 0;
}
