#ifndef IBLKCB_H
#define IBLKCB_H


static inline void
sla_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 >> 7;
    *r8 <<= 1;
    zf = !(*r8);
    nf = 0; hf = 0;

#ifdef GENERATE_LOGS
    fprintf(log_file, "sla\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
swap_r8(Uint8 * const r8, const char *name)
{
    *r8 = *r8 << 4 | *r8 >> 4;
    zf = !(*r8);
    nf = 0; hf = 0; cf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "swap\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%04X\n", name, *r8);
#endif // GENERATE_LOGS
}


static inline void
bit_bitn_r8(const Uint8 n, const Uint8 r8, const char *name)
{
    zf = !((r8 >> n) & 1);
    nf = 0; hf = 1;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "bit\tbit%d,\t%s", n, name);
    fprintf(log_file, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
res_bitn_r8(const Uint8 n, Uint8 * const r8, const char *name)
{
    *r8 &= ~(1 << n);
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "res\tbit%d,\t%s", n, name);
    fprintf(log_file, "\t\t%s:\t%02X\n", name, *r8);
#endif // GENERATE_LOGS
}


static inline void
set_bitn_r8(const Uint8 n, Uint8 * const r8, const char *name)
{
    *r8 |= (1 << n);
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "set\tbit%d,\t%s", n, name);
    fprintf(log_file, "\t\t%s:\t%02X\n", name, *r8);
#endif // GENERATE_LOGS
}

#endif //IBLKCB_H
