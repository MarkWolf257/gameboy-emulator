#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define MEMORY_SIZE 0x10000
#define BANK_SIZE 0x4000
#define CYCLES_PER_LINE 114
#define VERTICAL_LINES 154



// Register types
typedef uint8_t register8_t;

typedef union register16_t
{
  struct
  {
    uint8_t lo, hi;
  } reg;

  uint16_t reg16;
} register16_t;



// Structures for bitmap
typedef struct
{
  uint16_t type;
  uint32_t size;
  uint16_t reserved1, reserved2;
  uint32_t offset;
} bmp_header_t;

typedef struct
{
  uint32_t size, width, height;
  uint16_t planes, bits_per_pixel;
  uint32_t compression, image_size, x_resolution, y_resolution, colors_used, colors_important;
} bmp_info_header_t;


// Registers and memory
uint8_t  memory[MEMORY_SIZE];
register16_t af, bc, de, hl, sp, pc;
uint8_t zf, nf, hf, cf;
uint8_t cycle_count = 0;
bool interrupts_enabled = true;


// Functions Block 0
static inline void ld_r16_n16(register16_t *r16, const char *name)
{
  r16->reg.lo = memory[++pc.reg16];
  r16->reg.hi = memory[++pc.reg16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t%04X\n", name, r16->reg16);
  #endif
}

static inline void ld_mr16_a(register16_t *r16, const char *name)
{
  memory[r16->reg16] = af.reg.hi;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[%s],\ta", name);
  fprintf(LOG_FILE, "\t\t%s:\t%04X\t[%s]:\t%02X\n", name, r16->reg16, name, memory[r16->reg16]);
  #endif
}

static inline void ld_a_mr16(register16_t r16, const char *name)
{
  af.reg.hi = memory[r16.reg16];
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\ta,\t[%s]", name);
  fprintf(LOG_FILE, "\t\t%s:\t%04X\ta:\t%02X\n", name, r16.reg16, af.reg.hi);
  #endif
}

static inline void inc_r16(register16_t *r16, const char *name)
{
  r16->reg16++;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "inc\t%s\t", name);
  fprintf(LOG_FILE, "\t\t%s:\t%04X\n", name, r16->reg16);
  #endif
}

static inline void dec_r16(register16_t *r16, const char *name)
{
  r16->reg16--;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "dec\t%s\t", name);
  fprintf(LOG_FILE, "\t\t%s:\t%04X\n", name, r16->reg16);
  #endif
}

static inline void add_hl_r16(register16_t r16, const char *name)
{
  
  hl.reg16 += r16.reg16;
  hf = (hl.reg16 & 0x0fff) < (r16.reg16 & 0x0fff);
  nf = 0;
  cf = hl.reg16 < r16.reg16;
  cycle_count += 2;
  
  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "add\thl,\t%s", name);
  fprintf(LOG_FILE, "\t\thl:\t%02X\tznhc:\t%d%d%d%d\n", hl.reg16, zf, nf, hf, cf);
  #endif
}

static inline void inc_r8(register8_t *r8, const char *name)
{
  (*r8)++;
  zf = !(*r8);
  nf = 0;
  hf = !((*r8) & 0xf);
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "inc\t%s\t", name);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
  #endif
}

static inline void dec_r8(register8_t *r8, const char *name)
{
  hf = !((*r8) & 0xf);
  (*r8)--;
  zf = !(*r8);
  nf = 1;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "dec\t%s\t", name);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", name, *r8, zf, nf, hf, cf);
  #endif
}

static inline void ld_r8_n8(register8_t *r8, const char *name)
{
  *r8 = memory[++pc.reg16];
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t%02X\n", name, *r8);
  #endif
}


// Functions block 1
static inline void ld_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
  *r8l = r8r;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", namel, *r8l);
  #endif
}

static inline void ld_mhl_r8(register8_t r8, const char *name)
{
  memory[hl.reg16] = r8;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[hl],\t%s" name);
  fprintf(LOG_FILE, "\t\thl:\t%04X\n", hl.reg16);
  #endif
}

