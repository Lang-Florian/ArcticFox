#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __OUTCOME__MODULE__
#define __OUTCOME__MODULE__


#include <string>
#include "types.cpp"


namespace outcome {
enum : outcome_t {
  none =                  0b00000000,
  checkmate =             0b00000100,
  checkmate_white =       0b00000100,
  checkmate_black =       0b00000101,
  draw =                  0b00001000,
  stalemate =             0b00011000,
  insufficient_material = 0b00101000,
  fifty_move_rule =       0b01001000,
  threefold_repetition =  0b10001000,
};

outcome_t checkmate_for(color_t color) {
  return 0b00000100 | color;
};

std::string to_string(outcome_t outcome) {
  if (outcome == checkmate_white)       return "1-0";
  if (outcome == checkmate_black)       return "0-1";
  if (outcome == draw)                  return "1/2-1/2";
  if (outcome == stalemate)             return "1/2-1/2 by Stalemate";
  if (outcome == insufficient_material) return "1/2-1/2 by Insufficient material";
  if (outcome == fifty_move_rule)       return "1/2-1/2 by Fifty move rule";
  if (outcome == threefold_repetition)  return "1/2-1/2 by Threefold repetition";
  return "-";
};
};


#endif