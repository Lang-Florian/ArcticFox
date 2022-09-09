#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __RANDOMNESS__MODULE__
#define __RANDOMNESS__MODULE__


#include <random>


namespace randomness {
std::mt19937_64 generator (std::random_device{}());

template <typename T>
T generate() {
  return generator();
};
};


#endif