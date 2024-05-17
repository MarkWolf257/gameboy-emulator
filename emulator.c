#define GENERATE_LOGS true
#define LOG_FILE stderr

#include "essentials.h"



// static inline void ld_m16_r8(register8_t r8, const char *name)
// {
//   register16_t m16;
//   m16.reg.lo = memory[++pc.reg16];
//   m16.reg.hi = memory[++pc.reg16];
//   memory[m16.reg16] = r8;
//   cycle_count += 4;

//   #ifdef GENERATE_LOGS
//   fprintf(LOG_FILE, "ld\t[%04X],\t%s", m16.reg16, name);
//   fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", m16.reg16, memory[m16.reg16]);
//   #endif
// }



// static inline void ld_r8_m16(register8_t *r8, const char *name)
// {
//   register16_t m16;
//   m16.reg.lo = memory[++pc.reg16];
//   m16.reg.hi = memory[++pc.reg16];
//   *r8 = memory[m16.reg16];
//   cycle_count += 4;

//   #ifdef GENERATE_LOGS
//   fprintf(LOG_FILE, "ld\t%s,\t[%04X]", name, m16.reg16);
//   fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
//   #endif
// }

// static inline void ldh_r8_m8(register8_t *r8, const char *name)
// {
//   register16_t m16;
//   m16.reg16 = 0xff00 + memory[++pc.reg16];
//   *r8 = memory[m16.reg16];
//   cycle_count += 3;

//   #ifdef GENERATE_LOGS
//   fprintf(LOG_FILE, "ldh\t%s,\t[%04X]", name, m16.reg16);
//   fprintf(LOG_FILE, "\t\t%s:\t%02X\n", name, *r8);
//   #endif
// }

// static inline void ldh_m8_r8(register8_t r8, const char *name)
// {
//   register16_t m16;
//   m16.reg16 = memory[++pc.reg16] | 0xff00;
//   memory[m16.reg16] = r8;
//   cycle_count += 3;

//   #ifdef GENERATE_LOGS
//   fprintf(LOG_FILE, "ldh\t[%04X],\t%s", m16.reg16, name);
//   fprintf(LOG_FILE, "\t\t[%04X]:\t%02X\n", m16.reg16, memory[m16.reg16]);
//   #endif
// }

// static inline void ld_mc_r8(register8_t r8, const char *name)
// {
//   memory[bc.reg.lo + 0xff00] = r8;
//   cycle_count += 2;

