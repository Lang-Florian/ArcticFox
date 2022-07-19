#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __ATTACK__RAYS__MODULE__
#define __ATTACK__RAYS__MODULE__


#include <array>

#include "constants.cpp"
#include "macros.cpp"
#include "types.cpp"


/*
 
  generate bishop and rook attack rays for a given square
  at compile time

*/


namespace attack_ray {
  constexpr std::array<bitboard_t, 64> bishop = {[]() constexpr {
    std::array<bitboard_t, 64> bishop{0ULL};
    for (auto square : square::all_squares) {
      bishop[square] = 0ULL;
      for (int i = 1; i <= square % 8 && i <= square / 8; i++)
        bishop[square] |= (bitboard(square) >> (9 * i));
      for (int i = 1; i <= square % 8 && i <= 7 - (square / 8); i++)
        bishop[square] |= (bitboard(square) << (7 * i));
      for (int i = 1; i <= 7 - (square % 8) && i <= 7 - (square / 8); i++)
        bishop[square] |= (bitboard(square) << (9 * i));
      for (int i = 1; i <= 7 - (square % 8) && i <= square / 8; i++)
        bishop[square] |= (bitboard(square) >> (7 * i));
    };
    return bishop;
  }()};

  constexpr std::array<bitboard_t, 64> rook = {[]() constexpr {
    std::array<bitboard_t, 64> rook{0ULL};
    for (auto square : square::all_squares) {
      rook[square] = 0ULL;
      for (int i = 1; i <= square % 8; i++)
        rook[square] |= (bitboard(square) >> i);
      for (int i = 1; i <= 7 - (square % 8); i++)
        rook[square] |= (bitboard(square) << i);
      for (int i = 1; i <= square / 8; i++)
        rook[square] |= (bitboard(square) >> (8 * i));
      for (int i = 1; i <= 7 - (square / 8); i++)
        rook[square] |= (bitboard(square) << (8 * i));
    };
    return rook;
  }()};
};


#endif // __ATTACK__RAYS__MODULE__