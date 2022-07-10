#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "string"


#ifndef __CONSTANTS__MODULE__
#define __CONSTANTS__MODULE__


// most used types
#define string_t std::string

#define U8 unsigned char
#define U16 unsigned short
#define U32 unsigned int
#define U64 unsigned long long

#define square_t unsigned char
#define color_t unsigned char
#define piece_t unsigned char
#define bitboard_t unsigned long long
#define castling_t unsigned char
#define move_t unsigned int
#define hash_t unsigned short
#define outcome_t unsigned char
#define eval_t unsigned long long


// some bit manipulation macros
#define bitboard(square) (1ULL << (square))

#define popcount(bitboard) __builtin_popcountll(bitboard)

#define get_lsb(bitboard) __builtin_ctzll(bitboard)

inline square_t pop_lsb(bitboard_t& bitboard) {
  square_t lsb = get_lsb(bitboard);
  bitboard &= bitboard - 1;
  return lsb;
};

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))

inline bitboard_t pop_bit(bitboard_t& bitboard, square_t square) {
  bitboard_t bit = get_bit(bitboard, square);
  bitboard &= ~(1ULL << square);
  return bit;
};


// initialize color definitions
namespace COLOR {
  enum : color_t {
    none =  0b00,
    white = 0b01,
    black = 0b10,
    both =  0b11,
  };

  static const color_t opponent_array[] = {
    none,
    black,
    white,
    both,
  };

  inline color_t opponent(color_t color) {
    return opponent_array[color];
  };

  inline color_t from_char(char c) {
    switch (c) {
      case 'w': return white;
      case 'b': return black;
      default:  return none;
    };
  };

  static const string_t string_array[] = {"", "w", "b", ""};

  inline string_t to_string(color_t color) {
    return string_array[color];
  };
};


// initialize square definitions
namespace SQUARE {
  enum : square_t {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
    none,
  };

  static const string_t string_array[] = {
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

  inline string_t to_string(square_t square) {
    return string_array[square];
  };

  inline color_t color(square_t square) {
    return (square + (square >> 3)) & 0b1;
  };
};


// initialize piece definitions
namespace PIECE{
  enum : piece_t {
    none =         0b00000,
    pawn =         0b00001,
    knight =       0b00010,
    bishop =       0b00011,
    rook =         0b00100,
    queen =        0b00101,
    king =         0b00110,
    none_ =        0b00111,
    white =        0b01000,
    white_pawn =   0b01001,
    white_knight = 0b01010,
    white_bishop = 0b01011,
    white_rook =   0b01100,
    white_queen =  0b01101,
    white_king =   0b01110,
    white_ =       0b01111,
    black =        0b10000,
    black_pawn =   0b10001,
    black_knight = 0b10010,
    black_bishop = 0b10011,
    black_rook =   0b10100,
    black_queen =  0b10101,
    black_king =   0b10110,
    black_ =       0b10111,
    both =         0b11000,
    both_pawn =    0b11001,
    both_knight =  0b11010,
    both_bishop =  0b11011,
    both_rook =    0b11100,
    both_queen =   0b11101,
    both_king =    0b11110,
    both_ =        0b11111,
  };

  inline color_t color(piece_t piece) {
    return (piece >> 3) & 0b11;
  };

  inline piece_t type(piece_t piece) {
    return piece & 0b111;
  };

  inline piece_t piece(color_t color, piece_t piece_type) {
    return (color << 3) | piece_type;
  };

  inline piece_t piece_color(piece_t piece) {
    return piece & 0b11000;
  };
  
  inline piece_t to_color(piece_t piece, color_t color) {
    return (piece & 0b111) | (color << 3);
  };

  inline piece_t from_char(char c) {
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

  static const string_t unicode_array[] = {
    " ", "\u265F", "\u265E", "\u265D", "\u265C", "\u265B", "\u265A", " ",
  };

  inline string_t unicode(piece_t piece) {
    return unicode_array[type(piece)];
  };

  static const string_t string_array[] = {
    " ", " ", " ", " ", " ", " ", " ", " ",
    " ", "P", "N", "B", "R", "Q", "K", " ",
    " ", "p", "n", "b", "r", "q", "k", " ",
    " ", " ", " ", " ", " ", " ", " ", " ",
  };

  inline string_t to_string(piece_t piece) {
    return string_array[piece];
  };

  static const string_t promotion_string_array[] = {
    "", "p", "n", "b", "r", "q", "k", "", 
  };

  inline string_t promotion_string(piece_t piece) {
    return promotion_string_array[type(piece)];
  };
};


// initialize some fen position definitions
namespace FEN {
  static const string_t initial = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  static const string_t kiwipete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  static const string_t pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
  static const string_t pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  static const string_t pos5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  static const string_t pos6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
};


// initialize terminal escape characters
namespace ESCAPE {
  inline string_t escape(square_t square, piece_t piece) {
    string_t string = "";
    if ((square + (square >> 3)) & 0b1) {
      // black background
      string += "\033[48;5;88m";
    } else {
      // white background
      string += "\033[48;5;244m";
    }
    if (PIECE::color(piece) == COLOR::white) {
      // white foreground
      string += "\033[38;5;255m";
    } else if (PIECE::color(piece) == COLOR::black) {
      // black foreground
      string += "\033[38;5;232m";
    } else {
      // black foreground
      string += "\033[38;5;232m";
    }
    return string;
  };

