#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __CASTLING__MODULE__
#define __CASTLING__MODULE__


#include <string>
#include "bitboard.cpp"
#include "types.cpp"


namespace castling {
enum : castling_t {
  none =        0b0000,
  white_king =  0b0001,
  white_queen = 0b0010,
  black_king =  0b0100,
  black_queen = 0b1000,
  white =       0b0011,
  black =       0b1100,
  king =        0b0101,
  queen =       0b1010,
};

castling_t from_char(char c) {
  switch (c) {
    case 'K': return white_king;
    case 'Q': return white_queen;
    case 'k': return black_king;
    case 'q': return black_queen;
    default:  return none;
  };
};

std::string to_string(castling_t castling) {
  std::string string = "";
  if (castling & white_king)  string += "K";
  if (castling & white_queen) string += "Q";
  if (castling & black_king)  string += "k";
  if (castling & black_queen) string += "q";
  if (string.empty())         string += "-";
  return string;
};

const bitboard_t white_king_attack_mask =  bitboard::e1 | bitboard::f1 | bitboard::g1;
const bitboard_t white_queen_attack_mask = bitboard::c1 | bitboard::d1 | bitboard::e1;
const bitboard_t black_king_attack_mask =  bitboard::e8 | bitboard::f8 | bitboard::g8;
const bitboard_t black_queen_attack_mask = bitboard::c8 | bitboard::d8 | bitboard::e8;

const bitboard_t white_king_piece_mask =  bitboard::f1 | bitboard::g1;
const bitboard_t white_queen_piece_mask = bitboard::b1 | bitboard::c1 | bitboard::d1;
const bitboard_t black_king_piece_mask =  bitboard::f8 | bitboard::g8;
const bitboard_t black_queen_piece_mask = bitboard::b8 | bitboard::c8 | bitboard::d8;
};


#endif