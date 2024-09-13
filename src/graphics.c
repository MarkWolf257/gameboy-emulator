#include "../include/memory.h"
#include "../include/graphics.h"


/* Stores palette indices for tiles
 * There are 4 tilemaps for different tilemap and tiledata
 * 00 - 9800, 8000
 * 01 - 9C00, 8000
 * 02 - 9800, 8800
 * 03 - 9C00, 8800 */
static Uint8 tilemaps[4][256][256];

// Stores palette indices for sprites
static Uint8 objectmap[176][176];

// Sprite datatype
typedef struct object {
    Uint8 x, y, index, attributes;
} object_t;


void read_vram()
{
    // Fill tilemaps for window and background
    size_t map_address = 0x9800;

    for (int k = 0; k < 2; k++)
        for (int tx = 0; tx < 32; tx++)
            for (int ty = 0; ty < 32; ty++)
            {
                Uint8 index = memory[map_address++];
                Uint16 address1 = 0x8000 + 16 * index;
                // Exploiting overflow
                index += 128;
                Uint16 address2 = 0x8800 + 16 * index;

                for (int y = 0; y < 8; y++)
                {
                    const Uint8 byte1 = memory[address1++];
                    const Uint8 byte2 = memory[address1++];
                    const Uint8 byte3 = memory[address2++];
                    const Uint8 byte4 = memory[address2++];

                    for (int x = 0; x < 8; x++)
                    {
                        tilemaps[ 2 + k ][ tx * 8 + y ][ ty * 8 + x ] =
                            (((byte1 << x) & 0x80) >> 7) | (((byte2 << x) & 0x80) >> 6);
                        tilemaps[ 0 + k ][ tx * 8 + y ][ ty * 8 + x ] =
                            (((byte3 << x) & 0x80) >> 7) | (((byte4 << x) & 0x80) >> 6);
                    }
                }
            }

    // Fill objectmap for sprites
    map_address = 0xFE00;
    int sprite_count[176] = {0};    // Count sprites in line for limit of 10
    const int w = 8, h = (memory[LCDC] & 4) ? 16 : 8;
    memset(objectmap, 0, sizeof(objectmap));

    for (int i = 0; i < 40; i++) {
        const Uint8 y = memory[map_address++];
        const Uint8 x = memory[map_address++];
        const Uint8 index = memory[map_address++];
        const Uint8 att = memory[map_address++];

        if (y >= 160) continue;
        if (x >= 168) {
            sprite_count[y]++;
            continue;
        }

        // These variables help in x-flip and y-flip
        Uint16 address = 0x8000 + index * 16 + ((att & 0x40) ? (h - 1) : 0);
        const int my = (att & 0x40) ? -1 : 1;
        const int mx = (att & 0x20) ? -1 : 1;
        const int mi = (att & 0x20) ? 7 : 0;

        int ty = y;
        for (int j = 0; j < h; j++, ty++) {
            const Uint8 byte1 = memory[address];
            address += my;
            const Uint8 byte2 = memory[address];
            address += my;

            if (sprite_count[ty] == 10) continue;

            int k = mi, tx = x, l;
            // Skip overlapping pixels due to x priority
            for (l = 0; l < w && objectmap[ty][tx] != 0; l++) tx++;
            // Put the rest except 0s
            for (; l < w; l++, tx++, k += mx) {
                const Uint8 pixel = (att & 0x90) | (((byte1 << k) & 0x80) >> 7) | (((byte2 << k) & 0x80) >> 6);
                if (pixel)
                    objectmap[ty][tx] = pixel;
            }
            sprite_count[ty]++;
        }
    }
}


void render_frame(const SDL_Surface *surface, line_log_t line_logs[GB_SCREEN_HEIGHT])
{
    for (int i = 0; i < GB_SCREEN_HEIGHT; i++) {
        const Uint8 lcdc = line_logs[i].lcdc;

        if ((lcdc & 0x80) == 0)
            continue;

        const Uint8 ly = i;

        // Gameboy greens!
        const Uint8 colors[4][3] = {
            {0xe0, 0xf8, 0xd0},
            {0x88, 0xc0, 0x70},
            {0x34, 0x68, 0x56},
            {0x08, 0x18, 0x20},
        };

        const Uint8 bgp = line_logs[i].bgp;
        const Uint8 bg_palette[] = {
            bgp & 0x03,
            (bgp >> 2) & 0x03,
            (bgp >> 4) & 0x03,
            (bgp >> 6) & 0x03
        };

        const Uint8 obp0 = line_logs[i].obp0;
        const Uint8 obp1 = line_logs[i].obp1;
        const Uint8 ob_palette[2][4] = {
            {
                obp0 & 0x03,
                (obp0 >> 2) & 0x03,
                (obp0 >> 4) & 0x03,
                (obp0 >> 6) & 0x03
            },
            {
                obp1 & 0x03,
                (obp1 >> 2) & 0x03,
                (obp1 >> 4) & 0x03,
                (obp1 >> 6) & 0x03
            }
        };


        const Uint8 by = line_logs[i].scy + ly;
        Uint8 bx = line_logs[i].scx;
        const Uint8 wy = line_logs[i].wy;
        Uint8 wx = line_logs[i].wx - 7;
        const Uint8 oy = 16 + ly;
        Uint8 ox = 8;
        const Uint8 k1 = (lcdc >> 3) & 0x03;
        const Uint8 k2 = ((lcdc >> 3) & 0x02) | ((lcdc >> 6) & 0x01);

        Uint8 *ptr = surface->pixels;
        ptr += surface->format->BytesPerPixel * GB_SCREEN_WIDTH * ly;

        for (size_t j = 0; j < GB_SCREEN_WIDTH; j++, bx++, ox++)
        {
            Uint8 value;
            const Uint8 object_pixel = (lcdc & 0x02) ? objectmap[oy][ox] : 0;
            const Uint8 background_pixel = (lcdc & 0x01) ? tilemaps[k1][by][bx] : 0;
            const Uint8 window_pixel = ((lcdc & 0x21) == 0x21) ? tilemaps[k2][ly - wy][j - wx] : 0;
            Uint8 bgw_pixel = 0;

            // Decide background/window priority
            if ((lcdc & 0x21) == 0x21 && wy <= ly && wx <= j)
                bgw_pixel = window_pixel;
            else if (lcdc & 0x01)
                bgw_pixel = background_pixel;

            // Decide pixel priority
            if ((object_pixel & 0x03) && ((object_pixel & 0x80) == 0 || bgw_pixel == 0))
                value = ob_palette[ (object_pixel & 0x10) >> 4 ][ object_pixel & 0x03 ];
            else if (bgw_pixel && (object_pixel & 0x80 || (object_pixel & 0x03) == 0))
                value = bg_palette[ bgw_pixel ];
            else
                value = 0;

            // Modify surface
            const Uint32 pixel = SDL_MapRGB(surface->format, colors[value][0], colors[value][1], colors[value][2]);
            memcpy(ptr, &pixel, surface->format->BytesPerPixel);
            ptr += surface->format->BytesPerPixel;
        }
    }
}