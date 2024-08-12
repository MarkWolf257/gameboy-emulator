#ifndef IBLK3_H
#define IBLK3_H


static inline void
ret_cc(const Uint8 cc, const char *name)
{
    if (cc) {
        pc = memory[sp + 2] << 8 | memory[sp + 1];
        sp += 2;
        pc -= 1;
        cycle_count += 3;
    }
    cycle_count += 2;

#ifdef GENERATE_LOGS
    fprintf(log_file, "ret%s\n", name);
#endif // GENERATE_LOGS
}


static inline void
jp_cc_n16(const Uint8 cc, const char *name)
{
    Uint16 n16 = pc;
    pc = get_ro_mem(n16 + 2) << 8 | get_ro_mem(n16 + 1);
    n16 += 2;
    cycle_count += 3;

#ifdef GENERATE_LOGS
    fprintf(log_file, "jp%s\t%04X\n", name, pc);
#endif // GENERATE_LOGS

    if (cc) {
        pc -= 1;
        cycle_count += 1;
    } else {
        pc = n16;
    }
}


static inline void
call_cc_n16(const Uint8 cc, const char *name)
{
    Uint16 n16 = get_ro_mem(pc + 2) << 8 | get_ro_mem(pc + 1);
    pc += 2;
    cycle_count += 3;

    if (cc) {
        pc += 1;
        memory[sp--] = pc >> 8;
        memory[sp--] = pc;
        pc = n16 - 1;
        cycle_count += 3;
    }

#ifdef GENERATE_LOGS
    fprintf(log_file, "call%s\t%04X\n", name, n16);
#endif // GENERATE_LOGS
}


static inline void
rst(const Uint16 address)
{
    pc += 1;
    memory[sp--] = pc >> 8;
    memory[sp--] = pc;
    pc = address - 1;
    cycle_count += 4;

#ifdef GENERATE_LOGS
    fprintf(log_file, "rst\t%04X\n", address);
#endif // GENERATE_LOGS
}


static inline void
pop_r16(Uint8 *hi, Uint8 *lo, const char *name)
{
    *lo = memory[++sp];
    *hi = memory[++sp];
    cycle_count += 3;

#ifdef GENERATE_LOGS
    fprintf(log_file, "pop\t%s\t", name);
    fprintf(log_file, "\t\t%s:\t%02X%02X\n", name, *hi, *lo);
#endif // GENERATE_LOGS
}


static inline void
push_r16(const Uint8 hi, const Uint8 lo, const char *name)
{
    memory[sp--] = hi;
    memory[sp--] = lo;
    cycle_count += 4;

#ifdef GENERATE_LOGS
    fprintf(log_file, "push\t%s\n", name);
#endif // GENERATE_LOGS
}

#endif //IBLK3_H
