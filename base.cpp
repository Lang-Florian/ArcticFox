#pragma once
#include <array>
#include <string>

// constants

#define ENGINE_NAME "ArcticFox"
#define VERSION "0.3"
#define AUTHOR "N3U1R0N"


#define MAX_GAME_LENGTH 512
#define MAX_MOVE_GENERATION_SIZE 512
#define MAX_PV_DEPTH 32
#define MAX_QSEARCH_DEPTH 32
#define ASCII_ART "                        ▒  ▒▒▒                              \n"\
                  "                       ▒▒▒▒▓▓█▓▒▒▒                          \n"\
                  "                       ▒▓▒▒▒▒▓▓▓▒▒▒                         \n"\
                  "                       ▒▓▓▓▓▓▓▓▒▒▓▓▒▒                       \n"\
                  "                     ▒▒▒▒▒▓▓▓▓▓▓▓▓▓▒▒                       \n"\
                  "                     ▒▒▒▓▓█▓██▓▓▓▓▒▒▒▒▒    ▒▓▓▒▒            \n"\
                  "                        ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒▓▒▒           \n"\
                  "                        ▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▒▒▓▓▓▓▒▓▓▓▒          \n"\
                  "                       ▒▒▒▒▒▒▒▒▓▓▒▒▒▓▓█▓▓▓▓▓▓▓▓█▓▒          \n"\
                  "                        ▒▓▓▓▓▓▓▓▓▓▒▓▓██▓▒▒▒▓▓▓▓█▓▒          \n"\
                  "             ▒▓▒         ▒▒▒▒▒▒▓▓▒▓▓██▓▓▒▒▒▒▓██▓▒▒          \n"\
                  "      ▒▒▒▒▒▒▒███▓           ▒▓▓▓▓▒▓▓▓▓▒▒▒▒▒▒▓▓▓▓▒           \n"\
                  "      ▒████▓▓▓██▒  ▒    ▒   ▒▓█▓▒▒▒▒▒▒▒▒▒▒▒▒▓▓▒             \n"\
                  "      ▒▓████████▒ ▓█▒  ▓█▒ ▒▓▓▓▒▒▒▒▒▓▒▒▒▒▓▓▓▓▒▒       ▒▒▓▓▒ \n"\
                  "▓▓▓▓▓▒ ▒▓███████▒ ▓█▒ ▒▓█▒▒▒█▓▓▓▓▒▓███▓▓▓███▓▓▒▒▒▒▒▒  ▒▓█▓▒ \n"\
                  "▓███▓▒ ▓████████▓▒▒▓▒ ▒██▓▒▓██▓▓▓▒▓██▓▓▓▓▒▓▓▓▒▒▓▓▓▒▒▒▒▒▓▓▓▒ \n"\
                  "▒▓██▓ ▒██████████▓██▓▒▓██▓▓▓██▓▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▓█▓▓▓▓▒▒▒▓▒  \n"\
                  "▓████▒▒▓██████████████████████▒▒▒▒▒▓▓▒▒▒▒▒▒▓▓▒▒▓█▓▒▒▒▒▒▓▓▓▒ \n"\
                  "▓████▒ ▒▒▓█████▓▒▒▒▒▒▓████████▓▓▓▓▓██▓▓▓▓▒▒▒   ▓▓▒   ▒▓████▒\n"\
                  " ▒▒▒▒    ▓█████▓▒  ▒▒▓█████████▓▒▓▓███▓▒     ▒▒▓██▓▒ ▒▓▓▓▓▓▒\n"\
                  "         ▒▒▒▒▒▓▓    ▒███████▓▓▓▒▒▓█████▒     ▓█████▓        \n"\
                  "            ▒▒▒▒▒     ▒▒▒▓▒      ▒▒▒▒▒▒▒     ▒▒▓▓▓▓▒        \n"\
                  "                                                            \n"\
                  "    ▒▒▓▓▒              ▒█▒   ▓█▓▒      ▒▓▓▓▓▓▒              \n"\
                  "   ▒▓███▓  ▒▓▓▓▓▒▒▓█▓▓▒▓██▓▒▒▓▓▒▒▒█▓▓▓▒▓█▓▒▒▒ ▒▓▓▓▓▒ ▒▓▓▒▓▓▒\n"\
                  "  ▒██▒▓█▓ ▒██▒▒▒▓█▓   ▓█▓▒  ▓█▓▒██▓   ▒██▓▓▓▒▓█▓▒▓█▓ ▒███▒  \n"\
                  " ▒██▓▓▓██▒▓█▓▒  ▒██▓▒▒▓█▓▒▒▒██▒▒▒██▒▒ ▓█▓▒  ▒▓█▓▓██▒▒▓▓██▓  "
