#define GENERATE_LOGS

#include "../include/gbmem.h"
#include "../include/process.h"

#include <stdio.h>


const int GB_SCREEN_WIDTH = 160;
const int GB_SCREEN_HEIGHT = 144;
const int CYCLES_PER_LINE = 114;
const int GB_VIRTUAL_HEIGHT = 154;

const int LCDC = 0xFF40;
const int SCY = 0xFF42;
const int SCX = 0xFF43;


static void execute_instructions(int);
static inline void render_frame(const SDL_Surface *);
static inline void read_tilemaps();


static Uint8 tilemaps[4][256][256]; // Stores palette indices
static int cycle_count = 0;

#ifdef GENERATE_LOGS
FILE *log_file;
#endif //GENERATE_LOGS


void init_process()
{
#ifdef GENERATE_LOGS

    log_file = fopen("execution_log.txt", "w");
    if (log_file == NULL) {
        printf("Error! Writing to log file");
        exit(EXIT_FAILURE);
    }

#endif // GENERATE_LOGS
}


void process_and_render_frame(const SDL_Surface *surface)
{
    memory[LY] = 0;
    for (size_t i = 0; i < GB_VIRTUAL_HEIGHT; i++) {
        execute_instructions(CYCLES_PER_LINE);

        if (i < GB_SCREEN_HEIGHT)
            render_frame(surface);

        memory[LY]++;
        cycle_count %= CYCLES_PER_LINE;

        if (i == 0x90)
            memory[0xff0f] |= 0x01;
    }

    read_tilemaps();
}


static inline void
read_tilemaps()
{
    size_t map_address = 0x9800;

    for (int k = 0; k < 2; k++)
        for (int i = 0; i < 32; i++)
            for (int j = 0; j < 32; j++)
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
                        tilemaps[ 0 + k ][ i * 8 + y ][ j * 8 + x ] =
                            (((byte1 << x) & 0x80) >> 7) | (((byte2 << x) & 0x80) >> 6);
                        tilemaps[ 2 + k ][ i * 8 + y ][ j * 8 + x ] =
                            (((byte3 << x) & 0x80) >> 7) | (((byte4 << x) & 0x80) >> 6);
                    }

                }
            }
}


static inline void
render_frame(const SDL_Surface *surface)
{
    const Uint8 lcdc = memory[LCDC];
    const Uint8 ly = memory[LY];
    const Uint8 palette[] = { 255, 170, 85, 0 };

    if ((lcdc & 0x80) == 0)
        return;

    if (lcdc & 0x01) {
        const Uint8 y = memory[SCY] + ly;
        Uint8 x = memory[SCX];
        const Uint8 k = (lcdc >> 2) & 0x03;

        Uint8 *ptr = surface->pixels;
        ptr += surface->format->BytesPerPixel * GB_SCREEN_WIDTH * ly;

        for (size_t i = 0; i < GB_SCREEN_WIDTH; i++, x++)
        {
            const Uint8 value = palette[ tilemaps[k][y][x] ];
            memset(ptr, SDL_MapRGB(surface->format, value, value, value), surface->format->BytesPerPixel);
            ptr += surface->format->BytesPerPixel;
        }
    }
}





static Uint8
    a = 0x01,
    f = 0xB0,
    b = 0x00,
    c = 0x13,
    d = 0x00,
    e = 0xD8,
    h = 0x01,
    l = 0x4D;
static Uint16
    pc = 0x0100,
    sp = 0xFFFE;
static Uint8
    zf = 1,
    nf = 0,
    hf = 1,
    cf = 1;
static Uint8 ext_ime = 0;


#include "../include/iblk0.h"
#include "../include/iblk1.h"
#include "../include/iblk2.h"
#include "../include/iblk3.h"
#include "../include/iblkcb.h"


static Uint8 int_ime = 0;


