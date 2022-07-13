#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __TIME__MODULE__
#define __TIME__MODULE__


#include <chrono>

#include "constants.cpp"


/*

  functions for getting the current time in
  different units

*/
namespace timing {
  // get the current time in nanoseconds
  u64_t ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in milliseconds
  u64_t ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in seconds
  u64_t s() {
    return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in minutes
  u64_t min() {
    return std::chrono::duration_cast<std::chrono::minutes>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };
};


#endif // __TIME__MODULE__