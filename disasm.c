#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"


int main()
{
  register16_t PC;
  uint8_t opcode;
  uint8_t memory[CART_SIZE];


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


  // Open output file
  FILE *fout = fopen("output.txt", "w");
  if (fout == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }


  // Initialize program counter
  PC.reg16 = 0;


  // Disassemble
  do {
    fprintf(fout, "%04X:\t", PC.reg16);
    opcode = memory[PC.reg16];

    switch (opcode)
    {
      case 0x00:
        fprintf(fout, "nop\n");
        break;
      
      default:
        fprintf(fout, "Unknown opcode: %02X", opcode);
    }

    PC.reg16++;
  } while (PC.reg16 != CART_SIZE - 1);
  

  fclose(*fout);
  return 0;
}