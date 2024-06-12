#include "../include/gbmem.h"
#include "../include/gbppu.h"


const int CYCLES_PER_LINE = 114;
const int VERTICAL_LINES = 154;
const int GB_SCREEN_WIDTH = 160;
const int GB_SCREEN_HEIGHT = 144;
const int WIDTH_IN_TILES = GB_SCREEN_WIDTH / 8;
const int HEIGHT_IN_TILES = GB_SCREEN_HEIGHT / 8;


void init_ppu()
{
    // uint16_t address = 0x9800;
    // uint16_t offset = 0x8000;
    // for (int i = 0; i < 32; i++)
    //     for (int j = 0; j < 32; j++)
    //     {
    //         uint16_t address1 = offset + 16 * memory[address++];
    //
    //         for (int x = 0; x < 8; x++)
    //         {
    //             const uint8_t byte1 = memory[address1++];
    //             const uint8_t byte2 = memory[address1++];
    //             printf("%02X %02X ", byte1, byte2);
    //
    //             for (int y = 0; y < 8; y++)
    //             {
    //                 const uint8_t value = (~(((byte1 >> y) & 1) | (((byte2 >> y) & 1) << 1)) & 3) * 85;
    //                 tilemap[i][j].pixels[x][y].blue = value;
    //                 tilemap[i][j].pixels[x][y].green = value;
    //                 tilemap[i][j].pixels[x][y].red = value;
    //                 // fwrite(&tilemap[i][j].pixels[x][y], sizeof(pixel), 1, f);
    //             }
    //         }
    //     }
}


void gbppu_process(const SDL_Surface *surface)
{
    const Uint8 lcdc = memory[0xFF40];

    if ((lcdc & 0x80) == 0)
        return;

    const Uint8 ly = memory[0xFF44];
    const Uint16 tiledata_address = (lcdc & 0x10) ? 0x8000 : 0x8800;

    // Render background
    if ((lcdc & 0x01) && ly <= GB_SCREEN_HEIGHT) {
        const Uint8 scy = memory[0xFF42];
        const Uint8 scx = memory[0xFF43];
        const Uint8 line = ly + scy;
        const Uint16 row = ((lcdc & 0x08) ? 0x9C00 : 0x9800) + (line / 8) * 32;

        int bit = 7 - (scx % 8);
        Uint8 tile = scx / 8;
        Uint8 index = memory[row + tile] + ~(lcdc | ~0x10);
        Uint8 byte1 = memory[tiledata_address + index * 16 + (line % 8) * 2];
        Uint8 byte2 = memory[tiledata_address + index * 16 + (line % 8) * 2 + 1];
        Uint8 *ptr = surface->pixels;
        ptr += surface->format->BytesPerPixel * GB_SCREEN_WIDTH * ly;
        for (int i = 0; i < GB_SCREEN_WIDTH; i++)
        {
            if (bit < 0) {
                bit = 7;
                tile = (tile + 1) % 32;
                index = memory[row + tile] + ~(lcdc | ~0x10);
                byte1 = memory[tiledata_address + index * 16 + (line % 8) * 2];
                byte2 = memory[tiledata_address + index * 16 + (line % 8) * 2 + 1];
            }
            const Uint8 value = (~(((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1)) & 3) * 85;

            memset(ptr, SDL_MapRGB(surface->format, value, value, value), surface->format->BytesPerPixel);
            ptr += surface->format->BytesPerPixel;
            bit--;
        }
    }
}