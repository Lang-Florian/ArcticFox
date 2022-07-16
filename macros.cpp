#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __MACROS__MODULE__
#define __MACROS__MODULE__


#include "types.cpp"


/*

  most used macros
  mainly for bitboard and square manipulation

*/


#define bitboard(square) (1ULL << (square))

#define popcount(bitboard) __builtin_popcountll(bitboard)

#define get_lsb(bitboard) __builtin_ctzll(bitboard)
square_t pop_lsb(bitboard_t& bitboard) {
  square_t lsb = get_lsb(bitboard);
  bitboard &= ~(1ULL << lsb);
  return lsb;
};

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
bitboard_t pop_bit(bitboard_t& bitboard, square_t square) {
  bitboard_t bit = get_bit(bitboard, square);
  bitboard &= ~(1ULL << square);
  return bit;
};


#endif // __MACROS__MODULE__