#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


// types

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long long u64_t;
typedef char i8_t;
typedef short i16_t;
typedef int i32_t;
typedef long long i64_t;

typedef u64_t bitboard_t;
typedef u8_t castling_t;
typedef u8_t color_t;
typedef u64_t hash_t;
typedef u32_t move_t;
typedef u8_t movetype_t;
typedef u8_t outcome_t;
typedef u8_t piece_t;
typedef i16_t score_t;
typedef u8_t square_t;

#define move_stack_t stack::Stack<move_t, MAX_MOVE_GENERATION_SIZE>
#define pv_t stack::Stack<move_t, MAX_PV_DEPTH>

#define none 0ULL


// macros

#define bitboard(square) (1ULL << (square))
#define popcount(bitboard) __builtin_popcountll(bitboard)
#define get_lsb(bitboard) __builtin_ctzll(bitboard)
#define pop_lsb(bitboard) __builtin_ctzll(bitboard); bitboard &= bitboard - 1
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) & (1ULL << (square))); (bitboard) &= ~(1ULL << (square))

#define opponent(color) ((color) ^ 0b10)

#define to_color(piece, color) (((piece) & 0b11100) | (color))
#define color(piece) ((piece) & 0b11)
#define piece_type(piece) ((piece) & 0b11100)


// squares

enum : square_t {
  a8 =  0, b8 =  1, c8 =  2, d8 =  3, e8 =  4, f8 =  5, g8 =  6, h8 =  7,
  a7 =  8, b7 =  9, c7 = 10, d7 = 11, e7 = 12, f7 = 13, g7 = 14, h7 = 15,
  a6 = 16, b6 = 17, c6 = 18, d6 = 19, e6 = 20, f6 = 21, g6 = 22, h6 = 23,
  a5 = 24, b5 = 25, c5 = 26, d5 = 27, e5 = 28, f5 = 29, g5 = 30, h5 = 31,
  a4 = 32, b4 = 33, c4 = 34, d4 = 35, e4 = 36, f4 = 37, g4 = 38, h4 = 39,
  a3 = 40, b3 = 41, c3 = 42, d3 = 43, e3 = 44, f3 = 45, g3 = 46, h3 = 47,
  a2 = 48, b2 = 49, c2 = 50, d2 = 51, e2 = 52, f2 = 53, g2 = 54, h2 = 55,
  a1 = 56, b1 = 57, c1 = 58, d1 = 59, e1 = 60, f1 = 61, g1 = 62, h1 = 63,
  none_square = 64,
};

std::string square_to_string(square_t square) {
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
  return labels[square];
};


