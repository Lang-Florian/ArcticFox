#pragma once

#include <type_traits>
#include "../attack.cpp"
#include "../base.cpp"
#include "../board.cpp"
#include "detail.cpp"

template <color_t color, movetype_t movetype, typename T>
void generate_king_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_king = to_color(king, color);
  // get the squares that would give discovered checks
  bitboard_t checking_squares = none;
  if (board.bitboards[color_king] & detail.bishop_discoverable)
    checking_squares |= ~bishop_ray[detail.opponent_king_square];
  if (board.bitboards[color_king] & detail.rook_discoverable)
    checking_squares |= ~rook_ray[detail.opponent_king_square];
  // define the targets for the king
  bitboard_t targets = none;
  if constexpr (movetype & quiet)
    targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & check)
    targets |= checking_squares;
  if constexpr (movetype & capture)
    targets |= board.bitboards[opponent];
  targets &= ~board.bitboards[color] & ~detail.unsafe_king_squares;
  // generate king moves
  bitboard_t possible_to = attack<king>(detail.color_king_square) & targets;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (possible_to) {
      square_t to = pop_lsb(possible_to);
      moves.push(move(detail.color_king_square, to, color_king, color_king, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
    };
  } else {
    moves += popcount(possible_to);
  };
};