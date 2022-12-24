#pragma once

#include <array>
#include "base.cpp"
#include "board.cpp"
#include "magic.cpp"

/***********************************************************************
 * 
 * Module to generate and access everything attack related.
 * 
***********************************************************************/

// generate bishop rays at compile time
constexpr std::array<bitboard_t, 64> _generate_bishop_ray() {
  std::array<bitboard_t, 64> bishop_ray{0ULL};
  for (square_t square = 0; square < 64; ++square) {
    bishop_ray[square] = 0ULL;
    for (int i = 1; i <= square % 8 && i <= square / 8; i++)
      bishop_ray[square] |= (bitboard(square) >> (9 * i));
    for (int i = 1; i <= square % 8 && i <= 7 - (square / 8); i++)
      bishop_ray[square] |= (bitboard(square) << (7 * i));
    for (int i = 1; i <= 7 - (square % 8) && i <= 7 - (square / 8); i++)
      bishop_ray[square] |= (bitboard(square) << (9 * i));
    for (int i = 1; i <= 7 - (square % 8) && i <= square / 8; i++)
      bishop_ray[square] |= (bitboard(square) >> (7 * i));
  };
  return bishop_ray;
};
constexpr std::array<bitboard_t, 64> bishop_ray = _generate_bishop_ray();

// generate rook rays at compile time
constexpr std::array<bitboard_t, 64> _generate_rook_ray() {
  std::array<bitboard_t, 64> rook_ray{0ULL};
  for (square_t square = 0; square < 64; ++square) {
    rook_ray[square] = 0ULL;
    for (int i = 1; i <= square % 8; i++)
      rook_ray[square] |= (bitboard(square) >> i);
    for (int i = 1; i <= 7 - (square % 8); i++)
      rook_ray[square] |= (bitboard(square) << i);
    for (int i = 1; i <= square / 8; i++)
      rook_ray[square] |= (bitboard(square) >> (8 * i));
    for (int i = 1; i <= 7 - (square / 8); i++)
      rook_ray[square] |= (bitboard(square) << (8 * i));
  };
  return rook_ray;
};
constexpr std::array<bitboard_t, 64> rook_ray = _generate_rook_ray();

// generate pawn attack tables at compile time
constexpr std::array<std::array<bitboard_t, 4>, 65> _generate_pawn_attack() {
  std::array<std::array<bitboard_t, 4>, 65> pawn_attack{none};
  for (square_t square = 0; square < 64; ++square) {
    pawn_attack[square][white] = ((bitboard(square) >> 9) & ~file_h) | ((bitboard(square) >> 7) & ~file_a);
    pawn_attack[square][black] = ((bitboard(square) << 9) & ~file_a) | ((bitboard(square) << 7) & ~file_h);
  };
  pawn_attack[none_square][white] = none;
  pawn_attack[none_square][black] = none;
  return pawn_attack;
};
constexpr std::array<std::array<bitboard_t, 4>, 65> pawn_attack = _generate_pawn_attack();

// generate knight attack tables at compile time
constexpr std::array<bitboard_t, 64> _generate_knight_attack() {
  std::array<bitboard_t, 64> knight_attack{none};
  for (square_t square = 0; square < 64; ++square) {
    knight_attack[square] = ((bitboard(square) >>  6) & ~file_a & ~file_b & ~rank_1) |
                            ((bitboard(square) <<  6) & ~file_g & ~file_h & ~rank_8) |
                            ((bitboard(square) >> 10) & ~file_g & ~file_h & ~rank_1) |
                            ((bitboard(square) << 10) & ~file_a & ~file_b & ~rank_8) |
                            ((bitboard(square) >> 15) & ~file_a & ~rank_1 & ~rank_2) |
                            ((bitboard(square) << 15) & ~file_h & ~rank_7 & ~rank_8) |
                            ((bitboard(square) >> 17) & ~file_h & ~rank_1 & ~rank_2) |
                            ((bitboard(square) << 17) & ~file_a & ~rank_7 & ~rank_8);
  };
  return knight_attack;
};
constexpr std::array<bitboard_t, 64> knight_attack = _generate_knight_attack();

// generate king attack tables at compile time
constexpr std::array<bitboard_t, 64> _generate_king_attack() {
  std::array<bitboard_t, 64> king_attack{none};
  for (square_t square = 0; square < 64; ++square) {
    king_attack[square] = ((bitboard(square) >> 1) & ~file_h) |
                          ((bitboard(square) << 1) & ~file_a) |
                          ((bitboard(square) >> 8) & ~rank_1) |
                          ((bitboard(square) << 8) & ~rank_8) |
                          ((bitboard(square) >> 7) & ~file_a & ~rank_1) |
                          ((bitboard(square) << 7) & ~file_h & ~rank_8) |
                          ((bitboard(square) >> 9) & ~file_h & ~rank_1) |
                          ((bitboard(square) << 9) & ~file_a & ~rank_8);
  };
  return king_attack;
};
constexpr std::array<bitboard_t, 64> king_attack = _generate_king_attack();

