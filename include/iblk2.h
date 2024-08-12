#ifndef IBLK2_H
#define IBLK2_H


static inline void
add_a_r8(const Uint8 r8, const char *name, const Uint8 carry)
{
    a += (r8 + (carry ? cf : 0));
    zf = !(a);
    nf = 0;
    hf = (a & 0xf) < (r8 & 0xf);
    cf = a < r8;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "%s\ta,\t%s", carry ? "adc" : "add", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
sub_a_r8(const Uint8 r8, const char *name, const Uint8 carry)
{
    hf = (a & 0xf) < (r8 & 0xf);
    cf = a < r8;
    a -= (r8 + (carry ? cf : 0));
    zf = !(a);
    nf = 0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "%s\ta,\t%s", carry ? "sbc" : "sub", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
and_a_r8(const Uint8 r8, const char *name)
{
    a &= r8;
    zf = !(a);
    nf = 0; hf = 1; cf =0;
    cycle_count += 1;

  #ifdef GENERATE_LOGS
    fprintf(log_file, "and\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
  #endif // GENERATE_LOGS
}


static inline void
xor_a_r8(const Uint8 r8, const char *name)
{
    a ^= r8;
    zf = !(a);
    nf = 0; hf = 0; cf =0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "xor\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
or_a_r8(const Uint8 r8, const char *name)
{
    a |= r8;
    zf = !(a);
    nf = 0; hf = 0; cf =0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "or\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", a, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
cp_a_r8(const Uint8 r8, const char *name)
{
    zf = a == r8;
    nf = 1;
    hf = (a & 0xf) < (r8 & 0xf);
    cf = a < r8;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "cp\ta,\t%s", name);
    fprintf(log_file, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
#endif // GENERATE_LOGS
}

#endif //IBLK2_H
