#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"

#define GENERATE_LOGS true
#define LOG_FILE stderr



static register16_t af, bc, de, hl, sp, pc;
static uint8_t zf, nf, hf, cf;
static uint8_t cycle_count = 0;


static inline void nop()
{
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "nop\n");
  #endif
}

static inline void jr_cc(uint8_t cc)
{
  uint8_t n8 = memory[++pc.reg16];
  if (cc)
  {
    pc.reg16 = (int16_t) pc.reg16 + (int8_t) n8;
    cycle_count += 1;
  }
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "jrz\t%d\n", (int8_t) n8);
  #endif
}


static inline void ld_r8_n8(register8_t *r8)
{
  *r8 = memory[++pc.reg16];
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\tb,\t%02X\n", *r8);
  #endif
}


static inline void ld_r16_n16(register16_t *r16)
{
  r16->reg.hi = memory[++pc.reg16];
  r16->reg.lo = memory[++pc.reg16];
  cycle_count += 3;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\tbc,\t%04X\n", r16->reg16);
  #endif
}

static inline void ld_m16_r8(register8_t *r8, const char *name)
{
  register16_t m16;
  m16.reg.lo = memory[++pc.reg16];
  m16.reg.hi = memory[++pc.reg16];
  memory[m16.reg16] = *r8;
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "ld\t[%04X],\t%s", m16.reg16, name);
  fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", m16.reg16, memory[m16.reg16]);
  #endif
}

static inline void xor_r8_r8(register8_t *r8l, register8_t *r8r, const char *namel, const char *namer)
{
  *r8l ^= *r8r;
  zf = !(*r8l);
  nf = 0; hf = 0; cf =0;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "xor\t%s\t%s", namel, namer);
  fprintf(LOG_FILE, "\t\t%s:\t%02X\tznhc:\t%d%d%d%d\n", namel, *r8l, zf, nf, hf, cf);
  #endif
}

static inline void jp()
{
  uint16_t n16 = pc.reg16;
  pc.reg.lo = memory[n16 + 1];
  pc.reg.hi = memory[n16 + 2];
  cycle_count += 4;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "jp\t%04X\n", pc.reg16);
  #endif
}

static inline void dec_r8(register8_t *r8)
{
  hf = !((*r8) & 0xf);
  (*r8)--;
  zf = !(*r8);
  nf = 1;
  cycle_count += 1;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "dec\tb\n");
  #endif
}

