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

void do_perft(std::string fen, int depth) {
  Board board;
  board.set_fen(fen);
  u64_t start = timing::nanoseconds();
  u64_t count = board.perft(depth);
  u64_t end = timing::nanoseconds();
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << (end - start) / 1e9 << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)(end - start) << endl;
};

int main() {
  do_perft(fen::startpos, 7);
  do_perft(fen::pos2, 6);
  do_perft(fen::pos3, 8);
  do_perft(fen::pos4, 6);
  do_perft(fen::pos5, 5);
  do_perft(fen::pos6, 6);
};