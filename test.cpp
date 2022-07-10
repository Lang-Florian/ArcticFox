#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"
#include "board.cpp"
#include "time.cpp"

#include "iostream"
#include "fstream"


#ifndef __TEST__MODULE__
#define __TEST__MODULE__


inline void test_perft() {
  std::fstream file;
  file.open("test_data/perft8.txt", std::ios::in);
  if (!file.is_open()) {
    std::cout << "Could not open file" << std::endl;
    return;
  };
  string_t fen;
  string_t perft_result_string;
  while (getline(file, fen)) {
    getline(file, perft_result_string);
    U64 perft_result = 0;
    for (int i = 0; i < perft_result_string.size(); i++) {
      perft_result = perft_result * 10 + perft_result_string[i] - '0';
    };
    Board board = Board(fen);
    time_t start = TIME::ms();
    U64 perft_result_calculated = board.perft(8, true);
    time_t end = TIME::ms();
    std::cout << "perft8 of " << fen << std::endl;
    std::cout << perft_result_calculated << " calculated / " << perft_result << " expected " << std::endl;
    std::cout << end - start << "ms / " <<  ((float)perft_result_calculated) / (1000 * (float)(end - start)) << "MNps" << std::endl;
  };
};


#endif

// U64 time, count, depth;
// Board board;

// board.set_fen(FEN::initial);
// time = TIME::ms();
// depth = 7;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;

// board.set_fen(FEN::kiwipete);
// time = TIME::ms();
// depth = 6;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;

// board.set_fen(FEN::pos3);
// time = TIME::ms();
// depth = 8;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;

// board.set_fen(FEN::pos4);
// time = TIME::ms();
// depth = 6;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;

// board.set_fen(FEN::pos5);
// time = TIME::ms();
// depth = 5;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;

// board.set_fen(FEN::pos6);
// time = TIME::ms();
// depth = 6;
// count = board.perft(depth, true);
// cout << "  Perft " << depth << " of " << endl << board.fen() << endl;
// cout << "  Total:\t" << count << "\t\tMN/s:\t" << ((float)count) / (1000 * (float)(TIME::ms() - time)) << endl;
// cout << endl;