// bitboard
enum : bitboard_t {
  A8 = bitboard(a8), B8 = bitboard(b8), C8 = bitboard(c8), D8 = bitboard(d8), E8 = bitboard(e8), F8 = bitboard(f8), G8 = bitboard(g8), H8 = bitboard(h8),
  A7 = bitboard(a7), B7 = bitboard(b7), C7 = bitboard(c7), D7 = bitboard(d7), E7 = bitboard(e7), F7 = bitboard(f7), G7 = bitboard(g7), H7 = bitboard(h7),
  A6 = bitboard(a6), B6 = bitboard(b6), C6 = bitboard(c6), D6 = bitboard(d6), E6 = bitboard(e6), F6 = bitboard(f6), G6 = bitboard(g6), H6 = bitboard(h6),
  A5 = bitboard(a5), B5 = bitboard(b5), C5 = bitboard(c5), D5 = bitboard(d5), E5 = bitboard(e5), F5 = bitboard(f5), G5 = bitboard(g5), H5 = bitboard(h5),
  A4 = bitboard(a4), B4 = bitboard(b4), C4 = bitboard(c4), D4 = bitboard(d4), E4 = bitboard(e4), F4 = bitboard(f4), G4 = bitboard(g4), H4 = bitboard(h4),
  A3 = bitboard(a3), B3 = bitboard(b3), C3 = bitboard(c3), D3 = bitboard(d3), E3 = bitboard(e3), F3 = bitboard(f3), G3 = bitboard(g3), H3 = bitboard(h3),
  A2 = bitboard(a2), B2 = bitboard(b2), C2 = bitboard(c2), D2 = bitboard(d2), E2 = bitboard(e2), F2 = bitboard(f2), G2 = bitboard(g2), H2 = bitboard(h2),
  A1 = bitboard(a1), B1 = bitboard(b1), C1 = bitboard(c1), D1 = bitboard(d1), E1 = bitboard(e1), F1 = bitboard(f1), G1 = bitboard(g1), H1 = bitboard(h1),
  file_a = 0x0101010101010101ULL,
  file_b = 0x0202020202020202ULL,
  file_c = 0x0404040404040404ULL,
  file_d = 0x0808080808080808ULL,
  file_e = 0x1010101010101010ULL,
  file_f = 0x2020202020202020ULL,
  file_g = 0x4040404040404040ULL,
  file_h = 0x8080808080808080ULL,
  rank_1 = 0xFF00000000000000ULL,
  rank_2 = 0x00FF000000000000ULL,
  rank_3 = 0x0000FF0000000000ULL,
  rank_4 = 0x000000FF00000000ULL,
  rank_5 = 0x00000000FF000000ULL,
  rank_6 = 0x0000000000FF0000ULL,
  rank_7 = 0x000000000000FF00ULL,
  rank_8 = 0x00000000000000FFULL,

  white_OO_king_traverse =  E1 | F1 | G1,
  white_OOO_king_traverse = C1 | D1 | E1,
  black_OO_king_traverse =  E8 | F8 | G8,
  black_OOO_king_traverse = C8 | D8 | E8,

  white_OO_rook_traverse =  F1 | G1,
  white_OOO_rook_traverse = B1 | C1 | D1,
  black_OO_rook_traverse =  F8 | G8,
  black_OOO_rook_traverse = B8 | C8 | D8,

  full =   0xFFFFFFFFFFFFFFFFULL,
};

const bitboard_t files[8] = {file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h};
const bitboard_t ranks[8] = {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};


// castling

enum : castling_t {
  white_OO =  0b0001,
  white_OOO = 0b0010,
  black_OO =  0b0100,
  black_OOO = 0b1000,
};

castling_t castling_from_char(char c) {
  switch (c) {
    case 'K': return white_OO;
    case 'Q': return white_OOO;
    case 'k': return black_OO;
    case 'q': return black_OOO;
    default:  return none;
  };
};

std::string castling_to_string(castling_t castling) {
  std::string string = "";
  if (castling & white_OO)  string += "K";
  if (castling & white_OOO) string += "Q";
  if (castling & black_OO)  string += "k";
  if (castling & black_OOO) string += "q";
  if (string.empty())       string += "-";
  return string;
};


// color
enum : color_t {
  white = 0b01,
  black = 0b11,
};

