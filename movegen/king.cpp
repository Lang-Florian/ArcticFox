#pragma once

#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate king moves
template <color_t turn, movetype_t movetype, typename T>
void generate_king_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(turn);
  constexpr piece_t turn_king = to_color(king, turn);

  bitboard_t checking_squares = none;

  if (board.bitboards[turn_king] & detail.bishop_discoverable)
    checking_squares |= ~bishop_ray[detail.opponent_king_square];
  if (board.bitboards[turn_king] & detail.rook_discoverable)
    checking_squares |= ~rook_ray[detail.opponent_king_square];
  
  bitboard_t targets = none;
  if constexpr (movetype & quiet)
    targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & check)
    targets |= checking_squares;
  if constexpr (movetype & capture)
    targets |= board.bitboards[opponent];
  targets &= ~board.bitboards[turn] & ~detail.unsafe_king_squares;
  
  bitboard_t possible_to = attack<king>(detail.king_square) & targets;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (possible_to) {
      square_t to = pop_lsb(possible_to);
      moves.push(piece_move<king>(detail.king_square, to, board.pieces[to], bitboard(to) & checking_squares));
    };
  } else {
    moves += popcount(possible_to);
  };
};