#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"
#include "board.cpp"


#ifndef __CHESSFOX__MODULE__
#define __CHESSFOX__MODULE__


class ChessFox {
  public:
    Board board;

  ChessFox(string_t fen) {
    board = Board(fen);
  };
};

#endif