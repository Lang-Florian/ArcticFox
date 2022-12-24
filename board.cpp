#pragma once

#include <array>
#include <string>
#include "modules/list.cpp"
#include "base.cpp"
#include "zobrist.cpp"

/***********************************************************************
 * 
 * Module to represent the board and all its properties.
 *
***********************************************************************/

struct undo_t {
  move_t move;
  castling_t castling;
  square_t enpassant;
  u16_t halfmove_clock;
  hash_t hash;
};

class Board {
public:
  std::array<bitboard_t, 32> bitboards;
  std::array<piece_t, 64> pieces;
  std::array<u8_t, 32> piece_counts;
  color_t turn;
  castling_t castling;
  square_t enpassant;
  u16_t halfmove_clock;
  u16_t fullmove_clock;
  Zobrist zobrist;
  List<undo_t, MAX_GAME_LENGTH> history;
  std::string starting_fen;

  // initialize the board
  Board(std::string fen=STARTPOS) {
    this->set_fen(fen);
  };

  // set the board to a given fen string
  void set_fen(std::string fen=STARTPOS) {
    this->starting_fen = fen;
    // clear all bitboards, pieces, etc.
    this->bitboards.fill(none);
    this->pieces.fill(none);
    this->piece_counts.fill(0);
    this->zobrist.clear();
    this->history.clear();
    // setup bitboards and pieces
    int index = 0;
    for (square_t square = 0; index < fen.length() && fen[index] != ' '; index++) {
      if (fen[index] == '/') continue;
      piece_t piece = piece_from_char(fen[index]);
      if (piece != none) {
        this->place_piece(piece, square);
        square++;
      } else {
        square += fen[index] - '0';
      };
    };
    // setup turn
    index++;
    this->turn = color_from_char(fen[index]);
    if (this->turn != white)
      this->zobrist.update_turn();
    // setup castling
    index++;
    this->castling = none;
    while (fen[++index] != ' ')
      this->castling |= castling_from_char(fen[index]);
    this->zobrist.update_castling(this->castling);
    // setup enpassant
    index++;
    this->enpassant = (fen[index] == '-') ? none_square : ((fen[index] - 'a') + 8 * (7 - (fen[++index] - '1')));
    this->zobrist.update_enpassant(this->enpassant);
    // setup halfmove clock
    index++;
    this->halfmove_clock = 0;
    while (fen[++index] != ' ')
      this->halfmove_clock = this->halfmove_clock * 10 + (fen[index] - '0');
    // setup fullmove clock
    this->fullmove_clock = 0;
    while (++index < fen.length() && fen[index] != ' ')
      this->fullmove_clock = 10 * this->fullmove_clock + (fen[index] - '0');
  };

  // get the fen string for the board
  std::string fen() {
    std::string fen = "";
    int index = 0;
    for (square_t square = 0; square < none_square; square++) {
      if (square % 8 == 0 && square != 0) {
        if (index != 0) {
          fen += std::to_string(index);
          index = 0;
        };
        fen += "/";
      };
      if (this->pieces[square] == none) {
        index++;
      } else {
        if (index != 0) {
          fen += std::to_string(index);
          index = 0;
        };
        fen += piece_to_string(this->pieces[square]);
      };
    };
    if (index != 0) fen += std::to_string(index);
    fen += " " + color_to_string(this->turn);
    fen += " " + castling_to_string(this->castling);
    fen += " " + square_to_string(this->enpassant);
    fen += " " + std::to_string(this->halfmove_clock);
    fen += " " + std::to_string(this->fullmove_clock);
    return fen;
  };

  // convert a uci move to a move_t, trusts the uci move is valid
  move_t from_uci(std::string uci) {
    if (uci.length() != 4 && uci.length() != 5) return none;
    square_t from = (uci[0] - 'a') + 8 * (7 - (uci[1] - '1'));
    square_t to = (uci[2] - 'a') + 8 * (7 - (uci[3] - '1'));
    piece_t moved_piece = this->pieces[from];
    piece_t target_piece = moved_piece;
    if (uci.length() == 5)
      target_piece = to_color(piece_from_char(uci[4]), this->turn);
    piece_t captured_piece = this->pieces[to];
    bool double_pawn_push = (piece_type(moved_piece) == pawn) && (((from - to) == 16) || ((from - to) == -16));
    bool enpassant = (piece_type(moved_piece) == pawn) && (to == this->enpassant);
    bool castling = (piece_type(moved_piece) == king) && (((from - to) == 2) || ((from - to) == -2));
    return move(from, to, moved_piece, target_piece, captured_piece, double_pawn_push, enpassant, castling, target_piece != moved_piece, false);
  };

  // place a piece on a square
  template <color_t color>
  void place_piece(piece_t piece, square_t square) {
    set_bit(this->bitboards[piece], square);
    set_bit(this->bitboards[piece_type(piece)], square);
    set_bit(this->bitboards[color], square);
    set_bit(this->bitboards[none], square);
    this->pieces[square] = piece;
    ++this->piece_counts[piece];
    ++this->piece_counts[piece_type(piece)];
    ++this->piece_counts[color];
    ++this->piece_counts[none];
    this->zobrist.update_piece(piece, square);
  };

