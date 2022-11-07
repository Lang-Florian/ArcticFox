#pragma once
#include <array>
#include <string>

// constants

#define ENGINE_NAME "ArcticFox"
#define VERSION "0.2"
#define AUTHOR "N3U1R0N"


#define MAX_GAME_LENGTH 512
#define MAX_MOVE_GENERATION_SIZE 512
#define MAX_PV_DEPTH 32
#define MAX_QSEARCH_DEPTH 32
#define TRANSPOSITION_TABLE_SIZE_LOG2 30
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


// macros

#define bitboard(square) (1ULL << (square))
#define popcount(bitboard) __builtin_popcountll(bitboard)
#define get_lsb(bitboard) __builtin_ctzll(bitboard)
#define pop_lsb(bitboard) __builtin_ctzll(bitboard); bitboard &= bitboard - 1
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define clear_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) & (1ULL << (square))); (bitboard) &= ~(1ULL << (square))


// squares

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

constexpr square_t all[64] = {
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1,
};

std::string to_string(square_t square) {
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
};


// bitboard

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
const bitboard_t files[8] = {file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h};

const bitboard_t rank_1 = 0xFF00000000000000ULL;
const bitboard_t rank_2 = 0x00FF000000000000ULL;
const bitboard_t rank_3 = 0x0000FF0000000000ULL;
const bitboard_t rank_4 = 0x000000FF00000000ULL;
const bitboard_t rank_5 = 0x00000000FF000000ULL;
const bitboard_t rank_6 = 0x0000000000FF0000ULL;
const bitboard_t rank_7 = 0x000000000000FF00ULL;
const bitboard_t rank_8 = 0x00000000000000FFULL;
const bitboard_t ranks[8] = {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};

const bitboard_t white_side = rank_1 | rank_2 | rank_3 | rank_4;
const bitboard_t black_side = rank_5 | rank_6 | rank_7 | rank_8;
const bitboard_t queen_side = file_a | file_b | file_c | file_d;
const bitboard_t king_side =  file_e | file_f | file_g | file_h;
const bitboard_t center = d4 | e4 | d5 | e5;

const bitboard_t white = a8 |  0 | c8 |  0 | e8 |  0 | g8 |  0 | \
                          0 | b7 |  0 | d7 |  0 | f7 |  0 | h7 | \
                         a6 |  0 | c6 |  0 | e6 |  0 | g6 |  0 | \
                          0 | b5 |  0 | d5 |  0 | f5 |  0 | h5 | \
                         a4 |  0 | c4 |  0 | e4 |  0 | g4 |  0 | \
                          0 | b3 |  0 | d3 |  0 | f3 |  0 | h3 | \
                         a2 |  0 | c2 |  0 | e2 |  0 | g2 |  0 | \
                          0 | b1 |  0 | d1 |  0 | f1 |  0 | h1;
const bitboard_t black =  0 | b8 |  0 | d8 |  0 | f8 |  0 | h8 | \
                         a7 |  0 | c7 |  0 | e7 |  0 | g7 |  0 | \
                          0 | b6 |  0 | d6 |  0 | f6 |  0 | h6 | \
                         a5 |  0 | c5 |  0 | e5 |  0 | g5 |  0 | \
                          0 | b4 |  0 | d4 |  0 | f4 |  0 | h4 | \
                         a3 |  0 | c3 |  0 | e3 |  0 | g3 |  0 | \
                          0 | b2 |  0 | d2 |  0 | f2 |  0 | h2 | \
                         a1 |  0 | c1 |  0 | e1 |  0 | g1 |  0;
};


// castling

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


// color

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
  const std::string labels[] = {"w", "b", ""};
  return labels[color];
};
};


namespace color::compiletime {
constexpr color_t opponent(color_t color) {
  return color ^ 0b1;
};
};


// piece

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


