#ifndef IBLK2_H
#define IBLK2_H

#include "gbcpu.h"


static inline void
add_a_r8(const register8_t r8, const char *name, const uint8_t carry)
{
  af.reg.hi += (r8 + (carry ? cf : 0));
  zf = !(af.reg.hi);
  nf = 0;
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  cycle_count += 1;

#ifdef LOG_FILE
  fprintf(LOG_FILE, "%s\ta,\t%s", carry ? "adc" : "add", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // LOG_FILE
}


static inline void
sub_a_r8(const register8_t r8, const char *name, const uint8_t carry)
{
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  af.reg.hi -= (r8 + (carry ? cf : 0));
  zf = !(af.reg.hi);
  nf = 0;
  cycle_count += 1;

#ifdef LOG_FILE
  fprintf(LOG_FILE, "%s\ta,\t%s", carry ? "sbc" : "sub", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // LOG_FILE
}


static inline void
and_a_r8(const register8_t r8, const char *name)
{
  af.reg.hi &= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 1; cf =0;
  cycle_count += 1;

  #ifdef LOG_FILE
  fprintf(LOG_FILE, "and\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
  #endif // LOG_FILE
}


static inline void
xor_a_r8(const register8_t r8, const char *name)
{
  af.reg.hi ^= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

#ifdef LOG_FILE
  fprintf(LOG_FILE, "xor\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // LOG_FILE
}


static inline void
or_a_r8(const register8_t r8, const char *name)
{
  af.reg.hi |= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

#ifdef LOG_FILE
  fprintf(LOG_FILE, "or\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
#endif // LOG_FILE
}


static inline void
cp_a_r8(const register8_t r8, const char *name)
{
  zf = af.reg.hi == r8;
  nf = 1;
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  cycle_count += 1;

#ifdef LOG_FILE
  fprintf(LOG_FILE, "cp\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
#endif // LOG_FILE
}

#endif //IBLK2_H
