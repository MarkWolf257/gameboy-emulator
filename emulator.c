#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"


register16_t af, bc, de, hl, sp, pc;
uint8_t zf, nf, hf, cf;
uint8_t memory[CART_SIZE], opcode;
bool interrupts_enabled = true;


int main()
{
  // Load ROM into memory
  FILE *fptr = fopen("rom.gb", "rb");
  if (fptr == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }

  if (fread(memory, 1, CART_SIZE, fptr) != CART_SIZE)
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
    fprintf(stderr, "%04X:\t", pc.reg16);
    opcode = memory[pc.reg16];

    switch (opcode)
    {
      case 0x00:
        fprintf(stderr, "nop\n");
        break;
      

      case 0x28:
        pc.reg16++;
        imm8 = memory[pc.reg16];
        pc.reg.lo += imm8;

        fprintf(stderr, "jrz\t0x%02X\n", imm8);
        break;
      

      case 0x3E:
        pc.reg16++;
        af.reg.hi = memory[pc.reg16];

        fprintf(stderr, "ld\ta,\t%02X\n", af.reg.hi);
        break;
      

      case 0xAF:
        af.reg.hi ^= af.reg.hi;
        fprintf(stderr, "xor\ta,\ta");
        fprintf(stderr, "\t\ta:\t%02X\n", af.reg.hi);
        break;
      

      case 0xC3:
        imm16.reg.lo = memory[pc.reg16 + 1];
        imm16.reg.hi = memory[pc.reg16 + 2];
        pc.reg16 = imm16.reg16;

        fprintf(stderr, "jp\t[%04X]\n", pc.reg16);
        continue;
      

      case 0xCD:
        imm16.reg.lo = memory[pc.reg16 + 1];
        imm16.reg.hi = memory[pc.reg16 + 2];
        memory[sp.reg16] = pc.reg16 + 2;
        sp.reg16--;
        pc.reg16 = imm16.reg16;

        fprintf(stderr, "call\t%04X\n", imm16.reg16);
        continue;
      

      case 0xE0:
        pc.reg16++;
        imm16.reg16 = memory[pc.reg16] + 0xff00;
        memory[imm16.reg16] = af.reg.hi;

        fprintf(stderr, "ld\t[%04X],\ta", imm16.reg16);
        fprintf(stderr, "\t\t%04X:\t%02X\n", imm16.reg16, memory[imm16.reg16]);
        break;
      

      case 0xEA:
        imm16.reg.lo = memory[pc.reg16 + 1];
        imm16.reg.hi = memory[pc.reg16 + 2];
        pc.reg16 += 2;
        memory[imm16.reg16] = af.reg.hi;

        fprintf(stderr, "ld\t[%04X],\ta", imm16.reg16);
        fprintf(stderr, "\t\t[%04X]:\t%02X\n", imm16.reg16, memory[imm16.reg16]);
        break;
      

      case 0xF3:
        interrupts_enabled = false;
        fprintf(stderr, "di\n");
        break;
      

      case 0xFE:
        pc.reg16++;
        imm8 = memory[pc.reg16];

        zf = af.reg.hi == imm8;
        nf = 1;
        hf = (af.reg.hi & 0xf + imm8 & 0xf) & 0x10;
        cf = af.reg.hi < imm8;
        
        fprintf(stderr, "cp\ta,\t0x%02X", imm8);
        fprintf(stderr, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
        break;
      

      default:
        fprintf(stderr, "Unimplemented opcode\n");
        exit(1);
    }

    pc.reg16++;
  } while(opcode != 0x10);

  return 0;
}