static inline void
execute_instructions(const int cycles_to_execute)
{
    while (cycle_count < cycles_to_execute) {
        // Call interrupt if enabled
        // Two variables simulate the enable interrupt delay
        for (int i = 0; ext_ime && int_ime && i < 5; i++)
            if ((memory[0xffff] & (1 << i)) && (memory[0xff0f] & (1 << i)))
            {
                memory[sp--] = pc >> 8;
                memory[sp--] = pc;
                pc = 0x0040 + i * 8;
                cycle_count += 5;

#ifdef GENERATE_LOGS
                fprintf(log_file, "\nINTERRUPT\t\t\t\tie:\t%02X\tif:\t%02X\n", memory[0xffff], memory[0xff0f]);
#endif // GENERATE_LOGS

                ext_ime = 0;
                memory[0xff0f] &= ~(1 << i);
            }
        int_ime = ext_ime;


#ifdef GENERATE_LOGS
        fprintf(log_file, "%04X:\t", pc);
#endif // GENERATE_LOGS

        Uint8 opcode = get_ro_mem(pc);

        char name[8];
        Uint8 n8, hi, lo;
        Uint16 n16;

        switch (opcode)
        {
            //
            // BLOCK 0
            //
            case 0x00: // nop
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "nop\n");
#endif // GENERATE_LOGS
                break;

            case 0x01: ld_r16_n16(&b, &c, "bc"); break;
            case 0x11: ld_r16_n16(&d, &e, "de"); break;
            case 0x21: ld_r16_n16(&h, &l, "hl"); break;
            case 0x31:
                ld_r16_n16(&hi, &lo, "sp");
                sp = hi << 8 | lo;
                break;

            case 0x02: ld_mr16_a(b, c, "bc"); break;
            case 0x12: ld_mr16_a(d, e, "de"); break;
            case 0x22: ld_mr16_a(h, l, "hl+"); l += 1; h += !l; break;
            case 0x32: ld_mr16_a(h, l, "hl-"); h -= !l; l -= 1; break;

            case 0x0A: ld_a_mr16(b, c, "bc"); break;
            case 0x1A: ld_a_mr16(d, e, "de"); break;
            case 0x2A: ld_a_mr16(h, l, "hl+"); l += 1; h += !l; break;
            case 0x3A: ld_a_mr16(h, l, "hl-"); h -= !l; l -= 1; break;

            // case 0x08:  fprintf("Unimplemented Opcode 0x08");  exit(1);

            case 0x03: inc_r16(&b, &c, "bc"); break;
            case 0x13: inc_r16(&d, &e, "de"); break;
            case 0x23: inc_r16(&h, &l, "hl"); break;
            case 0x33:
                hi = sp >> 8;
                lo = sp;
                inc_r16(&hi, &lo, "sp");
                sp = hi << 8 | lo;
                break;

            case 0x0B: dec_r16(&b, &c, "bc"); break;
            case 0x1B: dec_r16(&d, &e, "de"); break;
            case 0x2B: dec_r16(&h, &l, "hl"); break;
            case 0x3B:
                hi = sp >> 8;
                lo = sp;
                dec_r16(&hi, &lo, "sp");
                sp = hi << 8 | lo;
                break;

            case 0x09: add_hl_r16(b, c, "bc"); break;
            case 0x19: add_hl_r16(d, e, "de"); break;
            case 0x29: add_hl_r16(h, l, "hl"); break;
            case 0x39: add_hl_r16(sp >> 8, sp, "sp"); break;

            case 0x04: inc_r8(&b, "b"); break;
            case 0x0C: inc_r8(&c, "c"); break;
            case 0x14: inc_r8(&d, "d"); break;
            case 0x1C: inc_r8(&e, "e"); break;
            case 0x24: inc_r8(&h, "h"); break;
            case 0x2C: inc_r8(&l, "l"); break;
            case 0x34: // inc [hl]
                inc_r8(get_rw_memptr(h << 8 | l), "[hl]");
                cycle_count += 2;
                break;
            case 0x3C: inc_r8(&a, "a"); break;

            case 0x05: dec_r8(&b, "b"); break;
            case 0x0D: dec_r8(&c, "c"); break;
            case 0x15: dec_r8(&d, "d"); break;
            case 0x1D: dec_r8(&e, "e"); break;
            case 0x25: dec_r8(&h, "h"); break;
            case 0x2D: dec_r8(&l, "l"); break;
            case 0x35: // dec [hl]
                dec_r8(get_rw_memptr(h << 8 | l), "[hl]");
                cycle_count += 2;
                break;
            case 0x3D: dec_r8(&a, "a"); break;

            case 0x06: ld_r8_n8(&b, "b"); break;
            case 0x0E: ld_r8_n8(&c, "c"); break;
            case 0x16: ld_r8_n8(&d, "d"); break;
            case 0x1E: ld_r8_n8(&e, "e"); break;
            case 0x26: ld_r8_n8(&h, "h"); break;
            case 0x2E: ld_r8_n8(&l, "l"); break;
            case 0x36: // ld [hl], n8
                ld_r8_n8(get_rw_memptr(h << 8 | l), "[hl]");
                cycle_count += 1;
                break;
            case 0x3E:  ld_r8_n8(&a, "a");  break;

            case 0x2F:  // cpl
                a = ~a;
                nf = 1; hf = 1;
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "cpl\t\t");
                fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
#endif // GENERATE_LOGS
                break;

            // case 0x3F:  // ccf
            //   nf = 0; hf = 0; cf = !cf;
            //   cycle_count += 1;
            //   #ifdef log_file
            //   fprintf(log_file, "ccf\t\t");
            //   fprintf(log_file, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
            //   #endif
            //   break;

            case 0x18: jr_cc_n8(1, ""); break;
            case 0x20: jr_cc_n8(!zf, "nz"); break;
            case 0x28: jr_cc_n8(zf, "z"); break;
            // case 0x30: jr_cc_n8(!cf, "nc"); break;
            // case 0x38: jr_cc_n8(cf, "c"); break;

            // case 0x10: fprintf("stop instruction not implemented");  exit(1);



            //
            // BLOCK 1
            //
            case 0x40: ld_r8_r8(&b, "b", b, "b"); break;
            case 0x41: ld_r8_r8(&b, "b", c, "c"); break;
            case 0x42: ld_r8_r8(&b, "b", d, "d"); break;
            case 0x43: ld_r8_r8(&b, "b", e, "e"); break;
            case 0x44: ld_r8_r8(&b, "b", h, "h"); break;
            case 0x45: ld_r8_r8(&b, "b", l, "l"); break;
            case 0x46: ld_r8_mhl(&b, "b"); break;
            case 0x47: ld_r8_r8(&b, "b", a, "a"); break;

            case 0x48: ld_r8_r8(&c, "c", b, "b"); break;
            case 0x49: ld_r8_r8(&c, "c", c, "c"); break;
            case 0x4A: ld_r8_r8(&c, "c", d, "d"); break;
            case 0x4B: ld_r8_r8(&c, "c", e, "e"); break;
            case 0x4C: ld_r8_r8(&c, "c", h, "h"); break;
            case 0x4D: ld_r8_r8(&c, "c", l, "l"); break;
            case 0x4E: ld_r8_mhl(&c, "c"); break;
            case 0x4F: ld_r8_r8(&c, "c", a, "a"); break;

            case 0x50: ld_r8_r8(&d, "d", b, "b"); break;
            case 0x51: ld_r8_r8(&d, "d", c, "c"); break;
            case 0x52: ld_r8_r8(&d, "d", d, "d"); break;
            case 0x53: ld_r8_r8(&d, "d", e, "e"); break;
            case 0x54: ld_r8_r8(&d, "d", h, "h"); break;
            case 0x55: ld_r8_r8(&d, "d", l, "l"); break;
            case 0x56: ld_r8_mhl(&d, "d"); break;
            case 0x57: ld_r8_r8(&d, "d", a, "a"); break;

            case 0x58: ld_r8_r8(&e, "e", b, "b"); break;
            case 0x59: ld_r8_r8(&e, "e", c, "c"); break;
            case 0x5A: ld_r8_r8(&e, "e", d, "d"); break;
            case 0x5B: ld_r8_r8(&e, "e", e, "e"); break;
            case 0x5C: ld_r8_r8(&e, "e", h, "h"); break;
            case 0x5D: ld_r8_r8(&e, "e", l, "l"); break;
            case 0x5E: ld_r8_mhl(&e, "e"); break;
            case 0x5F: ld_r8_r8(&e, "e", a, "a"); break;

            case 0x60: ld_r8_r8(&h, "h", b, "b"); break;
            case 0x61: ld_r8_r8(&h, "h", c, "c"); break;
            case 0x62: ld_r8_r8(&h, "h", d, "d"); break;
            case 0x63: ld_r8_r8(&h, "h", e, "e"); break;
            case 0x64: ld_r8_r8(&h, "h", h, "h"); break;
            case 0x65: ld_r8_r8(&h, "h", l, "l"); break;
            case 0x66: ld_r8_mhl(&h, "h"); break;
            case 0x67: ld_r8_r8(&h, "h", a, "a"); break;

            case 0x68: ld_r8_r8(&l, "l", b, "b"); break;
            case 0x69: ld_r8_r8(&l, "l", c, "c"); break;
            case 0x6A: ld_r8_r8(&l, "l", d, "d"); break;
            case 0x6B: ld_r8_r8(&l, "l", e, "e"); break;
            case 0x6C: ld_r8_r8(&l, "l", h, "h"); break;
            case 0x6D: ld_r8_r8(&l, "l", l, "l"); break;
            case 0x6E: ld_r8_mhl(&l, "l"); break;
            case 0x6F: ld_r8_r8(&l, "l", a, "a"); break;

            case 0x70: ld_mhl_r8(b, "b"); break;
            case 0x71: ld_mhl_r8(c, "c"); break;
            case 0x72: ld_mhl_r8(d, "d"); break;
            case 0x73: ld_mhl_r8(e, "e"); break;
            case 0x74: ld_mhl_r8(h, "h"); break;
            case 0x75: ld_mhl_r8(l, "l"); break;
            case 0x77: ld_mhl_r8(a, "a"); break;

            case 0x78: ld_r8_r8(&a, "a", b, "b"); break;
            case 0x79: ld_r8_r8(&a, "a", c, "c"); break;
            case 0x7A: ld_r8_r8(&a, "a", d, "d"); break;
            case 0x7B: ld_r8_r8(&a, "a", e, "e"); break;
            case 0x7C: ld_r8_r8(&a, "a", h, "h"); break;
            case 0x7D: ld_r8_r8(&a, "a", l, "l"); break;
            case 0x7E: ld_r8_mhl(&a, "a"); break;
            case 0x7F: ld_r8_r8(&a, "a", a, "a"); break;

            // case 0x76:  fprintf("halt instruction not implemented");  exit(1);



            //
            // BLOCK 2
            //
            // case 0x80:  add_a_r8(b, "b", 0); break;
            // case 0x81:  add_a_r8(c, "c", 0); break;
            // case 0x82:  add_a_r8(d, "d", 0); break;
            // case 0x83:  add_a_r8(e, "e", 0); break;
            // case 0x84:  add_a_r8(h, "h", 0); break;
            case 0x85:  add_a_r8(l, "l", 0); break;
            // case 0x86:
            //   add_a_r8(get_memory(hl.reg16), "[hl]", 0);
            //   cycle_count += 1;
            //   break;
            case 0x87:  add_a_r8(a, "a", 0); break;

            // case 0x88:  add_a_r8(b, "b", 1); break;
            // case 0x89:  add_a_r8(c, "c", 1); break;
            // case 0x8A:  add_a_r8(d, "d", 1); break;
            // case 0x8B:  add_a_r8(e, "e", 1); break;
            // case 0x8C:  add_a_r8(h, "h", 1); break;
            // case 0x8D:  add_a_r8(l, "l", 1); break;
            // case 0x8E:
            //   add_a_r8(get_memory(hl.reg16), "[hl]", 1);
            //   cycle_count += 1;
            //   break;
            // case 0x8F:  add_a_r8(a, "a", 1); break;

            // case 0x90:  sub_a_r8(b, "b", 0); break;
            // case 0x91:  sub_a_r8(c, "c", 0); break;
            // case 0x92:  sub_a_r8(d, "d", 0); break;
            // case 0x93:  sub_a_r8(e, "e", 0); break;
            // case 0x94:  sub_a_r8(h, "h", 0); break;
            // case 0x95:  sub_a_r8(l, "l", 0); break;
            // case 0x96:
            //   sub_a_r8(get_memory(hl.reg16), "[hl]", 0);
            //   cycle_count += 1;
            //   break;
            // case 0x97:  sub_a_r8(a, "a", 0); break;

            // case 0x98:  sub_a_r8(b, "b", 1); break;
            // case 0x99:  sub_a_r8(c, "c", 1); break;
            // case 0x9A:  sub_a_r8(d, "d", 1); break;
            // case 0x9B:  sub_a_r8(e, "e", 1); break;
            // case 0x9C:  sub_a_r8(h, "h", 1); break;
            // case 0x9D:  sub_a_r8(l, "l", 1); break;
            // case 0x9E:
            //   sub_a_r8(get_memory(hl.reg16), "[hl]", 1);
            //   cycle_count += 1;
            //   break;
            // case 0x9F:  sub_a_r8(a, "a", 1); break;

            // case 0xA0:  and_a_r8(b, "b"); break;
            case 0xA1:  and_a_r8(c, "c"); break;
            // case 0xA2:  and_a_r8(d, "d"); break;
            // case 0xA3:  and_a_r8(e, "e"); break;
            // case 0xA4:  and_a_r8(h, "h"); break;
            // case 0xA5:  and_a_r8(l, "l"); break;
            // case 0xA6:
            //   and_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            case 0xA7:  and_a_r8(a, "a"); break;

            // case 0xA8:  xor_a_r8(b, "b"); break;
            case 0xA9:  xor_a_r8(c, "c"); break;
            // case 0xAA:  xor_a_r8(d, "d"); break;
            // case 0xAB:  xor_a_r8(e, "e"); break;
            // case 0xAC:  xor_a_r8(h, "h"); break;
            // case 0xAD:  xor_a_r8(l, "l"); break;
            // case 0xAE:
            //   xor_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            case 0xAF:  xor_a_r8(a, "a"); break;

            case 0xB0:  or_a_r8(b, "b"); break;
            case 0xB1:  or_a_r8(c, "c"); break;
            // case 0xB2:  or_a_r8(d, "d"); break;
            // case 0xB3:  or_a_r8(e, "e"); break;
            // case 0xB4:  or_a_r8(h, "h"); break;
            // case 0xB5:  or_a_r8(l, "l"); break;
            // case 0xB6:
            //   or_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            // case 0xB7:  or_a_r8(a, "a"); break;

            // case 0xB8:  cp_a_r8(b, "b"); break;
            // case 0xB9:  cp_a_r8(c, "c"); break;
            // case 0xBA:  cp_a_r8(d, "d"); break;
            // case 0xBB:  cp_a_r8(e, "e"); break;
            // case 0xBC:  cp_a_r8(h, "h"); break;
            // case 0xBD:  cp_a_r8(l, "l"); break;
            // case 0xBE:
            //   cp_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            // case 0xBF:  cp_a_r8(a, "a"); break;



            //
            // BLOCK 3
            //
            case 0xC6: // add a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                add_a_r8(n8, name, 0);
                cycle_count += 1;
                break;
            case 0xCE: // adc a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                add_a_r8(n8, name, 1);
                cycle_count += 1;
                break;
            case 0xD6: // sub a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                sub_a_r8(n8, name, 0);
                cycle_count += 1;
                break;
            case 0xDE: // sbc a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                sub_a_r8(n8, name, 1);
                cycle_count += 1;
                break;
            case 0xE6: // and a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                and_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xEE: // xor a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                xor_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xF6: // or a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                or_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xFE: // cp a, n8
                n8 = get_ro_mem(++pc);
                sprintf(name, "%02X", n8);
                cp_a_r8(n8, name);
                cycle_count += 1;
                break;

            case 0xC0:  ret_cc(!zf, "nz");  break;
            case 0xC8:  ret_cc(zf, "z");  break;
            // case 0xD0:  ret_cc(!cf, "nc");  break;
            // case 0xD8:  ret_cc(cf, "c");  break;
            case 0xC9:  // ret
                ret_cc(1, "");
                cycle_count -= 1;
                break;
            case 0xD9:  // reti
                ext_ime = 1;
                ret_cc(1, "i");
                cycle_count -= 1;
                break;

            case 0xC2:  jp_cc_n16(!zf, "nz"); break;
            case 0xCA:  jp_cc_n16(zf, "z"); break;
            // case 0xD2:  jp_cc_n16(!cf, "nc"); break;
            // case 0xDA:  jp_cc_n16(cf, "c"); break;
            case 0xC3:  jp_cc_n16(1, "");  break;
            case 0xE9:  // jp hl
                pc = (h << 8 | l) - 1;
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "jp\thl\n");
#endif // GENERATE_LOGS
                break;

            // case 0xC4:  call_cc_n16(!zf, "nz"); break;
            // case 0xCC:  call_cc_n16(zf, "z"); break;
            // case 0xD4:  call_cc_n16(!cf, "nc"); break;
            // case 0xDC:  call_cc_n16(cf, "c"); break;
            case 0xCD:  call_cc_n16(1, "");  break;

            // case 0xC7:  rst(0x00);  break;
            // case 0xCF:  rst(0x08);  break;
            // case 0xD7:  rst(0x10);  break;
            // case 0xDF:  rst(0x18);  break;
            // case 0xE7:  rst(0x20);  break;
            case 0xEF:  rst(0x28);  break;
            // case 0xF7:  rst(0x30);  break;
            // case 0xFF:  rst(0x38);  break;

            case 0xC1:  pop_r16(&b, &c, "bc"); break;
            case 0xD1:  pop_r16(&d, &e, "de"); break;
            case 0xE1:  pop_r16(&h, &l, "hl"); break;
            case 0xF1:
                pop_r16(&a, &f, "af");
                zf = (f & 0x80) >> 7;
                nf = (f & 0x40) >> 6;
                hf = (f & 0x20) >> 5;
                cf = (f & 0x10) >> 4;
                break;

            case 0xC5:  push_r16(b, c, "bc"); break;
            case 0xD5:  push_r16(d, e, "de"); break;
            case 0xE5:  push_r16(h, l, "hl"); break;
            case 0xF5:
                f = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
                push_r16(a, f, "af");
                break;

            case 0xE2:  // ldh [c], a
                n16 = 0xff00 | c;
                memory[n16] = a;
                cycle_count += 2;
