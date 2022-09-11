#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __KING__MODULE__
#define __KING__MODULE__


#include <type_traits>
#include "../base/base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate king moves
template <color_t color, movetype_t movetype, typename T>
void generate_king_moves(T& moves, board::Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t king = piece::compiletime::to_color(piece::king, color);

  bitboard_t checking_squares = bitboard::none;

  if (board.bitboards[king] & detail.bishop_discoverable)
    checking_squares |= ~attack::ray::bishop[detail.opponent_king_square];
  if (board.bitboards[king] & detail.rook_discoverable)
    checking_squares |= ~attack::ray::rook[detail.opponent_king_square];
  
  bitboard_t targets = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check)
    targets |= checking_squares;
  if constexpr (movetype & movetype::capture)
    targets |= board.bitboards[opponent];
  targets &= ~board.bitboards[color] & ~detail.unsafe_king_squares;
  
  bitboard_t possible_to = attack::attack<piece::king>(detail.king_square) & targets;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (possible_to) {
      square_t to = pop_lsb(possible_to);
      moves.push(move::compiletime::piece_move<king>(detail.king_square, to, board.pieces[to], bitboard(to) & checking_squares));
    };
  } else {
    moves += popcount(possible_to);
  };
};


#endif