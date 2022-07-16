#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __DISPLAY__MODULE__
#define __DISPLAY__MODULE__


#include <iostream>

#include "board.cpp"
#include "constants.cpp"
#include "macros.cpp"
#include "types.cpp"


/*

  functions for displaying the board
  and bitboards

*/


// escape characters for colors in the console
std::string escape(piece_t piece, square_t square) {
  if (piece::color(piece) == color::white && square::color(square) == color::white) {
    return "\033[38;5;255;48;5;244m";
  } else if (piece::color(piece) == color::white && square::color(square) == color::black) {
    return "\033[38;5;255;48;5;88m";
  } else if (piece::color(piece) == color::black && square::color(square) == color::white) {
    return "\033[38;5;232;48;5;244m";
  } else if (piece::color(piece) == color::black && square::color(square) == color::black) {
    return "\033[38;5;232;48;5;88m";
  } else if (square::color(square) == color::white) {
    return "\033[48;5;244m";
  } else if (square::color(square) == color::black) {
    return "\033[48;5;88m";
  };
  return "";
};

std::string escape() {
  return "\033[0m";
};

// print a bitboard
void pretty_print(bitboard_t bitboard) {
  for (square_t square = 0; square < 64; square++) {
    if (square % 8 == 0) {
      if (square != 0) std::cout << std::endl;
      std::cout <<  "\t" + std::to_string(8 - square / 8) + " ";
    };
    std::cout << escape(piece::white, square);
    std::cout << (get_bit(bitboard, square) ? " X " : "   ");
    std::cout << escape();
  };
  std::cout << std::endl << "\t   A  B  C  D  E  F  G  H";
  std::cout << std::endl << "\tBOARD:\t\t" << std::hex << std::showbase << std::uppercase << bitboard << std::dec << std::endl;
};

// print a board
void pretty_print(Board board) {
  for (square_t square = 0; square < 64; square++) {
    if (square % 8 == 0) {
      if (square != 0) std::cout << std::endl;
      std::cout <<  "\t" + std::to_string(8 - square / 8) + " ";
    };
    std::cout << escape(board.pieces[square], square);
    std::cout << " " + piece::unicode(board.pieces[square]) + " ";
    std::cout << escape();
  };
  std::cout << std::endl << "\t   A  B  C  D  E  F  G  H";
  std::cout << std::endl << "\tZOBRIST HASH:\t"   << std::hex << std::showbase << std::uppercase << board.zobrist.hash << std::dec;
  std::cout << std::endl << "\tTURN:\t\t"         << color::to_string(board.turn);
  std::cout << std::endl << "\tCASTLING:\t"       << castling::to_string(board.castling);
  std::cout << std::endl << "\tEN PASSANT:\t"     << square::to_string(board.enpassant);
  std::cout << std::endl << "\tHALFMOVE CLOCK:\t" << std::to_string(board.halfmove_clock);
  std::cout << std::endl << "\tFULLMOVE CLOCK:\t" << std::to_string(board.fullmove_clock);
  std::cout << std::endl << "\tFEN:\t\t"          << board.fen();
  std::cout << std::endl << "\tOUTCOME:\t"        << outcome::to_string(board.outcome());
  std::cout << std::endl << "\tMOVES MADE:\t";
  for (undo_t undo : board.history) {
    std::cout << move::uci(undo.move) + " ";
  };
  if (board.history.size() == 0) std::cout << "-";
  std::cout << std::endl;
};


#endif // __DISPLAY__MODULE__