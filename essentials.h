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


// Flag register
#define GET_ZERO_FLAG af.reg.HI & 0x80
#define GET_SUBTRACT_FLAG af.reg.HI & 0x40
#define GET_HALF_CARRY_FLAG af.reg.HI & 0x20
#define GET_CARRY_FLAG af.reg.HI & 0x10

#define SET_ZERO_FLAG af.reg.HI |= 0x80
#define SET_SUBTRACT_FLAG af.reg.HI |= 0x40
#define SET_HALF_CARRY_FLAG af.reg.HI |= 0x20
#define SET_CARRY_FLAG af.reg.HI |= 0x10