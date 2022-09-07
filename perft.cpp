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
  struct perft_result_t {
    u64_t nodes;
    float time;
    float mnps;
  };

  template<color_t color>
  u64_t perft(Board& board, int depth) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    if (depth == 0) return 1;
    if (depth == 1) return movegen::generate<color, legal, u64_t>(board);
    move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
    u64_t nodes = 0;
    for (move_t move : moves) {
      board.make<color>(move);
      nodes += perft<opponent>(board, depth - 1);
      board.unmake<color>();
    };
    return nodes;
  };

  perft_result_t perft(Board& board, int depth, bool print) {
    u64_t start_time = timing::nanoseconds();
    move_stack_t moves;
    if (board.turn == color::white) moves = movegen::generate<color::white, legal, move_stack_t>(board);
    else moves = movegen::generate<color::black, legal, move_stack_t>(board);
    u64_t nodes = 0;
    for (move_t move : moves) {
      board.make(move);
      u64_t local_nodes;
      if (board.turn == color::white) local_nodes = perft<color::white>(board, depth - 1);
      else local_nodes = perft<color::black>(board, depth - 1);
      board.unmake();
      if (print) {
        std::cout << move::to_string(move) << ": " << local_nodes << std::endl;
      };
      nodes += local_nodes;
    };
    u64_t end_time = timing::nanoseconds();
    float time = (float)(end_time - start_time) * 1e-9;
    float mnps = (nodes / time) * 1e-6;
    if (print) {
      std::cout << std::endl << "Nodes searched: " << nodes << std::endl;
      std::cout << "Total time: " << time << " s" << std::endl;
      std::cout << "MNps: " << mnps << std::endl;
    };
    return perft_result_t{ nodes, time, mnps };
  };
};


#endif // __PERFT__MODULE__