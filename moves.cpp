#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"

#include "string"


#ifndef __MOVES__MODULE__
#define __MOVES__MODULE__


/* some move definitions and macros
  
  MOVE

  0000 0000 0000 0000 0000 0000 0011 1111     from
  0000 0000 0000 0000 0000 1111 1100 0000     to
  0000 0000 0000 0001 1111 0000 0000 0000     moved_piece
  0000 0000 0011 1110 0000 0000 0000 0000     target_piece
  0000 0111 1100 0000 0000 0000 0000 0000     captured_piece
  0000 1000 0000 0000 0000 0000 0000 0000     double_pawn_push
  0001 0000 0000 0000 0000 0000 0000 0000     enpassant
  0010 0000 0000 0000 0000 0000 0000 0000     castling
  0100 0000 0000 0000 0000 0000 0000 0000     promotion
  1000 0000 0000 0000 0000 0000 0000 0000     capture

*/


namespace MOVE {
  // encode a move
  inline move_t move(square_t from,
                     square_t to,
                     piece_t moved_piece,
                     piece_t target_piece=PIECE::none,
                     piece_t captured_piece=PIECE::none,
                     bool double_pawn_push=false,
                     bool enpassant=false,
                     bool castling=false) {
    return ((from) << 0) |
           ((to) << 6) |
           ((moved_piece) << 12) |
           ((target_piece) << 17) |
           ((captured_piece) << 22) |
           ((double_pawn_push) << 27) |
           ((enpassant) << 28) |
           ((castling) << 29) |
           ((target_piece != PIECE::none) << 30) |
           ((captured_piece != PIECE::none) << 31);
  };

  // decode a move
  inline square_t from(move_t move) {
    return (move >> 0) & 0b111111;
  };

  inline square_t to(move_t move) {
    return (move >> 6) & 0b111111;
  };

  inline piece_t moved_piece(move_t move) {
    return (move >> 12) & 0b11111;
  };

  inline piece_t target_piece(move_t move) {
    return (move >> 17) & 0b11111;
  };

  inline piece_t captured_piece(move_t move) {
    return (move >> 22) & 0b11111;
  };

  inline bool double_pawn_push(move_t move) {
    return (move >> 27) & 0b1;
  };

  inline bool enpassant(move_t move) {
    return (move >> 28) & 0b1;
  };

  inline bool castling(move_t move) {
    return (move >> 29) & 0b1;
  };

  inline bool promotion(move_t move) {
    return (move >> 30) & 0b1;
  };
  
  inline bool capture(move_t move) {
    return (move >> 31) & 0b1;
  };

  inline string_t to_string(move_t move) {
    return SQUARE::to_string(from(move)) + SQUARE::to_string(to(move)) + PIECE::promotion_string(target_piece(move));
  };

  inline castling_t removed_castling(move_t move) {
    return (
      (from(move) == SQUARE::E1) * CASTLING::white |
      (from(move) == SQUARE::A1 || to(move) == SQUARE::A1) * CASTLING::white_queen |
      (from(move) == SQUARE::H1 || to(move) == SQUARE::H1) * CASTLING::white_king |
      (from(move) == SQUARE::E8) * CASTLING::black |
      (from(move) == SQUARE::A8 || to(move) == SQUARE::A8) * CASTLING::black_queen |
      (from(move) == SQUARE::H8 || to(move) == SQUARE::H8) * CASTLING::black_king
    );
  };
};


#endif