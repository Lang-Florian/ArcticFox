#pragma once

#include <type_traits>
#include "../attack.cpp"
#include "../base.cpp"
#include "../board.cpp"
#include "detail.cpp"

template <color_t color, movetype_t movetype, typename T>
void generate_pawn_capture_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t color_knight = to_color(knight, color);
  constexpr piece_t color_bishop = to_color(bishop, color);
  constexpr piece_t color_rook = to_color(rook, color);
  constexpr piece_t color_queen = to_color(queen, color);
  constexpr piece_t opponent_pawn = to_color(pawn, opponent);
  constexpr bitboard_t prepromotion_rank = (color == white) ? rank_7 : rank_2;
  // get the types of pawns
  bitboard_t free_pawns = board.bitboards[color_pawn] & ~detail.bishop_pinned & ~detail.rook_pinned;
  bitboard_t bishop_pinned_pawns = board.bitboards[color_pawn] & detail.bishop_pinned & ~detail.rook_pinned;
  // get the squares that could give direct checks
  bitboard_t pawn_checking_squares = attack<opponent_pawn>(detail.opponent_king_square);
  bitboard_t knight_checking_squares = attack<knight>(detail.opponent_king_square);
  bitboard_t bishop_checking_squares_no_pawns = attack<bishop>(detail.opponent_king_square, board.bitboards[none] & ~(board.bitboards[color_pawn] & prepromotion_rank));
  // define the targets for the types of pawns
  bitboard_t non_discoverable_targets = none;
  bitboard_t non_discoverable_knight_promoting_targets = none;
  bitboard_t non_discoverable_bishop_promoting_targets = none;
  bitboard_t non_discoverable_rook_promoting_targets = none;
  bitboard_t non_discoverable_queen_promoting_targets = none;
  bitboard_t discoverable_targets = none;
  if constexpr (movetype & check) {
    non_discoverable_targets |= pawn_checking_squares;
    non_discoverable_knight_promoting_targets |= knight_checking_squares;
    non_discoverable_bishop_promoting_targets |= bishop_checking_squares_no_pawns;
    non_discoverable_rook_promoting_targets |= detail.rook_checking_squares;
    non_discoverable_queen_promoting_targets |= (bishop_checking_squares_no_pawns | detail.rook_checking_squares);
    discoverable_targets |= full;
  };
  if constexpr (movetype & capture) {
    non_discoverable_targets |= full;
    non_discoverable_knight_promoting_targets |= full;
    non_discoverable_bishop_promoting_targets |= full;
    non_discoverable_rook_promoting_targets |= full;
    non_discoverable_queen_promoting_targets |= full;
    discoverable_targets |= full;
  };
  non_discoverable_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_knight_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_bishop_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_rook_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_queen_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  discoverable_targets &= detail.evasion_targets & board.bitboards[opponent];
  // generate pawn capture moves
  bitboard_t discoverable_promoting_free_pawns = free_pawns & prepromotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_promoting_free_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_knight, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_bishop, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_rook, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_queen, board.pieces[to], 0, 0, 0, 1, 1));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };
  bitboard_t discoverable_not_promoting_free_pawns = free_pawns & ~prepromotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_pawn, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & prepromotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_knight, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_bishop, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_rook, board.pieces[to], 0, 0, 0, 1, 1));
        moves.push(move(from, to, color_pawn, color_queen, board.pieces[to], 0, 0, 0, 1, 1));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };
  bitboard_t discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~prepromotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_pawn, board.pieces[to], 0, 0, 0, 0, 1));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t non_discoverable_promoting_free_pawns = free_pawns & prepromotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_free_pawns);
    bitboard_t bishop_checking_squares = attack<bishop>(detail.opponent_king_square, board.bitboards[none] & ~bitboard(from));
    bitboard_t queen_checking_squares = bishop_checking_squares | detail.rook_checking_squares;
    bitboard_t possible_to_knight = attack<color_pawn>(from) & non_discoverable_knight_promoting_targets;
    bitboard_t possible_to_bishop = attack<color_pawn>(from) & non_discoverable_bishop_promoting_targets;
    bitboard_t possible_to_rook = attack<color_pawn>(from) & non_discoverable_rook_promoting_targets;
    bitboard_t possible_to_queen = attack<color_pawn>(from) & non_discoverable_queen_promoting_targets;
    if constexpr ((movetype & check) && !(movetype & capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move(from, to, color_pawn, color_knight, board.pieces[to], 0, 0, 0, 1, bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move(from, to, color_pawn, color_bishop, board.pieces[to], 0, 0, 0, 1, bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move(from, to, color_pawn, color_rook, board.pieces[to], 0, 0, 0, 1, bitboard(to) & detail.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move(from, to, color_pawn, color_queen, board.pieces[to], 0, 0, 0, 1, bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };
  bitboard_t non_discoverable_not_promoting_free_pawns = free_pawns & ~prepromotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_pawn, board.pieces[to], 0, 0, 0, 0, bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t non_discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & prepromotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_bishop_pinned_pawns);
    bitboard_t bishop_checking_squares = attack<bishop>(detail.opponent_king_square, board.bitboards[none] & ~bitboard(from));
    bitboard_t queen_checking_squares = bishop_checking_squares | detail.rook_checking_squares;
    bitboard_t possible_to_knight = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & non_discoverable_knight_promoting_targets;
    bitboard_t possible_to_bishop = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & non_discoverable_bishop_promoting_targets;
    bitboard_t possible_to_rook = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & non_discoverable_rook_promoting_targets;
    bitboard_t possible_to_queen = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & non_discoverable_queen_promoting_targets;
    if constexpr ((movetype & check) && !(movetype & capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move(from, to, color_pawn, color_knight, board.pieces[to], 0, 0, 0, 1, bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move(from, to, color_pawn, color_bishop, board.pieces[to], 0, 0, 0, 1, bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move(from, to, color_pawn, color_rook, board.pieces[to], 0, 0, 0, 1, bitboard(to) & detail.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move(from, to, color_pawn, color_queen, board.pieces[to], 0, 0, 0, 1, bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };
  bitboard_t non_discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~prepromotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack<color_pawn>(from) & bishop_ray[detail.color_king_square] & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move(from, to, color_pawn, color_pawn, board.pieces[to], 0, 0, 0, 0, bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};