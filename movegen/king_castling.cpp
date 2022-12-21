#pragma once


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate king castling moves
template <color_t color, movetype_t movetype, typename T>
void generate_king_castling_moves(T& moves, Board& board, detail_t& detail) {
  if constexpr (color == white) {
    bool castle_king = (
      !(detail.attacked_squares & white_OO_king_traverse) &&
      !(board.bitboards[none] & white_OO_rook_traverse) &&
      (board.castling & white_OO) &&
      (((movetype & check) && (F1 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(F1 & detail.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(detail.attacked_squares & white_OOO_king_traverse) &&
      !(board.bitboards[none] & white_OOO_rook_traverse) &&
      (board.castling & white_OOO) &&
      (((movetype & check) && (D1 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(D1 & detail.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(castling_move<color, true>(F1 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(castling_move<color, false>(D1 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  } else {
    bool castle_king = (
      !(detail.attacked_squares & black_OO_king_traverse) &&
      !(board.bitboards[none] & black_OO_rook_traverse) &&
      (board.castling & black_OO) &&
      (((movetype & check) && (F8 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(F8 & detail.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(detail.attacked_squares & black_OOO_king_traverse) &&
      !(board.bitboards[none] & black_OOO_rook_traverse) &&
      (board.castling & black_OOO) &&
      (((movetype & check) && (D8 & detail.rook_checking_squares_castling)) || ((movetype & quiet) && !(D8 & detail.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(castling_move<color, true>(F8 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(castling_move<color, false>(D8 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  };
};