#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __CONSTANTS__MODULE__
#define __CONSTANTS__MODULE__


#include <string>


/*

  all the important types and constants and some useful "macro-type" functions

*/


// important types
#define u8_t unsigned char
#define u16_t unsigned short
#define u32_t unsigned int
#define u64_t unsigned long long

#define bitboard_t u64_t
#define castling_t u8_t
#define color_t u8_t
#define gen_t u8_t
#define hash_t u64_t
#define move_t u32_t
#define outcome_t u8_t
#define piece_t u8_t
#define square_t u8_t


// some bit manipulations
#define bitboard(square) (1ULL << (square))
#define popcount(bitboard) __builtin_popcountll(bitboard)
#define get_lsb(bitboard) __builtin_ctzll(bitboard)
square_t pop_lsb(bitboard_t& bitboard) {
  square_t lsb = get_lsb(bitboard);
  bitboard &= ~(1ULL << lsb);
  return lsb;
};
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
bitboard_t pop_bit(bitboard_t& bitboard, square_t square) {
  bitboard_t bit = get_bit(bitboard, square);
  bitboard &= ~(1ULL << square);
  return bit;
};

#define push_offset(turn) (8 - 16 * (turn == color::white))

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

  int bishop_i = 0;
  int rook_i = 1;

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
      default: return " ";
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
  static const std::string starting = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  static const std::string pos2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  static const std::string pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
  static const std::string pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  static const std::string pos5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  static const std::string pos6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
};


// escape constants
namespace escape {
  std::string escape(piece_t piece, square_t square) {
    std::string string = "";
    if (piece::color(piece) == color::white) {
      // white foreground
      string += "\033[38;5;255m";
    } else {
      // black foreground
      string += "\033[38;5;232m";
    };
    if (square::color(square) == color::white) {
      // white background
      string += "\033[48;5;244m";
    } else {
      // black background
      string += "\033[48;5;88m";
    };
    return string;
  };

  std::string escape() {
    return "\033[0m";
  };
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
  
  std::string to_string(bitboard_t bitboard) {
    std::string string = "";
    for (square_t square = 0; square < 64; square++) {
      if (square % 8 == 0 && square != 0) string += "\n";
      if (square % 8 == 0) string +=  " " + std::to_string(8 - square / 8) + " ";
      string += escape::escape(square, piece::black);
      string += get_bit(bitboard, square) ? " X " : "   ";
      string += escape::escape();
    };
    string += "\n    A  B  C  D  E  F  G  H\n\n";
    string += "  BOARD: " + std::to_string(bitboard);
    return string;
  };
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
    none =                  0b00000010,

    checkmate_white =       0b00000100,
    checkmate_black =       0b00000101,

    draw =                  0b00001010,
    stalemate =             0b00011010,
    insufficient_material = 0b00101010,
    fifty_move_rule =       0b01001010,
    threefold_repetition =  0b10001010,
  };

  outcome_t checkmate_for(color_t color) {
    return 0b00000100 | color;
  };

  color_t winner(outcome_t outcome) {
    return outcome & 0b11;
  };
};


/* some move definitions and macros
  
  MOVE

  0000 0000 0000 0000 0000 0000 0011 1111     from
  0000 0000 0000 0000 0000 1111 1100 0000     to
  0000 0000 0000 0001 1111 0000 0000 0000     moved_piece
  0000 0000 0011 1110 0000 0000 0000 0000     target_piece
  0000 0111 1100 0000 0000 0000 0000 0000     captured_piece
  0000 1000 0000 0000 0000 0000 0000 0000     double_pawn_push
  0001 0000 0000 0000 0000 0000 0000 0000     enpassant
  0010 0000 0000 0000 0000 0000 0000 0000     castling
  0100 0000 0000 0000 0000 0000 0000 0000     promotion
  1000 0000 0000 0000 0000 0000 0000 0000     capture

*/
namespace move {
  // encode a move
  move_t move(square_t from,
              square_t to,
              piece_t moved_piece,
              piece_t target_piece=piece::none,
              piece_t captured_piece=piece::none,
              bool double_pawn_push=false,
              bool enpassant=false,
              bool castling=false) {
    return ((from) << 0) |
           ((to) << 6) |
           ((moved_piece) << 12) |
           ((target_piece) << 17) |
           ((captured_piece) << 22) |
           ((double_pawn_push) << 27) |
           ((enpassant) << 28) |
           ((castling) << 29) |
           ((target_piece != piece::none) << 30) |
           ((captured_piece != piece::none) << 31);
  };

  // decode a move
  square_t from(move_t move) {
    return (move >> 0) & 0b111111;
  };

  square_t to(move_t move) {
    return (move >> 6) & 0b111111;
  };

  piece_t moved_piece(move_t move) {
    return (move >> 12) & 0b11111;
  };

  piece_t target_piece(move_t move) {
    return (move >> 17) & 0b11111;
  };

  piece_t captured_piece(move_t move) {
    return (move >> 22) & 0b11111;
  };

  bool double_pawn_push(move_t move) {
    return (move >> 27) & 0b1;
  };

  bool enpassant(move_t move) {
    return (move >> 28) & 0b1;
  };

  bool castling(move_t move) {
    return (move >> 29) & 0b1;
  };

  bool promotion(move_t move) {
    return (move >> 30) & 0b1;
  };
  
  bool capture(move_t move) {
    return (move >> 31) & 0b1;
  };

  std::string to_string(move_t move) {
    return square::to_string(from(move)) + square::to_string(to(move)) + piece::promotion_string(target_piece(move));
  };

  castling_t removed_castling(move_t move) {
    return (
      (from(move) == square::e1 || from(move) == square::h1 || to(move) == square::h1) << 0 |
      (from(move) == square::e1 || from(move) == square::a1 || to(move) == square::a1) << 1 |
      (from(move) == square::e8 || from(move) == square::h8 || to(move) == square::h8) << 2 |
      (from(move) == square::e8 || from(move) == square::a8 || to(move) == square::a8) << 3
    );
  };
};


// some move generation constants
enum : gen_t {
  legal,
  evasion,
};


#endif // __CONSTANTS__MODULE__