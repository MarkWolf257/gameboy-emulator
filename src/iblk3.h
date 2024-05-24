#ifndef IBLK3_H
#define IBLK3_H

#include "gbcpu.h"
#include "gbmem.h"


static inline void
ret_cc(const uint8_t cc, const char *name)
{
    if (cc) {
        pc.reg.lo = memory[++sp.reg16];
        pc.reg.hi = memory[++sp.reg16];
        --pc.reg16;
        cycle_count += 3;
    }
    cycle_count += 2;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "ret%s\n", name);
#endif // LOG_FILE
}


static inline void
jp_cc_n16(const uint8_t cc, const char *name)
{
    uint16_t n16 = pc.reg16;
    pc.reg.lo = get_memory(++n16);
    pc.reg.hi = get_memory(++n16);
    cycle_count += 3;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "jp%s\t%04X\n", name, pc.reg16);
#endif // LOG_FILE

    if (cc) {
        pc.reg16--;
        cycle_count += 1;
    } else {
        pc.reg16 = n16;
    }
}


static inline void
call_cc_n16(const uint8_t cc, const char *name)
{
    register16_t n16;
    n16.reg.lo = get_memory(++pc.reg16);
    n16.reg.hi = get_memory(++pc.reg16);
    cycle_count += 3;

    if (cc) {
        ++pc.reg16;
        memory[sp.reg16--] = pc.reg.hi;
        memory[sp.reg16--] = pc.reg.lo;
        pc.reg16 = n16.reg16 - 1;
        cycle_count += 3;
    }

#ifdef LOG_FILE
    fprintf(LOG_FILE, "call%s\t%04X\n", name, n16.reg16);
#endif // LOG_FILE
}


static inline void
rst(const uint16_t address)
{
    ++pc.reg16;
    memory[sp.reg16--] = pc.reg.hi;
    memory[sp.reg16--] = pc.reg.lo;
    pc.reg16 = address - 1;
    cycle_count += 4;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "rst\t%04X\n", address);
#endif // LOG_FILE
}


static inline void
pop_r16(register16_t * const r16, const char *name)
{
    r16->reg.lo = memory[++sp.reg16];
    r16->reg.hi = memory[++sp.reg16];
    cycle_count += 3;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "pop\t%s\t", name);
    fprintf(LOG_FILE, "\t\t%s:\t%04X\n", name, r16->reg16);
#endif // LOG_FILE
}


static inline void
push_r16(const register16_t r16, const char *name)
{
    memory[sp.reg16--] = r16.reg.hi;
    memory[sp.reg16--] = r16.reg.lo;
    cycle_count += 4;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "push\t%s\n", name);
#endif // LOG_FILE
}

#endif //IBLK3_H