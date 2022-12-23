#pragma once

#include <type_traits>
#include "../attack.cpp"
#include "../base.cpp"
#include "../board.cpp"
#include "detail.cpp"

template <color_t color, movetype_t movetype, typename T>
void generate_king_castling_moves(T& moves, Board& board, detail_t& detail) {
  if constexpr (color == white) {
    // check if white king can castle kingside
    bool castle_king = (
      !(detail.attacked_squares & white_OO_king_traverse) &&
      !(board.bitboards[none] & white_OO_rook_traverse) &&
      (board.castling & white_OO) &&
      (((movetype & check) && (F1 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(F1 & detail.rook_checking_squares_castling)))
    );
    // check if white king can castle queenside
    bool castle_queen = (
      !(detail.attacked_squares & white_OOO_king_traverse) &&
      !(board.bitboards[none] & white_OOO_rook_traverse) &&
      (board.castling & white_OOO) &&
      (((movetype & check) && (D1 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(D1 & detail.rook_checking_squares_castling)))
    );
    // generate white castling moves
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move(e1, g1, white_king, white_king, none, false, false, true, false, F1 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move(e1, c1, white_king, white_king, none, false, false, true, false, D1 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  } else {
    // check if black king can castle kingside
    bool castle_king = (
      !(detail.attacked_squares & black_OO_king_traverse) &&
      !(board.bitboards[none] & black_OO_rook_traverse) &&
      (board.castling & black_OO) &&
      (((movetype & check) && (F8 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(F8 & detail.rook_checking_squares_castling)))
    );
    // check if black king can castle queenside
    bool castle_queen = (
      !(detail.attacked_squares & black_OOO_king_traverse) &&
      !(board.bitboards[none] & black_OOO_rook_traverse) &&
      (board.castling & black_OOO) &&
      (((movetype & check) && (D8 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(D8 & detail.rook_checking_squares_castling)))
    );
    // generate black castling moves
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move(e8, g8, black_king, black_king, none, false, false, true, false, F8 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move(e8, c8, black_king, black_king, none, false, false, true, false, D8 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  };
};