#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"

#define GENERATE_LOGS true
#define LOG_FILE stderr



static register16_t af, bc, de, hl, sp, pc;
static uint8_t zf, nf, hf, cf;
static uint8_t cycle_count = 0;
static bool interrupts_enabled = true;


static inline void nop()
{
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "nop\n");
  #endif
}

static inline void jp_cc(uint8_t cc, const char *name)
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

static inline void jr_cc(uint8_t cc, const char *name)
{
  uint8_t n8 = memory[++pc.reg16];
  if (cc)
  {
    pc.reg16 = (int16_t) pc.reg16 + (int8_t) n8;
    cycle_count += 1;
  }
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "jr%s\t%d\n", name, (int8_t) n8);
  #endif
}

static inline void ld_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
  *r8l = r8r;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", namel, *r8l);
  #endif
}

static inline void ld_mhl_r8(int op, const char opname, register8_t r8, const char *name)
{
  memory[hl.reg16] = r8;
  hl.reg16 += op;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[hl%c],\t%s", opname, name);
  fprintf(LOG_FILE, "\t\thl:\t%04X\n", hl.reg16);
  #endif
}

static inline void ld_r8_mhl(register8_t *r8, const char *name, int op, const char opname)
{
  *r8 = memory[hl.reg16];
  hl.reg16 += op;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t[hl%c]", name, opname);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\thl:\t%04X\n", name, *r8, hl.reg16);
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

static inline void ld_r16_n16(register16_t *r16, const char *name)
{
  r16->reg.lo = memory[++pc.reg16];
  r16->reg.hi = memory[++pc.reg16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t%04X\n", name, r16->reg16);
  #endif
}

static inline void ld_m16_r8(register8_t r8, const char *name)
{
  register16_t m16;
  m16.reg.lo = memory[++pc.reg16];
  m16.reg.hi = memory[++pc.reg16];
  memory[m16.reg16] = r8;
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[%04X],\t%s", m16.reg16, name);
  fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", m16.reg16, memory[m16.reg16]);
  #endif
}

static inline void ld_mr16_r8(register16_t *r16, const char *name16, register8_t r8, const char *name8)
{
  memory[r16->reg16] = r8;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[%s],\t%s", name16, name8);
  fprintf(LOG_FILE, "\t\t[%s]:\t%02X\n", name16, memory[r16->reg16]);
  #endif
}

static inline void ld_r8_m16(register8_t *r8, const char *name)
{
  register16_t m16;
  m16.reg.lo = memory[++pc.reg16];
  m16.reg.hi = memory[++pc.reg16];
  *r8 = memory[m16.reg16];
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t%s,\t[%04X]", name, m16.reg16);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
  #endif
}

static inline void ldh_r8_m8(register8_t *r8, const char *name)
{
  register16_t m16;
  m16.reg16 = 0xff00 + memory[++pc.reg16];
  *r8 = memory[m16.reg16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ldh\t%s,\t[%04X]", name, m16.reg16);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
  #endif
}

static inline void ldh_m8_r8(register8_t r8, const char *name)
{
  register16_t m16;
  m16.reg16 = memory[++pc.reg16] | 0xff00;
  memory[m16.reg16] = r8;
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ldh\t[%04X],\t%s", m16.reg16, name);
  fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", m16.reg16, memory[m16.reg16]);
  #endif
}

static inline void ld_mc_r8(register8_t r8, const char *name)
{
  memory[bc.reg.lo + 0xff00] = r8;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[c],\t%s", name);
  fprintf(LOG_FILE, "\t\tc:\t%02X\t[c]:\t%02X\n", bc.reg.lo, memory[bc.reg.lo + 0xff00]);
  #endif
}

static inline void add_r16_r16(register16_t *r16l, const char *namel, register16_t r16r, const char *namer)
{
  uint16_t tmp = r16l->reg16;
  hf = (r16l->reg16 & 0x0fff + r16r.reg16 & 0x0fff) > 0x0fff;
  r16l->reg16 += r16r.reg16;
  nf = 0;
  cf = tmp > r16l->reg16;
  cycle_count += 2;
  
  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "add\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", namel, r16l->reg16);
  #endif
}

static inline void xor_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
  *r8l ^= r8r;
  zf = !(*r8l);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "xor\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", namel, *r8l, zf, nf, hf, cf);
  #endif
}

static inline void or_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
  *r8l |= r8r;
  zf = !(*r8l);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "or\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", namel, *r8l, zf, nf, hf, cf);
  #endif
}

static inline void and_r8_r8(register8_t *r8l, const char *namel, register8_t r8r, const char *namer)
{
  *r8l &= r8r;
  zf = !(*r8l);
  nf = 0; hf = 1; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "and\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", namel, *r8l, zf, nf, hf, cf);
  #endif
}