static inline void cp_r8_n8(register8_t *r8, const char *name)
{
  uint8_t n8 = memory[++pc.reg16];
  zf = *r8 == n8;
  nf = 1;
  hf = ((*r8 & 0x0f + n8 % 0x0f) & 0x10) >> 4;
  cf = *r8 < n8;
  cycle_count += 2;

  #ifdef GENERATE_LOGS
  fprintf(LOG_FILE, "cp\t%s,\t0x%02X", name, n8);
  fprintf(LOG_FILE, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
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
  bool interrupts_enabled = true;


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
      case 0x00:
        nop();
        break;
      

      case 0x01:
        ld_r16_n16(&bc);
        break;
      

      case 0x05:
        dec_r8(&(bc.reg.hi));
        break;
      

      case 0x06:
        ld_r8_n8(&(bc.reg.hi));
        break;
      

      // case 0x09:
      //   hl.reg16 += bc.reg16;
      //   nf = 0;
      //   hf = (hl.reg16 & 0x0fff + bc.reg16 & 0x0fff) > 0x0fff;
      //   cf = (hl.reg16 & 0x0fff + bc.reg16 & 0x0fff) > 0x0fff;
      //   fprintf(stderr, "add\thl,\tbc");
      //   fprintf(stderr, "\t\thl:\t%02X\n", hl.reg16);
      //   cycle_count += 2;
      //   break;
      

      // case 0x0B:
      //   bc.reg16--;
      //   fprintf(stderr, "dec\tbc\t");
      //   fprintf(stderr, "\t\tbc:\t%04X\n", bc.reg16);
      //   cycle_count += 2;
      //   break;
      

      // case 0x0C:
      //   bc.reg.lo++;
      //   zf = !bc.reg.lo;
      //   nf = 0;
      //   hf = !(bc.reg.lo & 0x0f);
      //   fprintf(stderr, "inc\tc\t");
      //   fprintf(stderr, "\t\tc:\t%02X\tznhc:\t%d%d%d%d\n", bc.reg.lo, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      // case 0x0E:
      //   bc.reg.lo = memory[++pc.reg16];
      //   fprintf(stderr, "ld\tc,\t%02X\n", bc.reg.lo);
      //   cycle_count += 2;
      //   break;
      

      // case 0x11:
      //   de.reg.lo = memory[++pc.reg16];
      //   de.reg.hi = memory[++pc.reg16];
      //   fprintf(stderr, "ld\tde,\t%04X\n", de.reg16);
      //   cycle_count += 3;
      //   break;
      

      // case 0x12:
      //   memory[de.reg16] = af.reg.hi;
      //   fprintf(stderr, "ld\t[de],\ta");
      //   fprintf(stderr, "\t\t[de]:\t%02X\n", memory[de.reg16]);
      //   cycle_count += 2;
      //   break;
      

      // case 0x13:
      //   de.reg16++;
      //   fprintf(stderr, "inc\tde\t");
      //   fprintf(stderr, "\t\tde:\t%02X\n", de.reg16);
      //   cycle_count += 2;
      //   break;

      
      // case 0x15:
      //   hf = !(de.reg.hi & 0x0f);
      //   de.reg.hi--;
      //   zf = !de.reg.hi;
      //   nf = 1;

      //   fprintf(stderr, "dec\td\t");
      //   fprintf(stderr, "\t\td:\t%02X\tznhc:\t%d%d%d%d\n", de.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      case 0x18:  jr_cc(true);  break;
      

      // case 0x1D:
      //   hf = !(de.reg.lo & 0x0f);
      //   de.reg.lo--;
      //   zf = !de.reg.lo;
      //   nf = 1;

      //   fprintf(stderr, "dec\te\t");
      //   fprintf(stderr, "\t\te:\t%02X\tznhc:\t%d%d%d%d\n", de.reg.lo, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

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
      

      // case 0x20:
      //   pc.reg16++;
      //   imm8 = memory[pc.reg16];
      //   if (!zf) {
      //     pc.reg16 = (int16_t) pc.reg16 + (int8_t) imm8;;
      //     cycle_count += 1;
      //   }

      //   fprintf(stderr, "jrnz\t%d\n", (int8_t) imm8);
      //   cycle_count += 2;
      //   break;
      

      // case 0x21:
      //   hl.reg.lo = memory[++pc.reg16];
      //   hl.reg.hi = memory[++pc.reg16];

      //   fprintf(stderr, "ld\thl,\t%04X\n", hl.reg16);
      //   cycle_count += 3;
      //   break;
      

      // case 0x22:
      //   memory[hl.reg16++] = af.reg.hi;
      //   fprintf(stderr, "ld\t[hl+],\ta\n");
      //   cycle_count += 2;
      //   break;
      

      // case 0x23:
      //   hl.reg16++;
      //   fprintf(stderr, "inc\thl\t");
      //   fprintf(stderr, "\t\thl:\t%04X\n", hl.reg16);
      //   cycle_count += 2;
      //   break;
      

      // case 0x26:
      //   hl.reg.hi = memory[++pc.reg16];
      //   fprintf(stderr, "ld\th,\t%02X\n", hl.reg.hi);
      //   cycle_count += 2;
      //   break;
      

      case 0x28:  jr_cc(zf);  break;
      

      // case 0x2A:
      //   af.reg.hi = memory[hl.reg16++];
      //   fprintf(stderr, "ld\ta,\t[hl+]");
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 2;
      //   break;
      

      // case 0x2B:
      //   hl.reg16--;
      //   fprintf(stderr, "dec\thl\t");
      //   fprintf(stderr, "\t\thl:\t%04X\n", hl.reg16);
      //   cycle_count += 2;
      //   break;
      

      // case 0x31:
      //   sp.reg.lo = memory[++pc.reg16];
      //   sp.reg.hi = memory[++pc.reg16];

      //   fprintf(stderr, "ld\tsp,\t%04X\n", sp.reg16);
      //   cycle_count += 3;
      //   break;
      

      // case 0x36:
      //   memory[hl.reg16] = memory[++pc.reg16];
      //   fprintf(stderr, "ld\t[hl],\t%02X\n", memory[hl.reg16]);
      //   cycle_count += 3;
      //   break;
      

      // case 0x3D:
      //   hf = !(af.reg.hi & 0x0f);
      //   af.reg.hi--;
      //   zf = !af.reg.hi;
      //   nf = 1;

      //   fprintf(stderr, "dec\ta\t");
      //   fprintf(stderr, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      // case 0x3E:
      //   pc.reg16++;
      //   af.reg.hi = memory[pc.reg16];

      //   fprintf(stderr, "ld\ta,\t%02X\n", af.reg.hi);
      //   cycle_count += 2;
      //   break;
      

      // case 0x47:
      //   bc.reg.hi = af.reg.hi;
      //   fprintf(stderr, "ld\tb,\ta");
      //   fprintf(stderr, "\t\tb:\t%02X\n", bc.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x4F:
      //   bc.reg.lo = af.reg.hi;
      //   fprintf(stderr, "ld\tc,\ta");
      //   fprintf(stderr, "\t\tc:\t%02X\n", bc.reg.lo);
      //   cycle_count += 1;
      //   break;
      

      // case 0x57:
      //   de.reg.hi = af.reg.hi;
      //   fprintf(stderr, "ld\td,\ta");
      //   fprintf(stderr, "\t\td:\t%02X\n", de.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x5F:
      //   de.reg.lo = af.reg.hi;
      //   fprintf(stderr, "ld\te,\ta");
      //   fprintf(stderr, "\t\te:\t%02X\n", de.reg.lo);
      //   cycle_count += 1;
      //   break;
      

      // case 0x62:
      //   hl.reg.hi = de.reg.hi;
      //   fprintf(stderr, "ld\th,\td");
      //   fprintf(stderr, "\t\th:\t%02X\n", hl.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x6B:
      //   hl.reg.lo = de.reg.lo;
      //   fprintf(stderr, "ld\tl,\te");
      //   fprintf(stderr, "\t\tl:\t%02X\n", hl.reg.lo);
      //   cycle_count += 1;
      //   break;
      

      // case 0x78:
      //   af.reg.hi = bc.reg.hi;
      //   fprintf(stderr, "ld\ta,\tb");
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x79:
      //   af.reg.hi = bc.reg.lo;
      //   fprintf(stderr, "ld\ta,\tc");
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x7A:
      //   af.reg.hi = de.reg.hi;
      //   fprintf(stderr, "ld\ta,\td");
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 1;
      //   break;
      

      // case 0x7E:
      //   af.reg.hi = memory[hl.reg16];
      //   fprintf(stderr, "ld\ta,\t[hl]");
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 2;
      //   break;
      

      // case 0xA7:
      //   af.reg.hi &= af.reg.hi;
      //   zf = !af.reg.hi;
      //   nf = 0;
      //   hf = 1;
      //   cf = 0;

      //   fprintf(stderr, "and\ta,\ta");
      //   fprintf(stderr, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      // case 0xAB:
      //   af.reg.hi ^= de.reg.lo;
      //   zf = !af.reg.hi;
      //   nf = 0;
      //   hf = 0;
      //   cf = 0;
      //   fprintf(stderr, "xor\ta,\te");
      //   fprintf(stderr, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      case 0xAF:  xor_r8_r8(&(af.reg.hi), &(af.reg.hi), "a", "a");  break;
      

      // case 0xB0:
      //   af.reg.hi |= bc.reg.hi;
      //   zf = !af.reg.hi;
      //   nf = 0;
      //   hf = 0;
      //   cf = 0;
      //   fprintf(stderr, "or\ta,\tb");
      //   fprintf(stderr, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      // case 0xB1:
      //   af.reg.hi |= bc.reg.lo;
      //   zf = !af.reg.hi;
      //   nf = 0;
      //   hf = 0;
      //   cf = 0;
      //   fprintf(stderr, "or\ta,\tc");
      //   fprintf(stderr, "\t\ta:\t%02X,\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      // case 0xB8:
      //   zf = af.reg.hi == bc.reg.hi;
      //   nf = 1;
      //   hf = (af.reg.hi & 0xf + bc.reg.hi & 0xf) & 0x10;
      //   cf = af.reg.hi < bc.reg.hi;
        
      //   fprintf(stderr, "cp\ta,\tb");
      //   fprintf(stderr, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
      //   cycle_count += 1;
      //   break;
      

      case 0xC3:  jp(); continue;
      

      // case 0xC5:
      //   memory[sp.reg16--] = bc.reg.hi;
      //   memory[sp.reg16--] = bc.reg.lo;
      //   fprintf(stderr, "push\tbc\n");
      //   cycle_count += 4;
      //   break;
      

      // case 0xC8:
      //   if (zf) {
      //     pc.reg.lo = memory[++sp.reg16];
      //     pc.reg.hi = memory[++sp.reg16];
      //     cycle_count += 3;
      //   }
      //   fprintf(stderr, "retz\n");
      //   cycle_count += 2;
      //   break;
      

      // case 0xC9:
      //   pc.reg.lo = memory[++sp.reg16];
      //   pc.reg.hi = memory[++sp.reg16];

      //   fprintf(stderr, "ret\n");
      //   cycle_count += 4;
      //   break;
      

      // case 0xCA:
      //   imm16.reg.lo = memory[++pc.reg16];
      //   imm16.reg.hi = memory[++pc.reg16];

      //   fprintf(stderr, "jpz\t%04X\n", imm16.reg16);
      //   if (zf) {
      //     pc.reg16 = imm16.reg16;
      //     continue;
      //   }
      //   break;
      

      // case 0xCB:
      //   pc.reg16++;
      //   opcode = memory[pc.reg16];
      //   switch (opcode) {
      //     case 0x87:
      //       af.reg.hi &= 0xFD;
      //       fprintf(stderr, "res\tbit1,\ta");
      //       fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //       cycle_count += 2;
      //       break;
          
      //     case 0xCF:
      //       af.reg.hi |= 0x02;
      //       fprintf(stderr, "set\tbit1,\ta");
      //       fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //       cycle_count += 2;
      //       break;

      //     default:
      //       fprintf(stderr, "Unknown CB prefix instruction");
      //       exit(1);
      //   }
      //   break;
      

      // case 0xCD:
      //   imm16.reg.lo = memory[pc.reg16 + 1];
      //   imm16.reg.hi = memory[pc.reg16 + 2];
      //   pc.reg16 += 2;
      //   memory[sp.reg16--] = pc.reg.hi;
      //   memory[sp.reg16--] = pc.reg.lo;
      //   pc.reg16 = imm16.reg16;

      //   fprintf(stderr, "call\t%04X\n", imm16.reg16);
      //   cycle_count += 6;
      //   continue;
      
      
      // case 0xD1:
      //   de.reg.lo = memory[++sp.reg16];
      //   de.reg.hi = memory[++sp.reg16];
      //   fprintf(stderr, "pop\tde\n");
      //   cycle_count += 3;
      //   break;
      

      // case 0xD2:
      //   imm16.reg.lo = memory[++pc.reg16];
      //   imm16.reg.hi = memory[++pc.reg16];

      //   fprintf(stderr, "jpnc\t%04X\n", imm16.reg16);
      //   if (!cf) {
      //     pc.reg16 = imm16.reg16;
      //     continue;
      //   }
      //   break;
      

      // case 0xD5:
      //   memory[sp.reg16--] = de.reg.hi;
      //   memory[sp.reg16--] = de.reg.lo;
      //   fprintf(stderr, "push\tde\n");
      //   cycle_count += 4;
      //   break;
      

      // case 0xE0:
      //   pc.reg16++;
      //   imm16.reg16 = memory[pc.reg16] + 0xff00;
      //   memory[imm16.reg16] = af.reg.hi;

      //   fprintf(stderr, "ldh\t[%04X],\ta", imm16.reg16);
      //   fprintf(stderr, "\t\t%04X:\t%02X\n", imm16.reg16, memory[imm16.reg16]);
      //   cycle_count += 3;
      //   break;
      

      // case 0xE1:
      //   hl.reg.lo = memory[++sp.reg16];
      //   hl.reg.hi = memory[++sp.reg16];
      //   fprintf(stderr, "pop\thl\n");
      //   cycle_count += 3;
      //   break;
      

      // case 0xE2:
      //   memory[0xff00 + bc.reg.lo] = af.reg.hi;
      //   fprintf(stderr, "ldh\t[c],\ta");
      //   fprintf(stderr, "\t\t[c]:\t%02X\n", memory[0xff00 + bc.reg.lo]);
      //   cycle_count += 2;
      //   break;
      

      // case 0xE5:
      //   memory[sp.reg16--] = hl.reg.hi;
      //   memory[sp.reg16--] = hl.reg.lo;
      //   fprintf(stderr, "push\thl\n");
      //   cycle_count += 4;
      //   break;
      

      // case 0xE6:
      //   pc.reg16++;
      //   imm8 = memory[pc.reg16];
      //   af.reg.hi &= imm8;

      //   fprintf(stderr, "and\ta,\t%02X", imm8);
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 2;
      //   break;
      

      case 0xEA:  ld_m16_r8(&(af.reg.hi), "a"); break;
      

      // case 0xF0:
      //   pc.reg16++;
      //   imm16.reg16 = 0xff00 + memory[pc.reg16];
      //   af.reg.hi = memory[imm16.reg16];

      //   fprintf(stderr, "ldh\ta,\t[%04X]", imm16.reg16);
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 3;
      //   break;
      

      // case 0xF1:
      //   af.reg.lo = memory[++sp.reg16];
      //   af.reg.hi = memory[++sp.reg16];
      //   zf = (af.reg.lo & 0x80) >> 7;
      //   nf = (af.reg.lo & 0x40) >> 6;
      //   hf = (af.reg.lo & 0x20) >> 5;
      //   cf = (af.reg.lo & 0x10) >> 4;
      //   fprintf(stderr, "pop\taf\t");
      //   fprintf(stderr, "\t\taf:\t%04X\tznhc:\t%d%d%d%d\n", af.reg16, zf, nf, hf, cf);
      //   cycle_count += 3;
      //   break;
      

      // case 0xF3:
      //   interrupts_enabled = false;
      //   fprintf(stderr, "di\n");
      //   cycle_count += 1;
      //   break;
      

      // case 0xF5:
      //   af.reg.lo = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
      //   memory[sp.reg16--] = af.reg.hi;
      //   memory[sp.reg16--] = af.reg.lo;
      //   fprintf(stderr, "push\taf\t");
      //   fprintf(stderr, "\t\taf:\t%04X\n", af.reg16);
      //   cycle_count += 4;
      //   break;
      

      // case 0xFA:
      //   imm16.reg.lo = memory[++pc.reg16];
      //   imm16.reg.hi = memory[++pc.reg16];
      //   af.reg.hi = memory[imm16.reg16];
      //   fprintf(stderr, "ld\ta,\t[%04X]", imm16.reg16);
      //   fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
      //   cycle_count += 4;
      //   break;
      

      case 0xFE:  cp_r8_n8(&(af.reg.hi), "a");  break;
      

      default:
        fprintf(stderr, "Unimplemented opcode\n");
        exit(1);
    }

    pc.reg16++;
  } while(opcode != 0x10);

  return 0;
}