color_t color_from_char(char c) {
  switch (c) {
    case 'w': return white;
    case 'b': return black;
    default:  return none;
  };
};

std::string color_to_string(color_t color) {
  const std::string labels[] = {"", "w", "", "b"};
  return labels[color];
};


// piece

enum : piece_t {
  white_pawn =   0b00101,
  white_knight = 0b01001,
  white_bishop = 0b01101,
  white_rook =   0b10001,
  white_queen =  0b10101,
  white_king =   0b11001,

  black_pawn =   0b00111,
  black_knight = 0b01011,
  black_bishop = 0b01111,
  black_rook =   0b10011,
  black_queen =  0b10111,
  black_king =   0b11011,
  
  pawn =         0b00100,
  knight =       0b01000,
  bishop =       0b01100,
  rook =         0b10000,
  queen =        0b10100,
  king =         0b11000,
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

constexpr std::array<piece_t, 6> all_pieces_by_color[4] = {
  {
    pawn, knight, bishop, rook, queen, king,
  },
  {
    white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king,
  },
  {
    none, none, none, none, none, none,
  },
  {
    black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
  },
};

piece_t piece_from_char(char c) {
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

std::string piece_to_string(piece_t piece) {
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


// moves

/* some move definitions and macros

  MOVE

  0000 0000 0000 0000 0000 0000 0011 1111     from
  0000 0000 0000 0000 0000 1111 1100 0000     to
  0000 0000 0000 0001 1111 0000 0000 0000     target_piece
  0000 0000 0000 0010 0000 0000 0000 0000     double_pawn_push
  0000 0000 0000 0100 0000 0000 0000 0000     enpassant
  0000 0000 1111 1000 0000 0000 0000 0000     moved_piece
  0001 1111 0000 0000 0000 0000 0000 0000     captured_piece
  0010 0000 0000 0000 0000 0000 0000 0000     promotion
  0100 0000 0000 0100 0000 0000 0000 0000     castling
  1000 0000 0000 0000 0000 0000 0000 0000     check

*/

#define from(move) (((move) >> 0) & 0b111111)
#define to(move) (((move) >> 6) & 0b111111)
#define target_piece(move) (((move) >> 12) & 0b11111)
#define double_pawn_push(move) (((move) >> 17) & 0b1)
#define enpassant(move) (((move) >> 18) & 0b1)
#define moved_piece(move) (((move) >> 19) & 0b11111)
#define captured_piece(move) (((move) >> 24) & 0b11111)
#define promotion(move) (((move) >> 29) & 0b1)
#define castling(move) (((move) >> 30) & 0b1)
#define check(move) (((move) >> 31) & 0b1)
#define capture(move) (captured_piece(move) != none)

move_t move(square_t from,
            square_t to,
            piece_t moved_piece,
            piece_t target_piece,
            piece_t captured_piece,
            bool double_pawn_push,
            bool enpassant,
            bool castling,
            bool promotion,
            bool check) {
  return (from << 0) |
         (to << 6) |
         (target_piece << 12) |
         (double_pawn_push << 17) |
         (enpassant << 18) |
         (moved_piece << 19) |
         (captured_piece << 24) |
         (promotion << 29) |
         (castling << 30) |
         (check << 31);
};

std::string move_to_string(move_t move) {
  if (move == none) return "0000";
  return (
    square_to_string(from(move)) +
    square_to_string(to(move)) +
    (promotion(move) ? piece_to_string(to_color(target_piece(move), black)) : "")
  );
};

castling_t removed_castling(move_t move) {
  return (
    (from(move) == e1 || from(move) == h1 || to(move) == h1) << 0 |
    (from(move) == e1 || from(move) == a1 || to(move) == a1) << 1 |
    (from(move) == e8 || from(move) == h8 || to(move) == h8) << 2 |
    (from(move) == e8 || from(move) == a8 || to(move) == a8) << 3
  );
};

move_t mvv_lva_key(move_t move) {
  constexpr move_t move_piece_mask = 0b11111 << 19;
  if (capture(move)) {
    return move ^ move_piece_mask;
  };
  return move;
};

struct {
  bool operator()(move_t move1, move_t move2) const {
    return mvv_lva_key(move1) > mvv_lva_key(move2);
  };
} comparison;

struct {
  bool operator()(move_t move1, move_t move2) const {
    return mvv_lva_key(move1) < mvv_lva_key(move2);
  };
} reverse_comparison;

template<piece_t piece>
constexpr move_t piece_move(square_t from, square_t to, piece_t captured_piece, bool check) {
  constexpr move_t base = (piece << 12) | (piece << 19);
  return base | (from << 0) | (to << 6) | (captured_piece << 24) | (check << 31);
};

template<piece_t piece, bool check>
constexpr move_t piece_move(square_t from, square_t to, piece_t captured_piece) {
  constexpr move_t base = (piece << 12) | (piece << 19) | (check << 31);
  return base | (from << 0) | (to << 6) | (captured_piece << 24);
};

template<color_t color, bool king_side>
constexpr move_t castling_move(bool check) {
  constexpr move_t white_king_side_base = (e1 << 0) | (g1 << 6) | (white_king << 12) | (white_king << 19) | (1 << 30);
  constexpr move_t white_queen_side_base = (e1 << 0) | (c1 << 6) | (white_king << 12) | (white_king << 19) | (1 << 30);
  constexpr move_t black_king_side_base = (e8 << 0) | (g8 << 6) | (black_king << 12) | (black_king << 19) | (1 << 30);
  constexpr move_t black_queen_side_base = (e8 << 0) | (c8 << 6) | (black_king << 12) | (black_king << 19) | (1 << 30);
  if constexpr (color == white) {
    if constexpr (king_side) {
      return white_king_side_base | (check << 31);
    } else {
      return white_queen_side_base | (check << 31);
    };
  } else {
    if constexpr (king_side) {
      return black_king_side_base | (check << 31);
    } else {
      return black_queen_side_base | (check << 31);
    };
  };
};

template<piece_t piece>
constexpr move_t capture_promotion_move(square_t from, square_t to, piece_t captured_piece, bool check) {
  constexpr color_t color = color(piece);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr move_t base = (piece << 12) | (color_pawn << 19) | (1 << 29);
  return base | (from << 0) | (to << 6) | (captured_piece << 24) | (check << 31);
};

template<piece_t piece, bool check>
constexpr move_t capture_promotion_move(square_t from, square_t to, piece_t captured_piece) {
  constexpr color_t color = color(piece);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr move_t base = (piece << 12) | (color_pawn << 19) | (1 << 29) | (check << 31);
  return base | (from << 0) | (to << 6) | (captured_piece << 24);
};

template<piece_t piece>
constexpr move_t push_promotion_move(square_t from, square_t to, bool check) {
  constexpr color_t color = color(piece);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr move_t base = (piece << 12) | (color_pawn << 19) | (none << 24) | (1 << 29);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color, bool double_pawn_push>
constexpr move_t push_move(square_t from, square_t to, bool check) {
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr move_t base = (color_pawn << 12) | (color_pawn << 19) | (none << 24) | (double_pawn_push << 17);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color>
constexpr move_t enpassant_move(square_t from, square_t to, bool check) {
  constexpr color_t opponent = opponent(color);
  constexpr piece_t color_pawn = to_color(pawn, color);
  constexpr piece_t opponent_pawn = to_color(pawn, opponent);
  constexpr move_t base = (color_pawn << 12) | (color_pawn << 19) | (opponent_pawn << 24) | (1 << 18);
  return base | (from << 0) | (to << 6) | (check << 31);
};


// movetype

enum : movetype_t {
  quiet =     0b001,
  check =     0b010,
  capture =   0b100,
  non_quiet = 0b110,
  legal =     0b111,
};