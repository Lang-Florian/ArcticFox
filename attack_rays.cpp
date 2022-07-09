#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"

#include "array"


#ifndef __ATTACK_RAYS__
#define __ATTACK_RAYS__


namespace ATTACK_RAY {
  // generate the short bishop attack rays
  static constexpr std::array<bitboard_t, 64> generate_bishop_short() {
    std::array<bitboard_t, 64> bishop_short_attack_rays{0ULL};
    for (square_t square = 0; square < 64; square++) {
      bishop_short_attack_rays[square] = 0ULL;
      for (int i = 1; i < square % 8 && i < square / 8; i++)
        bishop_short_attack_rays[square] |= (bitboard(square) >> (9 * i));
      for (int i = 1; i < square % 8 && i < 7 - (square / 8); i++)
        bishop_short_attack_rays[square] |= (bitboard(square) << (7 * i));
      for (int i = 1; i < 7 - (square % 8) && i < 7 - (square / 8); i++)
        bishop_short_attack_rays[square] |= (bitboard(square) << (9 * i));
      for (int i = 1; i < 7 - (square % 8) && i < square / 8; i++)
        bishop_short_attack_rays[square] |= (bitboard(square) >> (7 * i));
    };
    return bishop_short_attack_rays;
  };

  static constexpr std::array<bitboard_t, 64> bishop_short = generate_bishop_short();

  // generate the short rook attack rays
  static constexpr std::array<bitboard_t, 64> generate_rook_short() {
    std::array<bitboard_t, 64> rook_short_attack_rays{0ULL};
    for (square_t square = 0; square < 64; square++) {
      rook_short_attack_rays[square] = 0ULL;
      for (int i = 1; i < square % 8; i++)
        rook_short_attack_rays[square] |= (bitboard(square) >> i);
      for (int i = 1; i < 7 - (square % 8); i++)
        rook_short_attack_rays[square] |= (bitboard(square) << i);
      for (int i = 1; i < square / 8; i++)
        rook_short_attack_rays[square] |= (bitboard(square) >> (8 * i));
      for (int i = 1; i < 7 - (square / 8); i++)
        rook_short_attack_rays[square] |= (bitboard(square) << (8 * i));
    };
    return rook_short_attack_rays;
  };

  static constexpr std::array<bitboard_t, 64> rook_short = generate_rook_short();

  // generate the bishop attack rays
  static constexpr std::array<bitboard_t, 64> generate_bishop() {
    std::array<bitboard_t, 64> bishop_attack_rays{0ULL};
    for (square_t square = 0; square < 64; square++) {
      bishop_attack_rays[square] = 0ULL;
      for (int i = 1; i <= square % 8 && i <= square / 8; i++)
        bishop_attack_rays[square] |= (bitboard(square) >> (9 * i));
      for (int i = 1; i <= square % 8 && i <= 7 - (square / 8); i++)
        bishop_attack_rays[square] |= (bitboard(square) << (7 * i));
      for (int i = 1; i <= 7 - (square % 8) && i <= 7 - (square / 8); i++)
        bishop_attack_rays[square] |= (bitboard(square) << (9 * i));
      for (int i = 1; i <= 7 - (square % 8) && i <= square / 8; i++)
        bishop_attack_rays[square] |= (bitboard(square) >> (7 * i));
    };
    return bishop_attack_rays;
  };

  static constexpr std::array<bitboard_t, 64> bishop = generate_bishop();

  // generate the rook attack rays
  static constexpr std::array<bitboard_t, 64> generate_rook() {
    std::array<bitboard_t, 64> rook_attack_rays{0ULL};
    for (square_t square = 0; square < 64; square++) {
      rook_attack_rays[square] = 0ULL;
      for (int i = 1; i <= square % 8; i++)
        rook_attack_rays[square] |= (bitboard(square) >> i);
      for (int i = 1; i <= 7 - (square % 8); i++)
        rook_attack_rays[square] |= (bitboard(square) << i);
      for (int i = 1; i <= square / 8; i++)
        rook_attack_rays[square] |= (bitboard(square) >> (8 * i));
      for (int i = 1; i <= 7 - (square / 8); i++)
        rook_attack_rays[square] |= (bitboard(square) << (8 * i));
    };
    return rook_attack_rays;
  };

  static constexpr std::array<bitboard_t, 64> rook = generate_rook();
};


#endif