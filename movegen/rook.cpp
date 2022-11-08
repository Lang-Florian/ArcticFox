#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __ROOK__MODULE__
#define __ROOK__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate rook moves
template <color_t color, movetype_t movetype, typename T>
void generate_rook_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);

  bitboard_t checking_squares = detail.rook_checking_squares;

  bitboard_t non_discoverable_targets = bitboard::none;
  bitboard_t bishop_discoverable_targets = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_targets |= checking_squares;
    bishop_discoverable_targets |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_targets |= board.bitboards[opponent];
    bishop_discoverable_targets |= board.bitboards[opponent];
  };
  non_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];
  bishop_discoverable_targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t non_discoverable_rook_pinned_rooks = board.bitboards[rook] & ~detail.bishop_pinned & detail.rook_pinned & ~detail.bishop_discoverable;
  while (non_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(non_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & non_discoverable_targets & attack::ray::rook[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<rook>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_free_rooks = board.bitboards[rook] & ~detail.bishop_pinned & ~detail.rook_pinned & ~detail.bishop_discoverable;
  while (non_discoverable_free_rooks) {
    square_t from = pop_lsb(non_discoverable_free_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<rook>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t bishop_discoverable_rook_pinned_rooks = board.bitboards[rook] & ~detail.bishop_pinned & detail.rook_pinned & detail.bishop_discoverable;
  while (bishop_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(bishop_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & bishop_discoverable_targets & attack::ray::rook[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<rook, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t bishop_discoverable_free_rooks = board.bitboards[rook] & ~detail.bishop_pinned & ~detail.rook_pinned & detail.bishop_discoverable;
  while (bishop_discoverable_free_rooks) {
    square_t from = pop_lsb(bishop_discoverable_free_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & bishop_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<rook, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};


#endif