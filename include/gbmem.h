#ifndef GBMEM_H
#define GBMEM_H

#include <inttypes.h>

#define GB_MEMORY_SIZE 0x10000
#define GB_BANK_SIZE 0x4000

extern uint8_t *memory;


// Implement MBC here
static inline uint8_t
get_ro_mem(const uint16_t address)
{
    return memory[address];
}

// Returns pointer to memory if writable
// Otherwise returns pointer to MBC registers
// Implement MBC here
static inline uint8_t *
get_rw_memptr(const uint16_t address)
{
    if (address >= 0x8000)
        return &memory[address];
    return &memory[GB_MEMORY_SIZE]; // Disposable memory
}

#endif //GBMEM_H