static inline void and_r8_n8(register8_t *r8, const char *name)
{
  *r8 &= memory[++pc.reg16];
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "and\t%s,\t%02X", name, memory[pc.reg16]);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
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

static inline void cp_r8_n8(register8_t r8, const char *name)
{
  uint8_t n8 = memory[++pc.reg16];
  zf = r8 == n8;
  nf = 1;
  hf = ((r8 & 0x0f + n8 % 0x0f) & 0x10) >> 4;
  cf = r8 < n8;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "cp\t%s,\t0x%02X", name, n8);
  fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
  #endif
}

static inline void cp_r8_r8(register8_t r8l, const char *namel, register8_t r8r, const char *namer)
{
  zf = r8l == r8r;
  nf = 1;
  hf = ((r8l & 0x0f + r8r % 0x0f) & 0x10) >> 4;
  cf = r8l < r8r;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "cp\t%s,\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
  #endif
}

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

static inline void push_r16(register16_t r16, const char *name)
{
  memory[sp.reg16--] = r16.reg.hi;
  memory[sp.reg16--] = r16.reg.lo;
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "push\t%s\n", name);
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

static inline void call_n16()
{
  register16_t n16;
  n16.reg.lo = memory[++pc.reg16];
  n16.reg.hi = memory[++pc.reg16];
  memory[sp.reg16--] = pc.reg.hi;
  memory[sp.reg16--] = pc.reg.lo;
  pc.reg16 = n16.reg16;
  cycle_count += 6;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "call\t%04X\n", pc.reg16);
  #endif
}

static inline void ret()
{
  pc.reg.lo = memory[++sp.reg16];
  pc.reg.hi = memory[++sp.reg16];
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ret\n");
  #endif
}

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

static inline void interrupts(bool b)
{
  interrupts_enabled = b;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "di\n");
  #endif
}


void ppu()
{
  if (cycle_count >= CYCLES_PER_LINE) {
    memory[0xff44] = (memory[0xff44] + 1) % VERTICAL_LINES;
    cycle_count %= CYCLES_PER_LINE;
  }
}


