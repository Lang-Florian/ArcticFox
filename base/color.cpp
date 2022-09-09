#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __COLOR__MODULE__
#define __COLOR__MODULE__


#include <string>
#include "types.cpp"


namespace color {
enum : color_t {
  white = 0b00,
  black = 0b01,
  none =  0b10,
};

color_t opponent(color_t color) {
  return color ^ 0b1;
};

color_t from_char(char c) {
  switch (c) {
    case 'w': return white;
    case 'b': return black;
    default:  return none;
  };
};

std::string to_string(color_t color) {
  const std::string labels[] = {"w", "b", ""};
  return labels[color];
};
};


namespace color::compiletime {
constexpr color_t opponent(color_t color) {
  return color ^ 0b1;
};
};


#endif