static inline void ld_r8_mhl(register8_t *r8, const char *name)
{
  *r8 = memory[hl.reg16];
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t[hl]", name);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\thl:\t%04X\n", name, *r8, hl.reg16);
  #endif
}


// Functions block 2
static inline void add_a_r8(register8_t r8, const char *name, bool carry)
{
  af.reg.hi += (r8 + carry ? c : 0);
  zf = !(af.reg.hi);
  nf = 0;
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "%s\ta,\t%s", carry ? "adc" : "add", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d", af.reg.hi, zf, nf, hf, cf);
  #endif
}

static inline void sub_a_r8(register8_t r8, const char *name, bool carry)
{
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  af.reg.hi -= (r8 + carry ? c : 0);
  zf = !(af.reg.hi);
  nf = 0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "%s\ta,\t%s", carry ? "sbc" : "sub", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d", af.reg.hi, zf, nf, hf, cf);
  #endif
}

static inline void and_a_r8(register8_t r8, const char *name)
{
  af.reg.hi &= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 1; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "and\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
  #endif
}

static inline void xor_a_r8(register8_t r8, const char *name)
{
  af.reg.hi ^= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "xor\t%s,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
  #endif
}

static inline void or_a_r8(register8_t r8, const char *name)
{
  af.reg.hi |= r8;
  zf = !(af.reg.hi);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "or\t%s,\t%s", name);
  fprintf(LOG_FILE, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
  #endif
}

static inline void cp_a_r8(register8_t r8, const char *name)
{
  zf = af.reg.hi == r8;
  nf = 1;
  hf = (af.reg.hi & 0xf) < (r8 & 0xf);
  cf = af.reg.hi < r8;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "cp\ta,\t%s", name);
  fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
  #endif
}


// Functions block 3
static inline void ret_cc(uint8_t cc, const char *name)
{
  if (cc) {
    pc.reg.lo = memory[++sp.reg16];
    pc.reg.hi = memory[++sp.reg16];
    cycle_count += 3;
  }
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ret%s\n", name);
  #endif
}

static inline void jp_cc_n16(uint8_t cc, const char *name)
{
  uint16_t n16 = pc.reg16;
  pc.reg.lo = memory[++n16];
  pc.reg.hi = memory[++n16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "jp%s\t%04X\n", name, pc.reg16);
  #endif

  if (cc) {
    pc.reg16--;
    cycle_count += 1;
  } else {
    pc.reg16 = n16;
  }
}
// NEEDS FIXING BOTH CALL AND RST
static inline void call_cc_n16(uint8_t cc, const char *name)
{
  register16_t n16;
  n16.reg.lo = memory[++pc.reg16];
  n16.reg.hi = memory[++pc.reg16];
  cycle_count += 3;

  if (cc) {
    memory[sp.reg16--] = pc.reg.hi;
    memory[sp.reg16--] = pc.reg.lo;
    pc.reg16 = n16.reg16;
    cycle_count += 3;
  }
  
  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "call%s\t%04X\n", n16.reg16);
  #endif
}

static inline void rst(uint16_t address)
{
  memory[sp.reg16--] = pc.reg.hi;
  memory[sp.reg16--] = pc.reg.lo;
  pc.reg16 = address;
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "rst\t%04X\n", address);
  #endif
}

static inline void pop_r16(register16_t *r16, const char *name)
{
  r16->reg.lo = memory[++sp.reg16];
  r16->reg.hi = memory[++sp.reg16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "pop\t%s\t", name);
  fprintf(LOG_FILE, "\t\t%s:\t%04X\n", name, r16->reg16);
  #endif
}

static inline void push_r16(register16_t r16, const char *name)
{
  memory[sp.reg16--] = r16.reg.hi;
  memory[sp.reg16--] = r16.reg.lo;
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "push\t%s\n", name);
  #endif
}


// Functions CB prefix
static inline void res_bitn_r8(uint8_t n, register8_t *r8, const char *name)
{
  *r8 &= ~(1 << n);
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "res\tbit%d,\t%s", n, name);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
  #endif
}

static inline void set_bitn_r8(uint8_t n, register8_t *r8, const char *name)
{
  *r8 |= (1 << n);
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "set\tbit%d,\t%s", n, name);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
  #endif
}