// access to all the piece attack tables
template <piece_t piece>
bitboard_t attack(square_t square, bitboard_t occupancy=none) {
  if constexpr (piece == white_pawn) {
    return pawn_attack[square][white];
  } else if constexpr (piece == black_pawn) {
    return pawn_attack[square][black];
  } else if constexpr (piece_type(piece) == knight) {
    return knight_attack[square];
  } else if constexpr (piece_type(piece) == bishop) {
    return magic_table[
      bishop_magics[square].offset +
      (((occupancy | bishop_magics[square].mask) * bishop_magics[square].magic_number) >> 55)
    ];
  } else if constexpr (piece_type(piece) == rook) {
    return magic_table[
      rook_magics[square].offset +
      (((occupancy | rook_magics[square].mask) * rook_magics[square].magic_number) >> 52)
    ];
  } else if constexpr (piece_type(piece) == queen) {
    return (
      magic_table[
        bishop_magics[square].offset +
        (((occupancy | bishop_magics[square].mask) * bishop_magics[square].magic_number) >> 55)
      ] |
      magic_table[
        rook_magics[square].offset +
        (((occupancy | rook_magics[square].mask) * rook_magics[square].magic_number) >> 52)
      ]
    );
  } else if constexpr (piece_type(piece) == king) {
    return king_attack[square];
  } else {
    return none;
  };
};

// generate multi pawn attacks at runtime
template<color_t color>
bitboard_t multi_pawn_attack(bitboard_t pawns) {
  if constexpr (color == white)
    return ((pawns >> 9) & ~file_h) | ((pawns >> 7) & ~file_a);
  else
    return ((pawns << 9) & ~file_a) | ((pawns << 7) & ~file_h);
};

// generate multi knight attacks at runtime
bitboard_t multi_knight_attack(bitboard_t knights) {
  return (
    ((knights >>  6) & ~file_a & ~file_b & ~rank_1) |
    ((knights <<  6) & ~file_g & ~file_h & ~rank_8) |
    ((knights >> 10) & ~file_g & ~file_h & ~rank_1) |
    ((knights << 10) & ~file_a & ~file_b & ~rank_8) |
    ((knights >> 15) & ~file_a & ~rank_1 & ~rank_2) |
    ((knights << 15) & ~file_h & ~rank_7 & ~rank_8) |
    ((knights >> 17) & ~file_h & ~rank_1 & ~rank_2) |
    ((knights << 17) & ~file_a & ~rank_7 & ~rank_8)
  );
};

// get all the attacks on a square
bitboard_t attackers(Board& board, square_t square) {
  return (
    (attack<black_pawn>(square) & board.bitboards[white_pawn]) |
    (attack<white_pawn>(square) & board.bitboards[black_pawn]) |
    (attack<knight>(square) & board.bitboards[knight]) |
    (attack<bishop>(square, board.bitboards[none]) & (board.bitboards[bishop] | board.bitboards[queen])) |
    (attack<rook>(square, board.bitboards[none]) & (board.bitboards[rook] | board.bitboards[queen])) |
    (attack<king>(square) & board.bitboards[king])
  );
};

// get all the attacks of a color on a square
template <color_t color>
bitboard_t attackers(Board& board, square_t square) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t color_knight = to_color(knight, color);
  constexpr piece_t color_bishop = to_color(bishop, color);
  constexpr piece_t color_rook = to_color(rook, color);
  constexpr piece_t color_queen = to_color(queen, color);
  constexpr piece_t color_king = to_color(king, color);
  constexpr piece_t opponent_pawn = to_color(pawn, opponent);
  return (
    (attack<opponent_pawn>(square) & board.bitboards[color_pawn]) |
    (attack<knight>(square) & board.bitboards[color_knight]) |
    (attack<bishop>(square, board.bitboards[none]) & (board.bitboards[color_bishop] | board.bitboards[color_queen])) |
    (attack<rook>(square, board.bitboards[none]) & (board.bitboards[color_rook] | board.bitboards[color_queen])) |
    (attack<king>(square) & board.bitboards[color_king])
  );
};

// get all attacks of a color
template<color_t color>
bitboard_t attacks(Board& board) {
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t color_knight = to_color(knight, color);
  constexpr piece_t color_bishop = to_color(bishop, color);
  constexpr piece_t color_rook = to_color(rook, color);
  constexpr piece_t color_queen = to_color(queen, color);
  constexpr piece_t color_king = to_color(king, color);
  bitboard_t attacked_squares = 0;
  attacked_squares |= multi_pawn_attack<color>(board.bitboards[color_pawn]);
  attacked_squares |= multi_knight_attack(board.bitboards[color_knight]);
  bitboard_t bishop_movers = board.bitboards[color_bishop] | board.bitboards[color_queen];
  while (bishop_movers) {
    square_t square = pop_lsb(bishop_movers);
    attacked_squares |= attack<bishop>(square, board.bitboards[none]);
  };
  bitboard_t rook_movers = board.bitboards[color_rook] | board.bitboards[color_queen];
  while (rook_movers) {
    square_t square = pop_lsb(rook_movers);
    attacked_squares |= attack<rook>(square, board.bitboards[none]);
  };
  square_t king_square = get_lsb(board.bitboards[color_king]);
  attacked_squares |= attack<king>(king_square);
  return attacked_squares;
};