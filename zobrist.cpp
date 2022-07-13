#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __ZOBRIST__MODULE__
#define __ZOBRIST__MODULE__


#include <array>

#include "constants.cpp"
#include "random.cpp"


/*

  a class for the zobrist hash

*/
class Zobrist {
  private:
    std::array<std::array<hash_t, 64>, 24> piece{0ULL};
    std::array<hash_t, 16> castling{0ULL};
    std::array<hash_t, 65> enpassant{0ULL};
    hash_t turn = 0ULL;

  public:
    hash_t hash;

    // initialize the tables with random values
    Zobrist() {
      for (square_t square = 0; square < 64; square++) {
        this->piece[piece::white_pawn][square] = randomness::generate<hash_t>();
        this->piece[piece::white_knight][square] = randomness::generate<hash_t>();
        this->piece[piece::white_bishop][square] = randomness::generate<hash_t>();
        this->piece[piece::white_rook][square] = randomness::generate<hash_t>();
        this->piece[piece::white_queen][square] = randomness::generate<hash_t>();
        this->piece[piece::white_king][square] = randomness::generate<hash_t>();
        this->piece[piece::black_pawn][square] = randomness::generate<hash_t>();
        this->piece[piece::black_knight][square] = randomness::generate<hash_t>();
        this->piece[piece::black_bishop][square] = randomness::generate<hash_t>();
        this->piece[piece::black_rook][square] = randomness::generate<hash_t>();
        this->piece[piece::black_queen][square] = randomness::generate<hash_t>();
        this->piece[piece::black_king][square] = randomness::generate<hash_t>();
      };
      for (castling_t castling = 0; castling < 16; castling++) {
        this->castling[castling] = randomness::generate<hash_t>();
      };
      for (square_t square = 0; square < 65; square++) {
        this->enpassant[square] = randomness::generate<hash_t>();
      };
      this->turn = randomness::generate<hash_t>();
    };

    // update the hash with a piece change
    void update_piece(piece_t piece, square_t square) {
      this->hash ^= this->piece[piece][square];
    };

    // update the hash with a castling change
    void update_castling(castling_t castling) {
      this->hash ^= this->castling[castling];
    };

    // update the hash with a enpassant change
    void update_enpassant(square_t square) {
      this->hash ^= this->enpassant[square];
    };

    // update the hash with a turn change
    void update_turn() {
      this->hash ^= this->turn;
    };

    // clear the hash
    void clear() {
      this->hash = 0ULL;
    };

    // update hash with value
    void update(hash_t value) {
      this->hash = value;
    };
};



#endif // __ZOBRIST__MODULE__