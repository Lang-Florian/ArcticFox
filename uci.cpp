#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "base.cpp"
#include "board.cpp"
#include "debug.cpp"
#include "perft.cpp"
#include "search.cpp"
#include "transposition.cpp"

/*

  Module to handle the UCI protocol.

*/

// uci go command
void go(Board& board, std::istringstream& string_stream) {
  std::string token;
  int depth = 8;
  while (string_stream >> token) {
    if (token == "perft") {
      int depth;
      std::string movetype;
      string_stream >> depth;
      string_stream >> movetype;
      if (movetype == "quiet") {
        perft<quiet>(board, depth, true);
      } else if (movetype == "check") {
        perft<check>(board, depth, true);
      } else if (movetype == "capture") {
        perft<capture>(board, depth, true);
      } else {
        perft<legal>(board, depth, true);
      };
      return;
    } else if (token == "depth") {
      string_stream >> depth;
    };
  };
  search_result_t search_result = search(board, depth);
  std::cout << "bestmove " << move_to_string(search_result.pv[0]) << "\n";
};

// uci position command
void position(Board& board, std::istringstream& string_stream) {
  std::string token;
  string_stream >> token;
  if (token == "startpos") {
    board.set_fen(STARTPOS);
    string_stream >> token;
  } else if (token == "fen") {
    std::string fen;
    while (string_stream >> token && token != "moves")
      fen += token + " ";
    board.set_fen(fen);
  };
  if (token == "moves") {
    while (string_stream >> token) {
      board.make(board.from_uci(token));
    };
  };
};

// uci test command
void test(Board& board, std::istringstream& string_stream) {
  std::string token;
  string_stream >> token;
  if (token == "perft") {
    std::string epd_file_path;
    string_stream >> epd_file_path;
    perft_test_suite(board, epd_file_path);
  };
};

// uci main loop
void uci_loop() {
  std::cout << ENGINE_NAME << " v" << VERSION << " by " << AUTHOR << "\n";
  std::cout << "info string transposition table size " << (table_size() >> 30) << "GiB\n";
  Board board;
  std::string token, command;
  do {
    getline(std::cin, command);
    std::istringstream string_stream(command);
    token.clear();
    string_stream >> std::skipws >> token;
    if (token == "uci") {
      std::cout << "id name " << ENGINE_NAME
                << " v" << VERSION
                << "\nid author " << AUTHOR
                << "\nuciok\n";
    } else if (token == "isready") {
      std::cout << "readyok\n";
    } else if (token == "d") {
      print_board(board);
    } else if (token == "go") {
      go(board, string_stream);
    } else if (token == "position") {
      position(board, string_stream);
    } else if (token == "stockfish") {
      std::cout << ASCII_ART << "\n";
      std::cout << "Arctic Foxes are the most beautiful animals in the world.\n";
    } else if (token == "test") {
      test(board, string_stream);
    };
  } while (token != "quit" && token != "exit");
};