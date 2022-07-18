#include <iostream>

#include "attack_rays.cpp"
#include "attacks.cpp"
#include "board.cpp"
#include "constants.cpp"
#include "display.cpp"
#include "macros.cpp"
#include "magic.cpp"
#include "moves.cpp"
#include "random.cpp"
#include "stack.cpp"
#include "time.cpp"
#include "types.cpp"
#include "zobrist.cpp"

using namespace std;

int main() {
  Stack<perft_t, MAX_MOVE_LENGTH> perft_results;
  u64_t depth, count, time;
  Board board;

  board.set_fen(fen::starting);
  depth = 7;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;

  board.set_fen(fen::pos2);
  depth = 6;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;

  board.set_fen(fen::pos3);
  depth = 8;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;

  board.set_fen(fen::pos4);
  depth = 6;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;

  board.set_fen(fen::pos5);
  depth = 5;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;

  board.set_fen(fen::pos6);
  depth = 6;
  perft_results = board.perft(depth, legal);
  count = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.count;});
  time = perft_results.reduce(0ULL, [](auto a, auto b) {return a + b.time;});
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << time / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)time << endl;
};