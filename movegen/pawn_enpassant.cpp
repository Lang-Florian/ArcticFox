#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __PAWN_ENPASSANT__MODULE__
#define __PAWN_ENPASSANT__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate pawn enpassant moves
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_enpassant_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, opponent);

  bitboard_t enpassant_pawn;
  if constexpr (color == color::white) {
    enpassant_pawn = (bitboard(board.enpassant) << 8) & bitboard::rank_5;
  } else {
    enpassant_pawn = (bitboard(board.enpassant) >> 8) & bitboard::rank_4;
  };

  bool is_always_check = (detail.bishop_discoverable & enpassant_pawn) || (board.bitboards[opponent_king] & attack::attack<pawn>(board.enpassant));

  bitboard_t bishop_pinned_pawns = (
    board.bitboards[pawn] &
    attack::attack<opponent_pawn>(board.enpassant) &
    ~detail.enpassant_pinned &
    detail.bishop_pinned &
    ~detail.rook_pinned
  );
  while (bishop_pinned_pawns) {
    square_t from = pop_lsb(bishop_pinned_pawns);
    bool is_check = (
      is_always_check ||
      (detail.rook_discoverable & bitboard(from)) ||
      ((detail.bishop_discoverable & bitboard(from)) && (~attack::ray::bishop[detail.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack::attack<rook>(detail.opponent_king_square, (board.bitboards[piece::none] & ~bitboard(from) & ~enpassant_pawn) | bitboard(board.enpassant)) & (board.bitboards[rook] | board.bitboards[queen]))
    );
    bool enpassant = (
      (attack::ray::bishop[detail.king_square] & bitboard(board.enpassant)) &&
      (
        (detail.evasion_targets & bitboard(board.enpassant)) ||
        (enpassant_pawn == detail.checkers)
      ) && (
        (movetype & movetype::capture) ||
        ((movetype & movetype::check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move::compiletime::enpassant_move<color>(from, board.enpassant, is_check));
    } else {
      moves += enpassant;
    };
  };

  bitboard_t free_pawns = (
    board.bitboards[pawn] &
    attack::attack<opponent_pawn>(board.enpassant) &
    ~detail.enpassant_pinned &
    ~detail.bishop_pinned &
    ~detail.rook_pinned
  );
  while (free_pawns) {
    square_t from = pop_lsb(free_pawns);
    bool is_check = (
      is_always_check ||
      (detail.rook_discoverable & bitboard(from)) ||
      ((detail.bishop_discoverable & bitboard(from)) && (~attack::ray::bishop[detail.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack::attack<rook>(detail.opponent_king_square, (board.bitboards[piece::none] & ~bitboard(from) & ~enpassant_pawn) | bitboard(board.enpassant)) & (board.bitboards[rook] | board.bitboards[queen]))
    );
    bool enpassant = (
      (
        (detail.evasion_targets & bitboard(board.enpassant)) ||
        (enpassant_pawn == detail.checkers)
      ) && (
        (movetype & movetype::capture) ||
        ((movetype & movetype::check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move::compiletime::enpassant_move<color>(from, board.enpassant, is_check));
    } else {
      moves += enpassant;
    };
  };
};


#endif