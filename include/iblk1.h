#ifndef IBLK1_H
#define IBLK1_H


static inline void
ld_r8_r8(uint8_t * const r8l, const char *namel, const uint8_t r8r, const char *namer)
{
    *r8l = r8r;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "ld\t%s,\t%s", namel, namer);
    fprintf(log_file, "\t\t%s:\t%02X\n", namel, *r8l);
#endif // GENERATE_LOGS
}


static inline void
ld_mhl_r8(const uint8_t r8, const char *name)
{
    *get_rw_memptr(hl.reg16) = r8;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "ld\t[hl],\t%s", name);
    fprintf(log_file, "\t\thl:\t%04X\t[hl]:\t%02X\n", hl.reg16, *get_rw_memptr(hl.reg16));
#endif // GENERATE_LOGS
}


static inline void
ld_r8_mhl(uint8_t * const r8, const char *name)
{
    *r8 = get_ro_mem(hl.reg16);
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "ld\t%s,\t[hl]", name);
    fprintf(log_file, "\t\t%s:\t%02X\thl:\t%04X\n", name, *r8, hl.reg16);
#endif // GENERATE_LOGS
}

#endif //IBLK1_H
