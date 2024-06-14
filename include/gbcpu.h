#ifndef GBCPU_H
#define GBCPU_H


#include <inttypes.h>


// Register Types
// typedef union register16_t
// {
//     struct
//     {
//         uint8_t lo, hi;
//     } reg;
//
//     uint16_t reg16;
// } register16_t;





void init_gbcpu();
void gbcpu_process(size_t cycles_to_execute);


#endif //GBCPU_H