namespace move {
move_t null = (move_t)0;

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

square_t from(move_t move) {
  return (move >> 0) & 0b111111;
};

square_t to(move_t move) {
  return (move >> 6) & 0b111111;
};

piece_t target_piece(move_t move) {
  return (move >> 12) & 0b11111;
};

bool double_pawn_push(move_t move) {
  return (move >> 17) & 0b1;
};

bool enpassant(move_t move) {
  return (move >> 18) & 0b1;
};

piece_t moved_piece(move_t move) {
  return (move >> 19) & 0b11111;
};

piece_t captured_piece(move_t move) {
  return (move >> 24) & 0b11111;
};

bool promotion(move_t move) {
  return (move >> 29) & 0b1;
};

bool castling(move_t move) {
  return (move >> 30) & 0b1;
};

bool check(move_t move) {
  return (move >> 31) & 0b1;
};

bool capture(move_t move) {
  return captured_piece(move) != piece::none;
};

std::string to_string(move_t move) {
  if (move == null) return "0000";
  return (
    square::to_string(from(move)) +
    square::to_string(to(move)) +
    (promotion(move) ? piece::promotion_string(target_piece(move)) : "")
  );
};

castling_t removed_castling(move_t move) {
  return (
    (from(move) == square::e1 || from(move) == square::h1 || to(move) == square::h1) << 0 |
    (from(move) == square::e1 || from(move) == square::a1 || to(move) == square::a1) << 1 |
    (from(move) == square::e8 || from(move) == square::h8 || to(move) == square::h8) << 2 |
    (from(move) == square::e8 || from(move) == square::a8 || to(move) == square::a8) << 3
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
    return move::mvv_lva_key(move1) > move::mvv_lva_key(move2);
  };
} comparison;

struct {
  bool operator()(move_t move1, move_t move2) const {
    return move::mvv_lva_key(move1) < move::mvv_lva_key(move2);
  };
} reverse_comparison;
};


namespace move::compiletime {
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
  constexpr move_t white_king_side_base = (square::e1 << 0) | (square::g1 << 6) | (piece::white_king << 12) | (piece::white_king << 19) | (1 << 30);
  constexpr move_t white_queen_side_base = (square::e1 << 0) | (square::c1 << 6) | (piece::white_king << 12) | (piece::white_king << 19) | (1 << 30);
  constexpr move_t black_king_side_base = (square::e8 << 0) | (square::g8 << 6) | (piece::black_king << 12) | (piece::black_king << 19) | (1 << 30);
  constexpr move_t black_queen_side_base = (square::e8 << 0) | (square::c8 << 6) | (piece::black_king << 12) | (piece::black_king << 19) | (1 << 30);
  if constexpr (color == color::white) {
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
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (1 << 29);
  return base | (from << 0) | (to << 6) | (captured_piece << 24) | (check << 31);
};

template<piece_t piece, bool check>
constexpr move_t capture_promotion_move(square_t from, square_t to, piece_t captured_piece) {
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (1 << 29) | (check << 31);
  return base | (from << 0) | (to << 6) | (captured_piece << 24);
};

template<piece_t piece>
constexpr move_t push_promotion_move(square_t from, square_t to, bool check) {
  constexpr color_t color = piece::compiletime::color(piece);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (piece << 12) | (pawn << 19) | (piece::none << 24) | (1 << 29);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color, bool double_pawn_push>
constexpr move_t push_move(square_t from, square_t to, bool check) {
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr move_t base = (pawn << 12) | (pawn << 19) | (piece::none << 24) | (double_pawn_push << 17);
  return base | (from << 0) | (to << 6) | (check << 31);
};

template<color_t color>
constexpr move_t enpassant_move(square_t from, square_t to, bool check) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr move_t base = (pawn << 12) | (pawn << 19) | (opponent_pawn << 24) | (1 << 18);
  return base | (from << 0) | (to << 6) | (check << 31);
};
};


// movetype

namespace movetype {
enum : movetype_t {
  quiet =     0b001,
  check =     0b010,
  capture =   0b100,
  non_quiet = 0b110,
  legal =     0b111,
};
};