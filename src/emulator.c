#include "../include/emulator.h"
#include "../include/gbmem.h"
#include "../include/gbcpu.h"
#include "../include/gbppu.h"

#include <stdlib.h>
#include <stdbool.h>



// Global Variables
unsigned int cycle_count;
uint8_t *memory;

#ifdef GENERATE_LOGS
FILE *log_file;
#endif //GENERATE_LOGS


void init_emulator()
{
    init_gbcpu();
    cycle_count = 0;

    // Load ROM into memory
  memory = malloc((GB_MEMORY_SIZE + 1) * sizeof(*memory));

    FILE *fptr = fopen("../roms/rom0.gb", "rb");
    if (fptr == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    if (fread(memory, 1, GB_BANK_SIZE * 2, fptr) != GB_BANK_SIZE * 2)
    {
        printf("Error! reading file");
        exit(1);
    }
    fclose(fptr);

#ifdef GENERATE_LOGS

    log_file = fopen("execution_log.txt", "w");
    if (log_file == NULL) {
        printf("Error! Writing to log file");
        exit(EXIT_FAILURE);
    }

#endif // GENERATE_LOGS
}


// void ppu_process()
// {
//   if (cycle_count >= CYCLES_PER_LINE) {
//     memory[0xff44] = (memory[0xff44] + 1) % VERTICAL_LINES;
//     cycle_count %= CYCLES_PER_LINE;
//
//     if (memory[0xff44] == 0x90)
//       memory[0xff0f] |= 0x01;
//   }
// }

// void dump_vram()
// {
//   // FILE *fptr = fopen("vram.txt", "w");
//   // for (int i = 0x8000; i < 0x9FFF; i++) {
//   //   fprintf(fptr, "%02X ", memory[i]);
//   // }
//   // fprintf(fptr, "\n");
//   // fclose(fptr);
//
//   FILE *f = fopen("output.bmp", "w");
//   if (f == NULL) {
//     printf("Failed to open file!\n");
//     exit(1);
//   }
//   const uint16_t type = 0x4D42;
//   const bmp_header_t header = { 54 + 3*8*8*32*32, 0, 0, 54 };
//   const bmp_info_header_t info_header = { 40, 8*32, 8*32, 1, 24, 0, 3*8*8*32*32, 2835, 2835, 0, 0 };
//
//   fwrite(&type, sizeof(type), 1, f);
//   fwrite(&header, sizeof(header), 1, f);
//   fwrite(&info_header, sizeof(info_header), 1, f);
//
//   tile tilemap[32][32];
//   uint16_t address = 0x9800;
//   uint16_t offset = 0x8000;
//   printf("%llu\n", sizeof(pixel));
//   for (int i = 0; i < 32; i++)
//     for (int j = 0; j < 32; j++)
//     {
//       printf("\n%02X\t", memory[address]);
//       uint16_t address1 = offset + 16 * memory[address++];
//       printf("%04X:\n", address1);
//
//       for (int x = 0; x < 8; x++)
//       {
//         const uint8_t byte1 = memory[address1++];
//         const uint8_t byte2 = memory[address1++];
//         printf("%02X %02X ", byte1, byte2);
//
//         for (int y = 0; y < 8; y++)
//         {
//           const uint8_t value = (~(((byte1 >> y) & 1) | (((byte2 >> y) & 1) << 1)) & 3) * 85;
//           tilemap[i][j].pixels[x][y].blue = value;
//           tilemap[i][j].pixels[x][y].green = value;
//           tilemap[i][j].pixels[x][y].red = value;
//           // fwrite(&tilemap[i][j].pixels[x][y], sizeof(pixel), 1, f);
//         }
//       }
//     }
//
//   pixel frame[256][256];
//   for (int i = 31; i >= 0; i--)
//     for (int x = 7; x >= 0; x--)
//       for (int j = 0; j < 32; j++)
//         for (int y = 7; y >= 0; y--)
//           fwrite(&tilemap[i][j].pixels[x][y], sizeof(pixel), 1, f);
//   fclose(f);
//
//   SDL_Window* window = NULL;
//   SDL_Surface* screenSurface = NULL;
//
//   if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
//     printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
//   else
//   {
//     window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_SHOWN );
//     if( window == NULL )
//       printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
//     else
//     {
//       screenSurface = SDL_GetWindowSurface( window );
//       Uint8 *ptr = screenSurface->pixels;
//       for (int i = 0; i < 32; i++)
//         for (int x = 0; x < 8; x++)
//           for (int j = 0; j < 32; j++)
//             for (int y = 7; y >= 0; y--) {
//               memset(ptr, SDL_MapRGB(screenSurface->format, tilemap[i][j].pixels[x][y].red, tilemap[i][j].pixels[x][y].green, tilemap[i][j].pixels[x][y].blue), screenSurface->format->BytesPerPixel);
//               ptr += screenSurface->format->BytesPerPixel;
//             }
//
//       SDL_UpdateWindowSurface( window );
//       SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
//     }
//   }
//
//   SDL_DestroyWindow( window );
//   SDL_Quit();
// }


int main(int argc, char *args[])
{
    init_emulator();


    SDL_Window *emulator_window = NULL;
    SDL_Surface *emulator_surface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_window = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_SHOWN);
    if (emulator_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_surface = SDL_GetWindowSurface(emulator_window);


    SDL_Event e;
    bool quit = false;

    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }
        gbcpu_process(CYCLES_PER_LINE);
        gbppu_process(emulator_surface);
        SDL_UpdateWindowSurface(emulator_window);

        memory[0xff44] = (memory[0xff44] + 1) % VERTICAL_LINES;
        cycle_count %= CYCLES_PER_LINE;

        if (memory[0xff44] == 0x90)
            memory[0xff0f] |= 0x01;
    }


    SDL_DestroyWindow(emulator_window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

//
//   uint8_t opcode, n8;
//   register16_t n16;
//   char name[8];
//
//
//   do {
//     // Run PPU in parallel if enabled
//     if (memory[0xff40] & 0x80) {
//       ppu();
//     }
//
//
//     // Handle Interrupts
//     if (ime0 == 0)
//       ime = 0;
//
//     if (ime)
//     {
//       for (int i = 0; i < 5; i++)
//       {
//         if ((memory[0xffff] & (1 << i)) && (memory[0xff0f] & (1 << i)))
//         {
//           #ifdef LOG_FILE
//           fprintf(LOG_FILE, "\nINTERRUPT\t\t\t\tie:\t%02X\tif:\t%02X\n", memory[0xffff], memory[0xff0f]);
//           #endif // LOG_FILE
//
//           memory[sp.reg16--] = pc.reg.hi;
//           memory[sp.reg16--] = pc.reg.lo;
//           pc.reg16 = 0x0040 + i * 8;
//           cycle_count += 5;
//
//           ime0 = 0;
//           memory[0xff0f] &= ~(1 << i);
//         }
//       }
//     }
//
//     if (ime0 == 1)
//       ime = 1;
//
//
//
//     // Execute Instructions
//     #ifdef LOG_FILE
//     fprintf(LOG_FILE, "%04X:\t", pc.reg16);
//     #endif // LOG_FILE
//
//     opcode = get_memory(pc.reg16);
//
//
//     switch (opcode)
//     {
//       //
//       // BLOCK 0
//       //
//       case 0x00:  //nop
//         cycle_count += 1;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "nop\n");
// #endif // LOG_FILE
//         break;
//
//       case 0x01:  ld_r16_n16(&bc, "bc");  break;
//       case 0x11:  ld_r16_n16(&de, "de");  break;
//       case 0x21:  ld_r16_n16(&hl, "hl");  break;
//       case 0x31:  ld_r16_n16(&sp, "sp");  break;
//
//       case 0x02:  ld_mr16_a(bc, "bc"); break;
//       case 0x12:  ld_mr16_a(de, "de"); break;
//       case 0x22:  ld_mr16_a(hl, "hl+");  hl.reg16++; break;
//       case 0x32:  ld_mr16_a(hl, "hl-");  hl.reg16--; break;
//
//       case 0x0A:  ld_a_mr16(bc, "bc"); break;
//       case 0x1A:  ld_a_mr16(de, "de"); break;
//       case 0x2A:  ld_a_mr16(hl, "hl+");  hl.reg16++; break;
//       case 0x3A:  ld_a_mr16(hl, "hl-");  hl.reg16--; break;
//
//       // case 0x08:  fprintf("Unimplemented Opcode 0x08");  exit(1);
//
//       case 0x03:  inc_r16(&bc, "bc"); break;
//       case 0x13:  inc_r16(&de, "de"); break;
//       case 0x23:  inc_r16(&hl, "hl"); break;
//       case 0x33:  inc_r16(&sp, "sp"); break;
//
//       case 0x0B:  dec_r16(&bc, "bc"); break;
//       case 0x1B:  dec_r16(&de, "de"); break;
//       case 0x2B:  dec_r16(&hl, "hl"); break;
//       case 0x3B:  dec_r16(&sp, "sp"); break;
//
//       case 0x09:  add_hl_r16(bc, "bc"); break;
//       case 0x19:  add_hl_r16(de, "de"); break;
//       case 0x29:  add_hl_r16(hl, "hl"); break;
//       case 0x39:  add_hl_r16(sp, "sp"); break;
//
//       case 0x04:  inc_r8(&(bc.reg.hi), "b");  break;
//       case 0x0C:  inc_r8(&(bc.reg.lo), "c");  break;
//       case 0x14:  inc_r8(&(de.reg.hi), "d");  break;
//       case 0x1C:  inc_r8(&(de.reg.lo), "e");  break;
//       case 0x24:  inc_r8(&(hl.reg.hi), "h");  break;
//       case 0x2C:  inc_r8(&(hl.reg.lo), "l");  break;
//       case 0x34:  // inc [hl]
//         inc_r8(get_setter_memory(hl.reg16), "hl");
//         cycle_count += 2;
//         break;
//       case 0x3C:  inc_r8(&(af.reg.hi), "a");  break;
//
//       case 0x05:  dec_r8(&(bc.reg.hi), "b");  break;
//       case 0x0D:  dec_r8(&(bc.reg.lo), "c");  break;
//       case 0x15:  dec_r8(&(de.reg.hi), "d");  break;
//       case 0x1D:  dec_r8(&(de.reg.lo), "e");  break;
//       case 0x25:  dec_r8(&(hl.reg.hi), "h");  break;
//       case 0x2D:  dec_r8(&(hl.reg.lo), "l");  break;
//       case 0x35:  // dec [hl]
//         dec_r8(get_setter_memory(hl.reg16), "hl");
//         cycle_count += 2;
//         break;
//       case 0x3D:  dec_r8(&(af.reg.hi), "a");  break;
//
//       case 0x06:  ld_r8_n8(&(bc.reg.hi), "b");  break;
//       case 0x0E:  ld_r8_n8(&(bc.reg.lo), "c");  break;
//       case 0x16:  ld_r8_n8(&(de.reg.hi), "d");  break;
//       case 0x1E:  ld_r8_n8(&(de.reg.lo), "e");  break;
//       case 0x26:  ld_r8_n8(&(hl.reg.hi), "h");  break;
//       case 0x2E:  ld_r8_n8(&(hl.reg.lo), "l");  break;
//       case 0x36:  // ld [hl], n8
//         set_memory(hl.reg16, get_memory(++pc.reg16));
//         cycle_count += 3;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ld\t[hl]\t%02X\t", get_memory(hl.reg16));
//         fprintf(LOG_FILE, "\t\thl:\t%04X\n", hl.reg16);
// #endif // LOG_FILE
//         break;
//       case 0x3E:  ld_r8_n8(&(af.reg.hi), "a");  break;
//
//       case 0x2F:  // cpl
//         af.reg.hi = ~af.reg.hi;
//         nf = 1; hf = 1;
//         cycle_count += 1;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "cpl\t\t");
//         fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
// #endif // LOG_FILE
//         break;
//
//       // case 0x3F:  // ccf
//       //   nf = 0; hf = 0; cf = !cf;
//       //   cycle_count += 1;
//       //   #ifdef LOG_FILE
//       //   fprintf(LOG_FILE, "ccf\t\t");
//       //   fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
//       //   #endif
//       //   break;
//
//       case 0x18:  jr_cc_n8(1, "");  break;
//       case 0x20:  jr_cc_n8(!zf, "nz"); break;
//       case 0x28:  jr_cc_n8(zf, "z"); break;
//       // case 0x30:  jr_cc_n8(!cf, "nc"); break;
//       // case 0x38:  jr_cc_n8(cf, "c"); break;
//
//       // case 0x10:  fprintf("stop instruction not implemented");  exit(1);
//
//
//
//       //
//       // BLOCK 1
//       //
//       case 0x40:  ld_r8_r8(&(bc.reg.hi), "b", bc.reg.hi, "b");  break;
//       case 0x41:  ld_r8_r8(&(bc.reg.hi), "b", bc.reg.lo, "c");  break;
//       case 0x42:  ld_r8_r8(&(bc.reg.hi), "b", de.reg.hi, "d");  break;
//       case 0x43:  ld_r8_r8(&(bc.reg.hi), "b", de.reg.lo, "e");  break;
//       case 0x44:  ld_r8_r8(&(bc.reg.hi), "b", hl.reg.hi, "h");  break;
//       case 0x45:  ld_r8_r8(&(bc.reg.hi), "b", hl.reg.lo, "l");  break;
//       case 0x46:  ld_r8_mhl(&(bc.reg.hi), "b");  break;
//       case 0x47:  ld_r8_r8(&(bc.reg.hi), "b", af.reg.hi, "a");  break;
//
//       case 0x48:  ld_r8_r8(&(bc.reg.lo), "c", bc.reg.hi, "b");  break;
//       case 0x49:  ld_r8_r8(&(bc.reg.lo), "c", bc.reg.lo, "c");  break;
//       case 0x4A:  ld_r8_r8(&(bc.reg.lo), "c", de.reg.hi, "d");  break;
//       case 0x4B:  ld_r8_r8(&(bc.reg.lo), "c", de.reg.lo, "e");  break;
//       case 0x4C:  ld_r8_r8(&(bc.reg.lo), "c", hl.reg.hi, "h");  break;
//       case 0x4D:  ld_r8_r8(&(bc.reg.lo), "c", hl.reg.lo, "l");  break;
//       case 0x4E:  ld_r8_mhl(&(bc.reg.lo), "c");  break;
//       case 0x4F:  ld_r8_r8(&(bc.reg.lo), "c", af.reg.hi, "a");  break;
//
//       case 0x50:  ld_r8_r8(&(de.reg.hi), "d", bc.reg.hi, "b");  break;
//       case 0x51:  ld_r8_r8(&(de.reg.hi), "d", bc.reg.lo, "c");  break;
//       case 0x52:  ld_r8_r8(&(de.reg.hi), "d", de.reg.hi, "d");  break;
//       case 0x53:  ld_r8_r8(&(de.reg.hi), "d", de.reg.lo, "e");  break;
//       case 0x54:  ld_r8_r8(&(de.reg.hi), "d", hl.reg.hi, "h");  break;
//       case 0x55:  ld_r8_r8(&(de.reg.hi), "d", hl.reg.lo, "l");  break;
//       case 0x56:  ld_r8_mhl(&(de.reg.hi), "d");  break;
//       case 0x57:  ld_r8_r8(&(de.reg.hi), "d", af.reg.hi, "a");  break;
//
//       case 0x58:  ld_r8_r8(&(de.reg.lo), "e", bc.reg.hi, "b");  break;
//       case 0x59:  ld_r8_r8(&(de.reg.lo), "e", bc.reg.lo, "c");  break;
//       case 0x5A:  ld_r8_r8(&(de.reg.lo), "e", de.reg.hi, "d");  break;
//       case 0x5B:  ld_r8_r8(&(de.reg.lo), "e", de.reg.lo, "e");  break;
//       case 0x5C:  ld_r8_r8(&(de.reg.lo), "e", hl.reg.hi, "h");  break;
//       case 0x5D:  ld_r8_r8(&(de.reg.lo), "e", hl.reg.lo, "l");  break;
//       case 0x5E:  ld_r8_mhl(&(de.reg.lo), "e");  break;
//       case 0x5F:  ld_r8_r8(&(de.reg.lo), "e", af.reg.hi, "a");  break;
//
//       case 0x60:  ld_r8_r8(&(hl.reg.hi), "h", bc.reg.hi, "b");  break;
//       case 0x61:  ld_r8_r8(&(hl.reg.hi), "h", bc.reg.lo, "c");  break;
//       case 0x62:  ld_r8_r8(&(hl.reg.hi), "h", de.reg.hi, "d");  break;
//       case 0x63:  ld_r8_r8(&(hl.reg.hi), "h", de.reg.lo, "e");  break;
//       case 0x64:  ld_r8_r8(&(hl.reg.hi), "h", hl.reg.hi, "h");  break;
//       case 0x65:  ld_r8_r8(&(hl.reg.hi), "h", hl.reg.lo, "l");  break;
//       case 0x66:  ld_r8_mhl(&(hl.reg.hi), "h");  break;
//       case 0x67:  ld_r8_r8(&(hl.reg.hi), "h", af.reg.hi, "a");  break;
//
//       case 0x68:  ld_r8_r8(&(hl.reg.lo), "l", bc.reg.hi, "b");  break;
//       case 0x69:  ld_r8_r8(&(hl.reg.lo), "l", bc.reg.lo, "c");  break;
//       case 0x6A:  ld_r8_r8(&(hl.reg.lo), "l", de.reg.hi, "d");  break;
//       case 0x6B:  ld_r8_r8(&(hl.reg.lo), "l", de.reg.lo, "e");  break;
//       case 0x6C:  ld_r8_r8(&(hl.reg.lo), "l", hl.reg.hi, "h");  break;
//       case 0x6D:  ld_r8_r8(&(hl.reg.lo), "l", hl.reg.lo, "l");  break;
//       case 0x6E:  ld_r8_mhl(&(hl.reg.lo), "l");  break;
//       case 0x6F:  ld_r8_r8(&(hl.reg.lo), "l", af.reg.hi, "a");  break;
//
//       case 0x70:  ld_mhl_r8(bc.reg.hi, "b");  break;
//       case 0x71:  ld_mhl_r8(bc.reg.lo, "c");  break;
//       case 0x72:  ld_mhl_r8(de.reg.hi, "d");  break;
//       case 0x73:  ld_mhl_r8(de.reg.lo, "e");  break;
//       case 0x74:  ld_mhl_r8(hl.reg.hi, "h");  break;
//       case 0x75:  ld_mhl_r8(hl.reg.lo, "l");  break;
//       case 0x77:  ld_mhl_r8(af.reg.hi, "a");  break;
//
//       case 0x78:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.hi, "b");  break;
//       case 0x79:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.lo, "c");  break;
//       case 0x7A:  ld_r8_r8(&(af.reg.hi), "a", de.reg.hi, "d");  break;
//       case 0x7B:  ld_r8_r8(&(af.reg.hi), "a", de.reg.lo, "e");  break;
//       case 0x7C:  ld_r8_r8(&(af.reg.hi), "a", hl.reg.hi, "h");  break;
//       case 0x7D:  ld_r8_r8(&(af.reg.hi), "a", hl.reg.lo, "l");  break;
//       case 0x7E:  ld_r8_mhl(&(af.reg.hi), "a");  break;
//       case 0x7F:  ld_r8_r8(&(af.reg.hi), "a", af.reg.hi, "a");  break;
//
//       // case 0x76:  fprintf("halt instruction not implemented");  exit(1);
//
//
//
//       //
//       // BLOCK 2
//       //
//       // case 0x80:  add_a_r8(bc.reg.hi, "b", 0); break;
//       // case 0x81:  add_a_r8(bc.reg.lo, "c", 0); break;
//       // case 0x82:  add_a_r8(de.reg.hi, "d", 0); break;
//       // case 0x83:  add_a_r8(de.reg.lo, "e", 0); break;
//       // case 0x84:  add_a_r8(hl.reg.hi, "h", 0); break;
//       case 0x85:  add_a_r8(hl.reg.lo, "l", 0); break;
//       // case 0x86:
//       //   add_a_r8(get_memory(hl.reg16), "[hl]", 0);
//       //   cycle_count += 1;
//       //   break;
//       case 0x87:  add_a_r8(af.reg.hi, "a", 0); break;
//
//       // case 0x88:  add_a_r8(bc.reg.hi, "b", 1); break;
//       // case 0x89:  add_a_r8(bc.reg.lo, "c", 1); break;
//       // case 0x8A:  add_a_r8(de.reg.hi, "d", 1); break;
//       // case 0x8B:  add_a_r8(de.reg.lo, "e", 1); break;
//       // case 0x8C:  add_a_r8(hl.reg.hi, "h", 1); break;
//       // case 0x8D:  add_a_r8(hl.reg.lo, "l", 1); break;
//       // case 0x8E:
//       //   add_a_r8(get_memory(hl.reg16), "[hl]", 1);
//       //   cycle_count += 1;
//       //   break;
//       // case 0x8F:  add_a_r8(af.reg.hi, "a", 1); break;
//
//       // case 0x90:  sub_a_r8(bc.reg.hi, "b", 0); break;
//       // case 0x91:  sub_a_r8(bc.reg.lo, "c", 0); break;
//       // case 0x92:  sub_a_r8(de.reg.hi, "d", 0); break;
//       // case 0x93:  sub_a_r8(de.reg.lo, "e", 0); break;
//       // case 0x94:  sub_a_r8(hl.reg.hi, "h", 0); break;
//       // case 0x95:  sub_a_r8(hl.reg.lo, "l", 0); break;
//       // case 0x96:
//       //   sub_a_r8(get_memory(hl.reg16), "[hl]", 0);
//       //   cycle_count += 1;
//       //   break;
//       // case 0x97:  sub_a_r8(af.reg.hi, "a", 0); break;
//
//       // case 0x98:  sub_a_r8(bc.reg.hi, "b", 1); break;
//       // case 0x99:  sub_a_r8(bc.reg.lo, "c", 1); break;
//       // case 0x9A:  sub_a_r8(de.reg.hi, "d", 1); break;
//       // case 0x9B:  sub_a_r8(de.reg.lo, "e", 1); break;
//       // case 0x9C:  sub_a_r8(hl.reg.hi, "h", 1); break;
//       // case 0x9D:  sub_a_r8(hl.reg.lo, "l", 1); break;
//       // case 0x9E:
//       //   sub_a_r8(get_memory(hl.reg16), "[hl]", 1);
//       //   cycle_count += 1;
//       //   break;
//       // case 0x9F:  sub_a_r8(af.reg.hi, "a", 1); break;
//
//       // case 0xA0:  and_a_r8(bc.reg.hi, "b"); break;
//       case 0xA1:  and_a_r8(bc.reg.lo, "c"); break;
//       // case 0xA2:  and_a_r8(de.reg.hi, "d"); break;
//       // case 0xA3:  and_a_r8(de.reg.lo, "e"); break;
//       // case 0xA4:  and_a_r8(hl.reg.hi, "h"); break;
//       // case 0xA5:  and_a_r8(hl.reg.lo, "l"); break;
//       // case 0xA6:
//       //   and_a_r8(get_memory(hl.reg16), "[hl]");
//       //   cycle_count += 1;
//       //   break;
//       case 0xA7:  and_a_r8(af.reg.hi, "a"); break;
//
//       // case 0xA8:  xor_a_r8(bc.reg.hi, "b"); break;
//       case 0xA9:  xor_a_r8(bc.reg.lo, "c"); break;
//       // case 0xAA:  xor_a_r8(de.reg.hi, "d"); break;
//       // case 0xAB:  xor_a_r8(de.reg.lo, "e"); break;
//       // case 0xAC:  xor_a_r8(hl.reg.hi, "h"); break;
//       // case 0xAD:  xor_a_r8(hl.reg.lo, "l"); break;
//       // case 0xAE:
//       //   xor_a_r8(get_memory(hl.reg16), "[hl]");
//       //   cycle_count += 1;
//       //   break;
//       case 0xAF:  xor_a_r8(af.reg.hi, "a"); break;
//
//       case 0xB0:  or_a_r8(bc.reg.hi, "b"); break;
//       case 0xB1:  or_a_r8(bc.reg.lo, "c"); break;
//       // case 0xB2:  or_a_r8(de.reg.hi, "d"); break;
//       // case 0xB3:  or_a_r8(de.reg.lo, "e"); break;
//       // case 0xB4:  or_a_r8(hl.reg.hi, "h"); break;
//       // case 0xB5:  or_a_r8(hl.reg.lo, "l"); break;
//       // case 0xB6:
//       //   or_a_r8(get_memory(hl.reg16), "[hl]");
//       //   cycle_count += 1;
//       //   break;
//       // case 0xB7:  or_a_r8(af.reg.hi, "a"); break;
//
//       // case 0xB8:  cp_a_r8(bc.reg.hi, "b"); break;
//       // case 0xB9:  cp_a_r8(bc.reg.lo, "c"); break;
//       // case 0xBA:  cp_a_r8(de.reg.hi, "d"); break;
//       // case 0xBB:  cp_a_r8(de.reg.lo, "e"); break;
//       // case 0xBC:  cp_a_r8(hl.reg.hi, "h"); break;
//       // case 0xBD:  cp_a_r8(hl.reg.lo, "l"); break;
//       // case 0xBE:
//       //   cp_a_r8(get_memory(hl.reg16), "[hl]");
//       //   cycle_count += 1;
//       //   break;
//       // case 0xBF:  cp_a_r8(af.reg.hi, "a"); break;
//
//
//
//       //
//       // BLOCK 3
//       //
//       case 0xC6:  //add a, n8
//         sprintf(name, "%02X", get_memory(++pc.reg16));
//         add_a_r8(get_memory(pc.reg16), name, 0);
//         cycle_count += 1;
//         break;
//       // case 0xCE:  //adc a, n8
//       //   sprintf(name, "%02X", get_memory(++pc.reg16));
//       //   add_a_r8(get_memory(pc.reg16), name, 1);
//       //   cycle_count += 1;
//       //   break;
//       // case 0xD6:  //sub a, n8
//       //   sprintf(name, "%02X", get_memory(++pc.reg16));
//       //   sub_a_r8(get_memory(pc.reg16), name, 0);
//       //   cycle_count += 1;
//       //   break;
//       // case 0xDE:  //sbc a, n8
//       //   sprintf(name, "%02X", get_memory(++pc.reg16));
//       //   sub_a_r8(get_memory(pc.reg16), name, 1);
//       //   cycle_count += 1;
//       //   break;
//       case 0xE6:  //and a, n8
//         sprintf(name, "%02X", get_memory(++pc.reg16));
//         and_a_r8(get_memory(pc.reg16), name);
//         cycle_count += 1;
//         break;
//       // case 0xEE:  //xor a, n8
//       //   sprintf(name, "%02X", get_memory(++pc.reg16));
//       //   xor_a_r8(get_memory(pc.reg16), name);
//       //   cycle_count += 1;
//       //   break;
//       case 0xF6:  //or a, n8
//         sprintf(name, "%02X", get_memory(++pc.reg16));
//         or_a_r8(get_memory(pc.reg16), name);
//         cycle_count += 1;
//         break;
//       case 0xFE:  //cp a, n8
//         sprintf(name, "%02X", get_memory(++pc.reg16));
//         cp_a_r8(get_memory(pc.reg16), name);
//         cycle_count += 1;
//         break;
//
//       case 0xC0:  ret_cc(!zf, "nz");  break;
//       case 0xC8:  ret_cc(zf, "z");  break;
//       // case 0xD0:  ret_cc(!cf, "nc");  break;
//       // case 0xD8:  ret_cc(cf, "c");  break;
//       case 0xC9:  // ret
//         ret_cc(1, "");
//         cycle_count -= 1;
//         break;
//       case 0xD9:  // reti
//         ime0 = 1;
//         ret_cc(1, "i");
//         cycle_count -= 1;
//         break;
//
//       case 0xC2:  jp_cc_n16(!zf, "nz"); break;
//       case 0xCA:  jp_cc_n16(zf, "z"); break;
//       // case 0xD2:  jp_cc_n16(!cf, "nc"); break;
//       // case 0xDA:  jp_cc_n16(cf, "c"); break;
//       case 0xC3:  jp_cc_n16(1, "");  break;
//       case 0xE9:  // jp hl
//         pc.reg16 = hl.reg16 - 1;
//         cycle_count += 1;
//
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "jp\thl\n");
// #endif // LOG_FILE
//
//         break;
//
//       // case 0xC4:  call_cc_n16(!zf, "nz"); break;
//       // case 0xCC:  call_cc_n16(zf, "z"); break;
//       // case 0xD4:  call_cc_n16(!cf, "nc"); break;
//       // case 0xDC:  call_cc_n16(cf, "c"); break;
//       case 0xCD:  call_cc_n16(1, "");  break;
//
//       // case 0xC7:  rst(0x00);  break;
//       // case 0xCF:  rst(0x08);  break;
//       // case 0xD7:  rst(0x10);  break;
//       // case 0xDF:  rst(0x18);  break;
//       // case 0xE7:  rst(0x20);  break;
//       case 0xEF:  rst(0x28);  break;
//       // case 0xF7:  rst(0x30);  break;
//       // case 0xFF:  rst(0x38);  break;
//
//       case 0xC1:  pop_r16(&bc, "bc"); break;
//       case 0xD1:  pop_r16(&de, "de"); break;
//       case 0xE1:  pop_r16(&hl, "hl"); break;
//       case 0xF1:
//         pop_r16(&af, "af");
//         zf = (af.reg.lo & 0x80) >> 7;
//         nf = (af.reg.lo & 0x40) >> 6;
//         hf = (af.reg.lo & 0x20) >> 5;
//         cf = (af.reg.lo & 0x10) >> 4;
//         break;
//
//       case 0xC5:  push_r16(bc, "bc"); break;
//       case 0xD5:  push_r16(de, "de"); break;
//       case 0xE5:  push_r16(hl, "hl"); break;
//       case 0xF5:
//         af.reg.lo = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
//         push_r16(af, "af");
//         break;
//
//       case 0xE2:  // ldh [c], a
//         n16.reg.lo = bc.reg.lo;
//         n16.reg.hi = 0xff;
//         memory[n16.reg16] = af.reg.hi;
//         cycle_count += 2;
//
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ldh\t[c]\ta");
//         fprintf(LOG_FILE, "\t\tc:\t%02X\t[c]:\t%02X\n", bc.reg.lo, memory[n16.reg16]);
// #endif // LOG_FILE
//
//         break;
//
//       case 0xE0:  // ldh [n8], a
//         n16.reg.lo = memory[++pc.reg16];
//         n16.reg.hi = 0xff;
//         memory[n16.reg16] = af.reg.hi;
//         cycle_count += 3;
//
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ldh\t[%04X],\ta", n16.reg16);
//         fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", n16.reg16, memory[n16.reg16]);
// #endif // LOG_FILE
//
//         break;
//
//       case 0xEA:  // ld [n16], a
//         n16.reg.lo = get_memory(++pc.reg16);
//         n16.reg.hi = get_memory(++pc.reg16);
//         set_memory(n16.reg16, af.reg.hi);
//         cycle_count += 4;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ld\t[%04X]\ta\t", n16.reg16);
//         fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", n16.reg16, get_memory(n16.reg16));
// #endif // LOG_FILE
//         break;
//
//       case 0xF0:  // ldh a, [n8]
//         n16.reg.lo = memory[++pc.reg16];
//         n16.reg.hi = 0xff;
//         af.reg.hi = memory[n16.reg16];
//         cycle_count += 3;
//
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ldh\ta,\t[%04X]", n16.reg16);
//         fprintf(LOG_FILE, "\t\ta:\t%02X\n", af.reg.hi);
// #endif // LOG_FILE
//
//         break;
//
//       case 0xFA:  // ld a, [n16]
//         n16.reg.lo = get_memory(++pc.reg16);
//         n16.reg.hi = get_memory(++pc.reg16);
//         af.reg.hi = get_memory(n16.reg16);
//         cycle_count += 4;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ld\ta\t[%04X]\t", n16.reg16);
//         fprintf(LOG_FILE, "\t\ta:\t%02X\n", af.reg.hi);
// #endif // LOG_FILE
//         break;
//
//       case 0xF3:  // di
//         ime0 = 0;
//         cycle_count += 1;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "di\n");
// #endif // LOG_FILE
//         break;
//
//       case 0xFB:  // ei
//         ime0 = 1;
//         cycle_count += 1;
// #ifdef LOG_FILE
//         fprintf(LOG_FILE, "ei\n");
// #endif // LOG_FILE
//         break;
//
//
//
//       //
//       // BLOCK CB
//       //
//       case 0xCB:
//         pc.reg16++;
//         opcode = memory[pc.reg16];
//         switch (opcode) {
//           case 0x27:  sla_r8(&(af.reg.hi), "a");  break;
//           case 0x37:  swap_r8(&(af.reg.hi), "a"); break;
//
//           case 0x50: case 0x58: case 0x60: case 0x68:
//           case 0x70: case 0x78: case 0x40: case 0x48:
//             bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.hi, "b");
//             break;
//           case 0x51: case 0x59: case 0x61: case 0x69:
//           case 0x71: case 0x79: case 0x41: case 0x49:
//             bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.lo, "c");
//             break;
//           case 0x52: case 0x5A: case 0x62: case 0x6A:
//           case 0x72: case 0x7A: case 0x42: case 0x4A:
//             bit_bitn_r8((opcode >> 3) & 0x7, de.reg.hi, "d");
//             break;
//           case 0x53: case 0x5B: case 0x63: case 0x6B:
//           case 0x73: case 0x7B: case 0x43: case 0x4B:
//             bit_bitn_r8((opcode >> 3) & 0x7, de.reg.lo, "e");
//             break;
//           case 0x54: case 0x5C: case 0x64: case 0x6C:
//           case 0x74: case 0x7C: case 0x44: case 0x4C:
//             bit_bitn_r8((opcode >> 3) & 0x7, hl.reg.hi, "h");
//             break;
//           case 0x55: case 0x5D: case 0x65: case 0x6D:
//           case 0x75: case 0x7D: case 0x45: case 0x4D:
//             bit_bitn_r8((opcode >> 3) & 0x7, hl.reg.lo, "l");
//             break;
//           case 0x56: case 0x5E: case 0x66: case 0x6E:
//           case 0x76: case 0x7E: case 0x46: case 0x4E:
//             bit_bitn_r8((opcode >> 3) & 0x7, get_memory(hl.reg16), "[hl]");
//             cycle_count += 1;
//             break;
//           case 0x57: case 0x5F: case 0x67: case 0x6F:
//           case 0x77: case 0x7F: case 0x47: case 0x4F:
//             bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.hi, "a");
//             break;
//
//           case 0x86:
//             res_bitn_r8(0, get_setter_memory(hl.reg16), "[hl]");
//             cycle_count += 2;
//             break;
//           case 0x87:  res_bitn_r8(0, &(af.reg.hi), "a");  break;
//
//           case 0xCF:  set_bitn_r8(1, &(af.reg.hi), "a");  break;
//
//           default:
//             fprintf(LOG_FILE, "Unknown CB prefix instruction");
//             void dump_vram();
//             dump_vram();
//             exit(1);
//         }
//         break;
//
//
//       default:
//         fprintf(LOG_FILE, "Unimplemented opcode\n");
//         exit(1);
//     }
//
//
//     // Handle Joypad Register
//     if ((memory[0xff00] & 0x30) == 0x20)
//       memory[0xff00] = 0xEF;
//     else if ((memory[0xff00] & 0x30) == 0x10)
//       memory[0xff00] = 0xDF;
//
//
//     pc.reg16++;
//   } while(opcode != 0x10);
//
//   return 0;
// }
//
//
