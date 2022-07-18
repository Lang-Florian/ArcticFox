#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __CONSTANTS__MODULE__
#define __CONSTANTS__MODULE__


#include <string>

#include "macros.cpp"
#include "types.cpp"


#define MAX_GAME_LENGTH 512
#define MAX_MOVE_LENGTH 512


/*

  all the important constants and some useful "macro-like" functions
  to deal with them

*/


// color constants
namespace color {
  enum : color_t {
    white = 0b00,
    black = 0b01,
    none =  0b10,
  };

  color_t opponent(color_t color) {
    return color ^ 0b1;
  };

  color_t from_char(char c) {
    switch (c) {
      case 'w': return white;
      case 'b': return black;
      default:  return none;
    };
  };

  std::string to_string(color_t color) {
    switch (color) {
      case white: return "w";
      case black: return "b";
      default:    return "";
    };
  };

  namespace compiletime {
    constexpr color_t opponent(color_t color) {
      return color ^ 0b1;
    };
  };
};


// square constants
namespace square {
  enum : square_t {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    none,
  };

  const std::string labels[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "-",
  };
  
  std::string to_string(square_t square) {
    return labels[square];
  };

  color_t color(square_t square) {
    switch ((square + (square >> 3)) & 0b1) {
      case 0:  return color::white;
      case 1:  return color::black;
      default: return color::none;
    };
  };
};


