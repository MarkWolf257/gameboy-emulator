#ifndef IBLKCB_H
#define IBLKCB_H

#include "gbcpu.h"
#include "gbmem.h"


static inline void
sla_r8(register8_t *r8, const char *name)
{
    cf = *r8 >> 7;
    *r8 <<= 1;
    zf = !(*r8);
    nf = 0; hf = 0;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "sla\t%s\t", name);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
swap_r8(register8_t *r8, const char *name)
{
    *r8 = *r8 << 4 | *r8 >> 4;
    zf = !(*r8);
    nf = 0; hf = 0; cf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "swap\t%s\t", name);
    fprintf(LOG_FILE, "\t\t%s:\t%04X\n", name, *r8);
#endif // GENERATE_LOGS
}


static inline void
bit_bitn_r8(uint8_t n, register8_t r8, const char *name)
{
    zf = !((r8 >> n) & 1);
    nf = 0; hf = 1;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "bit\tbit%d,\t%s", n, name);
    fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
res_bitn_r8(uint8_t n, register8_t *r8, const char *name)
{
    *r8 &= ~(1 << n);
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "res\tbit%d,\t%s", n, name);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
#endif // GENERATE_LOGS
}


static inline void
set_bitn_r8(uint8_t n, register8_t *r8, const char *name)
{
    *r8 |= (1 << n);
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(LOG_FILE, "set\tbit%d,\t%s", n, name);
    fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
#endif // GENERATE_LOGS
}

#endif //IBLKCB_H
