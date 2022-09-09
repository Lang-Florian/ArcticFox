#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __MOVEGEN__MODULE__
#define __MOVEGEN__MODULE__


#include <string>
#include <type_traits>
#include "base/base.cpp"
#include "attack.cpp"
#include "board.cpp"


/*

  Module for move generation.

*/


namespace movegen {
struct info_t {
  square_t king_square;
  square_t opponent_king_square;
  bitboard_t checkers;
  square_t checker_square;
  bitboard_t bishop_checking_squares;
  bitboard_t rook_checking_squares;
  bitboard_t rook_checking_squares_castling;
  bitboard_t evasion_target;
  bitboard_t attacked_squares;
  bitboard_t unsafe_king_squares;
  bitboard_t bishop_pinned;
  bitboard_t rook_pinned;
  bitboard_t enpassant_pinned;
  bitboard_t bishop_discoverable;
  bitboard_t rook_discoverable;

  template<color_t color>
  void update(board::Board& board) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
    constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
    constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
    constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
    constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, opponent);
    constexpr piece_t opponent_queen = piece::compiletime::to_color(piece::queen, opponent);
    constexpr piece_t opponent_rook = piece::compiletime::to_color(piece::rook, opponent);
    constexpr piece_t opponent_bishop = piece::compiletime::to_color(piece::bishop, opponent);
    constexpr piece_t opponent_knight = piece::compiletime::to_color(piece::knight, opponent);
    constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
    this->king_square = get_lsb(board.bitboards[king]);
    this->opponent_king_square = get_lsb(board.bitboards[opponent_king]);
    bitboard_t king_bishop_attack = attack::attack<piece::bishop>(this->king_square, board.bitboards[piece::none]);
    bitboard_t king_rook_attack = attack::attack<piece::rook>(this->king_square, board.bitboards[piece::none]);
    this->checkers = attack::attackers<opponent>(board, this->king_square);
    this->checker_square = square::none;
    this->bishop_checking_squares = attack::attack<piece::bishop>(this->opponent_king_square, board.bitboards[color::none]);
    this->rook_checking_squares = attack::attack<piece::rook>(this->opponent_king_square, board.bitboards[color::none]);
    this->rook_checking_squares_castling = attack::attack<piece::rook>(this->opponent_king_square, board.bitboards[color::none] & ~board.bitboards[king]);
    this->evasion_target = bitboard::full;
    if (popcount(this->checkers) > 1) {
      this->evasion_target = bitboard::none;
    } else if (this->checkers) {
      this->checker_square = get_lsb(this->checkers);
      switch (board.pieces[this->checker_square]) {
        case opponent_queen:
          if (king_bishop_attack & board.bitboards[opponent_queen]) {
            this->evasion_target = bitboard(this->checker_square) | (king_bishop_attack & attack::attack<piece::bishop>(this->checker_square, board.bitboards[color::none]));
          } else if (king_rook_attack & board.bitboards[opponent_queen]) {
            this->evasion_target = bitboard(this->checker_square) | (king_rook_attack & attack::attack<piece::rook>(this->checker_square, board.bitboards[color::none]));
          };
          break;
        case opponent_rook:
          this->evasion_target = bitboard(this->checker_square) | (king_rook_attack & attack::attack<piece::rook>(this->checker_square, board.bitboards[color::none]));
          break;
        case opponent_bishop:
          this->evasion_target = bitboard(this->checker_square) | (king_bishop_attack & attack::attack<piece::bishop>(this->checker_square, board.bitboards[color::none]));
          break;
        default:
          this->evasion_target = bitboard(this->checker_square);
          break;
      };
    };
    this->attacked_squares = bitboard::none;
    this->unsafe_king_squares = bitboard::none;
    bitboard_t opponent_pawns = board.bitboards[opponent_pawn];
    while (opponent_pawns) {
      square_t square = pop_lsb(opponent_pawns);
      this->attacked_squares |= attack::attack<opponent_pawn>(square);
      this->unsafe_king_squares |= attack::attack<opponent_pawn>(square);
    };
    bitboard_t opponent_knights = board.bitboards[opponent_knight];
    while (opponent_knights) {
      square_t square = pop_lsb(opponent_knights);
      this->attacked_squares |= attack::attack<opponent_knight>(square);
      this->unsafe_king_squares |= attack::attack<opponent_knight>(square);
    };
    bitboard_t opponent_bishops = board.bitboards[opponent_bishop];
    while (opponent_bishops) {
      square_t square = pop_lsb(opponent_bishops);
      this->attacked_squares |= attack::attack<opponent_bishop>(square, board.bitboards[piece::none]);
      this->unsafe_king_squares |= attack::attack<opponent_bishop>(square, board.bitboards[piece::none] & ~board.bitboards[king]);
    };
    bitboard_t opponent_rooks = board.bitboards[opponent_rook];
    while (opponent_rooks) {
      square_t square = pop_lsb(opponent_rooks);
      this->attacked_squares |= attack::attack<opponent_rook>(square, board.bitboards[piece::none]);
      this->unsafe_king_squares |= attack::attack<opponent_rook>(square, board.bitboards[piece::none] & ~board.bitboards[king]);
    };
    bitboard_t opponent_queens = board.bitboards[opponent_queen];
    while (opponent_queens) {
      square_t square = pop_lsb(opponent_queens);
      this->attacked_squares |= attack::attack<opponent_queen>(square, board.bitboards[piece::none]);
      this->unsafe_king_squares |= attack::attack<opponent_queen>(square, board.bitboards[piece::none] & ~board.bitboards[king]);
    };
    this->attacked_squares |= attack::attack<opponent_king>(this->opponent_king_square);
    this->unsafe_king_squares |= attack::attack<opponent_king>(this->opponent_king_square);
    this->bishop_pinned = bitboard::none;
    bitboard_t bishop_attackers = (board.bitboards[opponent_bishop] | board.bitboards[opponent_queen]) & attack::ray::bishop[this->king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_pinned |= attack::attack<piece::bishop>(square, board.bitboards[color::none]) & king_bishop_attack;
    };
    this->rook_pinned = bitboard::none;
    bitboard_t rook_attackers = (board.bitboards[opponent_rook] | board.bitboards[opponent_queen]) & attack::ray::rook[this->king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_pinned |= attack::attack<piece::rook>(square, board.bitboards[color::none]) & king_rook_attack;
    };
    this->enpassant_pinned = bitboard::full;
    if (board.enpassant != square::none && color == board.turn) {
      this->enpassant_pinned = bitboard::none;
      bitboard_t enpassant_pawns = bitboard::none;
      if constexpr (color == color::white) {
        enpassant_pawns = bitboard(board.enpassant) << 8;
      } else {
        enpassant_pawns = bitboard(board.enpassant) >> 8;
      };
      bitboard_t enpassant_attackers = (board.bitboards[opponent_rook] | board.bitboards[opponent_queen]) & attack::ray::rook[this->king_square];
      while (enpassant_attackers) {
        square_t square = pop_lsb(enpassant_attackers);
        this->enpassant_pinned |= attack::attack<piece::rook>(square, board.bitboards[color::none] & ~enpassant_pawns) & attack::attack<piece::rook>(king_square, board.bitboards[color::none] & ~enpassant_pawns);
      };
    };
    this->bishop_discoverable = bitboard::none;
    bishop_attackers = (board.bitboards[bishop] | board.bitboards[queen]) & attack::ray::bishop[this->opponent_king_square];
    while (bishop_attackers) {
      square_t square = pop_lsb(bishop_attackers);
      this->bishop_discoverable |= attack::attack<bishop>(square, board.bitboards[color::none]) & this->bishop_checking_squares;
    };
    this->rook_discoverable = bitboard::none;
    rook_attackers = (board.bitboards[rook] | board.bitboards[queen]) & attack::ray::rook[this->opponent_king_square];
    while (rook_attackers) {
      square_t square = pop_lsb(rook_attackers);
      this->rook_discoverable |= attack::attack<rook>(square, board.bitboards[color::none]) & this->rook_checking_squares;
    };
  };
};

