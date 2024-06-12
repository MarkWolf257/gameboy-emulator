#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>

#define GENERATE_LOGS

extern unsigned int cycle_count;

#ifdef GENERATE_LOGS
extern FILE *log_file;
#endif // GENERATE_LOGS


#endif //EMULATOR_H