  void place_piece(piece_t piece, square_t square) {
    if (color(piece) == white) {
      this->place_piece<white>(piece, square);
    } else if (color(piece) == black) {
      this->place_piece<black>(piece, square);
    };
  };

  // remove a piece from a square
  template <color_t color>
  void remove_piece(square_t square) {
    piece_t piece = pieces[square];
    clear_bit(this->bitboards[piece], square);
    clear_bit(this->bitboards[piece_type(piece)], square);
    clear_bit(this->bitboards[color], square);
    clear_bit(this->bitboards[none], square);
    this->pieces[square] = none;
    --this->piece_counts[piece];
    --this->piece_counts[piece_type(piece)];
    --this->piece_counts[color];
    --this->piece_counts[none];
    this->zobrist.update_piece(piece, square);
  };

  void remove_piece(square_t square) {
    if (color(this->pieces[square]) == white) {
      this->remove_piece<white>(square);
    } else if (color(this->pieces[square]) == black) {
      this->remove_piece<black>(square);
    };
  };

  // make a move on the board
  template <color_t color>
  void make(move_t move) {
    constexpr color_t opponent = opponent(color);
    constexpr piece_t color_rook = to_color(rook, color);
    // add the undo object to the history
    this->history.push(undo_t {
      move,
      this->castling,
      this->enpassant,
      this->halfmove_clock,
      this->zobrist.hash,
    });
    // remove castling and enpassant square from the hash
    this->zobrist.update_castling(this->castling);
    this->zobrist.update_enpassant(this->enpassant);
    // remove castling rights if needed
    this->castling &= ~removed_castling(move);
    // remove enpassant square to be updated later
    this->enpassant = none_square;
    // change move clocks
    this->halfmove_clock = (this->halfmove_clock + 1) & (u16_t)((!(piece_type(moved_piece(move)) == pawn || capture(move))) * 0xFFFF);
    if constexpr (color == black)
      ++this->fullmove_clock;
    // do move
    square_t from = from(move);
    square_t to = to(move);
    this->remove_piece<color>(from);
    this->remove_piece<opponent>(to);
    this->place_piece<color>(target_piece(move), to);
    if (enpassant(move)) {
      if constexpr (color == white)
        this->remove_piece<opponent>(to + 8);
      else if constexpr (color == black)
        this->remove_piece<opponent>(to - 8);
    } else if (castling(move)) {
      this->remove_piece<color>(to + (to > from) * 3 - 2);
      this->place_piece<color>(color_rook, to - (to > from) * 2 + 1);
    } else if (double_pawn_push(move)) {
      if constexpr (color == white)
        this->enpassant = to + 8;
      else if constexpr (color == black)
        this->enpassant = to - 8;
    };
    // change turn
    this->turn = opponent;
    // add castling, enpassant square and the turn change to the hash
    this->zobrist.update_castling(this->castling);
    this->zobrist.update_enpassant(this->enpassant);
    this->zobrist.update_turn();
  };

  void make(move_t move) {
    if (this->turn == white)
      this->make<white>(move);
    else if (this->turn == black)
      this->make<black>(move);
  };

  // undo a move on the board
  template <color_t color>
  void unmake() {
    constexpr color_t opponent = opponent(color);
    constexpr piece_t opponent_pawn = to_color(pawn, opponent);
    constexpr piece_t color_rook = to_color(rook, color);
    // restore history
    undo_t undo = this->history.pop();
    move_t move = undo.move;
    this->castling = undo.castling;
    this->enpassant = undo.enpassant;
    this->halfmove_clock = undo.halfmove_clock;
    // change turn
    this->turn = color;
    // change move clocks
    if constexpr (color == black)
      --this->fullmove_clock;
    // undo move
    square_t from = from(move);
    square_t to = to(move);
    this->remove_piece<color>(to);
    this->place_piece<color>(moved_piece(move), from);
    if (enpassant(move)) {
      if constexpr (color == white)
        this->place_piece<opponent>(opponent_pawn, to + 8);
      else if constexpr (color == black)
        this->place_piece<opponent>(opponent_pawn, to - 8);
    } else if (castling(move)) {
      this->remove_piece<color>(to - (to > from) * 2 + 1);
      this->place_piece<color>(color_rook, to + (to > from) * 3 - 2);
    } else if (capture(move)) {
      this->place_piece<opponent>(captured_piece(move), to);
    };
    // restore hash
    this->zobrist.set(undo.hash);
  };

  void unmake() {
    if (this->turn == white)
      this->unmake<black>();
    else if (this->turn == black)
      this->unmake<white>();
  };

  // check if a position already exists in the history
  bool position_existed() {
    return this->history.count([this](undo_t undo) {
      return undo.hash == this->zobrist.hash;
    });
  };
};