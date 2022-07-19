#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __ZOBRIST__MODULE__
#define __ZOBRIST__MODULE__


#include <array>

#include "constants.cpp"
#include "random.cpp"
#include "types.cpp"


/*

  a class for the zobrist hash

*/


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
      for (auto square : square::all_squares) {
        for (auto piece : piece::all_pieces) {
          this->piece_hash[piece][square] = randomness::generate<hash_t>();
        };
      };
      for (castling_t castling = 0; castling < 16; castling++) {
        this->castling_hash[castling] = randomness::generate<hash_t>();
      };
      for (auto square : square::all_squares) {
        this->enpassant_hash[square] = randomness::generate<hash_t>();
      };
      this->turn_hash = randomness::generate<hash_t>();
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


#endif // __ZOBRIST__MODULE__