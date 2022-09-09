// #ifdef __INTELLISENSE__
//   #pragma diag_suppress 28
// #endif
// #ifndef __EVALUATION__MODULE__
// #define __EVALUATION__MODULE__


// #include <string>
// #include "board.cpp"
// #include "constants.cpp"
// #include "types.cpp"


// namespace evaluation {
//   enum : score_t {
//     inf =       32767,
//     checkmate = 30000,
//     draw =          0,
//     max_eval =  10000,
//   };

//   std::string to_string(score_t score) {
//     if (score == inf)        return "oo";
//     if (score == -inf)       return "-oo";
//     if (score == checkmate)  return "1-0";
//     if (score == -checkmate) return "0-1";
//     if (score > max_eval)    return "#" + std::to_string((checkmate - score + 1) >> 1);
//     if (score < -max_eval)   return "#-" + std::to_string((score + checkmate + 1) >> 1);
//     if (score == draw)       return "1/2-1/2";
//     return std::to_string(score);
//   };

//   score_t add_depth(score_t score) {
//     return score = (
//       ((score > max_eval) && (score != inf)) -
//       ((score < -max_eval) && (score != -inf)) -
//       score
//     );
//   };

//   score_t remove_depth(score_t score) {
//     return score = (
//       ((score < -max_eval) && (score != -inf)) -
//       ((score > max_eval) && (score != inf)) -
//       score
//     );
//   };

//   constexpr score_t pawn_value[64] = {
//       0,   0,   0,   0,   0,   0,   0,   0,
//     150, 150, 150, 150, 150, 150, 150, 150,
//     110, 110, 120, 130, 130, 120, 110, 110,
//     105, 105, 110, 125, 125, 110, 105, 105,
//     100, 100, 100, 120, 120, 100, 100, 100,
//     105,  95,  90, 100, 100,  90,  95, 105,
//     105, 110, 110,  80,  80, 110, 110, 105,
//       0,   0,   0,   0,   0,   0,   0,   0,
//   };

//   constexpr score_t knight_value[64] = {
//     300, 310, 320, 320, 320, 320, 310, 300,
//     310, 330, 350, 350, 350, 350, 330, 310,
//     320, 350, 360, 365, 365, 360, 350, 320,
//     320, 355, 365, 370, 370, 365, 355, 320,
//     320, 350, 365, 370, 370, 365, 350, 320,
//     320, 355, 360, 365, 365, 360, 355, 320,
//     310, 330, 350, 355, 355, 350, 330, 310,
//     300, 310, 320, 320, 320, 320, 310, 300,
//   };

//   constexpr score_t bishop_value[64] = {
//     330, 340, 340, 340, 340, 340, 340, 330,
//     340, 350, 350, 350, 350, 350, 350, 340,
//     340, 350, 355, 360, 360, 355, 350, 340,
//     340, 355, 355, 360, 360, 355, 355, 340,
//     340, 350, 360, 360, 360, 360, 350, 340,
//     340, 360, 360, 360, 360, 360, 360, 340,
//     340, 355, 350, 350, 350, 350, 355, 340,
//     330, 340, 340, 340, 340, 340, 340, 330,
//   };

//   constexpr score_t rook_value[64] = {
//     525, 525, 525, 525, 525, 525, 525, 525,
//     530, 535, 535, 535, 535, 535, 535, 530,
//     520, 525, 525, 525, 525, 525, 525, 520,
//     520, 525, 525, 525, 525, 525, 525, 520,
//     520, 525, 525, 525, 525, 525, 525, 520,
//     520, 525, 525, 525, 525, 525, 525, 520,
//     520, 525, 525, 525, 525, 525, 525, 520,
//     525, 525, 525, 530, 530, 525, 525, 525,
//   };

//   constexpr score_t queen_value[64] = {
//      980,  990,  990,  995,  995,  990,  990,  980,
//      990, 1000, 1000, 1000, 1000, 1000, 1000,  990,
//      990, 1000, 1005, 1005, 1005, 1005, 1000,  990,
//      995, 1000, 1005, 1005, 1005, 1005, 1000,  995,
//     1000, 1000, 1005, 1005, 1005, 1005, 1000,  995,
//      990, 1005, 1005, 1005, 1005, 1005, 1000,  990,
//      990, 1000, 1005, 1000, 1000, 1000, 1000,  990,
//      980,  990,  990,  995,  995,  990,  990,  980,
//   };

