#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __TIME__MODULE__
#define __TIME__MODULE__


#include <chrono>

#include "types.cpp"


/*

  functions for getting the current time in
  different units

*/


namespace timing {
  // get the current time in nanoseconds
  u64_t nanoseconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in microseconds
  u64_t microseconds() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in milliseconds
  u64_t milliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in seconds
  u64_t seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in minutes
  u64_t minutes() {
    return std::chrono::duration_cast<std::chrono::minutes>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };
};


#endif // __TIME__MODULE__