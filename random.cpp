#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"


#ifndef __RANDOM__MODULE__
#define __RANDOM__MODULE__


namespace RANDOM {
  // define the random seed
  U32 random_state = 1804289383;

  // generate random 16bit unsigned integer
  inline U16 random_U16() {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return (U16)(random_state & 0xFFFF);
  };

  // generate random 32bit unsigned integer
  inline U32 random_U32() {
    return ((U32)random_U16() <<  0) |
           ((U32)random_U16() << 16);
  };

  // generate random 64bit unsigned integer
  inline U64 random_U64() {
    return ((U64)random_U16() <<  0) |
           ((U64)random_U16() << 16) |
           ((U64)random_U16() << 32) |
           ((U64)random_U16() << 48);
  };

  // generate a random integer between lower and upper
  inline int random(int lower, int upper) {
    return lower + (random_U32() % (upper - lower));
  };

  // generate a random integer between 0 and upper
  inline int random(int upper) {
    return random(0, upper);
  };
};


#endif