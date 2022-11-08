#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __QUEEN__MODULE__
#define __QUEEN__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate queen moves
template <color_t color, movetype_t movetype, typename T>
void generate_queen_moves(T& moves, Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);

  bitboard_t checking_squares = detail.bishop_checking_squares | detail.rook_checking_squares;

  bitboard_t targets = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    targets |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check)
    targets |= checking_squares;
  if constexpr (movetype & movetype::capture)
    targets |= board.bitboards[opponent];
  targets &= detail.evasion_targets & ~board.bitboards[color];

  bitboard_t bishop_pinned_queens = board.bitboards[queen] & detail.bishop_pinned & ~detail.rook_pinned;
  while (bishop_pinned_queens) {
    square_t from = pop_lsb(bishop_pinned_queens);
    bitboard_t possible_to = attack<bishop>(from, board.bitboards[color::none]) & targets & bishop_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<queen>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t rook_pinned_queens = board.bitboards[queen] & ~detail.bishop_pinned & detail.rook_pinned;
  while (rook_pinned_queens) {
    square_t from = pop_lsb(rook_pinned_queens);
    bitboard_t possible_to = attack<rook>(from, board.bitboards[color::none]) & targets & rook_ray[detail.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<queen>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t free_queens = board.bitboards[queen] & ~detail.bishop_pinned & ~detail.rook_pinned;
  while (free_queens) {
    square_t from = pop_lsb(free_queens);
    bitboard_t possible_to = attack<queen>(from, board.bitboards[color::none]) & targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<queen>(from, to, board.pieces[to], bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};


#endif