//   constexpr score_t king_value_middlegame[64] = {
//     -30, -40, -40, -50, -50, -40, -40, -30,
//     -30, -40, -40, -50, -50, -40, -40, -30,
//     -30, -40, -40, -50, -50, -40, -40, -30,
//     -30, -40, -40, -50, -50, -40, -40, -30,
//     -20, -30, -30, -40, -40, -30, -30, -20,
//     -10, -20, -20, -20, -20, -20, -20, -10,
//      20,  20,   0,   0,   0,   0,  20,  20,
//      20,  30,  10,   0,   0,  10,  30,  20,
//   };

//   constexpr score_t king_value_endgame[64] = {
//     -50, -40, -30, -20, -20, -30, -40, -50,
//     -30, -20, -10,   0,   0, -10, -20, -30,
//     -30, -10,  20,  30,  30,  20, -10, -30,
//     -30, -10,  30,  40,  40,  30, -10, -30,
//     -30, -10,  30,  40,  40,  30, -10, -30,
//     -30, -10,  20,  30,  30,  20, -10, -30,
//     -30, -30,   0,   0,   0,   0, -30, -30,
//     -50, -30, -30, -30, -30, -30, -30, -50,
//   };

//   template <piece_t piece>
//   score_t value(square_t square, u8_t endgame_factor=0) {
//     if constexpr (piece == piece::white_pawn) {
//       return pawn_value[square];
//     } else if constexpr (piece == piece::black_pawn) {
//       return pawn_value[square ^ 56];
//     } else if constexpr (piece == piece::white_knight) {
//       return knight_value[square];
//     } else if constexpr (piece == piece::black_knight) {
//       return knight_value[square ^ 56];
//     } else if constexpr (piece == piece::white_bishop) {
//       return bishop_value[square];
//     } else if constexpr (piece == piece::black_bishop) {
//       return bishop_value[square ^ 56];
//     } else if constexpr (piece == piece::white_rook) {
//       return rook_value[square];
//     } else if constexpr (piece == piece::black_rook) {
//       return rook_value[square ^ 56];
//     } else if constexpr (piece == piece::white_queen) {
//       return queen_value[square];
//     } else if constexpr (piece == piece::black_queen) {
//       return queen_value[square ^ 56];
//     } else if constexpr (piece == piece::white_king) {
//       return ((endgame_factor * king_value_endgame[square]) >> 8) +
//              (((256 - endgame_factor) * king_value_middlegame[square]) >> 8);
//     } else if constexpr (piece == piece::black_king) {
//       return ((endgame_factor * king_value_endgame[square ^ 56]) >> 8) +
//              (((256 - endgame_factor) * king_value_middlegame[square ^ 56]) >> 8);
//     };
//     return 0;
//   };

//   constexpr score_t (*(value_array[32]))(square_t, u8_t) = {
//      value<0>,  value<1>,  value<2>,  value<3>,  value<4>,  value<5>,  value<6>,  value<7>,
//      value<8>,  value<9>, value<10>, value<11>, value<12>, value<13>, value<14>, value<15>,
//     value<16>, value<17>, value<18>, value<19>, value<20>, value<21>, value<22>, value<23>,
//     value<24>, value<25>, value<26>, value<27>, value<28>, value<29>, value<30>, value<31>,
//   };

//   template <color_t color>
//   score_t evaluate(Board& board) {
//     constexpr color_t opponent = color::compiletime::opponent(color);
//     constexpr std::array<piece_t, 6> pieces = piece::all_pieces_by_color[color];
//     constexpr std::array<piece_t, 6> opponent_pieces = piece::all_pieces_by_color[opponent];
//     score_t score = 10;
//     float endgame_factor = 0;
//     for (piece_t piece : pieces) {
//       bitboard_t piece_bitboard = board.bitboards[piece];
//       while (piece_bitboard) {
//         square_t square = pop_lsb(piece_bitboard);
//         score += value_array[piece](square, endgame_factor);
//       };
//     };
//     for (piece_t piece : opponent_pieces) {
//       bitboard_t piece_bitboard = board.bitboards[piece];
//       while (piece_bitboard) {
//         square_t square = pop_lsb(piece_bitboard);
//         score -= value_array[piece](square, endgame_factor);
//       };
//     };
//     return score;
//   };
// };


// #endif // __EVALUATION__MODULE__