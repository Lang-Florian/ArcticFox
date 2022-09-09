#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __BITBOARD__MODULE__
#define __BITBOARD__MODULE__


#include "macros.cpp"
#include "square.cpp"
#include "types.cpp"


namespace bitboard {
enum : bitboard_t {
  a8 = bitboard(square::a8), b8 = bitboard(square::b8), c8 = bitboard(square::c8), d8 = bitboard(square::d8), e8 = bitboard(square::e8), f8 = bitboard(square::f8), g8 = bitboard(square::g8), h8 = bitboard(square::h8),
  a7 = bitboard(square::a7), b7 = bitboard(square::b7), c7 = bitboard(square::c7), d7 = bitboard(square::d7), e7 = bitboard(square::e7), f7 = bitboard(square::f7), g7 = bitboard(square::g7), h7 = bitboard(square::h7),
  a6 = bitboard(square::a6), b6 = bitboard(square::b6), c6 = bitboard(square::c6), d6 = bitboard(square::d6), e6 = bitboard(square::e6), f6 = bitboard(square::f6), g6 = bitboard(square::g6), h6 = bitboard(square::h6),
  a5 = bitboard(square::a5), b5 = bitboard(square::b5), c5 = bitboard(square::c5), d5 = bitboard(square::d5), e5 = bitboard(square::e5), f5 = bitboard(square::f5), g5 = bitboard(square::g5), h5 = bitboard(square::h5),
  a4 = bitboard(square::a4), b4 = bitboard(square::b4), c4 = bitboard(square::c4), d4 = bitboard(square::d4), e4 = bitboard(square::e4), f4 = bitboard(square::f4), g4 = bitboard(square::g4), h4 = bitboard(square::h4),
  a3 = bitboard(square::a3), b3 = bitboard(square::b3), c3 = bitboard(square::c3), d3 = bitboard(square::d3), e3 = bitboard(square::e3), f3 = bitboard(square::f3), g3 = bitboard(square::g3), h3 = bitboard(square::h3),
  a2 = bitboard(square::a2), b2 = bitboard(square::b2), c2 = bitboard(square::c2), d2 = bitboard(square::d2), e2 = bitboard(square::e2), f2 = bitboard(square::f2), g2 = bitboard(square::g2), h2 = bitboard(square::h2),
  a1 = bitboard(square::a1), b1 = bitboard(square::b1), c1 = bitboard(square::c1), d1 = bitboard(square::d1), e1 = bitboard(square::e1), f1 = bitboard(square::f1), g1 = bitboard(square::g1), h1 = bitboard(square::h1),
  none = 0ULL, full = 0xFFFFFFFFFFFFFFFFULL,
};

const bitboard_t file_a = 0x0101010101010101ULL;
const bitboard_t file_b = 0x0202020202020202ULL;
const bitboard_t file_c = 0x0404040404040404ULL;
const bitboard_t file_d = 0x0808080808080808ULL;
const bitboard_t file_e = 0x1010101010101010ULL;
const bitboard_t file_f = 0x2020202020202020ULL;
const bitboard_t file_g = 0x4040404040404040ULL;
const bitboard_t file_h = 0x8080808080808080ULL;
const bitboard_t files[8] = {file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h};

const bitboard_t rank_1 = 0xFF00000000000000ULL;
const bitboard_t rank_2 = 0x00FF000000000000ULL;
const bitboard_t rank_3 = 0x0000FF0000000000ULL;
const bitboard_t rank_4 = 0x000000FF00000000ULL;
const bitboard_t rank_5 = 0x00000000FF000000ULL;
const bitboard_t rank_6 = 0x0000000000FF0000ULL;
const bitboard_t rank_7 = 0x000000000000FF00ULL;
const bitboard_t rank_8 = 0x00000000000000FFULL;
const bitboard_t ranks[8] = {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};

const bitboard_t white_side = rank_1 | rank_2 | rank_3 | rank_4;
const bitboard_t black_side = rank_5 | rank_6 | rank_7 | rank_8;
const bitboard_t queen_side = file_a | file_b | file_c | file_d;
const bitboard_t king_side =  file_e | file_f | file_g | file_h;
const bitboard_t center = d4 | e4 | d5 | e5;

const bitboard_t white = a8 |  0 | c8 |  0 | e8 |  0 | g8 |  0 | \
                          0 | b7 |  0 | d7 |  0 | f7 |  0 | h7 | \
                         a6 |  0 | c6 |  0 | e6 |  0 | g6 |  0 | \
                          0 | b5 |  0 | d5 |  0 | f5 |  0 | h5 | \
                         a4 |  0 | c4 |  0 | e4 |  0 | g4 |  0 | \
                          0 | b3 |  0 | d3 |  0 | f3 |  0 | h3 | \
                         a2 |  0 | c2 |  0 | e2 |  0 | g2 |  0 | \
                          0 | b1 |  0 | d1 |  0 | f1 |  0 | h1;
const bitboard_t black =  0 | b8 |  0 | d8 |  0 | f8 |  0 | h8 | \
                         a7 |  0 | c7 |  0 | e7 |  0 | g7 |  0 | \
                          0 | b6 |  0 | d6 |  0 | f6 |  0 | h6 | \
                         a5 |  0 | c5 |  0 | e5 |  0 | g5 |  0 | \
                          0 | b4 |  0 | d4 |  0 | f4 |  0 | h4 | \
                         a3 |  0 | c3 |  0 | e3 |  0 | g3 |  0 | \
                          0 | b2 |  0 | d2 |  0 | f2 |  0 | h2 | \
                         a1 |  0 | c1 |  0 | e1 |  0 | g1 |  0;
};


#endif