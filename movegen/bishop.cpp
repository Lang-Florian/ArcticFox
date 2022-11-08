#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __BISHOP__MODULE__
#define __BISHOP__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate bishop moves
template <color_t color, movetype_t movetype, typename T>
void generate_bishop_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);

  bitboard_t checking_squares = detail.bishop_checking_squares;

  bitboard_t non_discoverable_targets = bitboard::none;
  bitboard_t rook_discoverable_targets = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_targets |= checking_squares;
    rook_discoverable_targets |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    rook_discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];
  rook_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t non_discoverable_bishop_pinned_bishops = board.bitboards[bishop] & detail.bishop_pinned & ~detail.rook_pinned & ~detail.rook_discoverable;
  while (non_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(non_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[color::none]) & non_discoverable_targets & bishop_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<bishop>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_free_bishops = board.bitboards[bishop] & ~detail.bishop_pinned & ~detail.rook_pinned & ~detail.rook_discoverable;
  while (non_discoverable_free_bishops) {
    square_t from = pop_lsb(non_discoverable_free_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[color::none]) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<bishop>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t rook_discoverable_bishop_pinned_bishops = board.bitboards[bishop] & detail.bishop_pinned & ~detail.rook_pinned & detail.rook_discoverable;
  while (rook_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(rook_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[color::none]) & rook_discoverable_targets & bishop_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<bishop, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t rook_discoverable_free_bishops = board.bitboards[bishop] & ~detail.bishop_pinned & ~detail.rook_pinned & detail.rook_discoverable;
  while (rook_discoverable_free_bishops) {
    square_t from = pop_lsb(rook_discoverable_free_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[color::none]) & rook_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<bishop, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};


#endif