// piece constants
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

  constexpr piece_t all_pieces[12] = {
    white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
    black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
  };

  constexpr piece_t white_pieces[6] = {
    white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
  };

  constexpr piece_t black_pieces[6] = {
    black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
  };

  constexpr std::array<piece_t, 6> all_pieces_by_color[3] = {
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

  std::string unicode(piece_t piece) {
    switch (type(piece)) {
      case pawn:   return "\u265F";
      case knight: return "\u265E";
      case bishop: return "\u265D";
      case rook:   return "\u265C";
      case queen:  return "\u265B";
      case king:   return "\u265A";
      default:     return " ";
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

  namespace compiletime {
    constexpr piece_t to_color(piece_t piece, color_t color) {
      return (piece & 0b11100) | color;
    };
  };
};


// fen constants
namespace fen {
  const std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  const std::string pos2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  const std::string pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
  const std::string pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  const std::string pos5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  const std::string pos6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
};


// bitboard constants
namespace bitboard {
  enum : bitboard_t {
    a8 = bitboard(square::a8), b8 = bitboard(square::b8), c8 = bitboard(square::c8), d8 = bitboard(square::d8), e8 = bitboard(square::e8), f8 = bitboard(square::f8), g8 = bitboard(square::g8), h8 = bitboard(square::h8),
    a7 = bitboard(square::a7), b7 = bitboard(square::b7), c7 = bitboard(square::c7), d7 = bitboard(square::d7), e7 = bitboard(square::e7), f7 = bitboard(square::f7), g7 = bitboard(square::g7), h7 = bitboard(square::h7),
    a6 = bitboard(square::a6), b6 = bitboard(square::b6), c6 = bitboard(square::c6), d6 = bitboard(square::d6), e6 = bitboard(square::e6), f6 = bitboard(square::f6), g6 = bitboard(square::g6), h6 = bitboard(square::h6),
    a5 = bitboard(square::a5), b5 = bitboard(square::b5), c5 = bitboard(square::c5), d5 = bitboard(square::d5), e5 = bitboard(square::e5), f5 = bitboard(square::f5), g5 = bitboard(square::g5), h5 = bitboard(square::h5),
    a4 = bitboard(square::a4), b4 = bitboard(square::b4), c4 = bitboard(square::c4), d4 = bitboard(square::d4), e4 = bitboard(square::e4), f4 = bitboard(square::f4), g4 = bitboard(square::g4), h4 = bitboard(square::h4),
    a3 = bitboard(square::a3), b3 = bitboard(square::b3), c3 = bitboard(square::c3), d3 = bitboard(square::d3), e3 = bitboard(square::e3), f3 = bitboard(square::f3), g3 = bitboard(square::g3), h3 = bitboard(square::h3),
    a2 = bitboard(square::a2), b2 = bitboard(square::b2), c2 = bitboard(square::c2), d2 = bitboard(square::d2), e2 = bitboard(square::e2), f2 = bitboard(square::f2), g2 = bitboard(square::g2), h2 = bitboard(square::h2),
    a1 = bitboard(square::a1), b1 = bitboard(square::b1), c1 = bitboard(square::c1), d1 = bitboard(square::d1), e1 = bitboard(square::e1), f1 = bitboard(square::f1), g1 = bitboard(square::g1), h1 = bitboard(square::h1),
    none = 0ULL, full = 0xFFFFFFFFFFFFFFFFULL,
  };

  const bitboard_t file_a = 0x0101010101010101ULL;
  const bitboard_t file_b = 0x0202020202020202ULL;
  const bitboard_t file_c = 0x0404040404040404ULL;
  const bitboard_t file_d = 0x0808080808080808ULL;
  const bitboard_t file_e = 0x1010101010101010ULL;
  const bitboard_t file_f = 0x2020202020202020ULL;
  const bitboard_t file_g = 0x4040404040404040ULL;
  const bitboard_t file_h = 0x8080808080808080ULL;
  
  const bitboard_t rank_1 = 0xFF00000000000000ULL;
  const bitboard_t rank_2 = 0x00FF000000000000ULL;
  const bitboard_t rank_3 = 0x0000FF0000000000ULL;
  const bitboard_t rank_4 = 0x000000FF00000000ULL;
  const bitboard_t rank_5 = 0x00000000FF000000ULL;
  const bitboard_t rank_6 = 0x0000000000FF0000ULL;
  const bitboard_t rank_7 = 0x000000000000FF00ULL;
  const bitboard_t rank_8 = 0x00000000000000FFULL;

  const bitboard_t white_side = rank_1 | rank_2 | rank_3 | rank_4;
  const bitboard_t black_side = rank_5 | rank_6 | rank_7 | rank_8;
  const bitboard_t queen_side = file_a | file_b | file_c | file_d;
  const bitboard_t king_side =  file_e | file_f | file_g | file_h;
  const bitboard_t center = d4 | e4 | d5 | e5;

  const bitboard_t white_squares = a8 |  0 | c8 |  0 | e8 |  0 | g8 |  0 | \
                                    0 | b7 |  0 | d7 |  0 | f7 |  0 | h7 | \
                                   a6 |  0 | c6 |  0 | e6 |  0 | g6 |  0 | \
                                    0 | b5 |  0 | d5 |  0 | f5 |  0 | h5 | \
                                   a4 |  0 | c4 |  0 | e4 |  0 | g4 |  0 | \
                                    0 | b3 |  0 | d3 |  0 | f3 |  0 | h3 | \
                                   a2 |  0 | c2 |  0 | e2 |  0 | g2 |  0 | \
                                    0 | b1 |  0 | d1 |  0 | f1 |  0 | h1;
  const bitboard_t black_squares =  0 | b8 |  0 | d8 |  0 | f8 |  0 | h8 | \
                                   a7 |  0 | c7 |  0 | e7 |  0 | g7 |  0 | \
                                    0 | b6 |  0 | d6 |  0 | f6 |  0 | h6 | \
                                   a5 |  0 | c5 |  0 | e5 |  0 | g5 |  0 | \
                                    0 | b4 |  0 | d4 |  0 | f4 |  0 | h4 | \
                                   a3 |  0 | c3 |  0 | e3 |  0 | g3 |  0 | \
                                    0 | b2 |  0 | d2 |  0 | f2 |  0 | h2 | \
                                   a1 |  0 | c1 |  0 | e1 |  0 | g1 |  0;
};


// castling constants
namespace castling {
  enum : castling_t {
    none =        0b0000,
    white_king =  0b0001,
    white_queen = 0b0010,
    black_king =  0b0100,
    black_queen = 0b1000,
    white =       0b0011,
    black =       0b1100,
    king =        0b0101,
    queen =       0b1010,
  };

  castling_t from_char(char c) {
    switch (c) {
      case 'K': return white_king;
      case 'Q': return white_queen;
      case 'k': return black_king;
      case 'q': return black_queen;
      default:  return none;
    };
  };

  std::string to_string(castling_t castling) {
    std::string string = "";
    if (castling & white_king)  string += "K";
    if (castling & white_queen) string += "Q";
    if (castling & black_king)  string += "k";
    if (castling & black_queen) string += "q";
    if (string.empty())         string += "-";
    return string;
  };
  
  const bitboard_t white_king_attack_mask =  bitboard::e1 | bitboard::f1 | bitboard::g1;
  const bitboard_t white_queen_attack_mask = bitboard::c1 | bitboard::d1 | bitboard::e1;
  const bitboard_t black_king_attack_mask =  bitboard::e8 | bitboard::f8 | bitboard::g8;
  const bitboard_t black_queen_attack_mask = bitboard::c8 | bitboard::d8 | bitboard::e8;

  const bitboard_t white_king_piece_mask =  bitboard::f1 | bitboard::g1;
  const bitboard_t white_queen_piece_mask = bitboard::b1 | bitboard::c1 | bitboard::d1;
  const bitboard_t black_king_piece_mask =  bitboard::f8 | bitboard::g8;
  const bitboard_t black_queen_piece_mask = bitboard::b8 | bitboard::c8 | bitboard::d8;
};


// outcome constants
namespace outcome {
  enum : outcome_t {
    none =                  0b00000000,

    checkmate_white =       0b00000100,
    checkmate_black =       0b00000101,

    draw =                  0b00001000,
    stalemate =             0b00011000,
    insufficient_material = 0b00101000,
    fifty_move_rule =       0b01001000,
    threefold_repetition =  0b10001000,
  };

  outcome_t checkmate_for(color_t color) {
    return 0b00000100 | color;
  };

  std::string to_string(outcome_t outcome) {
    std::string string = "";
    if (outcome == checkmate_white)       string += "1-0";
    if (outcome == checkmate_black)       string += "0-1";
    if (outcome == draw)                  string += "1/2-1/2";
    if (outcome == stalemate)             string += "1/2-1/2 by Stalemate";
    if (outcome == insufficient_material) string += "1/2-1/2 by Insufficient material";
    if (outcome == fifty_move_rule)       string += "1/2-1/2 by Fifty move rule";
    if (outcome == threefold_repetition)  string += "1/2-1/2 by Threefold repetition";
    if (string.empty())                   string += "-";
    return string;
  };
};


// move generation constants
enum get_t {
  quiet =            0b0001,
  direct_check =     0b0010,
  discovered_check = 0b0100,
  capture =          0b1000,
  non_quiet =        0b1110,
  legal =            0b1111,
};


constexpr bool ignore_outcome = false;
constexpr bool include_outcome = true;


#endif // __CONSTANTS__MODULE__