#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __KNIGHT__MODULE__
#define __KNIGHT__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate knight moves
template <color_t color, movetype_t movetype, typename T>
void generate_knight_moves(T& moves, board::Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);

  bitboard_t checking_squares = attack::attack<knight>(detail.opponent_king_square);

  bitboard_t non_discoverable_targets = bitboard::none;
  bitboard_t discoverable_targets = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_targets |= checking_squares;
    discoverable_targets |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];
  discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t non_discoverable_free_knights = board.bitboards[knight] & ~detail.bishop_pinned & ~detail.rook_pinned & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_free_knights) {
    square_t from = pop_lsb(non_discoverable_free_knights);
    bitboard_t possible_to = attack::attack<knight>(from) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<knight>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t discoverable_free_knights = board.bitboards[knight] & ~detail.bishop_pinned & ~detail.rook_pinned & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_free_knights) {
    square_t from = pop_lsb(discoverable_free_knights);
    bitboard_t possible_to = attack::attack<knight>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<knight, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};


#endif