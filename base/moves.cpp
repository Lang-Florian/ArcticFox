#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MOVES__MODULE__
#define __MOVES__MODULE__


#include <string>
#include "piece.cpp"
#include "square.cpp"
#include "types.cpp"


/* some move definitions and macros

  MOVE

  0000 0000 0000 0000 0000 0000 0011 1111     from
  0000 0000 0000 0000 0000 1111 1100 0000     to
  0000 0000 0000 0001 1111 0000 0000 0000     target_piece
  0000 0000 0000 0010 0000 0000 0000 0000     double_pawn_push
  0000 0000 0000 0100 0000 0000 0000 0000     enpassant
  0000 0000 1111 1000 0000 0000 0000 0000     moved_piece
  0001 1111 0000 0000 0000 0000 0000 0000     captured_piece
  0010 0000 0000 0000 0000 0000 0000 0000     promotion
  0100 0000 0000 0100 0000 0000 0000 0000     castling
  1000 0000 0000 0000 0000 0000 0000 0000     check

*/


namespace move {
move_t null = (move_t)0;

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
         (enpassant << 18) |
         (moved_piece << 19) |
         (captured_piece << 24) |
         (promotion << 29) |
         (castling << 30) |
         (check << 31);
};

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

bool enpassant(move_t move) {
  return (move >> 18) & 0b1;
};

piece_t moved_piece(move_t move) {
  return (move >> 19) & 0b11111;
};

piece_t captured_piece(move_t move) {
  return (move >> 24) & 0b11111;
};

bool promotion(move_t move) {
  return (move >> 29) & 0b1;
};

bool castling(move_t move) {
  return (move >> 30) & 0b1;
};

bool check(move_t move) {
  return (move >> 31) & 0b1;
};

bool capture(move_t move) {
  return captured_piece(move) != piece::none;
};

std::string to_string(move_t move) {
  if (move == null) return "0000";
  return (
    square::to_string(from(move)) +
    square::to_string(to(move)) +
    (promotion(move) ? piece::promotion_string(target_piece(move)) : "")
  );
};

castling_t removed_castling(move_t move) {
  return (
    (from(move) == square::e1 || from(move) == square::h1 || to(move) == square::h1) << 0 |
    (from(move) == square::e1 || from(move) == square::a1 || to(move) == square::a1) << 1 |
    (from(move) == square::e8 || from(move) == square::h8 || to(move) == square::h8) << 2 |
    (from(move) == square::e8 || from(move) == square::a8 || to(move) == square::a8) << 3
  );
};

move_t mvv_lva_key(move_t move) {
  constexpr move_t move_piece_mask = 0b11111 << 19;
  if (capture(move)) {
    return move ^ move_piece_mask;
  };
  return move;
};

struct {
  bool operator()(move_t move1, move_t move2) const {
    return move::mvv_lva_key(move1) > move::mvv_lva_key(move2);
  };
} comparison;

struct {
  bool operator()(move_t move1, move_t move2) const {
    return move::mvv_lva_key(move1) < move::mvv_lva_key(move2);
  };
} reverse_comparison;
};


namespace move::compiletime {
template<piece_t piece>
constexpr move_t piece_move(square_t from, square_t to, piece_t captured_piece, bool check) {
  constexpr move_t base = (piece << 12) | (piece << 19);
  return base | (from << 0) | (to << 6) | (captured_piece << 24) | (check << 31);
};

template<piece_t piece, bool check>
constexpr move_t piece_move(square_t from, square_t to, piece_t captured_piece) {
  constexpr move_t base = (piece << 12) | (piece << 19) | (check << 31);
  return base | (from << 0) | (to << 6) | (captured_piece << 24);
};

template<color_t color, bool king_side>
constexpr move_t castling_move(bool check) {
  constexpr move_t white_king_side_base = (square::e1 << 0) | (square::g1 << 6) | (piece::white_king << 12) | (piece::white_king << 19) | (1 << 30);
  constexpr move_t white_queen_side_base = (square::e1 << 0) | (square::c1 << 6) | (piece::white_king << 12) | (piece::white_king << 19) | (1 << 30);
  constexpr move_t black_king_side_base = (square::e8 << 0) | (square::g8 << 6) | (piece::black_king << 12) | (piece::black_king << 19) | (1 << 30);
  constexpr move_t black_queen_side_base = (square::e8 << 0) | (square::c8 << 6) | (piece::black_king << 12) | (piece::black_king << 19) | (1 << 30);
  if constexpr (color == color::white) {
    if constexpr (king_side) {
      return white_king_side_base | (check << 31);
    } else {
      return white_queen_side_base | (check << 31);
    };
  } else {
    if constexpr (king_side) {
      return black_king_side_base | (check << 31);
    } else {
      return black_queen_side_base | (check << 31);
    };
  };
};

template<piece_t piece>
constexpr move_t capture_promotion_move(square_t from, square_t to, piece_t captured_piece, bool check) {
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (1 << 29);
  return base | (from << 0) | (to << 6) | (captured_piece << 24) | (check << 31);
};

template<piece_t piece, bool check>
constexpr move_t capture_promotion_move(square_t from, square_t to, piece_t captured_piece) {
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (1 << 29) | (check << 31);
  return base | (from << 0) | (to << 6) | (captured_piece << 24);
};

template<piece_t piece>
constexpr move_t push_promotion_move(square_t from, square_t to, bool check) {
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (piece::none << 24) | (1 << 29);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color, bool double_pawn_push>
constexpr move_t push_move(square_t from, square_t to, bool check) {
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (pawn << 12) | (pawn << 19) | (piece::none << 24) | (double_pawn_push << 17);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color>
constexpr move_t enpassant_move(square_t from, square_t to, bool check) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr move_t base = (pawn << 12) | (pawn << 19) | (opponent_pawn << 24) | (1 << 18);
  return base | (from << 0) | (to << 6) | (check << 31);
};
};


#endif