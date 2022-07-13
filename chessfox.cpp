#include <iostream>

#include "attack_rays.cpp"
#include "attacks.cpp"
#include "board.cpp"
#include "constants.cpp"
#include "magic.cpp"
#include "random.cpp"
#include "stack.cpp"
#include "time.cpp"
#include "zobrist.cpp"

using namespace std;


int main() {
  u64_t time, count, depth;
  Board board;

  board.set_fen(fen::starting);
  time = timing::ns();
  depth = 6;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;

  board.set_fen(fen::pos2);
  time = timing::ns();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;

  board.set_fen(fen::pos3);
  time = timing::ns();
  depth = 7;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;

  board.set_fen(fen::pos4);
  time = timing::ns();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;

  board.set_fen(fen::pos5);
  time = timing::ns();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;

  board.set_fen(fen::pos6);
  time = timing::ns();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << 1000 * ((float)count) / ((float)(timing::ns() - time)) << endl;
  cout << endl;
};