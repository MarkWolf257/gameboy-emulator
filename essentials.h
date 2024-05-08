#include <inttypes.h>

#define CART_SIZE 0x10000


// 16-bit register
typedef union register16_t
{
  struct
  {
    uint8_t lo;
    uint8_t hi;
  } reg;

  uint16_t reg16;
} register16_t;
