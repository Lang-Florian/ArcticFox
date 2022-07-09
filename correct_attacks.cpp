#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"


#ifndef __CORRECT__ATTACKS__MODULE__
#define __CORRECT__ATTACKS__MODULE__


namespace CORRECT_ATTACK {
  // generate correct bishop attacks for each square and occupancy
  static constexpr bitboard_t bishop(square_t square, bitboard_t occupancy) {
    bitboard_t bishop_attacks = 0ULL;
    for (int i = 1; i <= square % 8 && i <= square / 8; i++) {
      bishop_attacks |= (bitboard(square) >> (9 * i));
      if ((bitboard(square) >> (9 * i)) & occupancy) break;
    };
    for (int i = 1; i <= square % 8 && i <= 7 - (square / 8); i++) {
      bishop_attacks |= (bitboard(square) << (7 * i));
      if ((bitboard(square) << (7 * i)) & occupancy) break;
    };
    for (int i = 1; i <= 7 - (square % 8) && i <= 7 - (square / 8); i++) {
      bishop_attacks |= (bitboard(square) << (9 * i));
      if ((bitboard(square) << (9 * i)) & occupancy) break;
    };
    for (int i = 1; i <= 7 - (square % 8) && i <= square / 8; i++) {
      bishop_attacks |= (bitboard(square) >> (7 * i));
      if ((bitboard(square) >> (7 * i)) & occupancy) break;
    };
    return bishop_attacks;
  };

  // generate correct rook attacks for each square and occupancy
  static constexpr bitboard_t rook(square_t square, bitboard_t occupancy) {
    bitboard_t rook_attacks = 0ULL;
    for (int i = 1; i <= square % 8; i++) {
      rook_attacks |= (bitboard(square) >> i);
      if ((bitboard(square) >> i) & occupancy) break;
    };
    for (int i = 1; i <= 7 - (square % 8); i++) {
      rook_attacks |= (bitboard(square) << i);
      if ((bitboard(square) << i) & occupancy) break;
    };
    for (int i = 1; i <= square / 8; i++) {
      rook_attacks |= (bitboard(square) >> (8 * i));
      if ((bitboard(square) >> (8 * i)) & occupancy) break;
    };
    for (int i = 1; i <= 7 - (square / 8); i++) {
      rook_attacks |= (bitboard(square) << (8 * i));
      if ((bitboard(square) << (8 * i)) & occupancy) break;
    };
    return rook_attacks;
  };
};


#endif