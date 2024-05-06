#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


// 16-bit register
typedef union register16_t
{
  struct
  {
    uint8_t LO;
    uint8_t HI;
  } reg;

  uint16_t reg16;
} register16_t;


// Flag register
#define ZERO_FLAG AF.reg.HI & 0x80
#define SUBTRACT_FLAG AF.reg.HI & 0x40
#define HALF_CARRY_FLAG AF.reg.HI & 0x20
#define CARRY_FLAG AF.reg.HI & 0x10


register16_t AF, BC, DE, HL, SP, PC;
uint8_t opcode;
uint8_t memory[0x10000];


void dissassemble()
{
  while (1)
  {
    switch (opcode)
    {
      case 0x00:
        printf("NOP\n");
        break;
      
      case 0x01:
        
    }
  }
}


int main()
{
  // Load ROM
  FILE *fptr = fopen("rom.gb", "rb");
  if (fptr == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }

  if (fread(memory, 1, 0x10000, fptr) != 0x10000)
  {
    printf("Error! reading file");
    exit(1);
  }

  fclose(fptr);


  // Initialize registers
  PC.reg.LO = memory[0x102];
  PC.reg.HI = memory[0x103];
  printf("Entry Point: 0x%04X\n", PC.reg16);


  dissassemble();

  return 0;
}