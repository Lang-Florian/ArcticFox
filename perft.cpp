#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __PERFT__MODULE__
#define __PERFT__MODULE__


#include <iostream>
#include "base.cpp"
#include "modules/time.cpp"
#include "board.cpp"
#include "movegen/movegen.cpp"


/*

  Module to generate perft results.

*/


namespace perft {
struct perft_result_t {
  u64_t nodes;
  float time;
  float mnps;
};

template<color_t color, movetype_t movetype>
u64_t perft(board::Board& board, int depth) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  if (depth == 0) return 1;
  if (depth == 1) return movegen::generate<color, movetype, u64_t>(board);
  if (depth == 2) {
    move_stack_t legal_moves = movegen::generate<color, movetype::legal, move_stack_t>(board);
    u64_t nodes = 0;
    for (move_t move : legal_moves) {
      board.make<color>(move);
      nodes += movegen::generate<opponent, movetype, u64_t>(board);
      board.unmake<color>();
    };
    return nodes;
  };
  move_stack_t legal_moves = movegen::generate<color, movetype::legal, move_stack_t>(board);
  u64_t nodes = 0;
  for (move_t move : legal_moves) {
    board.make<color>(move);
    nodes += perft<opponent, movetype>(board, depth - 1);
    board.unmake<color>();
  };
  return nodes;
};

template<color_t color, movetype_t movetype>
perft_result_t perft(board::Board& board, int depth, bool print) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  u64_t start_time = nanoseconds();
  u64_t nodes = 0;
  move_stack_t legal_moves = movegen::generate<color, movetype::legal, move_stack_t>(board);
  move_stack_t moves = movegen::generate<color, movetype, move_stack_t>(board);
  for (move_t move : legal_moves) {
    u64_t local_nodes = 0;
    if (moves.contains(move) || depth != 1) {
      board.make<color>(move);
      local_nodes = perft<opponent, movetype>(board, depth - 1);
      board.unmake<color>();
    };
    nodes += local_nodes;
    if (print) {
      std::cout << move::to_string(move) << ": " << local_nodes << "\n";
    };
  };
  u64_t end_time = nanoseconds();
  float time = (float)(end_time - start_time) * 1e-9;
  float mnps = (nodes / time) * 1e-6;
  if (print) {
    std::cout << "\nNodes searched: " << nodes << "\n";
    std::cout << "Total time: " << time << " s\n";
    std::cout << "MNps: " << mnps << "\n";
  };
  return perft_result_t{ nodes, time, mnps };
};

template<movetype_t movetype>
perft_result_t perft(board::Board& board, int depth, bool print) {
  if (board.turn == color::white) {
    return perft<color::white, movetype>(board, depth, print);
  } else {
    return perft<color::black, movetype>(board, depth, print);
  };
};
};


#endif