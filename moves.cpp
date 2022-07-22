#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __MOVES__MODULE__
#define __MOVES__MODULE__


#include <string>

#include "constants.cpp"
#include "types.cpp"


/* some move definitions and macros

  MOVE

  0000 0000 0000 0000 0000 0000 0011 1111     from
  0000 0000 0000 0000 0000 1111 1100 0000     to
  0000 0000 0000 0001 1111 0000 0000 0000     target_piece
  0000 0000 0000 0010 0000 0000 0000 0000     double_pawn_push
  0000 0000 0000 0100 0000 0000 0000 0000     castling
  0000 0000 0000 1000 0000 0000 0000 0000     enpassant
  0000 0001 1111 0000 0000 0000 0000 0000     moved_piece
  0011 1110 0000 0000 0000 0000 0000 0000     captured_piece
  0100 0000 0000 0000 0000 0000 0000 0000     promotion
  1000 0000 0000 0000 0000 0000 0000 0000     check

*/


namespace move {
  // encode a move
  move_t move(square_t from,
              square_t to,
              piece_t moved_piece,
              piece_t target_piece,
              piece_t captured_piece,
              bool double_pawn_push,
              bool enpassant,
              bool castling,
              bool promotion,
              bool check) {
    return (from << 0) |
           (to << 6) |
           (target_piece << 12) |
           (double_pawn_push << 17) |
           (castling << 18) |
           (enpassant << 19) |
           (moved_piece << 20) |
           (captured_piece << 25) |
           (promotion << 30) |
           (check << 31);
  };

  // decode a move
  square_t from(move_t move) {
    return (move >> 0) & 0b111111;
  };

  square_t to(move_t move) {
    return (move >> 6) & 0b111111;
  };

  piece_t target_piece(move_t move) {
    return (move >> 12) & 0b11111;
  };

  bool double_pawn_push(move_t move) {
    return (move >> 17) & 0b1;
  };

  bool castling(move_t move) {
    return (move >> 18) & 0b1;
  };

  bool enpassant(move_t move) {
    return (move >> 19) & 0b1;
  };

  piece_t moved_piece(move_t move) {
    return (move >> 20) & 0b11111;
  };

  piece_t captured_piece(move_t move) {
    return (move >> 25) & 0b11111;
  };

  bool promotion(move_t move) {
    return (move >> 30) & 0b1;
  };
  
  bool check(move_t move) {
    return (move >> 31) & 0b1;
  };

  bool capture(move_t move) {
    return captured_piece(move) != piece::none;
  };

  // get a uci move string
  std::string uci(move_t move) {
    return (
      square::to_string(from(move)) +
      square::to_string(to(move)) +
      (promotion(move) ? piece::promotion_string(target_piece(move)) : "")
    );
  };

  // check which castling rights are removed by a move
  castling_t removed_castling(move_t move) {
    return (
      (from(move) == square::e1 || from(move) == square::h1 || to(move) == square::h1) << 0 |
      (from(move) == square::e1 || from(move) == square::a1 || to(move) == square::a1) << 1 |
      (from(move) == square::e8 || from(move) == square::h8 || to(move) == square::h8) << 2 |
      (from(move) == square::e8 || from(move) == square::a8 || to(move) == square::a8) << 3
    );
  };

  move_t none = (move_t)0;
};


#endif // __MOVES__MODULE__