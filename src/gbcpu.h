#ifndef GBCPU_H
#define GBCPU_H

#include <inttypes.h>


// Register Types
typedef uint8_t register8_t;

typedef union register16_t
{
    struct
    {
        register8_t lo, hi;
    } reg;

    uint16_t reg16;
} register16_t;


// CPU Variables
extern register16_t af, bc, de, hl, sp, pc;    //  Registers
extern uint8_t zf, nf, hf, cf, ime0;        //  Flags
extern uint8_t cycle_count;


#endif //GBCPU_H
