#ifndef IBLKCB_H
#define IBLKCB_H


static inline void
rlc_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 >> 7;
    *r8 = (*r8 << 1) | cf;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "rlc\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
rrc_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 & 0x01;
    *r8 = *r8 >> 1 | cf << 7;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "rrc\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
rl_r8(Uint8 * const r8, const char *name)
{
    nf = *r8 >> 7;  // Temporary
    *r8 = (*r8 << 1) | cf;
    cf = nf;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "rl\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
rr_r8(Uint8 * const r8, const char *name)
{
    nf = *r8 & 0x01;    // Temporary
    *r8 = (*r8 >> 1) | (cf << 7);
    cf = nf;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "rr\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
sla_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 >> 7;
    *r8 <<= 1;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "sla\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
sra_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 & 0x01;
    *r8 = (*r8 >> 1) | (*r8 & 0x80);
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "sra\t%s\t", name);
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
srl_r8(Uint8 * const r8, const char *name)
{
    cf = *r8 & 0x01;
    *r8 >>= 1;
    zf = !(*r8);
    nf = 0; hf = 0;
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "srl\t%s\t", name);
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
