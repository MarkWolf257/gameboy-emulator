#ifndef GBMEM_H
#define GBMEM_H

#include <inttypes.h>

#define GB_MEMORY_SIZE 0x10000
#define GB_BANK_SIZE 0x4000

extern uint8_t *memory;


//
// Wrapper functions for MBC Implementation
//
static inline uint8_t
get_memory(const uint16_t address)
{
    return memory[address];
}


static inline void
set_memory(const uint16_t address, const uint8_t value)
{
    if (address >= 0x8000)
        memory[address] = value;
}


/* While writing to read only memory certain registers are written to
 * instead of the memory if an MBC chip is present. Since reads are
 * allowed get_memory reads from memory rather than the registers.
 * This function returns the register value instead if read only
 * area is accesed. */
static inline uint8_t *
get_setter_memory(const uint16_t address)
{
    return &memory[address];
}

#endif //GBMEM_H
