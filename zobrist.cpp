#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "constants.cpp"
#include "random.cpp"

#include "array"


#ifndef __ZOBRIST__MODULE__
#define __ZOBRIST__MODULE__


namespace ZOBRIST {
  static constexpr U32 random(U32 hash) {
    hash ^= hash << 13;
    hash ^= hash >> 17;
    hash ^= hash << 5;
    return hash;
  };

  static constexpr std::array<std::array<hash_t, 65>, 32> generate_piece_hash_table() {
    std::array<std::array<hash_t, 65>, 32> piece_hash_table{0};
    U32 hash = 1804289383;
    for (square_t square = 0; square < 65; square++) {
      hash = random(hash);
      piece_hash_table[PIECE::white_pawn][square] =   (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::white_knight][square] = (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::white_bishop][square] = (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::white_rook][square] =   (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::white_queen][square] =  (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::white_king][square] =   (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_pawn][square] =   (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_knight][square] = (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_bishop][square] = (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_rook][square] =   (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_queen][square] =  (hash_t)hash;
      hash = random(hash);
      piece_hash_table[PIECE::black_king][square] =   (hash_t)hash;
    };
    return piece_hash_table;
  };

  static constexpr std::array<std::array<hash_t, 65>, 32> piece_hash_table = generate_piece_hash_table();
  
  static constexpr std::array<hash_t, 16> generate_castling_hash_table() {
    std::array<hash_t, 16> castling_hash_table{0};
    U32 hash = 3267813459;
    for (castling_t castling = 0; castling < 16; castling++) {
      hash = random(hash);
      castling_hash_table[castling] = (hash_t)hash;
    };
    return castling_hash_table;
  };

  static constexpr std::array<hash_t, 16> castling_hash_table = generate_castling_hash_table();

  static constexpr std::array<hash_t, 65> generate_enpassant_hash_table() {
    std::array<hash_t, 65> enpassant_hash_table{0};
    U32 hash = 2907283415;
    for (square_t square = 0; square < 65; square++) {
      hash = random(hash);
      enpassant_hash_table[square] = (hash_t)hash;
    };
    return enpassant_hash_table;
  };

  static constexpr std::array<hash_t, 65> enpassant_hash_table = generate_enpassant_hash_table();

  static constexpr hash_t turn_hash = (hash_t)random(1289203691);

  inline hash_t hash(hash_t hash,
                     piece_t piece=PIECE::none,
                     square_t square=SQUARE::none,
                     castling_t castling=CASTLING::none,
                     square_t enpassant_square=SQUARE::none,
                     bool turn=false) {
    return hash ^
           piece_hash_table[piece][square] ^
           castling_hash_table[castling] ^
           enpassant_hash_table[enpassant_square] ^
           (turn_hash * turn);
  };

  hash_t none = 0;
};


#endif