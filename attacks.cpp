#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"
#include "magic.cpp"

#include "array"


#ifndef __ATTACKS__MODULE__
#define __ATTACKS__MODULE__


namespace ATTACK {
  // generate the pawn attacks for both sides
  static constexpr std::array<std::array<bitboard_t, 64>, 2> generate_pawn() {
    std::array<std::array<bitboard_t, 64>, 2> pawn_attacks{BITBOARD::none};
    for (square_t square = 0; square < 64; square++) {
      pawn_attacks[COLOR::white & 0b1][square] = ((bitboard(square) >> 9) & ~BITBOARD::file_H) | ((bitboard(square) >> 7) & ~BITBOARD::file_A);
      pawn_attacks[COLOR::black & 0b1][square] = ((bitboard(square) << 9) & ~BITBOARD::file_A) | ((bitboard(square) << 7) & ~BITBOARD::file_H);
    };
    return pawn_attacks;
  };

  static constexpr std::array<std::array<bitboard_t, 64>, 2> __pawn__ = generate_pawn();

  inline bitboard_t pawn(color_t color, square_t square) {
    return __pawn__[color & 0b1][square];
  };

  // generate the knight attacks
  static constexpr std::array<bitboard_t, 64> generate_knight() {
    std::array<bitboard_t, 64> knight_attacks{BITBOARD::none};
    for (square_t square = 0; square < 64; square++) {
      knight_attacks[square] = ((bitboard(square) >>  6) & ~BITBOARD::file_A & ~BITBOARD::file_B & ~BITBOARD::rank_1) |
                               ((bitboard(square) <<  6) & ~BITBOARD::file_G & ~BITBOARD::file_H & ~BITBOARD::rank_8) |
                               ((bitboard(square) >> 10) & ~BITBOARD::file_G & ~BITBOARD::file_H & ~BITBOARD::rank_1) |
                               ((bitboard(square) << 10) & ~BITBOARD::file_A & ~BITBOARD::file_B & ~BITBOARD::rank_8) |
                               ((bitboard(square) >> 15) & ~BITBOARD::file_A & ~BITBOARD::rank_1 & ~BITBOARD::rank_2) |
                               ((bitboard(square) << 15) & ~BITBOARD::file_H & ~BITBOARD::rank_7 & ~BITBOARD::rank_8) |
                               ((bitboard(square) >> 17) & ~BITBOARD::file_H & ~BITBOARD::rank_1 & ~BITBOARD::rank_2) |
                               ((bitboard(square) << 17) & ~BITBOARD::file_A & ~BITBOARD::rank_7 & ~BITBOARD::rank_8);
    };
    return knight_attacks;
  };

  static constexpr std::array<bitboard_t, 64> __knight__= generate_knight();

  inline bitboard_t knight(square_t square) {
    return __knight__[square];
  };

  // generate the king attacks
  static constexpr std::array<bitboard_t, 64> generate_king() {
    std::array<bitboard_t, 64> king_attacks{BITBOARD::none};
    for (square_t square = 0; square < 64; square++) {
      king_attacks[square] = ((bitboard(square) >> 1) & ~BITBOARD::file_H) |
                             ((bitboard(square) << 1) & ~BITBOARD::file_A) |
                             ((bitboard(square) >> 8) & ~BITBOARD::rank_1) |
                             ((bitboard(square) << 8) & ~BITBOARD::rank_8) |
                             ((bitboard(square) >> 7) & ~BITBOARD::file_A & ~BITBOARD::rank_1) |
                             ((bitboard(square) << 7) & ~BITBOARD::file_H & ~BITBOARD::rank_8) |
                             ((bitboard(square) >> 9) & ~BITBOARD::file_H & ~BITBOARD::rank_1) |
                             ((bitboard(square) << 9) & ~BITBOARD::file_A & ~BITBOARD::rank_8);
    };
    return king_attacks;
  };

  static constexpr std::array<bitboard_t, 64> __king__ = generate_king();

  inline bitboard_t king(square_t square) {
    return __king__[square];
  };

  // define the functions to access the magic table
  inline bitboard_t bishop(square_t square, bitboard_t occupancy) {
    return MAGIC::table[
      MAGIC::bishop[square].offset +
      (((occupancy | MAGIC::bishop[square].mask) * MAGIC::bishop[square].magic_number) >> (64 - 9))
    ];
  };

  inline bitboard_t rook(square_t square, bitboard_t occupancy) {
    return MAGIC::table[
      MAGIC::rook[square].offset +
      (((occupancy | MAGIC::rook[square].mask) * MAGIC::rook[square].magic_number) >> (64 - 12))
    ];
  };

  inline bitboard_t queen(square_t square, bitboard_t occupancy) {
    return bishop(square, occupancy) | rook(square, occupancy);
  };
};


#endif