#ifndef IBLK1_H
#define IBLK1_H

#include "gbcpu.h"
#include "gbmem.h"


static inline void
ld_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
    *r8l = r8r;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "ld\t%s,\t%s", namel, namer);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\n", namel, *r8l);
#endif // GENERATE_LOGS
}


static inline void
ld_mhl_r8(register8_t r8, const char *name)
{
    memory[hl.reg16] = r8;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "ld\t[hl],\t%s", name);
    fprintf(LOG_FILE, "\t\thl:\t%04X\n", hl.reg16);
#endif // GENERATE_LOGS
}


static inline void
ld_r8_mhl(register8_t *r8, const char *name)
{
    *r8 = memory[hl.reg16];
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "ld\t%s,\t[hl]", name);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\thl:\t%04X\n", name, *r8, hl.reg16);
#endif // GENERATE_LOGS
}

#endif //IBLK1_H
