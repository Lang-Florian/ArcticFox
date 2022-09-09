#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __BOARD__MODULE__
#define __BOARD__MODULE__


#include <array>
#include <string>
#include "base/base.cpp"
#include "modules/stack.cpp"
#include "zobrist.cpp"


/*

  Module for the board.

*/


namespace board {
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
  color_t turn;
  castling_t castling;
  square_t enpassant;
  u16_t halfmove_clock;
  u16_t fullmove_clock;
  zobrist::Hash zobrist;
  stack::Stack<undo_t, MAX_GAME_LENGTH> history;
  std::string starting_fen;

  // initialize the board
  Board(std::string fen=STARTPOS) {
    this->set_fen(fen);
  };

  // set the board to a given fen string
  void set_fen(std::string fen=STARTPOS) {
    this->starting_fen = fen;

    // clear all
    this->bitboards.fill(bitboard::none);
    this->pieces.fill(piece::none);
    this->zobrist.clear();
    this->history.clear();
    
    // setup bitboards and pieces
    int index = 0;
    for (square_t square = 0; index < fen.length() && fen[index] != ' '; index++) {
      if (fen[index] == '/') continue;
      piece_t piece = piece::from_char(fen[index]);
      if (piece != piece::none) {
        this->place_piece(piece, square);
        square++;
      } else {
        square += fen[index] - '0';
      };
    };
    
    // setup turn
    index++;
    this->turn = color::from_char(fen[index]);
    if (this->turn != color::white) this->zobrist.update_turn();

    // setup castling
    index++;
    this->castling = castling::none;
    while (fen[++index] != ' ') {
      this->castling |= castling::from_char(fen[index]);
    };
    this->zobrist.update_castling(this->castling);

    // setup enpassant
    index++;
    this->enpassant = (fen[index] == '-') ? square::none : ((fen[index] - 'a') + 8 * (7 - (fen[++index] - '1')));
    this->zobrist.update_enpassant(this->enpassant);
    
    // setup halfmove clock
    index++;
    this->halfmove_clock = 0;
    while (fen[++index] != ' ') {
      this->halfmove_clock = this->halfmove_clock * 10 + (fen[index] - '0');
    };

    // setup fullmove clock
    this->fullmove_clock = 0;
    while (++index < fen.length() && fen[index] != ' ') {
      this->fullmove_clock = 10 * this->fullmove_clock + (fen[index] - '0');
    };
  };

  // get the fen string for the board
  std::string fen() {
    std::string fen = "";
    int index = 0;
    for (auto square : square::all) {
      if (square % 8 == 0 && square != 0) {
        if (index != 0) {
          fen += std::to_string(index);
          index = 0;
        };
        fen += "/";
      };
      if (this->pieces[square] == piece::none) {
        index++;
      } else {
        if (index != 0) {
          fen += std::to_string(index);
          index = 0;
        };
        fen += piece::to_string(this->pieces[square]);
      };
    };
    if (index != 0) fen += std::to_string(index);
    fen += " " + color::to_string(this->turn);
    fen += " " + castling::to_string(this->castling);
    fen += " " + square::to_string(this->enpassant);
    fen += " " + std::to_string(this->halfmove_clock);
    fen += " " + std::to_string(this->fullmove_clock);
    return fen;
  };

  // convert a uci move to a move_t
  move_t from_uci(std::string uci) {
    square_t from = (uci[0] - 'a') + 8 * (7 - (uci[1] - '1'));
    square_t to = (uci[2] - 'a') + 8 * (7 - (uci[3] - '1'));
    piece_t moved_piece = this->pieces[from];
    piece_t target_piece = moved_piece;
    if (uci.length() == 5) {
      target_piece = piece::to_color(piece::from_char(uci[4]), this->turn);
    };
    piece_t captured_piece = this->pieces[to];
    bool double_pawn_push = (piece::type(moved_piece) == piece::pawn) && (((from - to) == 16) || ((from - to) == -16));
    bool enpassant = (piece::type(moved_piece) == piece::pawn) && (to == this->enpassant);
    bool castling = (piece::type(moved_piece) == piece::king) && (((from - to) == 2) || ((from - to) == -2));
    return move::move(from, to, moved_piece, target_piece, captured_piece, double_pawn_push, enpassant, castling, target_piece != moved_piece, false);
  };

  // push a uci move to the stack
  void push_uci(std::string uci) {
    this->make(this->from_uci(uci));
  };

  // place a piece on a square
  template <color_t color>
  void place_piece(piece_t piece, square_t square) {
    set_bit(this->bitboards[piece], square);
    set_bit(this->bitboards[piece::type(piece)], square);
    set_bit(this->bitboards[color], square);
    set_bit(this->bitboards[piece::none], square);
    this->pieces[square] = piece;
    this->zobrist.update_piece(piece, square);
  };

