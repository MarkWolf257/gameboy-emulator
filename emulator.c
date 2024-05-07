#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"


register16_t af, bc, de, hl, sp, pc;
uint8_t memory[CART_SIZE];
uint8_t opcode;


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
  PC.reg.lo = memory[0x0102];
  PC.reg.hi = memory[0x0103];
  fprintf(stderr, "Entry Point:\t%04X", PC.reg16);


  do {
    fprintf(stderr, "%04X:\t", PC.reg16);
    opcode = memory[PC.reg16];

    switch (opcode)
    {
      case 0x00:
        fprintf(stderr, "nop\n");
        break;
      
      default:
        fprintf(stderr, "Unimplemented opcode\n");
        exit(1);
    }

    PC.reg16++;
  } while(opcode != 0x10);
}