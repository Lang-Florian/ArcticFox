#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __PIECE__MODULE__
#define __PIECE__MODULE__


#include <array>
#include <string>
#include "types.cpp"


namespace piece{
enum : piece_t {
  white =        0b00000,
  white_pawn =   0b00100,
  white_knight = 0b01000,
  white_bishop = 0b01100,
  white_rook =   0b10000,
  white_queen =  0b10100,
  white_king =   0b11000,
  WHITE =        0b11100,
  black =        0b00001,
  black_pawn =   0b00101,
  black_knight = 0b01001,
  black_bishop = 0b01101,
  black_rook =   0b10001,
  black_queen =  0b10101,
  black_king =   0b11001,
  BLACK =        0b11101,
  none =         0b00010,
  pawn =         0b00110,
  knight =       0b01010,
  bishop =       0b01110,
  rook =         0b10010,
  queen =        0b10110,
  king =         0b11010,
  NONE =         0b11110,
};

constexpr piece_t all[12] = {
  white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
  black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
};

constexpr piece_t white_all[6] = {
  white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
};

constexpr piece_t black_all[6] = {
  black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
};

constexpr std::array<piece_t, 6> all_by_color[3] = {
  {
    white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
  },
  {
    black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
  },
  {
    pawn, knight, bishop, rook, queen, king,
  },
};

color_t color(piece_t piece) {
  return piece & 0b11;
};

piece_t type(piece_t piece) {
  return (piece & 0b11100) | 0b00010;
};

piece_t to_color(piece_t piece, color_t color) {
  return (piece & 0b11100) | color;
};

piece_t from_char(char c) {
  switch (c) {
    case 'P': return white_pawn;
    case 'N': return white_knight;
    case 'B': return white_bishop;
    case 'R': return white_rook;
    case 'Q': return white_queen;
    case 'K': return white_king;
    case 'p': return black_pawn;
    case 'n': return black_knight;
    case 'b': return black_bishop;
    case 'r': return black_rook;
    case 'q': return black_queen;
    case 'k': return black_king;
    default:  return none;
  };
};

std::string to_string(piece_t piece) {
  switch (piece) {
    case white_pawn:   return "P";
    case white_knight: return "N";
    case white_bishop: return "B";
    case white_rook:   return "R";
    case white_queen:  return "Q";
    case white_king:   return "K";
    case black_pawn:   return "p";
    case black_knight: return "n";
    case black_bishop: return "b";
    case black_rook:   return "r";
    case black_queen:  return "q";
    case black_king:   return "k";
    default:           return " ";
  };
};

std::string promotion_string(piece_t piece) {
  switch (type(piece)) {
    case knight: return "n";
    case bishop: return "b";
    case rook:   return "r";
    case queen:  return "q";
    default: return "";
  };
};
};


namespace piece::compiletime {
constexpr piece_t to_color(piece_t piece, color_t color) {
  return (piece & 0b11100) | color;
};

constexpr color_t color(piece_t piece) {
  return piece & 0b11;
};
};


#endif