// generate all moves of a given movetype
template<color_t color, movetype_t movetype, typename T>
T generate(board::Board& board) {
  T moves;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    moves.clear();
  } else {
    moves = 0;
  };
  info_t info;
  info.update<color>(board);
  if (popcount(info.checkers) > 1) {
    generate_king<color, movetype>(moves, board, info);
    return moves;
  };
  generate_castling<color, movetype>(moves, board, info);
  generate_king<color, movetype>(moves, board, info);
  generate_queen<color, movetype>(moves, board, info);
  generate_rook<color, movetype>(moves, board, info);
  generate_bishop<color, movetype>(moves, board, info);
  generate_knight<color, movetype>(moves, board, info);
  generate_pawn<color, movetype>(moves, board, info);
  return moves;
};

// generate castling moves
template <color_t color, movetype_t movetype, typename T>
void generate_castling(T& moves, board::Board& board, info_t& info) {
  if constexpr (color == color::white) {
    bool castle_king = (
      !(info.attacked_squares & castling::white_king_attack_mask) &&
      !(board.bitboards[color::none] & castling::white_king_piece_mask) &&
      (board.castling & castling::white_king) &&
      (((movetype & movetype::check) && (bitboard::f1 & info.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::f1 & info.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(info.attacked_squares & castling::white_queen_attack_mask) &&
      !(board.bitboards[color::none] & castling::white_queen_piece_mask) &&
      (board.castling & castling::white_queen) &&
      (((movetype & movetype::check) && (bitboard::d1 & info.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::d1 & info.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move::move(square::e1, square::g1, piece::white_king, piece::white_king, piece::none, false, false, true, false, bitboard::f1 & info.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move::move(square::e1, square::c1, piece::white_king, piece::white_king, piece::none, false, false, true, false, bitboard::d1 & info.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  } else {
    bool castle_king = (
      !(info.attacked_squares & castling::black_king_attack_mask) &&
      !(board.bitboards[color::none] & castling::black_king_piece_mask) &&
      (board.castling & castling::black_king) &&
      (((movetype & movetype::check) && (bitboard::f8 & info.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::f8 & info.rook_checking_squares_castling)))
    );
    bool castle_queen = (
      !(info.attacked_squares & castling::black_queen_attack_mask) &&
      !(board.bitboards[color::none] & castling::black_queen_piece_mask) &&
      (board.castling & castling::black_queen) &&
      (((movetype & movetype::check) && (bitboard::d8 & info.rook_checking_squares_castling)) || ((movetype & movetype::quiet) && !(bitboard::d8 & info.rook_checking_squares_castling)))
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (castle_king)
        moves.push(move::move(square::e8, square::g8, piece::black_king, piece::black_king, piece::none, false, false, true, false, bitboard::f8 & info.rook_checking_squares_castling));
      if (castle_queen)
        moves.push(move::move(square::e8, square::c8, piece::black_king, piece::black_king, piece::none, false, false, true, false, bitboard::d8 & info.rook_checking_squares_castling));
    } else {
      moves += castle_king + castle_queen;
    };
  };
};

// generate king moves
template <color_t color, movetype_t movetype, typename T>
void generate_king(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
  bitboard_t checking_squares = bitboard::none;
  if (board.bitboards[king] & info.bishop_discoverable)
    checking_squares |= ~attack::ray::bishop[info.opponent_king_square];
  if (board.bitboards[king] & info.rook_discoverable)
    checking_squares |= ~attack::ray::rook[info.opponent_king_square];
  bitboard_t target = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    target |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check)
    target |= checking_squares;
  if constexpr (movetype & movetype::capture)
    target |= board.bitboards[opponent];
  bitboard_t possible_to = target & attack::attack<piece::king>(info.king_square) & ~board.bitboards[color] & ~info.unsafe_king_squares;
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (possible_to) {
      square_t to = pop_lsb(possible_to);
      moves.push(move::move(info.king_square, to, king, king, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
    };
  } else {
    moves += popcount(possible_to);
  };
};

// generate queen moves
template <color_t color, movetype_t movetype, typename T>
void generate_queen(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  bitboard_t checking_squares = info.bishop_checking_squares | info.rook_checking_squares;
  bitboard_t target = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    target |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check)
    target |= checking_squares;
  if constexpr (movetype & movetype::capture)
    target |= board.bitboards[opponent];
  target &= info.evasion_target;
  bitboard_t bishop_pinned_queens = board.bitboards[queen] & info.bishop_pinned & ~info.rook_pinned;
  while (bishop_pinned_queens) {
    square_t from = pop_lsb(bishop_pinned_queens);
    bitboard_t possible_to = attack::attack<piece::bishop>(from, board.bitboards[color::none]) & ~board.bitboards[color] & target & attack::ray::bishop[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, queen, queen, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t rook_pinned_queens = board.bitboards[queen] & ~info.bishop_pinned & info.rook_pinned;
  while (rook_pinned_queens) {
    square_t from = pop_lsb(rook_pinned_queens);
    bitboard_t possible_to = attack::attack<piece::rook>(from, board.bitboards[color::none]) & ~board.bitboards[color] & target & attack::ray::rook[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, queen, queen, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t free_queens = board.bitboards[queen] & ~info.bishop_pinned & ~info.rook_pinned;
  while (free_queens) {
    square_t from = pop_lsb(free_queens);
    bitboard_t possible_to = attack::attack<piece::queen>(from, board.bitboards[color::none]) & ~board.bitboards[color] & target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, queen, queen, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};

// generate rook moves
template <color_t color, movetype_t movetype, typename T>
void generate_rook(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  bitboard_t checking_squares = info.rook_checking_squares;
  bitboard_t non_discoverable_target = bitboard::none;
  bitboard_t bishop_discoverable_target = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_target |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_target |= checking_squares;
    bishop_discoverable_target |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_target |= board.bitboards[opponent];
    bishop_discoverable_target |= board.bitboards[opponent];
  };
  non_discoverable_target &= info.evasion_target;
  bishop_discoverable_target &= info.evasion_target;
  bitboard_t non_discoverable_rook_pinned_rooks = board.bitboards[rook] & ~info.bishop_pinned & info.rook_pinned & ~info.bishop_discoverable;
  while (non_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(non_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & ~board.bitboards[color] & non_discoverable_target & attack::ray::rook[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, rook, rook, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t non_discoverable_free_rooks = board.bitboards[rook] & ~info.bishop_pinned & ~info.rook_pinned & ~info.bishop_discoverable;
  while (non_discoverable_free_rooks) {
    square_t from = pop_lsb(non_discoverable_free_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & ~board.bitboards[color] & non_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, rook, rook, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t bishop_discoverable_rook_pinned_rooks = board.bitboards[rook] & ~info.bishop_pinned & info.rook_pinned & info.bishop_discoverable;
  while (bishop_discoverable_rook_pinned_rooks) {
    square_t from = pop_lsb(bishop_discoverable_rook_pinned_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & ~board.bitboards[color] & bishop_discoverable_target & attack::ray::rook[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, rook, rook, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t bishop_discoverable_free_rooks = board.bitboards[rook] & ~info.bishop_pinned & ~info.rook_pinned & info.bishop_discoverable;
  while (bishop_discoverable_free_rooks) {
    square_t from = pop_lsb(bishop_discoverable_free_rooks);
    bitboard_t possible_to = attack::attack<rook>(from, board.bitboards[color::none]) & ~board.bitboards[color] & bishop_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, rook, rook, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};

// generate bishop moves
template <color_t color, movetype_t movetype, typename T>
void generate_bishop(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  bitboard_t checking_squares = info.bishop_checking_squares;
  bitboard_t non_discoverable_target = bitboard::none;
  bitboard_t rook_discoverable_target = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_target |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_target |= checking_squares;
    rook_discoverable_target |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_target |= board.bitboards[opponent];
    rook_discoverable_target |= board.bitboards[opponent];
  };
  non_discoverable_target &= info.evasion_target;
  rook_discoverable_target &= info.evasion_target;
  bitboard_t non_discoverable_bishop_pinned_bishops = board.bitboards[bishop] & info.bishop_pinned & ~info.rook_pinned & ~info.rook_discoverable;
  while (non_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(non_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack::attack<bishop>(from, board.bitboards[color::none]) & ~board.bitboards[color] & non_discoverable_target & attack::ray::bishop[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, bishop, bishop, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t non_discoverable_free_bishops = board.bitboards[bishop] & ~info.bishop_pinned & ~info.rook_pinned & ~info.rook_discoverable;
  while (non_discoverable_free_bishops) {
    square_t from = pop_lsb(non_discoverable_free_bishops);
    bitboard_t possible_to = attack::attack<bishop>(from, board.bitboards[color::none]) & ~board.bitboards[color] & non_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, bishop, bishop, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t rook_discoverable_bishop_pinned_bishops = board.bitboards[bishop] & info.bishop_pinned & ~info.rook_pinned & info.rook_discoverable;
  while (rook_discoverable_bishop_pinned_bishops) {
    square_t from = pop_lsb(rook_discoverable_bishop_pinned_bishops);
    bitboard_t possible_to = attack::attack<bishop>(from, board.bitboards[color::none]) & ~board.bitboards[color] & rook_discoverable_target & attack::ray::bishop[info.king_square];
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, bishop, bishop, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t rook_discoverable_free_bishops = board.bitboards[bishop] & ~info.bishop_pinned & ~info.rook_pinned & info.rook_discoverable;
  while (rook_discoverable_free_bishops) {
    square_t from = pop_lsb(rook_discoverable_free_bishops);
    bitboard_t possible_to = attack::attack<bishop>(from, board.bitboards[color::none]) & ~board.bitboards[color] & rook_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, bishop, bishop, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};

// generate knight moves
template <color_t color, movetype_t movetype, typename T>
void generate_knight(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  bitboard_t checking_squares = attack::attack<knight>(info.opponent_king_square);
  bitboard_t non_discoverable_target = bitboard::none;
  bitboard_t discoverable_target = bitboard::none;
  if constexpr (movetype & movetype::quiet)
    non_discoverable_target |= ~(checking_squares | board.bitboards[opponent]);
  if constexpr (movetype & movetype::check) {
    non_discoverable_target |= checking_squares;
    discoverable_target |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_target |= board.bitboards[opponent];
    discoverable_target |= board.bitboards[opponent];
  };
  non_discoverable_target &= info.evasion_target;
  discoverable_target &= info.evasion_target;
  bitboard_t non_discoverable_free_knights = board.bitboards[knight] & ~info.bishop_pinned & ~info.rook_pinned & ~(info.bishop_discoverable | info.rook_discoverable);
  while (non_discoverable_free_knights) {
    square_t from = pop_lsb(non_discoverable_free_knights);
    bitboard_t possible_to = attack::attack<knight>(from) & ~board.bitboards[color] & non_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, knight, knight, board.pieces[to], false, false, false, false, bitboard(to) & checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t discoverable_free_knights = board.bitboards[knight] & ~info.bishop_pinned & ~info.rook_pinned & (info.bishop_discoverable | info.rook_discoverable);
  while (discoverable_free_knights) {
    square_t from = pop_lsb(discoverable_free_knights);
    bitboard_t possible_to = attack::attack<knight>(from) & ~board.bitboards[color] & discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, knight, knight, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};

// generate pawn moves
template <color_t color, movetype_t movetype, typename T>
void generate_pawn(T& moves, board::Board& board, info_t& info) {
  generate_pawn_captures<color, movetype>(moves, board, info);
  generate_pawn_pushes<color, movetype>(moves, board, info);
};

// generate pawn movetype::captures
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_captures(T& moves, board::Board& board, info_t& info) {
  generate_pawn_enpassant<color, movetype>(moves, board, info);
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr bitboard_t pre_promotion_rank = (color == color::white) ? bitboard::rank_7 : bitboard::rank_2;
  bitboard_t free_pawns = board.bitboards[pawn] & ~info.bishop_pinned & ~info.rook_pinned;
  bitboard_t bishop_pinned_pawns = board.bitboards[pawn] & info.bishop_pinned & ~info.rook_pinned;
  bitboard_t pawn_checking_squares = attack::attack<opponent_pawn>(info.opponent_king_square);
  bitboard_t knight_checking_squares = attack::attack<knight>(info.opponent_king_square);
  bitboard_t bishop_checking_squares_no_pawns = attack::attack<bishop>(info.opponent_king_square, board.bitboards[piece::none] & ~board.bitboards[pawn]);
  bitboard_t queen_checking_squares = info.bishop_checking_squares | info.rook_checking_squares;
  bitboard_t queen_checking_squares_no_pawns = (bishop_checking_squares_no_pawns | info.rook_checking_squares);
  bitboard_t non_discoverable_target = bitboard::none;
  bitboard_t non_discoverable_knight_promoting_target = bitboard::none;
  bitboard_t non_discoverable_bishop_promoting_target = bitboard::none;
  bitboard_t non_discoverable_rook_promoting_target = bitboard::none;
  bitboard_t non_discoverable_queen_promoting_target = bitboard::none;
  bitboard_t discoverable_target = bitboard::none;
  if constexpr (movetype & movetype::check) {
    non_discoverable_target |= pawn_checking_squares;
    non_discoverable_knight_promoting_target |= knight_checking_squares;
    non_discoverable_bishop_promoting_target |= bishop_checking_squares_no_pawns;
    non_discoverable_rook_promoting_target |= info.rook_checking_squares;
    non_discoverable_queen_promoting_target |= queen_checking_squares_no_pawns;
    discoverable_target |= bitboard::full;
  };
  if constexpr (movetype & movetype::capture) {
    non_discoverable_target |= bitboard::full;
    non_discoverable_knight_promoting_target |= bitboard::full;
    non_discoverable_bishop_promoting_target |= bitboard::full;
    non_discoverable_rook_promoting_target |= bitboard::full;
    non_discoverable_queen_promoting_target |= bitboard::full;
    discoverable_target |= bitboard::full;
  };
  non_discoverable_target &= info.evasion_target;
  non_discoverable_knight_promoting_target &= info.evasion_target;
  non_discoverable_bishop_promoting_target &= info.evasion_target;
  non_discoverable_rook_promoting_target &= info.evasion_target;
  non_discoverable_queen_promoting_target &= info.evasion_target;
  discoverable_target &= info.evasion_target;
  bitboard_t discoverable_promoting_free_pawns = free_pawns & pre_promotion_rank & (info.bishop_discoverable | info.rook_discoverable);
  bitboard_t discoverable_not_promoting_free_pawns = free_pawns & ~pre_promotion_rank & (info.bishop_discoverable | info.rook_discoverable);
  bitboard_t discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & pre_promotion_rank & (info.bishop_discoverable | info.rook_discoverable);
  bitboard_t discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~pre_promotion_rank & (info.bishop_discoverable | info.rook_discoverable);
  while (discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, knight, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, bishop, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, rook, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, queen, board.pieces[to], false, false, false, true, true));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };
  while (discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, pawn, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  while (discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, knight, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, bishop, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, rook, board.pieces[to], false, false, false, true, true));
        moves.push(move::move(from, to, pawn, queen, board.pieces[to], false, false, false, true, true));
      };
    } else {
      moves += popcount(possible_to) << 2;
    };
  };
  while (discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, pawn, board.pieces[to], false, false, false, false, true));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  bitboard_t non_discoverable_promoting_free_pawns = free_pawns & pre_promotion_rank & ~(info.bishop_discoverable | info.rook_discoverable);
  bitboard_t non_discoverable_not_promoting_free_pawns = free_pawns & ~pre_promotion_rank & ~(info.bishop_discoverable | info.rook_discoverable);
  bitboard_t non_discoverable_promoting_bishop_pinned_pawns = bishop_pinned_pawns & pre_promotion_rank & ~(info.bishop_discoverable | info.rook_discoverable);
  bitboard_t non_discoverable_not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~pre_promotion_rank & ~(info.bishop_discoverable | info.rook_discoverable);
  while (non_discoverable_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_free_pawns);
    bitboard_t bishop_checking_squares = attack::attack<piece::bishop>(info.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t queen_checking_squares = attack::attack<piece::queen>(info.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t possible_to_knight = attack::attack<pawn>(from) & board.bitboards[opponent] & non_discoverable_knight_promoting_target;
    bitboard_t possible_to_bishop = attack::attack<pawn>(from) & board.bitboards[opponent] & non_discoverable_bishop_promoting_target;
    bitboard_t possible_to_rook = attack::attack<pawn>(from) & board.bitboards[opponent] & non_discoverable_rook_promoting_target;
    bitboard_t possible_to_queen = attack::attack<pawn>(from) & board.bitboards[opponent] & non_discoverable_queen_promoting_target;
    if constexpr ((movetype & movetype::check) && !(movetype & movetype::capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move::move(from, to, pawn, knight, board.pieces[to], false, false, false, true, bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move::move(from, to, pawn, bishop, board.pieces[to], false, false, false, true, bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move::move(from, to, pawn, rook, board.pieces[to], false, false, false, true, bitboard(to) & info.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move::move(from, to, pawn, queen, board.pieces[to], false, false, false, true, bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };
  while (non_discoverable_not_promoting_free_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_free_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & non_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, pawn, board.pieces[to], false, false, false, false, bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
  while (non_discoverable_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_promoting_bishop_pinned_pawns);
    bitboard_t bishop_checking_squares = attack::attack<piece::bishop>(info.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t queen_checking_squares = attack::attack<piece::queen>(info.opponent_king_square, board.bitboards[piece::none] & ~bitboard(from));
    bitboard_t possible_to_knight = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & non_discoverable_knight_promoting_target;
    bitboard_t possible_to_bishop = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & non_discoverable_bishop_promoting_target;
    bitboard_t possible_to_rook = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & non_discoverable_rook_promoting_target;
    bitboard_t possible_to_queen = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & non_discoverable_queen_promoting_target;
    if constexpr ((movetype & movetype::check) && !(movetype & movetype::capture)) {
      possible_to_bishop &= bishop_checking_squares;
      possible_to_queen &= queen_checking_squares;
    };
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to_knight) {
        square_t to = pop_lsb(possible_to_knight);
        moves.push(move::move(from, to, pawn, knight, board.pieces[to], false, false, false, true, bitboard(to) & knight_checking_squares));
      };
      while (possible_to_bishop) {
        square_t to = pop_lsb(possible_to_bishop);
        moves.push(move::move(from, to, pawn, bishop, board.pieces[to], false, false, false, true, bitboard(to) & bishop_checking_squares));
      };
      while (possible_to_rook) {
        square_t to = pop_lsb(possible_to_rook);
        moves.push(move::move(from, to, pawn, rook, board.pieces[to], false, false, false, true, bitboard(to) & info.rook_checking_squares));
      };
      while (possible_to_queen) {
        square_t to = pop_lsb(possible_to_queen);
        moves.push(move::move(from, to, pawn, queen, board.pieces[to], false, false, false, true, bitboard(to) & queen_checking_squares));
      };
    } else {
      moves += popcount(possible_to_knight) + popcount(possible_to_bishop) + popcount(possible_to_rook) + popcount(possible_to_queen);
    };
  };
  while (non_discoverable_not_promoting_bishop_pinned_pawns) {
    square_t from = pop_lsb(non_discoverable_not_promoting_bishop_pinned_pawns);
    bitboard_t possible_to = attack::attack<pawn>(from) & board.bitboards[opponent] & attack::ray::bishop[info.king_square] & non_discoverable_target;
    if constexpr (std::is_same_v<T, move_stack_t>) {
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(from, to, pawn, pawn, board.pieces[to], false, false, false, false, bitboard(to) & pawn_checking_squares));
      };
    } else {
      moves += popcount(possible_to);
    };
  };
};

// generate pawn pushes
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_enpassant(T& moves, board::Board& board, info_t& info) {
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, opponent);
  constexpr int enpassant_offset = (color == color::white ? 8 : -8);
  if (board.enpassant == square::none) return;
  bool is_always_check = (info.bishop_discoverable & bitboard(board.enpassant + enpassant_offset)) || (board.bitboards[opponent_king] & attack::attack<pawn>(board.enpassant));
  bitboard_t bishop_pinned_pawns = (
    board.bitboards[pawn] &
    attack::attack<opponent_pawn>(board.enpassant) &
    ~info.enpassant_pinned &
    info.bishop_pinned &
    ~info.rook_pinned
  );
  while (bishop_pinned_pawns) {
    square_t from = pop_lsb(bishop_pinned_pawns);
    bool is_check = (
      is_always_check ||
      (info.rook_discoverable & bitboard(from)) ||
      ((info.bishop_discoverable & bitboard(from)) && (~attack::ray::bishop[info.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack::attack<rook>(info.opponent_king_square, (board.bitboards[piece::none] & ~bitboard(from) & ~bitboard(board.enpassant + enpassant_offset)) | bitboard(board.enpassant)) & (board.bitboards[rook] | board.bitboards[queen]))
    );
    bool enpassant = (
      (attack::ray::bishop[info.king_square] & bitboard(board.enpassant)) &&
      (
        (info.evasion_target & bitboard(board.enpassant)) ||
        (board.enpassant + enpassant_offset == info.checker_square)
      ) && (
        (movetype & movetype::capture) ||
        ((movetype & movetype::check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move::move(from, board.enpassant, pawn, pawn, opponent_pawn, false, true, false, false, is_check));
    } else {
      moves += enpassant;
    };
  };
  bitboard_t free_pawns = (
    board.bitboards[pawn] &
    attack::attack<opponent_pawn>(board.enpassant) &
    ~info.enpassant_pinned &
    ~info.bishop_pinned &
    ~info.rook_pinned
  );
  while (free_pawns) {
    square_t from = pop_lsb(free_pawns);
    bool is_check = (
      is_always_check ||
      (info.rook_discoverable & bitboard(from)) ||
      ((info.bishop_discoverable & bitboard(from)) && (~attack::ray::bishop[info.opponent_king_square] & bitboard(board.enpassant))) ||
      (attack::attack<rook>(info.opponent_king_square, (board.bitboards[piece::none] & ~bitboard(from) & ~bitboard(board.enpassant + enpassant_offset)) | bitboard(board.enpassant)) & (board.bitboards[rook] | board.bitboards[queen]))
    );
    bool enpassant = (
      (
        (info.evasion_target & bitboard(board.enpassant)) ||
        (board.enpassant + enpassant_offset == info.checker_square)
      ) && (
        (movetype & movetype::capture) ||
        ((movetype & movetype::check) && is_check)
      )
    );
    if constexpr (std::is_same_v<T, move_stack_t>) {
      if (enpassant)
        moves.push(move::move(from, board.enpassant, pawn, pawn, opponent_pawn, false, true, false, false, is_check));
    } else {
      moves += enpassant;
    };
  };
};

// generate pawn pushes
template <color_t color, movetype_t movetype, typename T>
void generate_pawn_pushes(T& moves, board::Board& board, info_t& info) {
  constexpr color_t opponent = color::compiletime::opponent(color);
  constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
  constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
  constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
  constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
  constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
  constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
  constexpr bitboard_t promotion_rank = (color == color::white) ? bitboard::rank_8 : bitboard::rank_1;
  constexpr int push_offset = (color == color::white ? -8 : 8);
  constexpr int doublepush_offset = 2 * push_offset;
  bitboard_t free_pawns = board.bitboards[pawn] & ~info.bishop_pinned & ~info.rook_pinned;
  bitboard_t rook_pinned_pawns = board.bitboards[pawn] & ~info.bishop_pinned & info.rook_pinned;
  bitboard_t pawn_checking_squares = attack::attack<opponent_pawn>(info.opponent_king_square);
  bitboard_t knight_checking_squares = attack::attack<knight>(info.opponent_king_square);
  bitboard_t non_discoverable_target = bitboard::none;
  bitboard_t non_discoverable_knight_promoting_target = bitboard::none;
  bitboard_t non_discoverable_bishop_promoting_target = bitboard::none;
  bitboard_t non_discoverable_rook_promoting_target = bitboard::none;
  bitboard_t non_discoverable_queen_promoting_target = bitboard::none;
  bitboard_t discoverable_target = bitboard::none;
  if constexpr (movetype & movetype::quiet) {
    non_discoverable_target |= ~pawn_checking_squares;
    non_discoverable_knight_promoting_target |= ~knight_checking_squares;
    non_discoverable_bishop_promoting_target |= ~info.bishop_checking_squares;
    non_discoverable_rook_promoting_target |= ~info.rook_checking_squares;
    non_discoverable_queen_promoting_target |= ~(info.bishop_checking_squares | info.rook_checking_squares);
  };
  if constexpr (movetype & movetype::check) {
    non_discoverable_target |= pawn_checking_squares;
    non_discoverable_knight_promoting_target |= knight_checking_squares;
    non_discoverable_bishop_promoting_target |= info.bishop_checking_squares;
    non_discoverable_rook_promoting_target |= info.rook_checking_squares;
    non_discoverable_queen_promoting_target |= (info.bishop_checking_squares | info.rook_checking_squares);
    discoverable_target |= bitboard::full;
  };
  non_discoverable_target &= info.evasion_target;
  non_discoverable_knight_promoting_target &= info.evasion_target;
  non_discoverable_bishop_promoting_target &= info.evasion_target;
  non_discoverable_rook_promoting_target &= info.evasion_target;
  non_discoverable_queen_promoting_target &= info.evasion_target;
  discoverable_target &= info.evasion_target;
  bitboard_t pushable_pawns;
  bitboard_t doublepushable_pawns;
  bitboard_t promoting_pawns;
  if constexpr (color == color::white) {
    pushable_pawns = (
      (free_pawns & ~(board.bitboards[color::none] << 8) & (info.evasion_target << 8) & ~bitboard::rank_7) |
      (rook_pinned_pawns & ~(board.bitboards[color::none] << 8) & (info.evasion_target << 8) & (attack::ray::rook[info.king_square] << 8))
    );
    doublepushable_pawns = (
      (free_pawns & ~(board.bitboards[color::none] << 8) & ~(board.bitboards[color::none] << 16) & (info.evasion_target << 16) & bitboard::rank_2) |
      (rook_pinned_pawns & ~(board.bitboards[color::none] << 8) & ~(board.bitboards[color::none] << 16) & (info.evasion_target << 16) & bitboard::rank_2 & (attack::ray::rook[info.king_square] << 16))
    );
    promoting_pawns = free_pawns & ~(board.bitboards[color::none] << 8) & (info.evasion_target << 8) & bitboard::rank_7;
  } else {
    pushable_pawns = (
      (free_pawns & ~(board.bitboards[color::none] >> 8) & (info.evasion_target >> 8) & ~bitboard::rank_2) |
      (rook_pinned_pawns & ~(board.bitboards[color::none] >> 8) & (info.evasion_target >> 8) & (attack::ray::rook[info.king_square] >> 8))
    );
    doublepushable_pawns = (
      (free_pawns & ~(board.bitboards[color::none] >> 8) & ~(board.bitboards[color::none] >> 16) & (info.evasion_target >> 16) & bitboard::rank_7) |
      (rook_pinned_pawns & ~(board.bitboards[color::none] >> 8) & ~(board.bitboards[color::none] >> 16) & (info.evasion_target >> 16) & bitboard::rank_7 & (attack::ray::rook[info.king_square] >> 16))
    );
    promoting_pawns = free_pawns & ~(board.bitboards[color::none] >> 8) & (info.evasion_target >> 8) & bitboard::rank_2;
  };
  bitboard_t discoverable = info.bishop_discoverable | (info.rook_discoverable & bitboard::ranks[(info.opponent_king_square >> 3) ^ 0b111]);
  bitboard_t discoverable_pushable_pawns = pushable_pawns & discoverable;
  bitboard_t discoverable_doublepushable_pawns = doublepushable_pawns & discoverable;
  bitboard_t discoverable_promoting_pawns = promoting_pawns & discoverable;
  bitboard_t non_discoverable_pushable_pawns = pushable_pawns & ~discoverable;
  bitboard_t non_discoverable_doublepushable_pawns = doublepushable_pawns & ~discoverable;
  bitboard_t non_discoverable_promoting_pawns = promoting_pawns & ~discoverable;
  bitboard_t knight_promoting_pawns;
  bitboard_t bishop_promoting_pawns;
  bitboard_t rook_promoting_pawns;
  bitboard_t queen_promoting_pawns;
  if constexpr (color == color::white) {
    pushable_pawns = (
      (discoverable_pushable_pawns & (discoverable_target << 8)) |
      (non_discoverable_pushable_pawns & (non_discoverable_target << 8))
    );
    doublepushable_pawns = (
      (discoverable_doublepushable_pawns & (discoverable_target << 16)) |
      (non_discoverable_doublepushable_pawns & (non_discoverable_target << 16))
    );
    knight_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_knight_promoting_target << 8))
    );
    bishop_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_bishop_promoting_target << 8))
    );
    rook_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_rook_promoting_target << 8))
    );
    queen_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target << 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_queen_promoting_target << 8))
    );
  } else {
    pushable_pawns = (
      (discoverable_pushable_pawns & (discoverable_target >> 8)) |
      (non_discoverable_pushable_pawns & (non_discoverable_target >> 8))
    );
    doublepushable_pawns = (
      (discoverable_doublepushable_pawns & (discoverable_target >> 16)) |
      (non_discoverable_doublepushable_pawns & (non_discoverable_target >> 16))
    );
    knight_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_knight_promoting_target >> 8))
    );
    bishop_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_bishop_promoting_target >> 8))
    );
    rook_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_rook_promoting_target >> 8))
    );
    queen_promoting_pawns = (
      (discoverable_promoting_pawns & (discoverable_target >> 8)) |
      (non_discoverable_promoting_pawns & (non_discoverable_queen_promoting_target >> 8))
    );
  };
  if constexpr (std::is_same_v<T, move_stack_t>) {
    while (pushable_pawns) {
      square_t from = pop_lsb(pushable_pawns);
      square_t to = from + push_offset;
      moves.push(move::move(from, to, pawn, pawn, piece::none, false, false, false, false, (bitboard(to) & pawn_checking_squares) || (bitboard(from) & discoverable)));
    };
    while (doublepushable_pawns) {
      square_t from = pop_lsb(doublepushable_pawns);
      square_t to = from + doublepush_offset;
      moves.push(move::move(from, to, pawn, pawn, piece::none, true, false, false, false, (bitboard(to) & pawn_checking_squares) || (bitboard(from) & discoverable)));
    };
    while (knight_promoting_pawns) {
      square_t from = pop_lsb(knight_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move::move(from, to, pawn, knight, piece::none, false, false, false, true, (bitboard(to) & knight_checking_squares) || (bitboard(from) & discoverable)));
    };
    while (bishop_promoting_pawns) {
      square_t from = pop_lsb(bishop_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move::move(from, to, pawn, bishop, piece::none, false, false, false, true, (bitboard(to) & info.bishop_checking_squares) || (bitboard(from) & discoverable)));
    };
    while (rook_promoting_pawns) {
      square_t from = pop_lsb(rook_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move::move(from, to, pawn, rook, piece::none, false, false, false, true, (bitboard(to) & info.rook_checking_squares) || (bitboard(from) & discoverable)));
    };
    while (queen_promoting_pawns) {
      square_t from = pop_lsb(queen_promoting_pawns);
      square_t to = from + push_offset;
      moves.push(move::move(from, to, pawn, queen, piece::none, false, false, false, true, (bitboard(to) & (info.bishop_checking_squares | info.rook_checking_squares)) || (bitboard(from) & discoverable)));
    };
  } else {
    moves += popcount(pushable_pawns);
    moves += popcount(doublepushable_pawns);
    moves += popcount(knight_promoting_pawns);
    moves += popcount(bishop_promoting_pawns);
    moves += popcount(rook_promoting_pawns);
    moves += popcount(queen_promoting_pawns);
  };
};
};


#endif