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

  template<color_t color, gen_t gen>
  u64_t perft(Board& board, int depth) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    if (depth == 0) return 1;
    if (depth == 1) return movegen::generate<color, gen, u64_t>(board);
    if (depth == 2) {
      move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
      u64_t nodes = 0;
      for (move_t move : moves) {
        board.make<color>(move);
        nodes += movegen::generate<opponent, gen, u64_t>(board);
        board.unmake<color>();
      };
      return nodes;
    };
    move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
    u64_t nodes = 0;
    for (move_t move : moves) {
      board.make<color>(move);
      nodes += perft<opponent, gen>(board, depth - 1);
      board.unmake<color>();
    };
    return nodes;
  };

  template<color_t color, gen_t gen>
  perft_result_t perft(Board& board, int depth, bool print) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    u64_t start_time = timing::nanoseconds();
    u64_t nodes = 0;
    move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
    for (move_t move : moves) {
      board.make<color>(move);
      u64_t local_nodes = perft<opponent, gen>(board, depth - 1);
      nodes += local_nodes;
      board.unmake<color>();
      if (print) {
        std::cout << move::to_string(move) << ": " << local_nodes << std::endl;
      };
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

  template<gen_t gen>
  perft_result_t perft(Board& board, int depth, bool print) {
    if (board.turn == color::white) {
      return perft<color::white, gen>(board, depth, print);
    } else {
      return perft<color::black, gen>(board, depth, print);
    };
  };
};


#endif // __PERFT__MODULE__