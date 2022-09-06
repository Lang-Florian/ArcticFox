#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __PERFT__MODULE__
#define __PERFT__MODULE__


#include <iostream>

#include "board.cpp"
#include "constants.cpp"
#include "movegen.cpp"
#include "time.cpp"
#include "types.cpp"


/*

  perft

*/


namespace perft {
  template<color_t color>
  u64_t perft(Board& board, int depth) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    if (depth == 0) return 1;
    if (depth == 1) return movegen::generate<color, legal, u64_t>(board);
    move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
    u64_t count = 0;
    for (move_t move : moves) {
      board.make<color>(move);
      count += perft<opponent>(board, depth - 1);
      board.unmake<color>();
    };
    return count;
  };

  void perft(Board& board, int depth) {
    u64_t start_time = timing::nanoseconds();
    move_stack_t moves;
    if (board.turn == color::white) moves = movegen::generate<color::white, legal, move_stack_t>(board);
    else moves = movegen::generate<color::black, legal, move_stack_t>(board);
    u64_t count = 0;
    for (move_t move : moves) {
      board.make(move);
      u64_t local_count;
      if (board.turn == color::white) local_count = perft<color::white>(board, depth - 1);
      else local_count = perft<color::black>(board, depth - 1);
      board.unmake();
      std::cout << move::to_string(move) << ": " << local_count << std::endl;
      count += local_count;
    };
    u64_t end_time = timing::nanoseconds();
    float duration = (float)(end_time - start_time) * 1e-9;
    float mnps = (count / duration) * 1e-6;
    std::cout << std::endl << "Nodes searched: " << count << std::endl;
    std::cout << "Time taken: " << duration << " seconds" << std::endl;
    std::cout << "MNps per second: " << mnps << std::endl;
  };
};


#endif // __PERFT__MODULE__