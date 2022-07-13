#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __RANDOM__MODULE__
#define __RANDOM__MODULE__


#include <random>


/*

  a function for generating random numbers of any integer type

*/
namespace randomness {
  std::mt19937_64 generator (std::random_device{}());

  template <typename T>
  T generate() {
    return generator();
  };
};


#endif // __RANDOM__MODULE__