#ifdef GENERATE_LOGS
                fprintf(log_file, "ldh\t[c]\ta");
                fprintf(log_file, "\t\tc:\t%02X\t[c]:\t%02X\n", c, memory[n16]);
#endif // GENERATE_LOGS
                break;

            case 0xE0:  // ldh [n8], a
                n16 = 0xff00 | memory[++pc];
                memory[n16] = a;
                cycle_count += 3;
#ifdef GENERATE_LOGS
                fprintf(log_file, "ldh\t[%04X],\ta", n16);
                fprintf(log_file, "\t\t[%04X]:\t%02X\n", n16, memory[n16]);
#endif // GENERATE_LOGS
                break;

            case 0xEA:  // ld [n16], a
                n16 = get_ro_mem(pc + 2) << 8 | get_ro_mem(pc + 1);
                pc += 2;
                sprintf(name, "[%04X]", n16);
                ld_r8_r8(get_rw_memptr(n16), name, a, "a");
                cycle_count += 3;
                break;

            case 0xF0:  // ldh a, [n8]
                n16 = 0xff00 | memory[++pc];
                a = memory[n16];
                cycle_count += 3;
#ifdef GENERATE_LOGS
                fprintf(log_file, "ldh\ta,\t[%04X]", n16);
                fprintf(log_file, "\t\ta:\t%02X\n", a);