int main()
{
  uint8_t opcode, n8;
  uint16_t n16;
  


  // Load ROM into memory
  FILE *fptr = fopen("rom.gb", "rb");
  if (fptr == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }

  if (fread(memory, 1, BANK_SIZE * 2, fptr) != BANK_SIZE * 2)
  {
    printf("Error! reading file");
    exit(1);
  }

  fclose(fptr);


  // Initialize registers
  af.reg16 = 0x0100;
  pc.reg16 = 0x0100;
  sp.reg16 = 0xfffe;
  fprintf(stderr, "Entry Point:\t%04X\n", pc.reg16);
  uint8_t imm8;
  register16_t imm16;


  do {
    ppu();
    fprintf(stderr, "%04X:\t", pc.reg16);
    opcode = memory[pc.reg16];

    switch (opcode)
    {
      case 0x00:  nop();  break;

      case 0x01:  ld_r16_n16(&bc, "bc");  break;

      case 0x05:  dec_r8(&(bc.reg.hi), "b");  break;

      case 0x06:  ld_r8_n8(&(bc.reg.hi), "b"); break;

      case 0x09:  add_r16_r16(&hl, "hl", bc, "bc"); break;

      case 0x0B:  dec_r16(&bc, "bc"); break;

      case 0x0C:  inc_r8(&(bc.reg.lo), "c");  break;

      case 0x0E:  ld_r8_n8(&(bc.reg.lo), "c");  break;

      case 0x11:  ld_r16_n16(&de, "de");  break;

      case 0x12:  ld_mr16_r8(&de, "de", af.reg.hi, "a");  break;

      case 0x13:  inc_r16(&de, "de"); break;
      
      case 0x15:  dec_r8(&(de.reg.hi), "d");  break;

      case 0x18:  jr_cc(true, "");  break;

      case 0x1D:  dec_r8(&(de.reg.lo), "e");  break;

      // case 0x1F:
      //   imm8 = cf;
      //   cf = af.reg.hi & 0x01;
      //   af.reg.hi = (af.reg.hi >> 1) | imm8 << 7;
      //   zf = 0;
      //   nf = 0;
      //   hf = 0;

      //   fprintf(stderr, "rra\t\t");
      //   fprintf(stderr, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      case 0x20:  jr_cc(!zf, "nz"); break;
      

      case 0x21:  ld_r16_n16(&hl, "hl");  break;
      

      case 0x22:  ld_mhl_r8(1, '+', af.reg.hi, "a"); break;
      

      case 0x23:  inc_r16(&hl, "hl"); break;
      

      case 0x26:  ld_r8_n8(&(hl.reg.hi), "h");  break;
      

      case 0x28:  jr_cc(zf, "z");  break;
      

      case 0x2A:  ld_r8_mhl(&(af.reg.hi), "a", 1, '+'); break;
      

      case 0x2B:  dec_r16(&hl, "hl"); break;
      

      case 0x31:  ld_r16_n16(&sp, "sp");  break;
      

      case 0x36:  ld_r8_n8(&memory[hl.reg16], "[hl]");  cycle_count += 1; break;
      

      case 0x3D:  dec_r8(&(af.reg.hi), "a");  break;
      

      case 0x3E:  ld_r8_n8(&(af.reg.hi), "a");  break;
      

      case 0x47:  ld_r8_r8(&(bc.reg.hi), "b", af.reg.hi, "a");  break;
      

      case 0x4F:  ld_r8_r8(&(bc.reg.lo), "c", af.reg.hi, "a");  break;
      

      case 0x57:  ld_r8_r8(&(de.reg.hi), "d", af.reg.hi, "a");  break;
      

      case 0x5F:  ld_r8_r8(&(de.reg.lo), "e", af.reg.hi, "a");  break;
      

      case 0x62:  ld_r8_r8(&(hl.reg.hi), "h", de.reg.hi, "d");  break;
      

      case 0x6B:  ld_r8_r8(&(hl.reg.lo), "l", de.reg.lo, "e");  break;
      

      case 0x78:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.hi, "b");  break;
      

      case 0x79:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.lo, "c");  break;
      

      case 0x7A:  ld_r8_r8(&(af.reg.hi), "a", de.reg.hi, "d");  break;
      

      case 0x7E:  ld_r8_mhl(&(af.reg.hi), "a", 0, ' '); break;
      

      case 0xA7:  and_r8_r8(&(af.reg.hi), "a", af.reg.hi, "a");  break;
      

      case 0xAB:  xor_r8_r8(&(af.reg.hi), "a", de.reg.lo, "e"); break;
      

      case 0xAF:  xor_r8_r8(&(af.reg.hi), "a", af.reg.hi, "a");  break;
      

      case 0xB0:  or_r8_r8(&(af.reg.hi), "a", bc.reg.hi, "b");  break;
      

      case 0xB1:  or_r8_r8(&(af.reg.hi), "a", bc.reg.lo, "c");  break;
      

      case 0xB8:  cp_r8_r8(af.reg.hi, "a", bc.reg.hi, "b"); break;
      

      case 0xC3:  jp_cc(true, ""); break;
      

      case 0xC5:  push_r16(bc, "bc"); break;
      

      case 0xC8:  ret_cc(zf, "z");  break;
      

      case 0xC9:  ret();  break;
      

      case 0xCA:  jp_cc(zf, "z"); break;
      

      case 0xCB:
        pc.reg16++;
        opcode = memory[pc.reg16];
        switch (opcode) {
          case 0x87:  res_bitn_r8(1, &(af.reg.hi), "a");  break;
          
          case 0xCF:  set_bitn_r8(1, &(af.reg.hi), "a");  break;

          default:
            fprintf(stderr, "Unknown CB prefix instruction");
            exit(1);
        }
        break;
      

      case 0xCD:  call_n16(); continue;
      
      
      case 0xD1:  pop_r16(&de, "de"); break;
      

      case 0xD2:  jp_cc(!cf, "nc"); break;
      

      case 0xD5:  push_r16(de, "de");  break;
      

      case 0xE0:  ldh_m8_r8(af.reg.hi, "a"); break;
      

      case 0xE1:  pop_r16(&hl, "hl"); break;
      

      case 0xE2:  ld_mc_r8(af.reg.hi, "a"); break;
      

      case 0xE5:  push_r16(hl, "hl"); break;
      

      case 0xE6:  and_r8_n8(&(af.reg.hi), "a"); break;
      

      case 0xEA:  ld_m16_r8(af.reg.hi, "a"); break;
      

      case 0xF0:  ldh_r8_m8(&(af.reg.hi), "a"); break;
      

      case 0xF1:
        pop_r16(&af, "af");
        zf = (af.reg.lo & 0x80) >> 7;
        nf = (af.reg.lo & 0x40) >> 6;
        hf = (af.reg.lo & 0x20) >> 5;
        cf = (af.reg.lo & 0x10) >> 4;
        break;
      

      case 0xF3:  interrupts(false);  break;
      

      case 0xF5:
        af.reg.lo = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
        push_r16(af, "af");
        break;
      

      case 0xFA:  ld_r8_m16(&(af.reg.hi), "a"); break;
      

      case 0xFE:  cp_r8_n8(af.reg.hi, "a");  break;
      

      default:
        fprintf(stderr, "Unimplemented opcode\n");
        exit(1);
    }

    pc.reg16++;
  } while(opcode != 0x10);

  return 0;
}