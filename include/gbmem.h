#ifndef GBMEM_H
#define GBMEM_H

#include <SDL2/SDL.h>

#define GB_MEMORY_SIZE 0x10000
#define GB_BANK_SIZE 0x4000

#define LY 0xff44

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
