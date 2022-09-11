#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __DETAIL__MODULE__
#define __DETAIL__MODULE__


#include "../base/base.cpp"
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
  void update(board::Board& board) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
    constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
    constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
    constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
    constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, opponent);
    constexpr piece_t opponent_queen = piece::compiletime::to_color(piece::queen, opponent);
    constexpr piece_t opponent_rook = piece::compiletime::to_color(piece::rook, opponent);
    constexpr piece_t opponent_bishop = piece::compiletime::to_color(piece::bishop, opponent);
    constexpr piece_t opponent_knight = piece::compiletime::to_color(piece::knight, opponent);
    constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);

    this->king_square = get_lsb(board.bitboards[king]);
    this->opponent_king_square = get_lsb(board.bitboards[opponent_king]);

    bitboard_t king_bishop_attack = attack::attack<piece::bishop>(this->king_square, board.bitboards[piece::none]);
    bitboard_t king_rook_attack = attack::attack<piece::rook>(this->king_square, board.bitboards[piece::none]);

    this->checkers = attack::attackers<opponent>(board, this->king_square);
    
    this->bishop_checking_squares = attack::attack<piece::bishop>(this->opponent_king_square, board.bitboards[color::none]);
    this->rook_checking_squares = attack::attack<piece::rook>(this->opponent_king_square, board.bitboards[color::none]);
    this->rook_checking_squares_castling = attack::attack<piece::rook>(this->opponent_king_square, board.bitboards[color::none] & ~board.bitboards[king]);

    this->attacked_squares = attack::pawns<opponent>(board.bitboards[opponent_pawn]);
    this->unsafe_king_squares = attack::pawns<opponent>(board.bitboards[opponent_pawn]);
    this->attacked_squares |= attack::knights(board.bitboards[opponent_knight]);
    this->unsafe_king_squares |= attack::knights(board.bitboards[opponent_knight]);
    bitboard_t opponent_bishop_movers = board.bitboards[opponent_bishop] | board.bitboards[opponent_queen];
    while (opponent_bishop_movers) {
      square_t square = pop_lsb(opponent_bishop_movers);
      this->attacked_squares |= attack::attack<piece::bishop>(square, board.bitboards[piece::none]);
      this->unsafe_king_squares |= attack::attack<piece::bishop>(square, board.bitboards[piece::none] & ~board.bitboards[king]);
    };
    bitboard_t opponent_rook_movers = board.bitboards[opponent_rook] | board.bitboards[opponent_queen];
    while (opponent_rook_movers) {
      square_t square = pop_lsb(opponent_rook_movers);
      this->attacked_squares |= attack::attack<piece::rook>(square, board.bitboards[piece::none]);
      this->unsafe_king_squares |= attack::attack<piece::rook>(square, board.bitboards[piece::none] & ~board.bitboards[king]);
    };
    this->attacked_squares |= attack::attack<piece::king>(this->opponent_king_square);
    this->unsafe_king_squares |= attack::attack<piece::king>(this->opponent_king_square);

    this->bishop_discoverable = bitboard::none;
    bitboard_t bishop_attackers = (board.bitboards[bishop] | board.bitboards[queen]) & attack::ray::bishop[this->opponent_king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_discoverable |= attack::attack<bishop>(square, board.bitboards[color::none]) & this->bishop_checking_squares;
    };
    this->rook_discoverable = bitboard::none;
    bitboard_t rook_attackers = (board.bitboards[rook] | board.bitboards[queen]) & attack::ray::rook[this->opponent_king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_discoverable |= attack::attack<rook>(square, board.bitboards[color::none]) & this->rook_checking_squares;
    };

    this->evasion_targets = bitboard::full;
    if (popcount(this->checkers) > 1) {
      return;
    } else if (this->checkers) {
      square_t checker_square = get_lsb(this->checkers);
      switch (board.pieces[checker_square]) {
        case opponent_queen:
          if (king_bishop_attack & board.bitboards[opponent_queen]) {
            this->evasion_targets = bitboard(checker_square) | (king_bishop_attack & attack::attack<piece::bishop>(checker_square, board.bitboards[color::none]));
          } else if (king_rook_attack & board.bitboards[opponent_queen]) {
            this->evasion_targets = bitboard(checker_square) | (king_rook_attack & attack::attack<piece::rook>(checker_square, board.bitboards[color::none]));
          };
          break;
        case opponent_rook:
          this->evasion_targets = bitboard(checker_square) | (king_rook_attack & attack::attack<piece::rook>(checker_square, board.bitboards[color::none]));
          break;
        case opponent_bishop:
          this->evasion_targets = bitboard(checker_square) | (king_bishop_attack & attack::attack<piece::bishop>(checker_square, board.bitboards[color::none]));
          break;
        default:
          this->evasion_targets = bitboard(checker_square);
          break;
      };
    };

    this->bishop_pinned = bitboard::none;
    bishop_attackers = (board.bitboards[opponent_bishop] | board.bitboards[opponent_queen]) & attack::ray::bishop[this->king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_pinned |= attack::attack<piece::bishop>(square, board.bitboards[color::none]) & king_bishop_attack;
    };
    bitboard_t opponent_pawn_to_capture = bitboard::none;
    bitboard_t capturing_pawns = bitboard::none;
    if constexpr (color == color::white) {
      opponent_pawn_to_capture = (bitboard(board.enpassant) << 8) & bitboard::rank_5;
      this->enpassant_pinned = attack::pawns<opponent>(opponent_pawn_to_capture & this->bishop_pinned) >> 8;
    } else {
      opponent_pawn_to_capture = (bitboard(board.enpassant) >> 8) & bitboard::rank_4;
      this->enpassant_pinned = attack::pawns<opponent>(opponent_pawn_to_capture & this->bishop_pinned) << 8;
    };
    this->rook_pinned = bitboard::none;
    rook_attackers = (board.bitboards[opponent_rook] | board.bitboards[opponent_queen]) & attack::ray::rook[this->king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_pinned |= attack::attack<piece::rook>(square, board.bitboards[color::none]) & king_rook_attack;
      this->enpassant_pinned |= (
        attack::attack<piece::rook>(square, board.bitboards[color::none] & ~opponent_pawn_to_capture) &
        attack::attack<piece::rook>(this->king_square, board.bitboards[color::none] & ~opponent_pawn_to_capture)
      );
    };
  };
};


#endif