  inline string_t escape() {
    return "\033[0m";
  };
};


// initialize some bitboard definitions
namespace BITBOARD {
  enum : bitboard_t {
    A8 = bitboard(SQUARE::A8), B8 = bitboard(SQUARE::B8), C8 = bitboard(SQUARE::C8), D8 = bitboard(SQUARE::D8), E8 = bitboard(SQUARE::E8), F8 = bitboard(SQUARE::F8), G8 = bitboard(SQUARE::G8), H8 = bitboard(SQUARE::H8),
    A7 = bitboard(SQUARE::A7), B7 = bitboard(SQUARE::B7), C7 = bitboard(SQUARE::C7), D7 = bitboard(SQUARE::D7), E7 = bitboard(SQUARE::E7), F7 = bitboard(SQUARE::F7), G7 = bitboard(SQUARE::G7), H7 = bitboard(SQUARE::H7),
    A6 = bitboard(SQUARE::A6), B6 = bitboard(SQUARE::B6), C6 = bitboard(SQUARE::C6), D6 = bitboard(SQUARE::D6), E6 = bitboard(SQUARE::E6), F6 = bitboard(SQUARE::F6), G6 = bitboard(SQUARE::G6), H6 = bitboard(SQUARE::H6),
    A5 = bitboard(SQUARE::A5), B5 = bitboard(SQUARE::B5), C5 = bitboard(SQUARE::C5), D5 = bitboard(SQUARE::D5), E5 = bitboard(SQUARE::E5), F5 = bitboard(SQUARE::F5), G5 = bitboard(SQUARE::G5), H5 = bitboard(SQUARE::H5),
    A4 = bitboard(SQUARE::A4), B4 = bitboard(SQUARE::B4), C4 = bitboard(SQUARE::C4), D4 = bitboard(SQUARE::D4), E4 = bitboard(SQUARE::E4), F4 = bitboard(SQUARE::F4), G4 = bitboard(SQUARE::G4), H4 = bitboard(SQUARE::H4),
    A3 = bitboard(SQUARE::A3), B3 = bitboard(SQUARE::B3), C3 = bitboard(SQUARE::C3), D3 = bitboard(SQUARE::D3), E3 = bitboard(SQUARE::E3), F3 = bitboard(SQUARE::F3), G3 = bitboard(SQUARE::G3), H3 = bitboard(SQUARE::H3),
    A2 = bitboard(SQUARE::A2), B2 = bitboard(SQUARE::B2), C2 = bitboard(SQUARE::C2), D2 = bitboard(SQUARE::D2), E2 = bitboard(SQUARE::E2), F2 = bitboard(SQUARE::F2), G2 = bitboard(SQUARE::G2), H2 = bitboard(SQUARE::H2),
    A1 = bitboard(SQUARE::A1), B1 = bitboard(SQUARE::B1), C1 = bitboard(SQUARE::C1), D1 = bitboard(SQUARE::D1), E1 = bitboard(SQUARE::E1), F1 = bitboard(SQUARE::F1), G1 = bitboard(SQUARE::G1), H1 = bitboard(SQUARE::H1),
    none = 0ULL, full = 0xFFFFFFFFFFFFFFFFULL,
  };

  static const bitboard_t file_A = 0x0101010101010101ULL;
  static const bitboard_t file_B = 0x0202020202020202ULL;
  static const bitboard_t file_C = 0x0404040404040404ULL;
  static const bitboard_t file_D = 0x0808080808080808ULL;
  static const bitboard_t file_E = 0x1010101010101010ULL;
  static const bitboard_t file_F = 0x2020202020202020ULL;
  static const bitboard_t file_G = 0x4040404040404040ULL;
  static const bitboard_t file_H = 0x8080808080808080ULL;

  static const bitboard_t rank_1 = 0xFF00000000000000ULL;
  static const bitboard_t rank_2 = 0x00FF000000000000ULL;
  static const bitboard_t rank_3 = 0x0000FF0000000000ULL;
  static const bitboard_t rank_4 = 0x000000FF00000000ULL;
  static const bitboard_t rank_5 = 0x00000000FF000000ULL;
  static const bitboard_t rank_6 = 0x0000000000FF0000ULL;
  static const bitboard_t rank_7 = 0x000000000000FF00ULL;
  static const bitboard_t rank_8 = 0x00000000000000FFULL;