  void place_piece(piece_t piece, square_t square) {
    if (piece::color(piece) == color::white) {
      this->place_piece<color::white>(piece, square);
    } else if (piece::color(piece) == color::black) {
      this->place_piece<color::black>(piece, square);
    };
  };

  // remove a piece from a square
  template <color_t color>
  void remove_piece(square_t square) {
    piece_t piece = pieces[square];
    clear_bit(this->bitboards[piece], square);
    clear_bit(this->bitboards[piece::type(piece)], square);
    clear_bit(this->bitboards[color], square);
    clear_bit(this->bitboards[piece::none], square);
    this->pieces[square] = piece::none;
    this->zobrist.update_piece(piece, square);
  };

  void remove_piece(square_t square) {
    if (piece::color(this->pieces[square]) == color::white) {
      this->remove_piece<color::white>(square);
    } else if (piece::color(this->pieces[square]) == color::black) {
      this->remove_piece<color::black>(square);
    };
  };

  // make a move on the board
  template <color_t color>
  void make(move_t move) {
    constexpr color_t opponent = color::compiletime::opponent(color);
    constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);

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
    this->castling &= ~move::removed_castling(move);

    // remove enpassant square to be updated later
    this->enpassant = square::none;

    // change move clocks
    this->halfmove_clock = (this->halfmove_clock + 1) & (u16_t)((!(piece::type(move::moved_piece(move)) == piece::pawn || move::capture(move))) * 0xFFFF);
    if constexpr (color == color::black) {
      this->fullmove_clock++;
    };

    // do move
    square_t from = move::from(move);
    square_t to = move::to(move);
    this->remove_piece<color>(from);
    this->remove_piece<opponent>(to);
    this->place_piece<color>(move::target_piece(move), to);
    if (move::enpassant(move)) {
      if constexpr (color == color::white) {
        this->remove_piece<opponent>(to + 8);
      } else if constexpr (color == color::black) {
        this->remove_piece<opponent>(to - 8);
      };
    } else if (move::castling(move)) {
      this->remove_piece<color>(to + (to > from) * 3 - 2);
      this->place_piece<color>(rook, to - (to > from) * 2 + 1);
    } else if (move::double_pawn_push(move)) {
      if constexpr (color == color::white) {
        this->enpassant = to + 8;
      } else if constexpr (color == color::black) {
        this->enpassant = to - 8;
      };
    };

    // change turn
    this->turn = opponent;

    // add castling, enpassant square and the turn change to the hash
    this->zobrist.update_castling(this->castling);
    this->zobrist.update_enpassant(this->enpassant);
    this->zobrist.update_turn();
  };

  void make(move_t move) {
    if (this->turn == color::white) {
      this->make<color::white>(move);
    } else if (this->turn == color::black) {
      this->make<color::black>(move);
    };
  };

  // undo a move on the board
  template <color_t color>
  void unmake() {
    constexpr color_t opponent = color::compiletime::opponent(color);
    constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
    constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);

    // restore history
    undo_t undo = this->history.pop();
    move_t move = undo.move;
    this->castling = undo.castling;
    this->enpassant = undo.enpassant;
    this->halfmove_clock = undo.halfmove_clock;

    // change turn
    this->turn = color;

    // change move clocks
    if constexpr (color == color::black) {
      this->fullmove_clock--;
    };
    
    // undo move
    square_t from = move::from(move);
    square_t to = move::to(move);
    this->remove_piece<color>(to);
    this->place_piece<color>(move::moved_piece(move), from);
    if (move::enpassant(move)) {
      if constexpr (color == color::white) {
        this->place_piece<opponent>(opponent_pawn, to + 8);
      } else if constexpr (color == color::black) {
        this->place_piece<opponent>(opponent_pawn, to - 8);
      };
    } else if (move::castling(move)) {
      this->remove_piece<color>(to - (to > from) * 2 + 1);
      this->place_piece<color>(rook, to + (to > from) * 3 - 2);
    } else if (move::capture(move)) {
      this->place_piece<opponent>(move::captured_piece(move), to);
    };

    // restore hash
    this->zobrist.set(undo.hash);
  };

  void unmake() {
    if (this->turn == color::white) {
      this->unmake<color::black>();
    } else if (this->turn == color::black) {
      this->unmake<color::white>();
    };
  };

  // check if a position already exists in the history
  bool position_existed() {
    return this->history.count([this](undo_t undo) {return undo.hash == this->zobrist.hash;});
  };
};
};


#endif