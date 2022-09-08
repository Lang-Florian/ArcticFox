#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __UCI__MODULE__
#define __UCI__MODULE__


#include <iostream>
#include <sstream>
#include <string>

#include "board.cpp"
#include "constants.cpp"
#include "debug.cpp"
#include "search.cpp"
#include "perft.cpp"


/*
  
  uci protocol

*/


void go(Board& board, std::istringstream& string_stream) {
  std::string token;
  while (string_stream >> token) {
    if (token == "perft") {
      int depth;
      string_stream >> depth;
      string_stream >> token;
      if (token == "quiet") {
        perft::perft<quiet>(board, depth, true);
      } else if (token == "check") {
        perft::perft<check>(board, depth, true);
      } else if (token == "capture") {
        perft::perft<capture>(board, depth, true);
      } else {
        perft::perft<legal>(board, depth, true);
      };
      return;
    };
  };
  std::cout << "bestmove " << move::to_string(search::search(board, 8).pv[0]) << std::endl;
};

void position(Board& board, std::istringstream& string_stream) {
  std::string token;
  string_stream >> token;
  if (token == "startpos") {
    board.set_fen();
    string_stream >> token;
  } else if (token == "fen") {
    std::string fen;
    while (string_stream >> token && token != "moves")
      fen += token + " ";
    board.set_fen(fen);
  };
  if (token == "moves") {
    while (string_stream >> token) {
      board.push_uci(token);
    };
  };
};

void test(Board& board, std::istringstream& string_stream) {
  std::string token;
  string_stream >> token;
  if (token == "perft") {
    std::string edp_file_path;
    string_stream >> edp_file_path;
    perft_test_suite(board, edp_file_path);
  };
};

void uci() {
  std::cout << ENGINE_NAME << " v" << VERSION << " by " << AUTHOR << std::endl;
  Board board;
  std::string token, command;
  do {
    getline(std::cin, command);
    std::istringstream string_stream(command);
    token.clear();
    string_stream >> std::skipws >> token;
    if (token == "uci") {
      std::cout << "id name " << ENGINE_NAME << std::endl
                << "id author " << AUTHOR << std::endl
                << "uciok"  << std::endl;
    } else if (token == "go") {
      go(board, string_stream);
    } else if (token == "position") {
      position(board, string_stream);
    } else if (token == "isready") {
      std::cout << "readyok" << std::endl;
    } else if (token == "d") {
      print_board(board);
    } else if (token == "arcticfox") {
      std::cout << ascii_art << std::endl;
      std::cout << "Arctic Foxes are the most beautiful animals in the world." << std::endl;
    } else if (token == "test") {
      test(board, string_stream);
    };
  } while (token != "quit");
};


#endif // __UCI__MODULE__