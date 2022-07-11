#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "attack_rays.cpp"
#include "attacks.cpp"
#include "board.cpp"
#include "constants.cpp"
#include "correct_attacks.cpp"
#include "magic.cpp"
#include "moves.cpp"
#include "random.cpp"
#include "time.cpp"
#include "zobrist.cpp"

#include "iostream"


using namespace std;

int main() {
  U64 time, count, depth;
  Board board;

  board.set_fen(FEN::initial);
  time = TIME::ms();
  depth = 6;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;

  board.set_fen(FEN::kiwipete);
  time = TIME::ms();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;

  board.set_fen(FEN::pos3);
  time = TIME::ms();
  depth = 7;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;

  board.set_fen(FEN::pos4);
  time = TIME::ms();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;

  board.set_fen(FEN::pos5);
  time = TIME::ms();
  depth = 4;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;

  board.set_fen(FEN::pos6);
  time = TIME::ms();
  depth = 5;
  count = board.perft(depth, true);
  cout << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
  cout << endl;
};