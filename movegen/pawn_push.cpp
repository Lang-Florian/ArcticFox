#pragma once

#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate pawn push moves
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_push_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t color_knight = to_color(knight, color);
  constexpr piece_t color_bishop = to_color(bishop, color);
  constexpr piece_t color_rook = to_color(rook, color);
  constexpr piece_t color_queen = to_color(queen, color);
  constexpr piece_t opponent_pawn = to_color(pawn, opponent);

  constexpr bitboard_t promotion_rank = (color == white) ? rank_8 : rank_1;
  constexpr int push_offset = (color == white ? -8 : 8);
  constexpr int doublepush_offset = 2 * push_offset;

  bitboard_t free_pawns = board.bitboards[color_pawn] & ~detail.bishop_pinned & ~detail.rook_pinned;
  bitboard_t rook_pinned_pawns = board.bitboards[color_pawn] & ~detail.bishop_pinned & detail.rook_pinned;
  bitboard_t pawn_checking_squares = attack<opponent_pawn>(detail.opponent_king_square);
  bitboard_t knight_checking_squares = attack<knight>(detail.opponent_king_square);
  bitboard_t rook_checking_squares_no_pawns = attack<rook>(detail.opponent_king_square, board.bitboards[none] & ~board.bitboards[color_pawn]);

  bitboard_t non_discoverable_targets = none;
  bitboard_t non_discoverable_knight_promoting_targets = none;
  bitboard_t non_discoverable_bishop_promoting_targets = none;
  bitboard_t non_discoverable_rook_promoting_targets = none;
  bitboard_t non_discoverable_queen_promoting_targets = none;
  bitboard_t discoverable_targets = none;
  if constexpr (movetype & quiet) {
    non_discoverable_targets |= ~pawn_checking_squares;
    non_discoverable_knight_promoting_targets |= ~knight_checking_squares;
    non_discoverable_bishop_promoting_targets |= ~detail.bishop_checking_squares;
    non_discoverable_rook_promoting_targets |= ~rook_checking_squares_no_pawns;
    non_discoverable_queen_promoting_targets |= ~(detail.bishop_checking_squares | rook_checking_squares_no_pawns);
  };
  if constexpr (movetype & check) {
    non_discoverable_targets |= pawn_checking_squares;
    non_discoverable_knight_promoting_targets |= knight_checking_squares;
    non_discoverable_bishop_promoting_targets |= detail.bishop_checking_squares;
    non_discoverable_rook_promoting_targets |= rook_checking_squares_no_pawns;
    non_discoverable_queen_promoting_targets |= (detail.bishop_checking_squares | rook_checking_squares_no_pawns);
    discoverable_targets |= full;
  };
  non_discoverable_targets &= detail.evasion_targets;
  non_discoverable_knight_promoting_targets &= detail.evasion_targets;
  non_discoverable_bishop_promoting_targets &= detail.evasion_targets;
  non_discoverable_rook_promoting_targets &= detail.evasion_targets;
  non_discoverable_queen_promoting_targets &= detail.evasion_targets;
  discoverable_targets &= detail.evasion_targets;

  bitboard_t pushable;
  bitboard_t doublepushable;
  bitboard_t promoting;
  if constexpr (color == white) {
    pushable = (
      (free_pawns & ~(board.bitboards[none] << 8) & (detail.evasion_targets << 8) & ~rank_7) |
      (rook_pinned_pawns & ~(board.bitboards[none] << 8) & (detail.evasion_targets << 8) & (rook_ray[detail.king_square] << 8))
    );
    doublepushable = (
      (free_pawns & ~(board.bitboards[none] << 8) & ~(board.bitboards[none] << 16) & (detail.evasion_targets << 16) & rank_2) |
      (rook_pinned_pawns & ~(board.bitboards[none] << 8) & ~(board.bitboards[none] << 16) & (detail.evasion_targets << 16) & rank_2 & (rook_ray[detail.king_square] << 16))
    );
    promoting = free_pawns & ~(board.bitboards[none] << 8) & (detail.evasion_targets << 8) & rank_7;
  } else {
    pushable = (
      (free_pawns & ~(board.bitboards[none] >> 8) & (detail.evasion_targets >> 8) & ~rank_2) |
      (rook_pinned_pawns & ~(board.bitboards[none] >> 8) & (detail.evasion_targets >> 8) & (rook_ray[detail.king_square] >> 8))
    );
    doublepushable = (
      (free_pawns & ~(board.bitboards[none] >> 8) & ~(board.bitboards[none] >> 16) & (detail.evasion_targets >> 16) & rank_7) |
      (rook_pinned_pawns & ~(board.bitboards[none] >> 8) & ~(board.bitboards[none] >> 16) & (detail.evasion_targets >> 16) & rank_7 & (rook_ray[detail.king_square] >> 16))
    );
    promoting = free_pawns & ~(board.bitboards[none] >> 8) & (detail.evasion_targets >> 8) & rank_2;
  };
  bitboard_t discoverable = detail.bishop_discoverable | (detail.rook_discoverable & ranks[(detail.opponent_king_square >> 3) ^ 0b111]);

  bitboard_t discoverable_pushable_pawns = pushable & discoverable;
  bitboard_t discoverable_doublepushable_pawns = doublepushable & discoverable;
  bitboard_t discoverable_promoting_pawns = promoting & discoverable;
  bitboard_t non_discoverable_pushable_pawns = pushable & ~discoverable;
  bitboard_t non_discoverable_doublepushable_pawns = doublepushable & ~discoverable;
  bitboard_t non_discoverable_promoting_pawns = promoting & ~discoverable;

  bitboard_t pushable_pawns;
  bitboard_t doublepushable_pawns;
  bitboard_t knight_promoting_pawns;
  bitboard_t bishop_promoting_pawns;
  bitboard_t rook_promoting_pawns;
  bitboard_t queen_promoting_pawns;
  if constexpr (color == white) {
    pushable_pawns = (
      (discoverable_pushable_pawns & (discoverable_targets << 8)) |
      (non_discoverable_pushable_pawns & (non_discoverable_targets << 8))
    );
    doublepushable_pawns = (
      (discoverable_doublepushable_pawns & (discoverable_targets << 16)) |
      (non_discoverable_doublepushable_pawns & (non_discoverable_targets << 16))
    );
    knight_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_knight_promoting_targets << 8))
    );
    bishop_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_bishop_promoting_targets << 8))
    );
    rook_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_rook_promoting_targets << 8))
    );
    queen_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_queen_promoting_targets << 8))
    );
  } else {
    pushable_pawns = (
      (discoverable_pushable_pawns & (discoverable_targets >> 8)) |
      (non_discoverable_pushable_pawns & (non_discoverable_targets >> 8))
    );
    doublepushable_pawns = (
      (discoverable_doublepushable_pawns & (discoverable_targets >> 16)) |
      (non_discoverable_doublepushable_pawns & (non_discoverable_targets >> 16))
    );
    knight_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_knight_promoting_targets >> 8))
    );
    bishop_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_bishop_promoting_targets >> 8))
    );
    rook_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_rook_promoting_targets >> 8))
    );
    queen_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_targets >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_queen_promoting_targets >> 8))
    );
  };
  
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (pushable_pawns) {
      square_t from = pop_lsb(pushable_pawns);
      square_t to = from + push_offset;
      moves.push(move(from, to, color_pawn, color_pawn, none, 0, 0, 0, 0, (bitboard(from) & discoverable) || (bitboard(to) & pawn_checking_squares)));
    };
    while (doublepushable_pawns) {
      square_t from = pop_lsb(doublepushable_pawns);
      square_t to = from + doublepush_offset;
      moves.push(move(from, to, color_pawn, color_pawn, none, 1, 0, 0, 0, (bitboard(from) & discoverable) || (bitboard(to) & pawn_checking_squares)));
    };
    while (knight_promoting_pawns) {
      square_t from = pop_lsb(knight_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move(from, to, color_pawn, color_knight, none, 0, 0, 0, 1, (bitboard(from) & discoverable) || (bitboard(to) & knight_checking_squares)));
    };
    while (bishop_promoting_pawns) {
      square_t from = pop_lsb(bishop_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move(from, to, color_pawn, color_bishop, none, 0, 0, 0, 1, (bitboard(from) & discoverable) || (bitboard(to) & detail.bishop_checking_squares)));
    };
    while (rook_promoting_pawns) {
      square_t from = pop_lsb(rook_promoting_pawns);
      square_t to = from + push_offset;
      bitboard_t rook_checking_squares =  attack<rook>(detail.opponent_king_square, board.bitboards[none] & ~bitboard(from));
      bool is_check = (bitboard(from) & discoverable) || (bitboard(to) & rook_checking_squares);
      if constexpr (movetype == check) {
        if (is_check) {
          moves.push(move(from, to, color_pawn, color_rook, none, 0, 0, 0, 1, is_check));
        };
      } else {
        moves.push(move(from, to, color_pawn, color_rook, none, 0, 0, 0, 1, is_check));
      };
    };
    while (queen_promoting_pawns) {
      square_t from = pop_lsb(queen_promoting_pawns);
      square_t to = from + push_offset;
      bitboard_t queen_checking_squares = detail.bishop_checking_squares | attack<rook>(detail.opponent_king_square, board.bitboards[none] & ~bitboard(from));
      bool is_check = (bitboard(from) & discoverable) || (bitboard(to) & queen_checking_squares);
      if constexpr (movetype == check) {
        if (is_check) {
          moves.push(move(from, to, color_pawn, color_queen, none, 0, 0, 0, 1, is_check));
        };
      } else {
        moves.push(move(from, to, color_pawn, color_queen, none, 0, 0, 0, 1, is_check));
      };
    };
  } else {
    moves += popcount(pushable_pawns);
    moves += popcount(doublepushable_pawns);
    moves += popcount(knight_promoting_pawns);
    moves += popcount(bishop_promoting_pawns);
    moves += popcount(rook_promoting_pawns);
    moves += popcount(queen_promoting_pawns);
  };
};