#pragma once

#include <type_traits>
#include "../attack.cpp"
#include "../base.cpp"
#include "../board.cpp"
#include "detail.cpp"

template <color_t color, movetype_t movetype, typename T>
void generate_pawn_enpassant_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t color_rook = to_color(rook, color);
  constexpr piece_t color_queen = to_color(queen, color);
  constexpr piece_t opponent_pawn = to_color(pawn, opponent);
  constexpr piece_t opponent_king = to_color(king, opponent);
  // get the pawn that can be captured en passant
  bitboard_t enpassant_pawn;
  if constexpr (color == white) {
    enpassant_pawn = (bitboard(board.enpassant) << 8) & rank_5;
  } else {
    enpassant_pawn = (bitboard(board.enpassant) >> 8) & rank_4;
  };
  // check if en passant will always be a check
  bool is_always_check = (detail.bishop_discoverable & enpassant_pawn) || (board.bitboards[opponent_king] & attack<color_pawn>(board.enpassant));
  // generate en passant moves
  bitboard_t bishop_pinned_pawns = (
    board.bitboards[color_pawn] &
    attack<opponent_pawn>(board.enpassant) &
    ~detail.enpassant_pinned &
    detail.bishop_pinned &
    ~detail.rook_pinned
  );
  while (bishop_pinned_pawns) {
    square_t from = pop_lsb(bishop_pinned_pawns);
    bool is_check = (
      is_always_check ||
      (detail.rook_discoverable & bitboard(from)) ||
      ((detail.bishop_discoverable & bitboard(from)) && (~bishop_ray[detail.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack<rook>(detail.opponent_king_square, (board.bitboards[none] & ~bitboard(from) & ~enpassant_pawn) | bitboard(board.enpassant)) & (board.bitboards[color_rook] | board.bitboards[color_queen]))
    );
    bool enpassant = (
      (bishop_ray[detail.color_king_square] & bitboard(board.enpassant)) &&
      (
        (detail.evasion_targets & bitboard(board.enpassant)) ||
        (enpassant_pawn == detail.checkers)
      ) && (
        (movetype & capture) ||
        ((movetype & check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move(from, board.enpassant, color_pawn, color_pawn, opponent_pawn, 0, 1, 0, 0, is_check));
    } else {
      moves += enpassant;
    };
  };
  bitboard_t free_pawns = (
    board.bitboards[color_pawn] &
    attack<opponent_pawn>(board.enpassant) &
    ~detail.enpassant_pinned &
    ~detail.bishop_pinned &
    ~detail.rook_pinned
  );
  while (free_pawns) {
    square_t from = pop_lsb(free_pawns);
    bool is_check = (
      is_always_check ||
      (detail.rook_discoverable & bitboard(from)) ||
      ((detail.bishop_discoverable & bitboard(from)) && (~bishop_ray[detail.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack<rook>(detail.opponent_king_square, (board.bitboards[none] & ~bitboard(from) & ~enpassant_pawn) | bitboard(board.enpassant)) & (board.bitboards[color_rook] | board.bitboards[color_queen]))
    );
    bool enpassant = (
      (
        (detail.evasion_targets & bitboard(board.enpassant)) ||
        (enpassant_pawn == detail.checkers)
      ) && (
        (movetype & capture) ||
        ((movetype & check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move(from, board.enpassant, color_pawn, color_pawn, opponent_pawn, 0, 1, 0, 0, is_check));
    } else {
      moves += enpassant;
    };
  };
};