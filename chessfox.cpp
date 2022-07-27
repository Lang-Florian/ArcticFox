#include <iostream>
#include <sys/resource.h>
#include <bitset>

#include "attack_rays.cpp"
#include "attacks.cpp"
#include "board.cpp"
#include "constants.cpp"
#include "display.cpp"
#include "evaluation.cpp"
#include "macros.cpp"
#include "magic.cpp"
#include "moves.cpp"
#include "random.cpp"
#include "stack.cpp"
#include "time.cpp"
#include "transposition.cpp"
#include "types.cpp"
#include "zobrist.cpp"

using namespace std;


void do_perft(std::string fen, int depth) {
  Board board = Board(fen);
  u64_t start = timing::nanoseconds();
  u64_t count = board.perft<legal>(depth);
  u64_t end = timing::nanoseconds();
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << (end - start) / 1e9 << "s" << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)(end - start) << endl;
};


int main() {
  transposition::table.fill(transposition::entry_t {0, 0, 0});

  Board board;
  search_result_t result;

  board.set_fen(fen::startpos);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
  
  board.set_fen(fen::pos2);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
  
  board.set_fen(fen::pos3);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
  
  board.set_fen(fen::pos4);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
  
  board.set_fen(fen::pos5);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
  
  board.set_fen(fen::pos6);
  cout << board.fen() << endl;
  result = board.search(7);
  cout << result.score << endl;
  for (move_t move : result.continuation) {
    cout << move::uci(move) << endl;
  };
  cout << endl;
};