#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MOVETYPE__MODULE__
#define __MOVETYPE__MODULE__


#include "types.cpp"


namespace movetype {
enum : movetype_t {
  quiet =     0b001,
  check =     0b010,
  capture =   0b100,
  non_quiet = 0b110,
  legal =     0b111,
};
};


#endif