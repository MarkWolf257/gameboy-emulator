#ifndef GBMEM_H
#define GBMEM_H

#include <SDL2/SDL.h>

#define GB_MEMORY_SIZE 0x10000
#define GB_BANK_SIZE 0x4000

#define JOYP 0xFF00
#define DIV 0xFF04
#define IF 0xFF0F
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xff44
#define LYC 0xff45
#define DMA 0xff46
#define BGP 0xff47
#define OBP0 0xff48
#define OBP1 0xff49
#define WY 0xFF4A
#define WX 0xFF4B


extern Uint8 *memory;


// Implement MBC here
static inline Uint8
get_ro_mem(const Uint16 address)
{
    return memory[address];
}

// Returns pointer to memory if writable
// Otherwise returns pointer to MBC registers
// Implement MBC here
static inline Uint8 *
get_rw_memptr(const Uint16 address)
{
    if (address >= 0x8000)
        return &memory[address];
    return &memory[GB_MEMORY_SIZE]; // Disposable memory
}

#endif //GBMEM_H
