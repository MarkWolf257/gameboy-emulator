#ifndef IBLK2_H
#define IBLK2_H


static inline void
add_a_r8(const uint8_t r8, const char *name, const uint8_t carry)
{
    af.reg.hi += (r8 + (carry ? cf : 0));
    zf = !(af.reg.hi);
    nf = 0;
    hf = (af.reg.hi & 0xf) < (r8 & 0xf);
    cf = af.reg.hi < r8;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "%s\ta,\t%s", carry ? "adc" : "add", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
sub_a_r8(const uint8_t r8, const char *name, const uint8_t carry)
{
    hf = (af.reg.hi & 0xf) < (r8 & 0xf);
    cf = af.reg.hi < r8;
    af.reg.hi -= (r8 + (carry ? cf : 0));
    zf = !(af.reg.hi);
    nf = 0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "%s\ta,\t%s", carry ? "sbc" : "sub", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
and_a_r8(const uint8_t r8, const char *name)
{
    af.reg.hi &= r8;
    zf = !(af.reg.hi);
    nf = 0; hf = 1; cf =0;
    cycle_count += 1;

  #ifdef GENERATE_LOGS
    fprintf(log_file, "and\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
  #endif // GENERATE_LOGS
}


static inline void
xor_a_r8(const uint8_t r8, const char *name)
{
    af.reg.hi ^= r8;
    zf = !(af.reg.hi);
    nf = 0; hf = 0; cf =0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "xor\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
or_a_r8(const uint8_t r8, const char *name)
{
    af.reg.hi |= r8;
    zf = !(af.reg.hi);
    nf = 0; hf = 0; cf =0;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "or\ta,\t%s", name);
    fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // GENERATE_LOGS
}


static inline void
cp_a_r8(const uint8_t r8, const char *name)
{
    zf = af.reg.hi == r8;
    nf = 1;
    hf = (af.reg.hi & 0xf) < (r8 & 0xf);
    cf = af.reg.hi < r8;
    cycle_count += 1;

#ifdef GENERATE_LOGS
    fprintf(log_file, "cp\ta,\t%s", name);
    fprintf(log_file, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
#endif // GENERATE_LOGS
}

#endif //IBLK2_H