#endif // GENERATE_LOGS
                break;

            case 0xFA:  // ld a, [n16]
                n16 = get_ro_mem(pc + 2) << 8 | get_ro_mem(pc + 1);
                pc += 2;
                sprintf(name, "[%04X]", n16);
                ld_r8_r8(&a, "a", get_ro_mem(n16), name);
                cycle_count += 3;
                break;

            case 0xF3:  // di
                ext_ime = 0;
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "di\n");
#endif // GENERATE_LOGS
                break;

            case 0xFB:  // ei
                ext_ime = 1;
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "ei\n");
#endif // GENERATE_LOGS
                break;



            //
            // BLOCK CB
            //
            case 0xCB:
                pc++;
                opcode = memory[pc];
                switch (opcode) {
                    case 0x27:  sla_r8(&a, "a");  break;
                    case 0x37:  swap_r8(&a, "a"); break;

                    case 0x50: case 0x58: case 0x60: case 0x68:
                    case 0x70: case 0x78: case 0x40: case 0x48:
                        bit_bitn_r8((opcode >> 3) & 0x7, b, "b");
                        break;
                    case 0x51: case 0x59: case 0x61: case 0x69:
                    case 0x71: case 0x79: case 0x41: case 0x49:
                        bit_bitn_r8((opcode >> 3) & 0x7, c, "c");
                        break;
                    case 0x52: case 0x5A: case 0x62: case 0x6A:
                    case 0x72: case 0x7A: case 0x42: case 0x4A:
                        bit_bitn_r8((opcode >> 3) & 0x7, d, "d");
                        break;
                    case 0x53: case 0x5B: case 0x63: case 0x6B:
                    case 0x73: case 0x7B: case 0x43: case 0x4B:
                        bit_bitn_r8((opcode >> 3) & 0x7, e, "e");
                        break;
                    case 0x54: case 0x5C: case 0x64: case 0x6C:
                    case 0x74: case 0x7C: case 0x44: case 0x4C:
                        bit_bitn_r8((opcode >> 3) & 0x7, h, "h");
                        break;
                    case 0x55: case 0x5D: case 0x65: case 0x6D:
                    case 0x75: case 0x7D: case 0x45: case 0x4D:
                        bit_bitn_r8((opcode >> 3) & 0x7, l, "l");
                        break;
                    case 0x56: case 0x5E: case 0x66: case 0x6E:
                    case 0x76: case 0x7E: case 0x46: case 0x4E:
                        bit_bitn_r8((opcode >> 3) & 0x7, get_ro_mem(h << 8 | l), "[hl]");
                        cycle_count += 1;
                        break;
                    case 0x57: case 0x5F: case 0x67: case 0x6F:
                    case 0x77: case 0x7F: case 0x47: case 0x4F:
                        bit_bitn_r8((opcode >> 3) & 0x7, b, "a");
                        break;

                    case 0x86:
                        res_bitn_r8(0, get_rw_memptr(h << 8 | l), "[hl]");
                        cycle_count += 2;
                        break;
                    case 0x87:  res_bitn_r8(0, &a, "a");  break;

                    case 0xCF:  set_bitn_r8(1, &a, "a");  break;

                    default:
#ifdef GENERATE_LOGS
                        fprintf(log_file, "Unknown CB prefix instruction");
#endif // GENERATE_LOGS
                        // void dump_vram();
                        // dump_vram();
                        return;
                }
                break;


            default:
#ifdef GENERATE_LOGS
                fprintf(log_file, "Unimplemented opcode\n");
#endif // GENERATE_LOGS
                return;
        }

        // Handle Joypad Register
        if ((memory[0xff00] & 0x30) == 0x20)
            memory[0xff00] = 0xEF;
        else if ((memory[0xff00] & 0x30) == 0x10)
            memory[0xff00] = 0xDF;


        pc++;
    }
}