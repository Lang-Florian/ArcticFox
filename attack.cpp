#pragma once

#include <array>
#include "base.cpp"
#include "board.cpp"
#include "magic.cpp"

/*

  Module to generate piece attacks.

*/

// generate the bishop and rook attack rays at compile time
constexpr std::array<bitboard_t, 64> _generate_bishop_ray() {
  std::array<bitboard_t, 64> bishop_ray{0ULL};
  for (square_t square=0; square<64; ++square) {
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

constexpr std::array<bitboard_t, 64> _generate_rook_ray() {
  std::array<bitboard_t, 64> rook_ray{0ULL};
  for (square_t square=0; square<64; ++square) {
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

// generate the pawn knight and king attack tables at compile time
constexpr std::array<std::array<bitboard_t, 2>, 65> _generate_pawn_attack() {
  std::array<std::array<bitboard_t, 2>, 65> pawn_attack{bitboard::none};
  for (square_t square=0; square<64; ++square) {
    pawn_attack[square][color::white] = ((bitboard(square) >> 9) & ~bitboard::file_h) | ((bitboard(square) >> 7) & ~bitboard::file_a);
    pawn_attack[square][color::black] = ((bitboard(square) << 9) & ~bitboard::file_a) | ((bitboard(square) << 7) & ~bitboard::file_h);
  };
  pawn_attack[square::none][color::white] = bitboard::none;
  pawn_attack[square::none][color::black] = bitboard::none;
  return pawn_attack;
};
constexpr std::array<std::array<bitboard_t, 2>, 65> pawn_attack = _generate_pawn_attack();

constexpr std::array<bitboard_t, 64> _generate_knight_attack() {
  std::array<bitboard_t, 64> knight_attack{bitboard::none};
  for (square_t square=0; square<64; ++square) {
    knight_attack[square] = ((bitboard(square) >>  6) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_1) |
                     ((bitboard(square) <<  6) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_8) |
                     ((bitboard(square) >> 10) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_1) |
                     ((bitboard(square) << 10) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_8) |
                     ((bitboard(square) >> 15) & ~bitboard::file_a & ~bitboard::rank_1 & ~bitboard::rank_2) |
                     ((bitboard(square) << 15) & ~bitboard::file_h & ~bitboard::rank_7 & ~bitboard::rank_8) |
                     ((bitboard(square) >> 17) & ~bitboard::file_h & ~bitboard::rank_1 & ~bitboard::rank_2) |
                     ((bitboard(square) << 17) & ~bitboard::file_a & ~bitboard::rank_7 & ~bitboard::rank_8);
  };
  return knight_attack;
};
constexpr std::array<bitboard_t, 64> knight_attack = _generate_knight_attack();

constexpr std::array<bitboard_t, 64> _generate_king_attack() {
  std::array<bitboard_t, 64> king_attack{bitboard::none};
  for (square_t square=0; square<64; ++square) {
    king_attack[square] = ((bitboard(square) >> 1) & ~bitboard::file_h) |
                   ((bitboard(square) << 1) & ~bitboard::file_a) |
                   ((bitboard(square) >> 8) & ~bitboard::rank_1) |
                   ((bitboard(square) << 8) & ~bitboard::rank_8) |
                   ((bitboard(square) >> 7) & ~bitboard::file_a & ~bitboard::rank_1) |
                   ((bitboard(square) << 7) & ~bitboard::file_h & ~bitboard::rank_8) |
                   ((bitboard(square) >> 9) & ~bitboard::file_h & ~bitboard::rank_1) |
                   ((bitboard(square) << 9) & ~bitboard::file_a & ~bitboard::rank_8);
  };
  return king_attack;
};
constexpr std::array<bitboard_t, 64> king_attack = _generate_king_attack();

// access to all the piece attacks
template <piece_t piece>
bitboard_t attack(square_t square, bitboard_t occupancy=bitboard::none) {
  if constexpr (piece == piece::white_pawn) {
    return pawn_attack[square][color::white];
  } else if constexpr (piece == piece::black_pawn) {
    return pawn_attack[square][color::black];
  } else if constexpr (piece == piece::knight || piece == piece::white_knight || piece == piece::black_knight) {
    return knight_attack[square];
  } else if constexpr (piece == piece::bishop || piece == piece::white_bishop || piece == piece::black_bishop) {
    return magic_table[
      bishop_magics[square].offset +
      (((occupancy | bishop_magics[square].mask) * bishop_magics[square].magic_number) >> 55)
    ];
  } else if constexpr (piece == piece::rook || piece == piece::white_rook || piece == piece::black_rook) {
    return magic_table[
      rook_magics[square].offset +
      (((occupancy | rook_magics[square].mask) * rook_magics[square].magic_number) >> 52)
    ];
  } else if constexpr (piece == piece::queen || piece == piece::white_queen || piece == piece::black_queen) {
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
  } else if constexpr (piece == piece::king || piece == piece::white_king || piece == piece::black_king) {
    return king_attack[square];
  } else {
    return bitboard::none;
  };
};

template<color_t color>
bitboard_t multi_pawn_attack(bitboard_t pawns) {
  if constexpr (color == color::white) {
    return ((pawns >> 9) & ~bitboard::file_h) | ((pawns >> 7) & ~bitboard::file_a);
  } else {
    return ((pawns << 9) & ~bitboard::file_a) | ((pawns << 7) & ~bitboard::file_h);
  };
};

bitboard_t multi_knight_attack(bitboard_t knights) {
  return (
    ((knights >>  6) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_1) |
    ((knights <<  6) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_8) |
    ((knights >> 10) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_1) |
    ((knights << 10) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_8) |
    ((knights >> 15) & ~bitboard::file_a & ~bitboard::rank_1 & ~bitboard::rank_2) |
    ((knights << 15) & ~bitboard::file_h & ~bitboard::rank_7 & ~bitboard::rank_8) |
    ((knights >> 17) & ~bitboard::file_h & ~bitboard::rank_1 & ~bitboard::rank_2) |
    ((knights << 17) & ~bitboard::file_a & ~bitboard::rank_7 & ~bitboard::rank_8)
  );
};

// get all the attacks on a square
bitboard_t attackers(Board& board, square_t square) {
  return (
    (attack<piece::black_pawn>(square) & board.bitboards[piece::white_pawn]) |
    (attack<piece::white_pawn>(square) & board.bitboards[piece::black_pawn]) |
    (attack<piece::knight>(square) & board.bitboards[piece::knight]) |
    (attack<piece::bishop>(square, board.bitboards[piece::none]) & (board.bitboards[piece::bishop] | board.bitboards[piece::queen])) |
    (attack<piece::rook>(square, board.bitboards[piece::none]) & (board.bitboards[piece::rook] | board.bitboards[piece::queen])) |
    (attack<piece::king>(square) & board.bitboards[piece::king])
  );
};

// get all the attacks of a color on a square
template <color_t color>
bitboard_t attackers(Board& board, square_t square) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  return (
    (attack<opponent_pawn>(square) & board.bitboards[pawn]) |
    (attack<piece::knight>(square) & board.bitboards[knight]) |
    (attack<piece::bishop>(square, board.bitboards[piece::none]) & (board.bitboards[bishop] | board.bitboards[queen])) |
    (attack<piece::rook>(square, board.bitboards[piece::none]) & (board.bitboards[rook] | board.bitboards[queen])) |
    (attack<piece::king>(square) & board.bitboards[king])
  );
};

// get all attacks of a color
template<color_t color>
bitboard_t attacks(Board& board) {
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
  bitboard_t attacked_squares = multi_pawn_attack<color>(board.bitboards[pawn]);
  attacked_squares |= multi_knight_attack(board.bitboards[knight]);
  bitboard_t bishop_movers = board.bitboards[bishop] | board.bitboards[queen];
  while (bishop_movers) {
    square_t square = pop_lsb(bishop_movers);
    attacked_squares |= attack<piece::bishop>(square, board.bitboards[piece::none]);
  };
  bitboard_t rook_movers = board.bitboards[rook] | board.bitboards[queen];
  while (rook_movers) {
    square_t square = pop_lsb(rook_movers);
    attacked_squares |= attack<piece::rook>(square, board.bitboards[piece::none]);
  };
  square_t king_square = get_lsb(board.bitboards[king]);
  attacked_squares |= attack<piece::king>(king_square);
  return attacked_squares;
};