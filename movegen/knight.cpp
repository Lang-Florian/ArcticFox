#pragma once

#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate knight moves
template <color_t turn, movetype_t movetype, typename T>
void generate_knight_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(turn);
  constexpr piece_t color_knight = to_color(knight, turn);

  bitboard_t checking_squares = attack<knight>(detail.opponent_king_square);

  bitboard_t non_discoverable_targets = none;
  bitboard_t discoverable_targets = none;
  if constexpr (movetype & quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & check) {
    non_discoverable_targets |= checking_squares;
    discoverable_targets |= full;
  };
  if constexpr (movetype & capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[turn];
  discoverable_targets &= detail.evasion_targets & ~board.bitboards[turn];

  bitboard_t non_discoverable_free_knights = board.bitboards[color_knight] & ~detail.bishop_pinned & ~detail.rook_pinned & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_free_knights) {
    square_t from = pop_lsb(non_discoverable_free_knights);
    bitboard_t possible_to = attack<knight>(from) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_knight, color_knight, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t discoverable_free_knights = board.bitboards[color_knight] & ~detail.bishop_pinned & ~detail.rook_pinned & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_free_knights) {
    square_t from = pop_lsb(discoverable_free_knights);
    bitboard_t possible_to = attack<knight>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_knight, color_knight, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};