#include "../include/gbmem.h"
#include "../include/graphics.h"


static Uint8 tilemaps[4][256][256]; // Stores palette indices
static Uint8 objectmap[176][176];


typedef struct object {
    Uint8 x, y, index, attributes;
} object_t;

int comparator(const void *a, const void *b)
{
    return (((object_t *) a)->x < ((object_t *) b)->x) ? 1 : -1;
}


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

    // Fill tilemap for objects
    map_address = 0xFE00;
    // object_t objects[40];
    //
    // for (int i = 0; i < 40; i++) {
    //     objects[i].y = memory[map_address++];
    //     objects[i].x = memory[map_address++];
    //     objects[i].index = memory[map_address++];
    //     objects[i].attributes = memory[map_address++];
    // }
    //
    // qsort(&objects, 40, sizeof(object_t), &comparator);
    int sprite_count[176] = {0};
    const int w = 8, h = (memory[LCDC] & 4) ? 16 : 8;
    memset(objectmap, 0, sizeof(objectmap));

    for (int i = 0; i < 40; i++) {
        const Uint8 y = memory[map_address++];
        const Uint8 x = memory[map_address++];
        const Uint8 index = memory[map_address++];
        const Uint8 att = memory[map_address++];
        // printf("%d %d %d %d\n", y, x, index, att);

        if (y >= 160) continue;
        if (x >= 168) continue;

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
            for (l = 0; l < w && objectmap[ty][tx] != 0; l++) tx++;
            for (; l < w; l++, tx++, k += mx)
                objectmap[ty][tx] = (att & 0x90) | (((byte1 << k) & 0x80) >> 7) | (((byte2 << k) & 0x80) >> 6);
            sprite_count[ty]++;
        }
    }
    // printf("\n\n");
}


void render_frame(const SDL_Surface *surface, line_log_t scroll_logs[GB_SCREEN_HEIGHT])
{
    for (int i = 0; i < GB_SCREEN_HEIGHT; i++) {
        const Uint8 lcdc = scroll_logs[i].lcdc;
        const Uint8 ly = i;

        const Uint8 bgp = scroll_logs[i].bgp;
        const Uint8 bg_palette[] = {
            (~bgp & 0x03) * 85,
            ((~bgp >> 2) & 0x03) * 85,
            ((~bgp >> 4) & 0x03) * 85,
            ((~bgp >> 6) & 0x03) * 85
        };

        const Uint8 obp0 = scroll_logs[i].obp0;
        const Uint8 obp1 = scroll_logs[i].obp1;
        const Uint8 ob_palette[2][4] = {
            {
                (~obp0 & 0x03) * 85,
                ((~obp0 >> 2) & 0x03) * 85,
                ((~obp0 >> 4) & 0x03) * 85,
                ((~obp0 >> 6) & 0x03) * 85
            },
            {
                (~obp1 & 0x03) * 85,
                ((~obp1 >> 2) & 0x03) * 85,
                ((~obp1 >> 4) & 0x03) * 85,
                ((~obp1 >> 6) & 0x03) * 85
            }
        };

        if ((lcdc & 0x80) == 0)
            return;

        // Render background
        if (lcdc & 0x01) {
            const Uint8 by = scroll_logs[i].scy + ly;
            Uint8 bx = scroll_logs[i].scx;
            const Uint8 wy = scroll_logs[i].wy;
            Uint8 wx = scroll_logs[i].wx;
            const Uint8 oy = 16 + ly;
            Uint8 ox = 8;
            const Uint8 k1 = (lcdc >> 3) & 0x03;
            const Uint8 k2 = ((lcdc >> 3) & 0x02) | ((lcdc >> 6) & 0x01);

            Uint8 *ptr = surface->pixels;
            ptr += surface->format->BytesPerPixel * GB_SCREEN_WIDTH * ly;

            for (size_t j = 0; j < GB_SCREEN_WIDTH; j++, bx++, wx++, ox++)
            {
                Uint8 value;
                const Uint8 object_pixel = objectmap[oy][ox];
                const Uint8 background_pixel = tilemaps[k1][by][bx];
                if ((object_pixel & 0x03) != 0 && ((object_pixel & 0x80) == 0 || background_pixel == 0))
                    value = ob_palette[ (object_pixel & 0x10) >> 4 ][ object_pixel & 0x03 ];
                else
                    value = bg_palette[ tilemaps[k1][by][bx] ];
                memset(ptr, SDL_MapRGB(surface->format, value, value, value), surface->format->BytesPerPixel);
                ptr += surface->format->BytesPerPixel;
            }
        }

        // Render Window

        // Render Objects
    }
}