//   #ifdef GENERATE_LOGS
//   fprintf(LOG_FILE, "ld\t[c],\t%s", name);
//   fprintf(LOG_FILE, "\t\tc:\t%02X\t[c]:\t%02X\n", bc.reg.lo, memory[bc.reg.lo + 0xff00]);
//   #endif
// }


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
      case 0x00:  //nop
        cycle_count += 1;

        #ifdef GENERATE_LOGS
        fprintf(LOG_FILE, "nop\n");
        #endif;

        break;

      // case 0x01:  ld_r16_n16(&bc, "bc");  break;
      // case 0x11:  ld_r16_n16(&de, "de");  break;
      // case 0x21:  ld_r16_n16(&hl, "hl");  break;
      // case 0x31:  ld_r16_n16(&sp, "sp");  break;

      // case 0x02:  ld_mr16_a(&bc, "bc"); break;
      // case 0x12:  ld_mr16_a(&de, "de"); break;
      // case 0x22:  ld_mr16_a(&hl, "hl+");  hl++; break;
      // case 0x32:  ld_mr16_a(&hl, "hl-");  hl--; break;

      // case 0x0A:  ld_a_mr16(&bc, "bc"); break;
      // case 0x1A:  ld_a_mr16(&de, "de"); break;
      // case 0x2A:  ld_a_mr16(&hl, "hl+");  hl++; break;
      // case 0x3A:  ld_a_mr16(&hl, "hl-");  hl--; break;

      // case 0x08:  fprintf("Unimplemented Opcode 0x08");  exit(1);

      // case 0x03:  inc_r16(&bc, "bc"); break;
      // case 0x13:  inc_r16(&de, "de"); break;
      // case 0x23:  inc_r16(&hl, "hl"); break;
      // case 0x33:  inc_r16(&sp, "sp"); break;

      // case 0x0B:  dec_r16(&bc, "bc"); break;
      // case 0x1B:  dec_r16(&de, "de"); break;
      // case 0x2B:  dec_r16(&hl, "hl"); break;
      // case 0x3B:  dec_r16(&sp, "sp"); break;

      // case 0x09:  add_hl_r16(bc, "bc"); break;
      // case 0x19:  add_hl_r16(de, "de"); break;
      // case 0x29:  add_hl_r16(hl, "hl"); break;
      // case 0x39:  add_hl_r16(sp, "sp"); break;

      // case 0x04:  inc_r8(&(bc.reg.hi), "b");  break;
      // case 0x0C:  inc_r8(&(bc.reg.lo), "c");  break;
      // case 0x14:  inc_r8(&(de.reg.hi), "d");  break;
      // case 0x1C:  inc_r8(&(de.reg.lo), "e");  break;
      // case 0x24:  inc_r8(&(hl.reg.hi), "h");  break;
      // case 0x2C:  inc_r8(&(hl.reg.lo), "l");  break;
      // case 0x34:  fprintf("Unimplemented Opcode 0x34"); exit(1);
      // case 0x3C:  inc_r8(&(af.reg.hi), "a");  break;

      // case 0x05:  dec_r8(&(bc.reg.hi), "b");  break;
      // case 0x0D:  dec_r8(&(bc.reg.lo), "c");  break;
      // case 0x15:  dec_r8(&(de.reg.hi), "d");  break;
      // case 0x1D:  dec_r8(&(de.reg.lo), "e");  break;
      // case 0x25:  dec_r8(&(hl.reg.hi), "h");  break;
      // case 0x2D:  dec_r8(&(hl.reg.lo), "l");  break;
      // case 0x35:  fprintf("Unimplemented Opcode 0x35"); exit(1);
      // case 0x3D:  dec_r8(&(af.reg.hi), "a");  break;

      // case 0x06:  ld_r8_n8(&(bc.reg.hi), "b");  break;
      // case 0x0E:  ld_r8_n8(&(bc.reg.lo), "c");  break;
      // case 0x16:  ld_r8_n8(&(de.reg.hi), "d");  break;
      // case 0x1E:  ld_r8_n8(&(de.reg.lo), "e");  break;
      // case 0x26:  ld_r8_n8(&(hl.reg.hi), "h");  break;
      // case 0x26:  ld_r8_n8(&(hl.reg.lo), "l");  break;
      // case 0x36: 
      //   ld_r8_n8(&memory[hl.reg16], "[hl]"); 
      //   cycle_count += 1;
      //   break;
      // case 0x3E:  ld_r8_n8(&(af.reg.hi), "a");  break;

      // case 0x18:  jr_cc(true, "");  break;
      // case 0x20:  jr_cc(!zf, "nz"); break;
      // case 0x28:  jr_cc(zf, "z"); break;
      // case 0x30:  jr_cc(!cf, "nc"); break;
      // case 0x38:  jr_cc(cf, "c"); break;

      // case 0x10:  fprintf("stop instruction not implemented");  exit(1);
      


      // case 0x40:  ld_r8_r8(&(bc.reg.hi), "b", bc.reg.hi, "b");  break;
      // case 0x41:  ld_r8_r8(&(bc.reg.hi), "b", bc.reg.lo, "c");  break;
      // case 0x42:  ld_r8_r8(&(bc.reg.hi), "b", de.reg.hi, "d");  break;
      // case 0x43:  ld_r8_r8(&(bc.reg.hi), "b", de.reg.lo, "e");  break;
      // case 0x44:  ld_r8_r8(&(bc.reg.hi), "b", hl.reg.hi, "h");  break;
      // case 0x45:  ld_r8_r8(&(bc.reg.hi), "b", hl.reg.lo, "l");  break;
      // case 0x46:  ld_r8_mhl(&(bc.reg.hi), "b");  break;
      // case 0x47:  ld_r8_r8(&(bc.reg.hi), "b", af.reg.hi, "a");  break;

      // case 0x48:  ld_r8_r8(&(bc.reg.lo), "c", bc.reg.hi, "b");  break;
      // case 0x49:  ld_r8_r8(&(bc.reg.lo), "c", bc.reg.lo, "c");  break;
      // case 0x4A:  ld_r8_r8(&(bc.reg.lo), "c", de.reg.hi, "d");  break;
      // case 0x4B:  ld_r8_r8(&(bc.reg.lo), "c", de.reg.lo, "e");  break;
      // case 0x4C:  ld_r8_r8(&(bc.reg.lo), "c", hl.reg.hi, "h");  break;
      // case 0x4D:  ld_r8_r8(&(bc.reg.lo), "c", hl.reg.lo, "l");  break;
      // case 0x4E:  ld_r8_mhl(&(bc.reg.lo), "c");  break;
      // case 0x4F:  ld_r8_r8(&(bc.reg.lo), "c", af.reg.hi, "a");  break;

      // case 0x50:  ld_r8_r8(&(de.reg.hi), "d", bc.reg.hi, "b");  break;
      // case 0x51:  ld_r8_r8(&(de.reg.hi), "d", bc.reg.lo, "c");  break;
      // case 0x52:  ld_r8_r8(&(de.reg.hi), "d", de.reg.hi, "d");  break;
      // case 0x53:  ld_r8_r8(&(de.reg.hi), "d", de.reg.lo, "e");  break;
      // case 0x54:  ld_r8_r8(&(de.reg.hi), "d", hl.reg.hi, "h");  break;
      // case 0x55:  ld_r8_r8(&(de.reg.hi), "d", hl.reg.lo, "l");  break;
      // case 0x56:  ld_r8_mhl(&(de.reg.hi), "d");  break;
      // case 0x57:  ld_r8_r8(&(de.reg.hi), "d", af.reg.hi, "a");  break;

      // case 0x58:  ld_r8_r8(&(de.reg.lo), "e", bc.reg.hi, "b");  break;
      // case 0x59:  ld_r8_r8(&(de.reg.lo), "e", bc.reg.lo, "c");  break;
      // case 0x5A:  ld_r8_r8(&(de.reg.lo), "e", de.reg.hi, "d");  break;
      // case 0x5B:  ld_r8_r8(&(de.reg.lo), "e", de.reg.lo, "e");  break;
      // case 0x5C:  ld_r8_r8(&(de.reg.lo), "e", hl.reg.hi, "h");  break;
      // case 0x5D:  ld_r8_r8(&(de.reg.lo), "e", hl.reg.lo, "l");  break;
      // case 0x5E:  ld_r8_mhl(&(de.reg.lo), "e");  break;
      // case 0x5F:  ld_r8_r8(&(de.reg.lo), "e", af.reg.hi, "a");  break;

      // case 0x60:  ld_r8_r8(&(hl.reg.hi), "h", bc.reg.hi, "b");  break;
      // case 0x61:  ld_r8_r8(&(hl.reg.hi), "h", bc.reg.lo, "c");  break;
      // case 0x62:  ld_r8_r8(&(hl.reg.hi), "h", de.reg.hi, "d");  break;
      // case 0x63:  ld_r8_r8(&(hl.reg.hi), "h", de.reg.lo, "e");  break;
      // case 0x64:  ld_r8_r8(&(hl.reg.hi), "h", hl.reg.hi, "h");  break;
      // case 0x65:  ld_r8_r8(&(hl.reg.hi), "h", hl.reg.lo, "l");  break;
      // case 0x66:  ld_r8_mhl(&(hl.reg.hi), "h");  break;
      // case 0x67:  ld_r8_r8(&(hl.reg.hi), "h", af.reg.hi, "a");  break;

      // case 0x68:  ld_r8_r8(&(hl.reg.lo), "l", bc.reg.hi, "b");  break;
      // case 0x69:  ld_r8_r8(&(hl.reg.lo), "l", bc.reg.lo, "c");  break;
      // case 0x6A:  ld_r8_r8(&(hl.reg.lo), "l", de.reg.hi, "d");  break;
      // case 0x6B:  ld_r8_r8(&(hl.reg.lo), "l", de.reg.lo, "e");  break;
      // case 0x6C:  ld_r8_r8(&(hl.reg.lo), "l", hl.reg.hi, "h");  break;
      // case 0x6D:  ld_r8_r8(&(hl.reg.lo), "l", hl.reg.lo, "l");  break;
      // case 0x6E:  ld_r8_mhl(&(hl.reg.lo), "l");  break;
      // case 0x6F:  ld_r8_r8(&(hl.reg.lo), "l", af.reg.hi, "a");  break;

      // case 0x70:  ld_mhl_r8(bc.reg.hi, "b");  break;
      // case 0x71:  ld_mhl_r8(bc.reg.lo, "c");  break;
      // case 0x72:  ld_mhl_r8(de.reg.hi, "d");  break;
      // case 0x73:  ld_mhl_r8(de.reg.lo, "e");  break;
      // case 0x74:  ld_mhl_r8(hl.reg.hi, "h");  break;
      // case 0x75:  ld_mhl_r8(hl.reg.lo, "l");  break;
      // case 0x77:  ld_mhl_r8(af.reg.hi, "a");  break;

      // case 0x78:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.hi, "b");  break;
      // case 0x79:  ld_r8_r8(&(af.reg.hi), "a", bc.reg.lo, "c");  break;
      // case 0x7A:  ld_r8_r8(&(af.reg.hi), "a", de.reg.hi, "d");  break;
      // case 0x7B:  ld_r8_r8(&(af.reg.hi), "a", de.reg.lo, "e");  break;
      // case 0x7C:  ld_r8_r8(&(af.reg.hi), "a", hl.reg.hi, "h");  break;
      // case 0x7D:  ld_r8_r8(&(af.reg.hi), "a", hl.reg.lo, "l");  break;
      // case 0x7E:  ld_r8_mhl(&(af.reg.hi), "a");  break;
      // case 0x7F:  ld_r8_r8(&(af.reg.hi), "a", af.reg.hi, "a");  break;

      // case 0x76:  fprintf("halt instruction not implemented");  exit(1);



      // case 0x80:  add_a_r8(bc.reg.hi, "b", false); break;
      // case 0x81:  add_a_r8(bc.reg.lo, "c", false); break;
      // case 0x82:  add_a_r8(de.reg.hi, "d", false); break;
      // case 0x83:  add_a_r8(de.reg.lo, "e", false); break;
      // case 0x84:  add_a_r8(hl.reg.hi, "h", false); break;
      // case 0x85:  add_a_r8(hl.reg.lo, "l", false); break;
      // case 0x86:
      //   add_a_r8(memory[hl.reg16], "[hl]", false);
      //   cycle_count += 1;
      //   break;
      // case 0x87:  add_a_r8(af.reg.hi, "a", false); break;

      // case 0x88:  add_a_r8(bc.reg.hi, "b", true); break;
      // case 0x89:  add_a_r8(bc.reg.lo, "c", true); break;
      // case 0x8A:  add_a_r8(de.reg.hi, "d", true); break;
      // case 0x8B:  add_a_r8(de.reg.lo, "e", true); break;
      // case 0x8C:  add_a_r8(hl.reg.hi, "h", true); break;
      // case 0x8D:  add_a_r8(hl.reg.lo, "l", true); break;
      // case 0x8E:
      //   add_a_r8(memory[hl.reg16], "[hl]", true);
      //   cycle_count += 1;
      //   break;
      // case 0x8F:  add_a_r8(af.reg.hi, "a", true); break;

      // case 0x90:  sub_a_r8(bc.reg.hi, "b", false); break;
      // case 0x91:  sub_a_r8(bc.reg.lo, "c", false); break;
      // case 0x92:  sub_a_r8(de.reg.hi, "d", false); break;
      // case 0x93:  sub_a_r8(de.reg.lo, "e", false); break;
      // case 0x94:  sub_a_r8(hl.reg.hi, "h", false); break;
      // case 0x95:  sub_a_r8(hl.reg.lo, "l", false); break;
      // case 0x96:
      //   sub_a_r8(memory[hl.reg16], "[hl]", false);
      //   cycle_count += 1;
      //   break;
      // case 0x97:  sub_a_r8(af.reg.hi, "a", false); break;

      // case 0x98:  sub_a_r8(bc.reg.hi, "b", true); break;
      // case 0x99:  sub_a_r8(bc.reg.lo, "c", true); break;
      // case 0x9A:  sub_a_r8(de.reg.hi, "d", true); break;
      // case 0x9B:  sub_a_r8(de.reg.lo, "e", true); break;
      // case 0x9C:  sub_a_r8(hl.reg.hi, "h", true); break;
      // case 0x9D:  sub_a_r8(hl.reg.lo, "l", true); break;
      // case 0x9E:
      //   sub_a_r8(memory[hl.reg16], "[hl]", true);
      //   cycle_count += 1;
      //   break;
      // case 0x9F:  sub_a_r8(af.reg.hi, "a", true); break;

      // case 0xA0:  and_a_r8(bc.reg.hi, "b"); break;
      // case 0xA1:  and_a_r8(bc.reg.lo, "c"); break;
      // case 0xA2:  and_a_r8(de.reg.hi, "d"); break;
      // case 0xA3:  and_a_r8(de.reg.lo, "e"); break;
      // case 0xA4:  and_a_r8(hl.reg.hi, "h"); break;
      // case 0xA5:  and_a_r8(hl.reg.lo, "l"); break;
      // case 0xA6:
      //   and_a_r8(memory[hl.reg16], "[hl]");
      //   cycle_count += 1;
      //   break;
      // case 0xA7:  and_a_r8(af.reg.hi, "a"); break;

      // case 0xA8:  xor_a_r8(bc.reg.hi, "b"); break;
      // case 0xA9:  xor_a_r8(bc.reg.lo, "c"); break;
      // case 0xAA:  xor_a_r8(de.reg.hi, "d"); break;
      // case 0xAB:  xor_a_r8(de.reg.lo, "e"); break;
      // case 0xAC:  xor_a_r8(hl.reg.hi, "h"); break;
      // case 0xAD:  xor_a_r8(hl.reg.lo, "l"); break;
      // case 0xAE:
      //   xor_a_r8(memory[hl.reg16], "[hl]");
      //   cycle_count += 1;
      //   break;
      // case 0xAF:  xor_a_r8(af.reg.hi, "a"); break;

      // case 0xB0:  or_a_r8(bc.reg.hi, "b"); break;
      // case 0xB1:  or_a_r8(bc.reg.lo, "c"); break;
      // case 0xB2:  or_a_r8(de.reg.hi, "d"); break;
      // case 0xB3:  or_a_r8(de.reg.lo, "e"); break;
      // case 0xB4:  or_a_r8(hl.reg.hi, "h"); break;
      // case 0xB5:  or_a_r8(hl.reg.lo, "l"); break;
      // case 0xB6:
      //   or_a_r8(memory[hl.reg16], "[hl]");
      //   cycle_count += 1;
      //   break;
      // case 0xB7:  or_a_r8(af.reg.hi, "a"); break;

      // case 0xB8:  cp_a_r8(bc.reg.hi, "b"); break;
      // case 0xB9:  cp_a_r8(bc.reg.lo, "c"); break;
      // case 0xBA:  cp_a_r8(de.reg.hi, "d"); break;
      // case 0xBB:  cp_a_r8(de.reg.lo, "e"); break;
      // case 0xBC:  cp_a_r8(hl.reg.hi, "h"); break;
      // case 0xBD:  cp_a_r8(hl.reg.lo, "l"); break;
      // case 0xBE:
      //   cp_a_r8(memory[hl.reg16], "[hl]");
      //   cycle_count += 1;
      //   break;
      // case 0xBF:  cp_a_r8(af.reg.hi, "a"); break;



      // case 0xC6:  //add a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   add_a_r8(memory[pc.reg16], name, false);
      //   cycle_count += 1;
      //   break;
      // case 0xCE:  //adc a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   add_a_r8(memory[pc.reg16], name, true);
      //   cycle_count += 1;
      //   break;
      // case 0xD6:  //sub a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   sub_a_r8(memory[pc.reg16], name, false);
      //   cycle_count += 1;
      //   break;
      // case 0xDE:  //sbc a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   sub_a_r8(memory[pc.reg16], name, true);
      //   cycle_count += 1;
      //   break;
      // case 0xE6:  //and a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   and_a_r8(memory[pc.reg16], name);
      //   cycle_count += 1;
      //   break;
      // case 0xEE:  //xor a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   xor_a_r8(memory[pc.reg16], name);
      //   cycle_count += 1;
      //   break;
      // case 0xF6:  //or a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   or_a_r8(memory[pc.reg16], name);
      //   cycle_count += 1;
      //   break;
      // case 0xFE:  //cp a, n8
      //   char name[8];
      //   sprintf(name, "%02X", memory[++pc.reg16]);
      //   cp_a_r8(memory[pc.reg16], name);
      //   cycle_count += 1;
      //   break;
      
      // case 0xC0:  ret_cc(!zf, "nz");  break;
      // case 0xC8:  ret_cc(zf, "z");  break;
      // case 0xD0:  ret_cc(!cf, "nc");  break;
      // case 0xD8:  ret_cc(cf, "c");  break;
      // case 0xC9:  // ret
      //   ret_cc(true, "");
      //   cycle_count -= 1;
      //   break;
      // case 0xD9:  // reti
      //   interrupts_enabled = true;
      //   ret_cc(true, "i");
      //   cycle_count -= 1;
      //   break;
      
      // case 0xC2:  jp_cc_n16(!zf, "nz"); break;
      // case 0xCA:  jp_cc_n16(zf, "z"); break;
      // case 0xD2:  jp_cc_n16(!cf, "nc"); break;
      // case 0xDA:  jp_cc_n16(cf, "c"); break;
      // case 0xC3:  jp_cc_n16(true, "");  break;
      // case 0xE9:  // jp hl
      //   pc.reg16 = hl.reg16;
      //   cycle_count += 1;
      //   continue;
      
      // case 0xC4:  call_cc_n16(!zf, "nz"); break;
      // case 0xCC:  call_cc_n16(zf, "z"); break;
      // case 0xD4:  call_cc_n16(!cf, "nc"); break;
      // case 0xDC:  call_cc_n16(cf, "c"); break;
      // case 0xCD:  call_cc_n16(true, "");  break;

      // case 0xC7:  rst(0x00);  break;
      // case 0xCF:  rst(0x08);  break;
      // case 0xD7:  rst(0x10);  break;
      // case 0xDF:  rst(0x18);  break;
      // case 0xE7:  rst(0x20);  break;
      // case 0xEF:  rst(0x28);  break;
      // case 0xF7:  rst(0x30);  break;
      // case 0xFF:  rst(0x38);  break;

      // case 0xC1:  pop_r16(&bc, "bc");
      // case 0xD1:  pop_r16(&de, "de");
      // case 0xE1:  pop_r16(&hl, "hl");
      // case 0xF1:
      //   pop_r16(&af, "af");
      //   zf = (af.reg.lo & 0x80) >> 7;
      //   nf = (af.reg.lo & 0x40) >> 6;
      //   hf = (af.reg.lo & 0x20) >> 5;
      //   cf = (af.reg.lo & 0x10) >> 4;
      //   break;
      
      // case 0xC5:  push_r16(bc, "bc");
      // case 0xD5:  push_r16(de, "de");
      // case 0xE5:  push_r16(hl, "hl");
      // case 0xF5:
      //   af.reg.lo = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
      //   push_r16(af, "af");
      //   break;
      
      
      
      // case 0xCB:
      //   pc.reg16++;
      //   opcode = memory[pc.reg16];
      //   switch (opcode) {
      //     case 0x87:  res_bitn_r8(1, &(af.reg.hi), "a");  break;
          
      //     case 0xCF:  set_bitn_r8(1, &(af.reg.hi), "a");  break;

      //     default:
      //       fprintf(stderr, "Unknown CB prefix instruction");
      //       exit(1);
      //   }
      //   break;
      

      default:
        fprintf(stderr, "Unimplemented opcode\n");
        exit(1);
    }

    pc.reg16++;
  } while(opcode != 0x10);

  return 0;
}