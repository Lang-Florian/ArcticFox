#pragma once

#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate rook moves
template <color_t color, movetype_t movetype, typename T>
void generate_rook_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_rook = to_color(rook, color);

  bitboard_t checking_squares = detail.rook_checking_squares;

  bitboard_t non_discoverable_targets = none;
  bitboard_t bishop_discoverable_targets = none;
  if constexpr (movetype & quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & check) {
    non_discoverable_targets |= checking_squares;
    bishop_discoverable_targets |= full;
  };
  if constexpr (movetype & capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    bishop_discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];
  bishop_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t non_discoverable_rook_pinned_rooks = board.bitboards[color_rook] & ~detail.bishop_pinned & detail.rook_pinned & ~detail.bishop_discoverable;
  while (non_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(non_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack<rook>(from, board.bitboards[none]) & non_discoverable_targets & rook_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_rook, color_rook, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_free_rooks = board.bitboards[color_rook] & ~detail.bishop_pinned & ~detail.rook_pinned & ~detail.bishop_discoverable;
  while (non_discoverable_free_rooks) {
    square_t from = pop_lsb(non_discoverable_free_rooks);
    bitboard_t possible_to = attack<rook>(from, board.bitboards[none]) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_rook, color_rook, board.pieces[to], 0, 0, 0, 0, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t bishop_discoverable_rook_pinned_rooks = board.bitboards[color_rook] & ~detail.bishop_pinned & detail.rook_pinned & detail.bishop_discoverable;
  while (bishop_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(bishop_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack<rook>(from, board.bitboards[none]) & bishop_discoverable_targets & rook_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_rook, color_rook, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t bishop_discoverable_free_rooks = board.bitboards[rook] & ~detail.bishop_pinned & ~detail.rook_pinned & detail.bishop_discoverable;
  while (bishop_discoverable_free_rooks) {
    square_t from = pop_lsb(bishop_discoverable_free_rooks);
    bitboard_t possible_to = attack<rook>(from, board.bitboards[none]) & bishop_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_rook, color_rook, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};