  static const bitboard_t white_side = rank_1 | rank_2 | rank_3 | rank_4;
  static const bitboard_t black_side = rank_5 | rank_6 | rank_7 | rank_8;
  static const bitboard_t queen_side = file_A | file_B | file_C | file_D;
  static const bitboard_t king_side = file_E | file_F | file_G | file_H;
  static const bitboard_t center = D4 | E4 | D5 | E5;
  static const bitboard_t white_squares = A8 |  0 | C8 |  0 | E8 |  0 | G8 |  0 | \
                                           0 | B7 |  0 | D7 |  0 | F7 |  0 | H7 | \
                                          A6 |  0 | C6 |  0 | E6 |  0 | G6 |  0 | \
                                           0 | B5 |  0 | D5 |  0 | F5 |  0 | H5 | \
                                          A4 |  0 | C4 |  0 | E4 |  0 | G4 |  0 | \
                                           0 | B3 |  0 | D3 |  0 | F3 |  0 | H3 | \
                                          A2 |  0 | C2 |  0 | E2 |  0 | G2 |  0 | \
                                           0 | B1 |  0 | D1 |  0 | F1 |  0 | H1;
  static const bitboard_t black_squares =  0 | B8 |  0 | D8 |  0 | F8 |  0 | H8 | \
                                          A7 |  0 | C7 |  0 | E7 |  0 | G7 |  0 | \
                                           0 | B6 |  0 | D6 |  0 | F6 |  0 | H6 | \
                                          A5 |  0 | C5 |  0 | E5 |  0 | G5 |  0 | \
                                           0 | B4 |  0 | D4 |  0 | F4 |  0 | H4 | \
                                          A3 |  0 | C3 |  0 | E3 |  0 | G3 |  0 | \
                                           0 | B2 |  0 | D2 |  0 | F2 |  0 | H2 | \
                                          A1 |  0 | C1 |  0 | E1 |  0 | G1 |  0;
  
  inline string_t to_string(bitboard_t bitboard) {
    string_t string = "";
    for (square_t square = 0; square < 64; square++) {
      if (square % 8 == 0 && square != 0) string += "\n";
      if (square % 8 == 0) string +=  " " + std::to_string(8 - square / 8) + " ";
      string += ESCAPE::escape(square, PIECE::black);
      string += get_bit(bitboard, square) ? " X " : "   ";
      string += ESCAPE::escape();
    };
    string += "\n    A  B  C  D  E  F  G  H\n\n";
    string += "  BOARD: " + std::to_string(bitboard);
    return string;
  };
};


// initialize castling definitions
namespace CASTLING {
  enum : castling_t {
    none =             0b0000,
    white_king =       0b0001,
    white_queen =      0b0010,
    black_king =       0b0100,
    black_queen =      0b1000,
    white =            0b0011,
    black =            0b1100,
    king =             0b0101,
    queen =            0b1010,
  };

  inline castling_t from_char(char c) {
    switch (c) {
      case 'K': return white_king;
      case 'Q': return white_queen;
      case 'k': return black_king;
      case 'q': return black_queen;
      default:  return none;
    };
  };

  inline string_t to_string(castling_t castling) {
    string_t string = "";
    if (castling & white_king)  string += "K";
    if (castling & white_queen) string += "Q";
    if (castling & black_king)  string += "k";
    if (castling & black_queen) string += "q";
    if (string.empty()) string = "-";
    return string;
  };
  
  bitboard_t white_king_attack_mask =  BITBOARD::E1 | BITBOARD::F1 | BITBOARD::G1;
  bitboard_t white_queen_attack_mask = BITBOARD::C1 | BITBOARD::D1 | BITBOARD::E1;
  bitboard_t black_king_attack_mask =  BITBOARD::E8 | BITBOARD::F8 | BITBOARD::G8;
  bitboard_t black_queen_attack_mask = BITBOARD::C8 | BITBOARD::D8 | BITBOARD::E8;

  bitboard_t white_king_piece_mask =  BITBOARD::F1 | BITBOARD::G1;
  bitboard_t white_queen_piece_mask = BITBOARD::B1 | BITBOARD::C1 | BITBOARD::D1;
  bitboard_t black_king_piece_mask =  BITBOARD::F8 | BITBOARD::G8;
  bitboard_t black_queen_piece_mask = BITBOARD::B8 | BITBOARD::C8 | BITBOARD::D8;
};


// initialize outcome definitions
namespace OUTCOME {
  enum : outcome_t {
    none =                  0b00000000,

    checkmate =             0b00000100,
    checkmate_white =       0b00000101,
    checkmate_black =       0b00000110,

    draw =                  0b00001000,
    stalemate =             0b00011011,
    insufficient_material = 0b00101011,
    fifty_move_rule =       0b01001011,
    threefold_repetition =  0b10001011,
  };

  inline outcome_t checkmate_for(color_t color) {
    return checkmate | color;
  };

  inline color_t winner(outcome_t outcome) {
    return outcome & 0b11;
  };
};


#endif