#pragma once


#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"

/*

  Module for more detailed information about the board.

*/


struct detail_t {
  square_t king_square;
  square_t opponent_king_square;

  bitboard_t checkers;

  bitboard_t bishop_checking_squares;
  bitboard_t rook_checking_squares;
  bitboard_t rook_checking_squares_castling;

  bitboard_t attacked_squares;
  bitboard_t unsafe_king_squares;

  bitboard_t bishop_discoverable;
  bitboard_t rook_discoverable;

  bitboard_t evasion_targets;

  bitboard_t bishop_pinned;
  bitboard_t rook_pinned;
  bitboard_t enpassant_pinned;

  template<color_t color>
  void update(Board& board) {
    constexpr color_t opponent = opponent(color);
    constexpr piece_t turn_king = to_color(king, color);
    constexpr piece_t turn_queen = to_color(queen, color);
    constexpr piece_t turn_rook = to_color(rook, color);
    constexpr piece_t turn_bishop = to_color(bishop, color);
    constexpr piece_t opponent_king = to_color(king, opponent);
    constexpr piece_t opponent_queen = to_color(queen, opponent);
    constexpr piece_t opponent_rook = to_color(rook, opponent);
    constexpr piece_t opponent_bishop = to_color(bishop, opponent);
    constexpr piece_t opponent_knight = to_color(knight, opponent);
    constexpr piece_t opponent_pawn = to_color(pawn, opponent);

    this->king_square = get_lsb(board.bitboards[turn_king]);
    this->opponent_king_square = get_lsb(board.bitboards[opponent_king]);

    bitboard_t king_bishop_attack = attack<bishop>(this->king_square, board.bitboards[none]);
    bitboard_t king_rook_attack = attack<rook>(this->king_square, board.bitboards[none]);

    this->checkers = attackers<opponent>(board, this->king_square);
    
    this->bishop_checking_squares = attack<bishop>(this->opponent_king_square, board.bitboards[none]);
    this->rook_checking_squares = attack<rook>(this->opponent_king_square, board.bitboards[none]);
    this->rook_checking_squares_castling = attack<rook>(this->opponent_king_square, board.bitboards[none] & ~board.bitboards[king]);

    this->attacked_squares = multi_pawn_attack<opponent>(board.bitboards[opponent_pawn]);
    this->unsafe_king_squares = multi_pawn_attack<opponent>(board.bitboards[opponent_pawn]);
    this->attacked_squares |= multi_knight_attack(board.bitboards[opponent_knight]);
    this->unsafe_king_squares |= multi_knight_attack(board.bitboards[opponent_knight]);
    bitboard_t opponent_bishop_movers = board.bitboards[opponent_bishop] | board.bitboards[opponent_queen];
    while (opponent_bishop_movers) {
      square_t square = pop_lsb(opponent_bishop_movers);
      this->attacked_squares |= attack<bishop>(square, board.bitboards[none]);
      this->unsafe_king_squares |= attack<bishop>(square, board.bitboards[none] & ~board.bitboards[king]);
    };
    bitboard_t opponent_rook_movers = board.bitboards[opponent_rook] | board.bitboards[opponent_queen];
    while (opponent_rook_movers) {
      square_t square = pop_lsb(opponent_rook_movers);
      this->attacked_squares |= attack<rook>(square, board.bitboards[none]);
      this->unsafe_king_squares |= attack<rook>(square, board.bitboards[none] & ~board.bitboards[king]);
    };
    this->attacked_squares |= attack<king>(this->opponent_king_square);
    this->unsafe_king_squares |= attack<king>(this->opponent_king_square);

    this->bishop_discoverable = none;
    bitboard_t bishop_attackers = (board.bitboards[turn_bishop] | board.bitboards[turn_queen]) & bishop_ray[this->opponent_king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_discoverable |= attack<bishop>(square, board.bitboards[none]) & this->bishop_checking_squares;
    };
    this->rook_discoverable = none;
    bitboard_t rook_attackers = (board.bitboards[turn_rook] | board.bitboards[turn_queen]) & rook_ray[this->opponent_king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_discoverable |= attack<rook>(square, board.bitboards[none]) & this->rook_checking_squares;
    };

    this->evasion_targets = full;
    if (popcount(this->checkers) > 1) {
      return;
    } else if (this->checkers) {
      square_t checker_square = get_lsb(this->checkers);
      switch (board.pieces[checker_square]) {
        case opponent_queen:
          if (king_bishop_attack & board.bitboards[opponent_queen]) {
            this->evasion_targets = bitboard(checker_square) | (king_bishop_attack & attack<bishop>(checker_square, board.bitboards[none]));
          } else if (king_rook_attack & board.bitboards[opponent_queen]) {
            this->evasion_targets = bitboard(checker_square) | (king_rook_attack & attack<rook>(checker_square, board.bitboards[none]));
          };
          break;
        case opponent_rook:
          this->evasion_targets = bitboard(checker_square) | (king_rook_attack & attack<rook>(checker_square, board.bitboards[none]));
          break;
        case opponent_bishop:
          this->evasion_targets = bitboard(checker_square) | (king_bishop_attack & attack<bishop>(checker_square, board.bitboards[none]));
          break;
        default:
          this->evasion_targets = bitboard(checker_square);
          break;
      };
    };

    this->bishop_pinned = none;
    bishop_attackers = (board.bitboards[opponent_bishop] | board.bitboards[opponent_queen]) & bishop_ray[this->king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_pinned |= attack<bishop>(square, board.bitboards[none]) & king_bishop_attack;
    };
    bitboard_t opponent_pawn_to_capture = none;
    bitboard_t capturing_pawns = none;
    if constexpr (color == white) {
      opponent_pawn_to_capture = (bitboard(board.enpassant) << 8) & rank_5;
      this->enpassant_pinned = multi_pawn_attack<opponent>(opponent_pawn_to_capture & this->bishop_pinned) >> 8;
    } else {
      opponent_pawn_to_capture = (bitboard(board.enpassant) >> 8) & rank_4;
      this->enpassant_pinned = multi_pawn_attack<opponent>(opponent_pawn_to_capture & this->bishop_pinned) << 8;
    };
    this->rook_pinned = none;
    rook_attackers = (board.bitboards[opponent_rook] | board.bitboards[opponent_queen]) & rook_ray[this->king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_pinned |= attack<rook>(square, board.bitboards[none]) & king_rook_attack;
      this->enpassant_pinned |= (
        attack<rook>(square, board.bitboards[none] & ~opponent_pawn_to_capture) &
        attack<rook>(this->king_square, board.bitboards[none] & ~opponent_pawn_to_capture)
      );
    };
  };
};