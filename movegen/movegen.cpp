#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MOVEGEN__MODULE__
#define __MOVEGEN__MODULE__


#include <string>
#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"
#include "knight.cpp"
#include "bishop.cpp"
#include "rook.cpp"
#include "queen.cpp"
#include "king.cpp"
#include "king_castling.cpp"
#include "pawn_push.cpp"
#include "pawn_capture.cpp"
#include "pawn_enpassant.cpp"


/*

  Module for move generation.

*/


namespace movegen {
// generate all moves of a given movetype
template<color_t color, movetype_t movetype, typename T>
T generate(board::Board& board) {
  T moves;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    moves.clear();
  } else {
    moves = 0;
  };
  detail_t detail;
  detail.update<color>(board);
  if (popcount(detail.checkers) > 1) {
    generate_king_moves<color, movetype>(moves, board, detail);
    return moves;
  };
  generate_pawn_push_moves<color, movetype>(moves, board, detail);
  generate_pawn_capture_moves<color, movetype>(moves, board, detail);
  generate_pawn_enpassant_moves<color, movetype>(moves, board, detail);
  generate_knight_moves<color, movetype>(moves, board, detail);
  generate_bishop_moves<color, movetype>(moves, board, detail);
  generate_rook_moves<color, movetype>(moves, board, detail);
  generate_queen_moves<color, movetype>(moves, board, detail);
  generate_king_moves<color, movetype>(moves, board, detail);
  generate_king_castling_moves<color, movetype>(moves, board, detail);
  return moves;
};
};


#endif