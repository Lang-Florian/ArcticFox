#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __PAWN_CAPTURE__MODULE__
#define __PAWN_CAPTURE__MODULE__


#include <type_traits>
#include "../base.cpp"
#include "../attack.cpp"
#include "../board.cpp"
#include "detail.cpp"


// generate pawn capture moves
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_capture_moves(T& moves, board::Board& board, detail_t& detail) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);

  constexpr bitboard_t pre_promotion_rank = (color == color::white) ? bitboard::rank_7 : bitboard::rank_2;

  bitboard_t free_pawns = board.bitboards[pawn] & ~detail.bishop_pinned & ~detail.rook_pinned;
  bitboard_t bishop_pinned_pawns = board.bitboards[pawn] & detail.bishop_pinned & ~detail.rook_pinned;
  bitboard_t pawn_checking_squares = attack::attack<opponent_pawn>(detail.opponent_king_square);
  bitboard_t knight_checking_squares = attack::attack<knight>(detail.opponent_king_square);
  bitboard_t bishop_checking_squares_no_pawns = attack::attack<bishop>(detail.opponent_king_square, board.bitboards[piece::none] & ~board.bitboards[pawn]);
  bitboard_t queen_checking_squares_no_pawns = (bishop_checking_squares_no_pawns | detail.rook_checking_squares);

  bitboard_t non_discoverable_targets = bitboard::none;
  bitboard_t non_discoverable_knight_promoting_targets = bitboard::none;
  bitboard_t non_discoverable_bishop_promoting_targets = bitboard::none;
  bitboard_t non_discoverable_rook_promoting_targets = bitboard::none;
  bitboard_t non_discoverable_queen_promoting_targets = bitboard::none;
  bitboard_t discoverable_targets = bitboard::none;
  if constexpr (movetype & movetype::check) {
    non_discoverable_targets |= pawn_checking_squares;
    non_discoverable_knight_promoting_targets |= knight_checking_squares;
    non_discoverable_bishop_promoting_targets |= bishop_checking_squares_no_pawns;
    non_discoverable_rook_promoting_targets |= detail.rook_checking_squares;
    non_discoverable_queen_promoting_targets |= queen_checking_squares_no_pawns;
    discoverable_targets |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_targets |= bitboard::full;
    non_discoverable_knight_promoting_targets |= bitboard::full;
    non_discoverable_bishop_promoting_targets |= bitboard::full;
    non_discoverable_rook_promoting_targets |= bitboard::full;
    non_discoverable_queen_promoting_targets |= bitboard::full;
    discoverable_targets |= bitboard::full;
  };
  non_discoverable_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_knight_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_bishop_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_rook_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  non_discoverable_queen_promoting_targets &= detail.evasion_targets & board.bitboards[opponent];
  discoverable_targets &= detail.evasion_targets & board.bitboards[opponent];
  
  bitboard_t discoverable_promoting_free_pawns = free_pawns & pre_promotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::capture_promotion_move<knight, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<bishop, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<rook, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<queen, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };

  bitboard_t discoverable_not_promoting_free_pawns = free_pawns & ~pre_promotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<pawn, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & pre_promotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::capture_promotion_move<knight, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<bishop, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<rook, true>(from, to, board.pieces[to]));
        moves.push(move::compiletime::capture_promotion_move<queen, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };

  bitboard_t discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~pre_promotion_rank & (detail.bishop_discoverable | detail.rook_discoverable);
  while (discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<pawn, true>(from, to, board.pieces[to]));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_promoting_free_pawns = free_pawns & pre_promotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_free_pawns);

    bitboard_t bishop_checking_squares = attack::attack<bishop>(detail.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t queen_checking_squares = attack::attack<queen>(detail.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));

    bitboard_t possible_to_knight = attack::attack<pawn>(from) & non_discoverable_knight_promoting_targets;
    bitboard_t possible_to_bishop = attack::attack<pawn>(from) & non_discoverable_bishop_promoting_targets;
    bitboard_t possible_to_rook = attack::attack<pawn>(from) & non_discoverable_rook_promoting_targets;
    bitboard_t possible_to_queen = attack::attack<pawn>(from) & non_discoverable_queen_promoting_targets;
    if constexpr ((movetype & movetype::check) && !(movetype & movetype::capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move::compiletime::capture_promotion_move<knight>(from, to, board.pieces[to], bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move::compiletime::capture_promotion_move<bishop>(from, to, board.pieces[to], bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move::compiletime::capture_promotion_move<rook>(from, to, board.pieces[to], bitboard(to) & detail.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move::compiletime::capture_promotion_move<queen>(from, to, board.pieces[to], bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };

  bitboard_t non_discoverable_not_promoting_free_pawns = free_pawns & ~pre_promotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<pawn>(from, to, board.pieces[to], bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };

  bitboard_t non_discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & pre_promotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_bishop_pinned_pawns);

    bitboard_t bishop_checking_squares = attack::attack<bishop>(detail.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t queen_checking_squares = attack::attack<queen>(detail.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));

    bitboard_t possible_to_knight = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & non_discoverable_knight_promoting_targets;
    bitboard_t possible_to_bishop = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & non_discoverable_bishop_promoting_targets;
    bitboard_t possible_to_rook = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & non_discoverable_rook_promoting_targets;
    bitboard_t possible_to_queen = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & non_discoverable_queen_promoting_targets;
    if constexpr ((movetype & movetype::check) && !(movetype & movetype::capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move::compiletime::capture_promotion_move<knight>(from, to, board.pieces[to], bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move::compiletime::capture_promotion_move<bishop>(from, to, board.pieces[to], bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move::compiletime::capture_promotion_move<rook>(from, to, board.pieces[to], bitboard(to) & detail.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move::compiletime::capture_promotion_move<queen>(from, to, board.pieces[to], bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };

  bitboard_t non_discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~pre_promotion_rank & ~(detail.bishop_discoverable | detail.rook_discoverable);
  while (non_discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & attack::ray::bishop[detail.king_square] & non_discoverable_targets;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::compiletime::piece_move<pawn>(from, to, board.pieces[to], bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};


#endif