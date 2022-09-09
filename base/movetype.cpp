#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MOVETYPE__MODULE__
#define __MOVETYPE__MODULE__


#include "types.cpp"


namespace movetype {
enum : movetype_t {
  quiet =        0b0001,
  single_check = 0b0010,
  double_check = 0b0100,
  check =        0b0110,
  capture =      0b1000,
  non_quiet =    0b1110,
  legal =        0b1111,
};
};


#endif