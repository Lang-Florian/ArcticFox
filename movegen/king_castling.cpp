#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __KING_CASTLING__MODULE__
#define __KING_CASTLING__MODULE__


#include <type_traits>
#include "../base/base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate king castling moves
template <color_t color, movetype_t movetype, typename T>
void generate_king_castling_moves(T& moves, board::Board& board, detail_t& detail) {
  if constexpr (color == color::white) {
    bool castle_king = (
      !(detail.attacked_squares & castling::white_king_attack_mask) &&
      !(board.bitboards[color::none] & castling::white_king_piece_mask) &&
      (board.castling & castling::white_king) &&
      (((movetype & movetype::check) && (bitboard::f1 & detail.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::f1 & detail.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(detail.attacked_squares & castling::white_queen_attack_mask) &&
      !(board.bitboards[color::none] & castling::white_queen_piece_mask) &&
      (board.castling & castling::white_queen) &&
      (((movetype & movetype::check) && (bitboard::d1 & detail.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::d1 & detail.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move::compiletime::castling_move<color, true>(bitboard::f1 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move::compiletime::castling_move<color, false>(bitboard::d1 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  } else {
    bool castle_king = (
      !(detail.attacked_squares & castling::black_king_attack_mask) &&
      !(board.bitboards[color::none] & castling::black_king_piece_mask) &&
      (board.castling & castling::black_king) &&
      (((movetype & movetype::check) && (bitboard::f8 & detail.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::f8 & detail.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(detail.attacked_squares & castling::black_queen_attack_mask) &&
      !(board.bitboards[color::none] & castling::black_queen_piece_mask) &&
      (board.castling & castling::black_queen) &&
      (((movetype & movetype::check) && (bitboard::d8 & detail.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::d8 & detail.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move::compiletime::castling_move<color, true>(bitboard::f8 & detail.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move::compiletime::castling_move<color, false>(bitboard::d8 & detail.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  };
};


#endif