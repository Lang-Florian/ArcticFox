#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __DEBUG__MODULE__
#define __DEBUG__MODULE__


#include <iostream>

#include "constants.cpp"
#include "board.cpp"


/*
  
  debug

*/


void print_board(Board& board) {
  std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      std::cout << " | " << piece::to_string(board.pieces[8 * (7 - rank) + file]);
    };
    std::cout << " | " << rank + 1 << std::endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
  };
  std::cout << "   a   b   c   d   e   f   g   h" << std::endl << std::endl;
  std::cout << "Fen: " << board.fen() << std::endl;
};


#endif // __DEBUG__MODULE__