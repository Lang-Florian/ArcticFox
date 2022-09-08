#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __ATTACKS__MODULE__
#define __ATTACKS__MODULE__


#include <array>

#include "constants.cpp"
#include "macros.cpp"
#include "magic.cpp"
#include "types.cpp"


/*

  generate attack tables for pawns, knights and kings at compile time

*/


namespace attack_tables {
  constexpr std::array<std::array<bitboard_t, 2>, 64> pawn = {[]() constexpr {
    std::array<std::array<bitboard_t, 2>, 64> pawn{bitboard::none};
    for (auto square : square::all_squares) {
      pawn[square][color::white] = ((bitboard(square) >> 9) & ~bitboard::file_h) | ((bitboard(square) >> 7) & ~bitboard::file_a);
      pawn[square][color::black] = ((bitboard(square) << 9) & ~bitboard::file_a) | ((bitboard(square) << 7) & ~bitboard::file_h);
    };
    return pawn;
  }()};

  constexpr std::array<bitboard_t, 64> knight= {[]() constexpr {
    std::array<bitboard_t, 64> knight{bitboard::none};
    for (auto square : square::all_squares) {
      knight[square] = ((bitboard(square) >>  6) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_1) |
                       ((bitboard(square) <<  6) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_8) |
                       ((bitboard(square) >> 10) & ~bitboard::file_g & ~bitboard::file_h & ~bitboard::rank_1) |
                       ((bitboard(square) << 10) & ~bitboard::file_a & ~bitboard::file_b & ~bitboard::rank_8) |
                       ((bitboard(square) >> 15) & ~bitboard::file_a & ~bitboard::rank_1 & ~bitboard::rank_2) |
                       ((bitboard(square) << 15) & ~bitboard::file_h & ~bitboard::rank_7 & ~bitboard::rank_8) |
                       ((bitboard(square) >> 17) & ~bitboard::file_h & ~bitboard::rank_1 & ~bitboard::rank_2) |
                       ((bitboard(square) << 17) & ~bitboard::file_a & ~bitboard::rank_7 & ~bitboard::rank_8);
    };
    return knight;
  }()};

  constexpr std::array<bitboard_t, 64> king = {[]() constexpr {
    std::array<bitboard_t, 64> king{bitboard::none};
    for (auto square : square::all_squares) {
      king[square] = ((bitboard(square) >> 1) & ~bitboard::file_h) |
                     ((bitboard(square) << 1) & ~bitboard::file_a) |
                     ((bitboard(square) >> 8) & ~bitboard::rank_1) |
                     ((bitboard(square) << 8) & ~bitboard::rank_8) |
                     ((bitboard(square) >> 7) & ~bitboard::file_a & ~bitboard::rank_1) |
                     ((bitboard(square) << 7) & ~bitboard::file_h & ~bitboard::rank_8) |
                     ((bitboard(square) >> 9) & ~bitboard::file_h & ~bitboard::rank_1) |
                     ((bitboard(square) << 9) & ~bitboard::file_a & ~bitboard::rank_8);
    };
    return king;
  }()};
};


/*

  functions to get the attack bitboard for a given piece and square

*/

template <piece_t piece>
bitboard_t attack(square_t square, bitboard_t occupancy=bitboard::none) {
  if constexpr (piece == piece::white_pawn) {
    return attack_tables::pawn[square][color::white];
  } else if constexpr (piece == piece::black_pawn) {
    return attack_tables::pawn[square][color::black];
  } else if constexpr (piece == piece::knight || piece == piece::white_knight || piece == piece::black_knight) {
    return attack_tables::knight[square];
  } else if constexpr (piece == piece::bishop || piece == piece::white_bishop || piece == piece::black_bishop) {
    return magic::table[
      magic::bishop[square].offset +
      (((occupancy | magic::bishop[square].mask) * magic::bishop[square].magic_number) >> 55)
    ];
  } else if constexpr (piece == piece::rook || piece == piece::white_rook || piece == piece::black_rook) {
    return magic::table[
      magic::rook[square].offset +
      (((occupancy | magic::rook[square].mask) * magic::rook[square].magic_number) >> 52)
    ];
  } else if constexpr (piece == piece::queen || piece == piece::white_queen || piece == piece::black_queen) {
    return (
      magic::table[
        magic::bishop[square].offset +
        (((occupancy | magic::bishop[square].mask) * magic::bishop[square].magic_number) >> 55)
      ] |
      magic::table[
        magic::rook[square].offset +
        (((occupancy | magic::rook[square].mask) * magic::rook[square].magic_number) >> 52)
      ]
    );
  } else if constexpr (piece == piece::king || piece == piece::white_king || piece == piece::black_king) {
    return attack_tables::king[square];
  } else {
    return bitboard::none;
  };
};


constexpr bitboard_t (*(attack_array[32]))(square_t, bitboard_t) = {
   attack<0>,  attack<1>,  attack<2>,  attack<3>,  attack<4>,  attack<5>,  attack<6>,  attack<7>,
   attack<8>,  attack<9>, attack<10>, attack<11>, attack<12>, attack<13>, attack<14>, attack<15>,
  attack<16>, attack<17>, attack<18>, attack<19>, attack<20>, attack<21>, attack<22>, attack<23>,
  attack<24>, attack<25>, attack<26>, attack<27>, attack<28>, attack<29>, attack<30>, attack<31>
};


#endif // __ATTACKS__MODULE__