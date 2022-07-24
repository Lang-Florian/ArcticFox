#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __EVALUATION__MODULE__
#define __EVALUATION__MODULE__


#include <string>

#include "constants.cpp"
#include "types.cpp"


namespace eval {
  enum : eval_t {
    mate = 32767,
    mate_opponent = -32767,
    draw = 0,
  };

  constexpr eval_t pawn_value[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    150, 150, 150, 150, 150, 150, 150, 150,
    110, 110, 120, 130, 130, 120, 110, 110,
    105, 105, 110, 125, 125, 110, 105, 105,
    100, 100, 100, 120, 120, 100, 100, 100,
    105,  95,  90, 100, 100,  90,  95, 105,
    105, 110, 110,  80,  80, 110, 110, 105,
      0,   0,   0,   0,   0,   0,   0,   0,
  };

  constexpr eval_t knight_value[64] = {
    300, 310, 320, 320, 320, 320, 310, 300,
    310, 330, 350, 350, 350, 350, 330, 310,
    320, 350, 360, 365, 365, 360, 350, 320,
    320, 355, 365, 370, 370, 365, 355, 320,
    320, 350, 365, 370, 370, 365, 350, 320,
    320, 355, 360, 365, 365, 360, 355, 320,
    310, 330, 350, 355, 355, 350, 330, 310,
    300, 310, 320, 320, 320, 320, 310, 300,
  };

  constexpr eval_t bishop_value[64] = {
    330, 340, 340, 340, 340, 340, 340, 330,
    340, 350, 350, 350, 350, 350, 350, 340,
    340, 350, 355, 360, 360, 355, 350, 340,
    340, 355, 355, 360, 360, 355, 355, 340,
    340, 350, 360, 360, 360, 360, 350, 340,
    340, 360, 360, 360, 360, 360, 360, 340,
    340, 355, 350, 350, 350, 350, 355, 340,
    330, 340, 340, 340, 340, 340, 340, 330,
  };

  constexpr eval_t rook_value[64] = {
    525, 525, 525, 525, 525, 525, 525, 525,
    530, 535, 535, 535, 535, 535, 535, 530,
    520, 525, 525, 525, 525, 525, 525, 520,
    520, 525, 525, 525, 525, 525, 525, 520,
    520, 525, 525, 525, 525, 525, 525, 520,
    520, 525, 525, 525, 525, 525, 525, 520,
    520, 525, 525, 525, 525, 525, 525, 520,
    525, 525, 525, 530, 530, 525, 525, 525,
  };

  constexpr eval_t queen_value[64] = {
     980,  990,  990,  995,  995,  990,  990,  980,
     990, 1000, 1000, 1000, 1000, 1000, 1000,  990,
     990, 1000, 1005, 1005, 1005, 1005, 1000,  990,
     995, 1000, 1005, 1005, 1005, 1005, 1000,  995,
    1000, 1000, 1005, 1005, 1005, 1005, 1000,  995,
     990, 1005, 1005, 1005, 1005, 1005, 1000,  990,
     990, 1000, 1005, 1000, 1000, 1000, 1000,  990,
     980,  990,  990,  995,  995,  990,  990,  980,
  };

  constexpr eval_t king_value_middlegame[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,
     20,  30,  10,   0,   0,  10,  30,  20,
  };

  constexpr eval_t king_value_endgame[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50,
  };

  template <piece_t piece>
  eval_t value(square_t square, float endgame_factor=0) {
    if constexpr (piece == piece::white_pawn) {
      return pawn_value[square];
    } else if constexpr (piece == piece::black_pawn) {
      return pawn_value[square ^ 56];
    } else if constexpr (piece == piece::white_knight) {
      return knight_value[square];
    } else if constexpr (piece == piece::black_knight) {
      return knight_value[square ^ 56];
    } else if constexpr (piece == piece::white_bishop) {
      return bishop_value[square];
    } else if constexpr (piece == piece::black_bishop) {
      return bishop_value[square ^ 56];
    } else if constexpr (piece == piece::white_rook) {
      return rook_value[square];
    } else if constexpr (piece == piece::black_rook) {
      return rook_value[square ^ 56];
    } else if constexpr (piece == piece::white_queen) {
      return queen_value[square];
    } else if constexpr (piece == piece::black_queen) {
      return queen_value[square ^ 56];
    } else if constexpr (piece == piece::white_king) {
      return endgame_factor * eval::king_value_endgame[square] +
             (1 - endgame_factor) * eval::king_value_middlegame[square];
    } else if constexpr (piece == piece::black_king) {
      return endgame_factor * eval::king_value_endgame[square ^ 56] +
             (1 - endgame_factor) * eval::king_value_middlegame[square ^ 56];
    };
    return 0;
  };

  constexpr eval_t (*(value_array[32]))(square_t, float) = {
     value<0>,  value<1>,  value<2>,  value<3>,  value<4>,  value<5>,  value<6>,  value<7>,
     value<8>,  value<9>, value<10>, value<11>, value<12>, value<13>, value<14>, value<15>,
    value<16>, value<17>, value<18>, value<19>, value<20>, value<21>, value<22>, value<23>,
    value<24>, value<25>, value<26>, value<27>, value<28>, value<29>, value<30>, value<31>,
  };
};




#endif // __EVALUATION__MODULE__