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
  Board board;
  board.set_fen(fen);
  u64_t start = timing::nanoseconds();
  u64_t count = board.perft<check>(depth);
  u64_t end = timing::nanoseconds();
  cout << endl << "\tPerft " << depth << " of " << endl;
  cout << "\t" << board.fen() << endl;
  cout << "\tTotal:\t" << count << endl;
  cout << "\tTime:\t" << (end - start) / 1e9 << "s" << endl;
  cout << "\tMNps:\t" << 1000 * (float)count / (float)(end - start) << endl;
};

int main() {
  cout << transposition::table_size() / (float)(1ULL << 30) << " GB" << endl;

  do_perft(fen::startpos, 7);
  do_perft(fen::pos2, 5);
  do_perft(fen::pos3, 7);
  do_perft(fen::pos4, 5);
  do_perft(fen::pos5, 5);
  do_perft(fen::pos6, 5);

  // Board board;
  // search_result_t result;

  // board.set_fen(fen::startpos);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
  
  // board.set_fen(fen::pos2);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
  
  // board.set_fen(fen::pos3);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
  
  // board.set_fen(fen::pos4);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
  
  // board.set_fen(fen::pos5);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
  
  // board.set_fen(fen::pos6);
  // cout << board.fen() << endl;
  // result = board.search(7);
  // cout << result.eval << endl;
  // for (move_t move : result.continuation) {
  //   cout << move::uci(move) << endl;
  // };
  // cout << endl;
};