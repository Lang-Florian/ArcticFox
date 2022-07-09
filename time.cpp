#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"

#include <chrono>


#ifndef __TIME__MODULE__
#define __TIME__MODULE__


namespace TIME {
  // get the current time in milliseconds
  inline U64 ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };

  // get the current time in seconds
  inline U64 s() {
    return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  };
};


#endif