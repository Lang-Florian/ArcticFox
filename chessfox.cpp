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

  // Board board = Board("1k1r4/p1p5/1pPbQ2p/1N4p1/8/6P1/P1P2P1P/3R2K1 b - - 4 34");
  // int depth = 8;
  // pretty_print(board);
  // u64_t tbhits = 0;
  // u64_t nodes = 0;
  // search_result_t result = board.search<color::black>(12, -eval::inf, -eval::max_eval, tbhits, nodes);
  // cout << "Score: " << eval::to_string(result.score) << endl;
  // for (auto move : result.continuation) {
  //   cout << move::uci(move) << " ";
  // }
  // cout << endl;
  // if (result.continuation.size() == 0) return 0;
  // board.make(result.continuation[0]);
  // pretty_print(board);
  // result = board.search(depth);
  // cout << "Score: " << eval::to_string(result.score) << endl;
  // for (auto move : result.continuation) {
  //   cout << move::uci(move) << " ";
  // }
  // cout << endl;
  // if (result.continuation.size() == 0) return 0;
  // board.make(result.continuation[0]);
  // pretty_print(board);
  // result = board.search(depth);
  // cout << "Score: " << eval::to_string(result.score) << endl;
  // for (auto move : result.continuation) {
  //   cout << move::uci(move) << " ";
  // }
  // cout << endl;
  // if (result.continuation.size() == 0) return 0;
  // board.make(result.continuation[0]);
  // pretty_print(board);
  // result = board.search(depth);
  // cout << "Score: " << eval::to_string(result.score) << endl;
  // for (auto move : result.continuation) {
  //   cout << move::uci(move) << " ";
  // }
  // cout << endl;
  // if (result.continuation.size() == 0) return 0;
  // board.make(result.continuation[0]);
  // pretty_print(board);
  // result = board.search(depth);
  // cout << "Score: " << eval::to_string(result.score) << endl;
  // for (auto move : result.continuation) {
  //   cout << move::uci(move) << " ";
  // }
  // cout << endl;
  // if (result.continuation.size() == 0) return 0;
  // board.make(result.continuation[0]);
  // pretty_print(board);


  Board board;
  while (true) {
    pretty_print(board);
    std::string move_str;
    std::cout << "Enter a move: ";
    std::cin >> move_str;
    board.push_uci(move_str);
    // get a move string from the user
    search_result_t result = board.search(8);
    std::cout << "Score: " << eval::to_string(result.score) << std::endl;
    board.make(result.continuation[0]);
  };
  
  // wellllllll
  // 8/7P/R7/8/5k2/4p3/5PK1/8 w - - 0 46
  // e2e4 d7d5 e4e5 b8c6 f1b5 a7a6 b5c6 b7c6 g1f3 e7e6 e1g1 d8d7 d2d4 a6a5 c1d2 f8b4 c2c3
  // b4e7 f1e1 e8f8 d1a4 c6c5 a4d1 c5c4 b2b3 c4b3 a2b3 c8b7 c3c4 e7b4 d2b4 a5b4 a1a8 b7a8
  // d1d2 d5c4 d2b4 d7e7 b4b8 e7e8 b8e8 f8e8 b3c4 a8f3 g2f3 g8e7 b1c3 e7f5 e1a1 e8d7 c3b5
  // h8b8 a1a7 b8b5 c4b5 f5d4 b5b6 d4f3 g1g2 f3e5 a7c7 d7d6 c7c8 e5c6 b6b7 c6b8 c8b8 d6c6
  // b8f8 c6b7 f8f7 b7c6 f7g7 h7h5 g7h7 c6d5 h7h5 d5d4 h5a5 d4e4 h2h4 e6e5 h4h5 e4f5 a5a6
  // e5e4 h5h6 f5f4 h6h7 e4e3 h7h8q e6h4

//   cout << transposition::table_size() / (float)(1ULL << 30) << " GB" << endl;
//   cout << sizeof(transposition::entry_t) << " bytes" << endl;

//   do_perft(fen::startpos, 7);
//   do_perft(fen::pos2, 5);
//   do_perft(fen::pos3, 7);
//   do_perft(fen::pos4, 5);
//   do_perft(fen::pos5, 5);
//   do_perft(fen::pos6, 5);
  // Board board;// = Board("4Q3/8/8/8/8/3K4/6kp/8 w - - 0 1");
  // pretty_print(board);
  // for (int i=0; i<15; i++) {
  //   cout << "Depth" << i << endl;
  //   u64_t time = timing::nanoseconds();
  //   search_result_t result = board.search(i);
  //   std::cout << "Score: " << result.eval << std::endl;
  //   std::cout << "Time: " << (timing::nanoseconds() - time) * 1e-9 << "s" << std::endl;
  //   for (move_t move : result.continuation) {
  //     cout << move::uci(move) << " ";
  //   };
  //  cout << endl;
  // };

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