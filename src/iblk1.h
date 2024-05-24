#ifndef IBLK1_H
#define IBLK1_H

#include "gbcpu.h"
#include "gbmem.h"


static inline void
ld_r8_r8(register8_t * const r8l, const char *namel, const register8_t r8r, const char *namer)
{
    *r8l = r8r;
    cycle_count += 1;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "ld\t%s,\t%s", namel, namer);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\n", namel, *r8l);
#endif // LOG_FILE
}


static inline void
ld_mhl_r8(const register8_t r8, const char *name)
{
    set_memory(hl.reg16, r8);
    cycle_count += 2;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "ld\t[hl],\t%s", name);
    fprintf(LOG_FILE, "\t\thl:\t%04X\t[hl]:\t%02X\n", hl.reg16, get_memory(hl.reg16));
#endif // LOG_FILE
}


static inline void
ld_r8_mhl(register8_t * const r8, const char *name)
{
    *r8 = get_memory(hl.reg16);
    cycle_count += 2;

#ifdef LOG_FILE
    fprintf(LOG_FILE, "ld\t%s,\t[hl]", name);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\thl:\t%04X\n", name, *r8, hl.reg16);
#endif // LOG_FILE
}

#endif //IBLK1_H
