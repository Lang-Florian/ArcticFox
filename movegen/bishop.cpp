#pragma once

#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate bishop moves
template <color_t color, movetype_t movetype, typename T>
void generate_bishop_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_bishop = to_color(bishop, color);

  bitboard_t checking_squares = detail.bishop_checking_squares;

  bitboard_t non_discoverable_targets = none;
  bitboard_t rook_discoverable_targets = none;
  if constexpr (movetype & quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & check) {
    non_discoverable_targets |= checking_squares;
    rook_discoverable_targets |= full;
  };
  if constexpr (movetype & capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    rook_discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];
  rook_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t non_discoverable_bishop_pinned_bishops = board.bitboards[color_bishop] & detail.bishop_pinned & ~detail.rook_pinned & ~detail.rook_discoverable;
  while (non_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(non_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[none]) & non_discoverable_targets & bishop_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_bishop, color_bishop, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_free_bishops = board.bitboards[color_bishop] & ~detail.bishop_pinned & ~detail.rook_pinned & ~detail.rook_discoverable;
  while (non_discoverable_free_bishops) {
    square_t from = pop_lsb(non_discoverable_free_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[none]) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_bishop, color_bishop, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t rook_discoverable_bishop_pinned_bishops = board.bitboards[color_bishop] & detail.bishop_pinned & ~detail.rook_pinned & detail.rook_discoverable;
  while (rook_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(rook_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[none]) & rook_discoverable_targets & bishop_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_bishop, color_bishop, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t rook_discoverable_free_bishops = board.bitboards[color_bishop] & ~detail.bishop_pinned & ~detail.rook_pinned & detail.rook_discoverable;
  while (rook_discoverable_free_bishops) {
    square_t from = pop_lsb(rook_discoverable_free_bishops);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[none]) & rook_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_bishop, color_bishop, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};