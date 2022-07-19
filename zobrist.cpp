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
    std::array<std::array<hash_t, 64>, 32> piece_validation{0ULL};
    std::array<hash_t, 16> castling_hash{0ULL};
    std::array<hash_t, 16> castling_validation{0ULL};
    std::array<hash_t, 65> enpassant_hash{0ULL};
    std::array<hash_t, 65> enpassant_validation{0ULL};
    hash_t turn_hash = 0ULL;
    hash_t turn_validation = 0ULL;

  public:
    hash_t hash;
    hash_t validation;

    // initialize the tables with random values
    Zobrist() {
      for (auto square : square::all_squares) {
        for (auto piece : piece::all_pieces) {
          this->piece_hash[piece][square] = randomness::generate<hash_t>();
          this->piece_validation[piece][square] = randomness::generate<hash_t>();
        };
      };
      for (castling_t castling = 0; castling < 16; castling++) {
        this->castling_hash[castling] = randomness::generate<hash_t>();
        this->castling_validation[castling] = randomness::generate<hash_t>();
      };
      for (auto square : square::all_squares) {
        this->enpassant_hash[square] = randomness::generate<hash_t>();
        this->enpassant_validation[square] = randomness::generate<hash_t>();
      };
      this->turn_hash = randomness::generate<hash_t>();
      this->turn_validation = randomness::generate<hash_t>();
    };

    // update the hash with a piece change
    void update_piece(piece_t piece, square_t square) {
      this->hash ^= this->piece_hash[piece][square];
      this->validation ^= this->piece_validation[piece][square];
    };

    // update the hash with a castling change
    void update_castling(castling_t castling) {
      this->hash ^= this->castling_hash[castling];
      this->validation ^= this->castling_validation[castling];
    };

    // update the hash with a enpassant change
    void update_enpassant(square_t square) {
      this->hash ^= this->enpassant_hash[square];
      this->validation ^= this->enpassant_validation[square];
    };

    // update the hash with a turn change
    void update_turn() {
      this->hash ^= this->turn_hash;
      this->validation ^= this->turn_validation;
    };

    // clear the hash
    void clear() {
      this->hash = 0ULL;
      this->validation = 0ULL;
    };

    // set hash to a given value
    void set(hash_t hash, hash_t validation) {
      this->hash = hash;
      this->validation = validation;
    };
};


#endif // __ZOBRIST__MODULE__