#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MACROS__MODULE__
#define __MACROS__MODULE__


#define bitboard(square) (1ULL << (square))
#define popcount(bitboard) __builtin_popcountll(bitboard)
#define get_lsb(bitboard) __builtin_ctzll(bitboard)
#define pop_lsb(bitboard) __builtin_ctzll(bitboard); bitboard &= bitboard - 1
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) & (1ULL << (square))); (bitboard) &= ~(1ULL << (square))


#endif