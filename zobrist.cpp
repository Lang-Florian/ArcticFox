#pragma once

#include <array>
#include "modules/random.cpp"
#include "base.cpp"

class Zobrist {
private:
  std::array<std::array<hash_t, 64>, 32> piece_hash{0ULL};
  std::array<hash_t, 16> castling_hash{0ULL};
  std::array<hash_t, 65> enpassant_hash{0ULL};
  hash_t turn_hash = 0ULL;

public:
  hash_t hash;

  // initialize the tables with random values
  Zobrist() {
    for (square_t square = 0; square < none_square; square++) {
      for (auto piece : all_pieces) {
        this->piece_hash[piece][square] = random<hash_t>();
      };
    };
    for (castling_t castling = 0; castling < 16; castling++) {
      this->castling_hash[castling] = random<hash_t>();
    };
    for (square_t square = 0; square < none_square; square++) {
      this->enpassant_hash[square] = random<hash_t>();
    };
    this->turn_hash = random<hash_t>();
  };

  // update the hash with a piece change
  void update_piece(piece_t piece, square_t square) {
    this->hash ^= this->piece_hash[piece][square];
  };

  // update the hash with a castling change
  void update_castling(castling_t castling) {
    this->hash ^= this->castling_hash[castling];
  };

  // update the hash with a enpassant change
  void update_enpassant(square_t square) {
    this->hash ^= this->enpassant_hash[square];
  };

  // update the hash with a turn change
  void update_turn() {
    this->hash ^= this->turn_hash;
  };

  // clear the hash
  void clear() {
    this->hash = 0ULL;
  };

  // set hash to a given value
  void set(hash_t hash) {
    this->hash = hash;
  };
};