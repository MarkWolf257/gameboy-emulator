#include "../include/emulator.h"
#include "../include/gbmem.h"
#include "../include/gbcpu.h"


static register16_t af, bc, de, hl, pc, sp;
static uint8_t zf, nf, hf, cf;
static uint8_t ext_ime;


#include "../include/iblk0.h"
#include "../include/iblk1.h"
#include "../include/iblk2.h"
#include "../include/iblk3.h"
#include "../include/iblkcb.h"


static uint8_t int_ime;


void init_gbcpu()
{
    af.reg16 = 0x01B0;
    bc.reg16 = 0x0013;
    de.reg16 = 0x00D8;
    hl.reg16 = 0x014D;
    pc.reg16 = 0x0100;
    sp.reg16 = 0xFFFE;

    zf = 1;
    nf = 0;
    hf = 1;
    cf = 1;

    ext_ime = 0;
    int_ime = 0;
}


// Executes instructions in parallel to one line of ppu
void gbcpu_process(const size_t cycles_to_execute)
{
    while (cycle_count < cycles_to_execute) {
        // Call interrupt if enabled
        // Two variables simulate the enable interrupt delay
        for (int i = 0; ext_ime && int_ime && i < 5; i++)
            if ((memory[0xffff] & (1 << i)) && (memory[0xff0f] & (1 << i)))
            {
                memory[sp.reg16--] = pc.reg.hi;
                memory[sp.reg16--] = pc.reg.lo;
                pc.reg16 = 0x0040 + i * 8;
                cycle_count += 5;

#ifdef GENERATE_LOGS
                fprintf(log_file, "\nINTERRUPT\t\t\t\tie:\t%02X\tif:\t%02X\n", memory[0xffff], memory[0xff0f]);
#endif // GENERATE_LOGS

                ext_ime = 0;
                memory[0xff0f] &= ~(1 << i);
            }
        int_ime = ext_ime;


#ifdef GENERATE_LOGS
        fprintf(log_file, "%04X:\t", pc.reg16);
#endif // GENERATE_LOGS

        uint8_t opcode = get_ro_mem(pc.reg16);

        char name[8];
        uint8_t n8;
        register16_t n16;

        switch (opcode)
        {
            //
            // BLOCK 0
            //
            case 0x00: // nop
                cycle_count += 1;
#ifdef GENERATE_LOGS
                fprintf(log_file, "nop\n");
#endif // GENERATE_LOGS
                break;

            case 0x01: ld_r16_n16(&bc, "bc"); break;
            case 0x11: ld_r16_n16(&de, "de"); break;
            case 0x21: ld_r16_n16(&hl, "hl"); break;
            case 0x31: ld_r16_n16(&sp, "sp"); break;

            case 0x02: ld_mr16_a(bc, "bc"); break;
            case 0x12: ld_mr16_a(de, "de"); break;
            case 0x22: ld_mr16_a(hl, "hl+"); hl.reg16++; break;
            case 0x32: ld_mr16_a(hl, "hl-"); hl.reg16--; break;

            case 0x0A: ld_a_mr16(bc, "bc"); break;
            case 0x1A: ld_a_mr16(de, "de"); break;
            case 0x2A: ld_a_mr16(hl, "hl+"); hl.reg16++; break;
            case 0x3A: ld_a_mr16(hl, "hl-"); hl.reg16--; break;

            // case 0x08:  fprintf("Unimplemented Opcode 0x08");  exit(1);

            case 0x03: inc_r16(&bc, "bc"); break;
            case 0x13: inc_r16(&de, "de"); break;
            case 0x23: inc_r16(&hl, "hl"); break;
            case 0x33: inc_r16(&sp, "sp"); break;

            case 0x0B: dec_r16(&bc, "bc"); break;
            case 0x1B: dec_r16(&de, "de"); break;
            case 0x2B: dec_r16(&hl, "hl"); break;
            case 0x3B: dec_r16(&sp, "sp"); break;

            case 0x09: add_hl_r16(bc, "bc"); break;
            case 0x19: add_hl_r16(de, "de"); break;
            case 0x29: add_hl_r16(hl, "hl"); break;
            case 0x39: add_hl_r16(sp, "sp"); break;

            case 0x04: inc_r8(&(bc.reg.hi), "b"); break;
            case 0x0C: inc_r8(&(bc.reg.lo), "c"); break;
            case 0x14: inc_r8(&(de.reg.hi), "d"); break;
            case 0x1C: inc_r8(&(de.reg.lo), "e"); break;
            case 0x24: inc_r8(&(hl.reg.hi), "h"); break;
            case 0x2C: inc_r8(&(hl.reg.lo), "l"); break;
            case 0x34: // inc [hl]
                inc_r8(get_rw_memptr(hl.reg16), "[hl]");
                cycle_count += 2;
                break;
            case 0x3C: inc_r8(&(af.reg.hi), "a"); break;

            case 0x05: dec_r8(&(bc.reg.hi), "b"); break;
            case 0x0D: dec_r8(&(bc.reg.lo), "c"); break;
            case 0x15: dec_r8(&(de.reg.hi), "d"); break;
            case 0x1D: dec_r8(&(de.reg.lo), "e"); break;
            case 0x25: dec_r8(&(hl.reg.hi), "h"); break;
            case 0x2D: dec_r8(&(hl.reg.lo), "l"); break;
            case 0x35: // dec [hl]
                dec_r8(get_rw_memptr(hl.reg16), "[hl]");
                cycle_count += 2;
                break;
            case 0x3D: dec_r8(&(af.reg.hi), "a"); break;

            case 0x06: ld_r8_n8(&(bc.reg.hi), "b"); break;
            case 0x0E: ld_r8_n8(&(bc.reg.lo), "c"); break;
            case 0x16: ld_r8_n8(&(de.reg.hi), "d"); break;
            case 0x1E: ld_r8_n8(&(de.reg.lo), "e"); break;
            case 0x26: ld_r8_n8(&(hl.reg.hi), "h"); break;
            case 0x2E: ld_r8_n8(&(hl.reg.lo), "l"); break;
            case 0x36: // ld [hl], n8
                ld_r8_n8(get_rw_memptr(hl.reg16), "[hl]");
                cycle_count += 1;
                break;
            case 0x3E:  ld_r8_n8(&(af.reg.hi), "a");  break;

            case 0x2F:  // cpl
                af.reg.hi = ~af.reg.hi;
                nf = 1; hf = 1;
                cycle_count += 1;
    #ifdef GENERATE_LOGS
                fprintf(log_file, "cpl\t\t");
                fprintf(log_file, "\t\ta:\t%02X\tznhc:\t%d%d%d%d\n", af.reg.hi, zf, nf, hf, cf);
    #endif // GENERATE_LOGS
                break;

            // case 0x3F:  // ccf
            //   nf = 0; hf = 0; cf = !cf;
            //   cycle_count += 1;
            //   #ifdef log_file
            //   fprintf(log_file, "ccf\t\t");
            //   fprintf(log_file, "\t\tznhc:\t%d%d%d%d\n", zf, nf, hf, cf);
            //   #endif
            //   break;

            case 0x18: jr_cc_n8(1, ""); break;
            case 0x20: jr_cc_n8(!zf, "nz"); break;
            case 0x28: jr_cc_n8(zf, "z"); break;
            // case 0x30: jr_cc_n8(!cf, "nc"); break;
            // case 0x38: jr_cc_n8(cf, "c"); break;

            // case 0x10: fprintf("stop instruction not implemented");  exit(1);



            //
            // BLOCK 1
            //
            case 0x40: ld_r8_r8(&(bc.reg.hi), "b", bc.reg.hi, "b"); break;
            case 0x41: ld_r8_r8(&(bc.reg.hi), "b", bc.reg.lo, "c"); break;
            case 0x42: ld_r8_r8(&(bc.reg.hi), "b", de.reg.hi, "d"); break;
            case 0x43: ld_r8_r8(&(bc.reg.hi), "b", de.reg.lo, "e"); break;
            case 0x44: ld_r8_r8(&(bc.reg.hi), "b", hl.reg.hi, "h"); break;
            case 0x45: ld_r8_r8(&(bc.reg.hi), "b", hl.reg.lo, "l"); break;
            case 0x46: ld_r8_mhl(&(bc.reg.hi), "b"); break;
            case 0x47: ld_r8_r8(&(bc.reg.hi), "b", af.reg.hi, "a"); break;

            case 0x48: ld_r8_r8(&(bc.reg.lo), "c", bc.reg.hi, "b"); break;
            case 0x49: ld_r8_r8(&(bc.reg.lo), "c", bc.reg.lo, "c"); break;
            case 0x4A: ld_r8_r8(&(bc.reg.lo), "c", de.reg.hi, "d"); break;
            case 0x4B: ld_r8_r8(&(bc.reg.lo), "c", de.reg.lo, "e"); break;
            case 0x4C: ld_r8_r8(&(bc.reg.lo), "c", hl.reg.hi, "h"); break;
            case 0x4D: ld_r8_r8(&(bc.reg.lo), "c", hl.reg.lo, "l"); break;
            case 0x4E: ld_r8_mhl(&(bc.reg.lo), "c"); break;
            case 0x4F: ld_r8_r8(&(bc.reg.lo), "c", af.reg.hi, "a"); break;

            case 0x50: ld_r8_r8(&(de.reg.hi), "d", bc.reg.hi, "b"); break;
            case 0x51: ld_r8_r8(&(de.reg.hi), "d", bc.reg.lo, "c"); break;
            case 0x52: ld_r8_r8(&(de.reg.hi), "d", de.reg.hi, "d"); break;
            case 0x53: ld_r8_r8(&(de.reg.hi), "d", de.reg.lo, "e"); break;
            case 0x54: ld_r8_r8(&(de.reg.hi), "d", hl.reg.hi, "h"); break;
            case 0x55: ld_r8_r8(&(de.reg.hi), "d", hl.reg.lo, "l"); break;
            case 0x56: ld_r8_mhl(&(de.reg.hi), "d"); break;
            case 0x57: ld_r8_r8(&(de.reg.hi), "d", af.reg.hi, "a"); break;

            case 0x58: ld_r8_r8(&(de.reg.lo), "e", bc.reg.hi, "b"); break;
            case 0x59: ld_r8_r8(&(de.reg.lo), "e", bc.reg.lo, "c"); break;
            case 0x5A: ld_r8_r8(&(de.reg.lo), "e", de.reg.hi, "d"); break;
            case 0x5B: ld_r8_r8(&(de.reg.lo), "e", de.reg.lo, "e"); break;
            case 0x5C: ld_r8_r8(&(de.reg.lo), "e", hl.reg.hi, "h"); break;
            case 0x5D: ld_r8_r8(&(de.reg.lo), "e", hl.reg.lo, "l"); break;
            case 0x5E: ld_r8_mhl(&(de.reg.lo), "e"); break;
            case 0x5F: ld_r8_r8(&(de.reg.lo), "e", af.reg.hi, "a"); break;

            case 0x60: ld_r8_r8(&(hl.reg.hi), "h", bc.reg.hi, "b"); break;
            case 0x61: ld_r8_r8(&(hl.reg.hi), "h", bc.reg.lo, "c"); break;
            case 0x62: ld_r8_r8(&(hl.reg.hi), "h", de.reg.hi, "d"); break;
            case 0x63: ld_r8_r8(&(hl.reg.hi), "h", de.reg.lo, "e"); break;
            case 0x64: ld_r8_r8(&(hl.reg.hi), "h", hl.reg.hi, "h"); break;
            case 0x65: ld_r8_r8(&(hl.reg.hi), "h", hl.reg.lo, "l"); break;
            case 0x66: ld_r8_mhl(&(hl.reg.hi), "h"); break;
            case 0x67: ld_r8_r8(&(hl.reg.hi), "h", af.reg.hi, "a"); break;

            case 0x68: ld_r8_r8(&(hl.reg.lo), "l", bc.reg.hi, "b"); break;
            case 0x69: ld_r8_r8(&(hl.reg.lo), "l", bc.reg.lo, "c"); break;
            case 0x6A: ld_r8_r8(&(hl.reg.lo), "l", de.reg.hi, "d"); break;
            case 0x6B: ld_r8_r8(&(hl.reg.lo), "l", de.reg.lo, "e"); break;
            case 0x6C: ld_r8_r8(&(hl.reg.lo), "l", hl.reg.hi, "h"); break;
            case 0x6D: ld_r8_r8(&(hl.reg.lo), "l", hl.reg.lo, "l"); break;
            case 0x6E: ld_r8_mhl(&(hl.reg.lo), "l"); break;
            case 0x6F: ld_r8_r8(&(hl.reg.lo), "l", af.reg.hi, "a"); break;

            case 0x70: ld_mhl_r8(bc.reg.hi, "b"); break;
            case 0x71: ld_mhl_r8(bc.reg.lo, "c"); break;
            case 0x72: ld_mhl_r8(de.reg.hi, "d"); break;
            case 0x73: ld_mhl_r8(de.reg.lo, "e"); break;
            case 0x74: ld_mhl_r8(hl.reg.hi, "h"); break;
            case 0x75: ld_mhl_r8(hl.reg.lo, "l"); break;
            case 0x77: ld_mhl_r8(af.reg.hi, "a"); break;

            case 0x78: ld_r8_r8(&(af.reg.hi), "a", bc.reg.hi, "b"); break;
            case 0x79: ld_r8_r8(&(af.reg.hi), "a", bc.reg.lo, "c"); break;
            case 0x7A: ld_r8_r8(&(af.reg.hi), "a", de.reg.hi, "d"); break;
            case 0x7B: ld_r8_r8(&(af.reg.hi), "a", de.reg.lo, "e"); break;
            case 0x7C: ld_r8_r8(&(af.reg.hi), "a", hl.reg.hi, "h"); break;
            case 0x7D: ld_r8_r8(&(af.reg.hi), "a", hl.reg.lo, "l"); break;
            case 0x7E: ld_r8_mhl(&(af.reg.hi), "a"); break;
            case 0x7F: ld_r8_r8(&(af.reg.hi), "a", af.reg.hi, "a"); break;

            // case 0x76:  fprintf("halt instruction not implemented");  exit(1);



            //
            // BLOCK 2
            //
            // case 0x80:  add_a_r8(bc.reg.hi, "b", 0); break;
            // case 0x81:  add_a_r8(bc.reg.lo, "c", 0); break;
            // case 0x82:  add_a_r8(de.reg.hi, "d", 0); break;
            // case 0x83:  add_a_r8(de.reg.lo, "e", 0); break;
            // case 0x84:  add_a_r8(hl.reg.hi, "h", 0); break;
            case 0x85:  add_a_r8(hl.reg.lo, "l", 0); break;
            // case 0x86:
            //   add_a_r8(get_memory(hl.reg16), "[hl]", 0);
            //   cycle_count += 1;
            //   break;
            case 0x87:  add_a_r8(af.reg.hi, "a", 0); break;

            // case 0x88:  add_a_r8(bc.reg.hi, "b", 1); break;
            // case 0x89:  add_a_r8(bc.reg.lo, "c", 1); break;
            // case 0x8A:  add_a_r8(de.reg.hi, "d", 1); break;
            // case 0x8B:  add_a_r8(de.reg.lo, "e", 1); break;
            // case 0x8C:  add_a_r8(hl.reg.hi, "h", 1); break;
            // case 0x8D:  add_a_r8(hl.reg.lo, "l", 1); break;
            // case 0x8E:
            //   add_a_r8(get_memory(hl.reg16), "[hl]", 1);
            //   cycle_count += 1;
            //   break;
            // case 0x8F:  add_a_r8(af.reg.hi, "a", 1); break;

            // case 0x90:  sub_a_r8(bc.reg.hi, "b", 0); break;
            // case 0x91:  sub_a_r8(bc.reg.lo, "c", 0); break;
            // case 0x92:  sub_a_r8(de.reg.hi, "d", 0); break;
            // case 0x93:  sub_a_r8(de.reg.lo, "e", 0); break;
            // case 0x94:  sub_a_r8(hl.reg.hi, "h", 0); break;
            // case 0x95:  sub_a_r8(hl.reg.lo, "l", 0); break;
            // case 0x96:
            //   sub_a_r8(get_memory(hl.reg16), "[hl]", 0);
            //   cycle_count += 1;
            //   break;
            // case 0x97:  sub_a_r8(af.reg.hi, "a", 0); break;

            // case 0x98:  sub_a_r8(bc.reg.hi, "b", 1); break;
            // case 0x99:  sub_a_r8(bc.reg.lo, "c", 1); break;
            // case 0x9A:  sub_a_r8(de.reg.hi, "d", 1); break;
            // case 0x9B:  sub_a_r8(de.reg.lo, "e", 1); break;
            // case 0x9C:  sub_a_r8(hl.reg.hi, "h", 1); break;
            // case 0x9D:  sub_a_r8(hl.reg.lo, "l", 1); break;
            // case 0x9E:
            //   sub_a_r8(get_memory(hl.reg16), "[hl]", 1);
            //   cycle_count += 1;
            //   break;
            // case 0x9F:  sub_a_r8(af.reg.hi, "a", 1); break;

            // case 0xA0:  and_a_r8(bc.reg.hi, "b"); break;
            case 0xA1:  and_a_r8(bc.reg.lo, "c"); break;
            // case 0xA2:  and_a_r8(de.reg.hi, "d"); break;
            // case 0xA3:  and_a_r8(de.reg.lo, "e"); break;
            // case 0xA4:  and_a_r8(hl.reg.hi, "h"); break;
            // case 0xA5:  and_a_r8(hl.reg.lo, "l"); break;
            // case 0xA6:
            //   and_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            case 0xA7:  and_a_r8(af.reg.hi, "a"); break;

            // case 0xA8:  xor_a_r8(bc.reg.hi, "b"); break;
            case 0xA9:  xor_a_r8(bc.reg.lo, "c"); break;
            // case 0xAA:  xor_a_r8(de.reg.hi, "d"); break;
            // case 0xAB:  xor_a_r8(de.reg.lo, "e"); break;
            // case 0xAC:  xor_a_r8(hl.reg.hi, "h"); break;
            // case 0xAD:  xor_a_r8(hl.reg.lo, "l"); break;
            // case 0xAE:
            //   xor_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            case 0xAF:  xor_a_r8(af.reg.hi, "a"); break;

            case 0xB0:  or_a_r8(bc.reg.hi, "b"); break;
            case 0xB1:  or_a_r8(bc.reg.lo, "c"); break;
            // case 0xB2:  or_a_r8(de.reg.hi, "d"); break;
            // case 0xB3:  or_a_r8(de.reg.lo, "e"); break;
            // case 0xB4:  or_a_r8(hl.reg.hi, "h"); break;
            // case 0xB5:  or_a_r8(hl.reg.lo, "l"); break;
            // case 0xB6:
            //   or_a_r8(get_memory(hl.reg16), "[hl]");
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
            //   cp_a_r8(get_memory(hl.reg16), "[hl]");
            //   cycle_count += 1;
            //   break;
            // case 0xBF:  cp_a_r8(af.reg.hi, "a"); break;



            //
            // BLOCK 3
            //
            case 0xC6: // add a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                add_a_r8(n8, name, 0);
                cycle_count += 1;
                break;
            case 0xCE: // adc a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                add_a_r8(n8, name, 1);
                cycle_count += 1;
                break;
            case 0xD6: // sub a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                sub_a_r8(n8, name, 0);
                cycle_count += 1;
                break;
            case 0xDE: // sbc a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                sub_a_r8(n8, name, 1);
                cycle_count += 1;
                break;
            case 0xE6: // and a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                and_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xEE: // xor a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                xor_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xF6: // or a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                or_a_r8(n8, name);
                cycle_count += 1;
                break;
            case 0xFE: // cp a, n8
                n8 = get_ro_mem(++pc.reg16);
                sprintf(name, "%02X", n8);
                cp_a_r8(n8, name);
                cycle_count += 1;
                break;

            case 0xC0:  ret_cc(!zf, "nz");  break;
            case 0xC8:  ret_cc(zf, "z");  break;
            // case 0xD0:  ret_cc(!cf, "nc");  break;
            // case 0xD8:  ret_cc(cf, "c");  break;
            case 0xC9:  // ret
                ret_cc(1, "");
                cycle_count -= 1;
                break;
            case 0xD9:  // reti
                ext_ime = 1;
                ret_cc(1, "i");
                cycle_count -= 1;
                break;

            case 0xC2:  jp_cc_n16(!zf, "nz"); break;
            case 0xCA:  jp_cc_n16(zf, "z"); break;
            // case 0xD2:  jp_cc_n16(!cf, "nc"); break;
            // case 0xDA:  jp_cc_n16(cf, "c"); break;
            case 0xC3:  jp_cc_n16(1, "");  break;
            case 0xE9:  // jp hl
                pc.reg16 = hl.reg16 - 1;
                cycle_count += 1;
    #ifdef log_file
                fprintf(log_file, "jp\thl\n");
    #endif // log_file
                break;

            // case 0xC4:  call_cc_n16(!zf, "nz"); break;
            // case 0xCC:  call_cc_n16(zf, "z"); break;
            // case 0xD4:  call_cc_n16(!cf, "nc"); break;
            // case 0xDC:  call_cc_n16(cf, "c"); break;
            case 0xCD:  call_cc_n16(1, "");  break;

            // case 0xC7:  rst(0x00);  break;
            // case 0xCF:  rst(0x08);  break;
            // case 0xD7:  rst(0x10);  break;
            // case 0xDF:  rst(0x18);  break;
            // case 0xE7:  rst(0x20);  break;
            case 0xEF:  rst(0x28);  break;
            // case 0xF7:  rst(0x30);  break;
            // case 0xFF:  rst(0x38);  break;

            case 0xC1:  pop_r16(&bc, "bc"); break;
            case 0xD1:  pop_r16(&de, "de"); break;
            case 0xE1:  pop_r16(&hl, "hl"); break;
            case 0xF1:
                pop_r16(&af, "af");
                zf = (af.reg.lo & 0x80) >> 7;
                nf = (af.reg.lo & 0x40) >> 6;
                hf = (af.reg.lo & 0x20) >> 5;
                cf = (af.reg.lo & 0x10) >> 4;
                break;

            case 0xC5:  push_r16(bc, "bc"); break;
            case 0xD5:  push_r16(de, "de"); break;
            case 0xE5:  push_r16(hl, "hl"); break;
            case 0xF5:
                af.reg.lo = (zf << 7) | (nf << 6) | (hf << 5) | (cf << 4);
                push_r16(af, "af");
                break;

            case 0xE2:  // ldh [c], a
                n16.reg.lo = bc.reg.lo;
                n16.reg.hi = 0xff;
                memory[n16.reg16] = af.reg.hi;
                cycle_count += 2;
#ifdef log_file
                fprintf(log_file, "ldh\t[c]\ta");
                fprintf(log_file, "\t\tc:\t%02X\t[c]:\t%02X\n", bc.reg.lo, memory[n16.reg16]);
#endif // log_file
                break;

            case 0xE0:  // ldh [n8], a
                n16.reg.lo = memory[++pc.reg16];
                n16.reg.hi = 0xff;
                memory[n16.reg16] = af.reg.hi;
                cycle_count += 3;
#ifdef log_file
                fprintf(log_file, "ldh\t[%04X],\ta", n16.reg16);
                fprintf(log_file, "\t\t[%04X]:\t%02X\n", n16.reg16, memory[n16.reg16]);
#endif // log_file
                break;

            case 0xEA:  // ld [n16], a
                n16.reg.lo = get_ro_mem(++pc.reg16);
                n16.reg.hi = get_ro_mem(++pc.reg16);
                sprintf(name, "[%04X]", n16.reg16);
                ld_r8_r8(get_rw_memptr(n16.reg16), name, af.reg.hi, "a");
                cycle_count += 3;
                break;

            case 0xF0:  // ldh a, [n8]
                n16.reg.lo = memory[++pc.reg16];
                n16.reg.hi = 0xff;
                af.reg.hi = memory[n16.reg16];
                cycle_count += 3;
#ifdef log_file
                fprintf(log_file, "ldh\ta,\t[%04X]", n16.reg16);
                fprintf(log_file, "\t\ta:\t%02X\n", af.reg.hi);
#endif // log_file
                break;

            case 0xFA:  // ld a, [n16]
                n16.reg.lo = get_ro_mem(++pc.reg16);
                n16.reg.hi = get_ro_mem(++pc.reg16);
                sprintf(name, "[%04X]", n16.reg16);
                ld_r8_r8(&af.reg.hi, "a", get_ro_mem(n16.reg16), name);
                cycle_count += 3;
                break;

            case 0xF3:  // di
                ext_ime = 0;
                cycle_count += 1;
#ifdef log_file
                fprintf(log_file, "di\n");
#endif // log_file
                break;

            case 0xFB:  // ei
                ext_ime = 1;
                cycle_count += 1;
#ifdef log_file
                fprintf(log_file, "ei\n");
#endif // log_file
                break;



            //
            // BLOCK CB
            //
            case 0xCB:
                pc.reg16++;
                opcode = memory[pc.reg16];
                switch (opcode) {
                    case 0x27:  sla_r8(&(af.reg.hi), "a");  break;
                    case 0x37:  swap_r8(&(af.reg.hi), "a"); break;

                    case 0x50: case 0x58: case 0x60: case 0x68:
                    case 0x70: case 0x78: case 0x40: case 0x48:
                        bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.hi, "b");
                        break;
                    case 0x51: case 0x59: case 0x61: case 0x69:
                    case 0x71: case 0x79: case 0x41: case 0x49:
                        bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.lo, "c");
                        break;
                    case 0x52: case 0x5A: case 0x62: case 0x6A:
                    case 0x72: case 0x7A: case 0x42: case 0x4A:
                        bit_bitn_r8((opcode >> 3) & 0x7, de.reg.hi, "d");
                        break;
                    case 0x53: case 0x5B: case 0x63: case 0x6B:
                    case 0x73: case 0x7B: case 0x43: case 0x4B:
                        bit_bitn_r8((opcode >> 3) & 0x7, de.reg.lo, "e");
                        break;
                    case 0x54: case 0x5C: case 0x64: case 0x6C:
                    case 0x74: case 0x7C: case 0x44: case 0x4C:
                        bit_bitn_r8((opcode >> 3) & 0x7, hl.reg.hi, "h");
                        break;
                    case 0x55: case 0x5D: case 0x65: case 0x6D:
                    case 0x75: case 0x7D: case 0x45: case 0x4D:
                        bit_bitn_r8((opcode >> 3) & 0x7, hl.reg.lo, "l");
                        break;
                    case 0x56: case 0x5E: case 0x66: case 0x6E:
                    case 0x76: case 0x7E: case 0x46: case 0x4E:
                        bit_bitn_r8((opcode >> 3) & 0x7, get_ro_mem(hl.reg16), "[hl]");
                        cycle_count += 1;
                        break;
                    case 0x57: case 0x5F: case 0x67: case 0x6F:
                    case 0x77: case 0x7F: case 0x47: case 0x4F:
                        bit_bitn_r8((opcode >> 3) & 0x7, bc.reg.hi, "a");
                        break;

                    case 0x86:
                        res_bitn_r8(0, get_rw_memptr(hl.reg16), "[hl]");
                        cycle_count += 2;
                        break;
                    case 0x87:  res_bitn_r8(0, &(af.reg.hi), "a");  break;

                    case 0xCF:  set_bitn_r8(1, &(af.reg.hi), "a");  break;

                    default:
#ifdef GENERATE_LOGS
                        fprintf(log_file, "Unknown CB prefix instruction");
#endif // GENERATE_LOGS
                        // void dump_vram();
                        // dump_vram();
                        return;
                }
                break;


            default:
#ifdef GENERATE_LOGS
                fprintf(log_file, "Unimplemented opcode\n");
#endif // GENERATE_LOGS
                return;
        }

        // Handle Joypad Register
        if ((memory[0xff00] & 0x30) == 0x20)
            memory[0xff00] = 0xEF;
        else if ((memory[0xff00] & 0x30) == 0x10)
            memory[0xff00] = 0xDF;


        pc.reg16++;
    }
}