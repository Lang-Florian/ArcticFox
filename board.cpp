#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __BOARD__MODULE__
#define __BOARD__MODULE__


#include <array>
#include <iostream>
#include <string>

#include "attack_rays.cpp"
#include "attacks.cpp"
#include "constants.cpp"
#include "hashtable.cpp"
#include "macros.cpp"
#include "moves.cpp"
#include "stack.cpp"
#include "time.cpp"
#include "types.cpp"
#include "zobrist.cpp"


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
    Zobrist zobrist;
    Stack<undo_t, MAX_GAME_LENGTH> history;

    std::array<bitboard_t, 2> attacks;
    std::array<bitboard_t, 2> attacks_no_king;
    std::array<bitboard_t, 2> bishop_pins;
    std::array<bitboard_t, 2> rook_pins;
    std::array<bitboard_t, 2> enpassant_pins;
    std::array<square_t, 2> king_square;
    std::array<bitboard_t, 2> king_bishop_ray;
    std::array<bitboard_t, 2> king_rook_ray;
    std::array<bitboard_t, 2> king_bishop_attack;
    std::array<bitboard_t, 2> king_rook_attack;
    PerftHashTable perft_hash_table;

    // initialize the board
    Board(std::string fen=fen::startpos) {
      this->set_fen(fen);
    };

    // set the board to a given fen string
    void set_fen(std::string fen=fen::startpos) {
      // clear lists and stacks
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
      std::string string = "";
      int index = 0;
      for (auto square : square::all_squares) {
        if (square % 8 == 0 && square != 0) {
          if (index != 0) {
            string += std::to_string(index);
            index = 0;
          };
          string += "/";
        };
        if (this->pieces[square] == piece::none) {
          index++;
        } else {
          if (index != 0) {
            string += std::to_string(index);
            index = 0;
          };
          string += piece::to_string(this->pieces[square]);
        };
      };
      if (index != 0) string += std::to_string(index);
      string += " " + color::to_string(this->turn);
      string += " " + castling::to_string(this->castling);
      string += " " + square::to_string(this->enpassant);
      string += " " + std::to_string(this->halfmove_clock);
      string += " " + std::to_string(this->fullmove_clock);
      return string;
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

    void place_piece(piece_t piece, square_t square) {
      if (piece::color(piece) == color::white) {
        this->place_piece<color::white>(piece, square);
      } else if (piece::color(piece) == color::black) {
        this->place_piece<color::black>(piece, square);
      };
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

    void make(move_t move) {
      if (this->turn == color::white) {
        this->make<color::white>(move);
      } else if (this->turn == color::black) {
        this->make<color::black>(move);
      };
    };

    void unmake() {
      if (this->turn == color::white) {
        this->unmake<color::black>();
      } else if (this->turn == color::black) {
        this->unmake<color::white>();
      };
    };

    // get all the attacks on a square
    bitboard_t attackers(square_t square) {
      return (
        (attack<piece::black_pawn>(square) & this->bitboards[piece::white_pawn]) |
        (attack<piece::white_pawn>(square) & this->bitboards[piece::black_pawn]) |
        (attack<piece::knight>(square) & this->bitboards[piece::knight]) |
        (attack<piece::bishop>(square, this->bitboards[piece::none]) & this->bitboards[piece::bishop]) |
        (attack<piece::rook>(square, this->bitboards[piece::none]) & this->bitboards[piece::rook]) |
        (attack<piece::queen>(square, this->bitboards[piece::none]) & this->bitboards[piece::queen]) |
        (attack<piece::king>(square) & this->bitboards[piece::king])
      );
    };

    template <color_t color>
    bitboard_t attackers(square_t square) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
      return (
        (attack<opponent_pawn>(square) & this->bitboards[pawn]) |
        (attack<piece::knight>(square) & this->bitboards[knight]) |
        (attack<piece::bishop>(square, this->bitboards[piece::none]) & this->bitboards[bishop]) |
        (attack<piece::rook>(square, this->bitboards[piece::none]) & this->bitboards[rook]) |
        (attack<piece::queen>(square, this->bitboards[piece::none]) & this->bitboards[queen]) |
        (attack<piece::king>(square) & this->bitboards[king])
      );
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

    // see if current position is check
    template <color_t color>
    bool is_check() {
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      square_t king_square = get_lsb(this->bitboards[king]);
      return this->attackers<opponent>(king_square);
    };

    bool is_check() {
      if (this->turn == color::white) {
        return this->is_check<color::white>();
      } else if (this->turn == color::black) {
        return this->is_check<color::black>();
      };
      return false;
    };

    template <color_t color>
    void update_attacks() {
      constexpr std::array<piece_t, 6> all_pieces = piece::all_pieces_by_color[color];
      constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, color::compiletime::opponent(color));
      this->attacks[color] = bitboard::none;
      this->attacks_no_king[color] = bitboard::none;
      for (piece_t piece : all_pieces) {
        bitboard_t pieces = this->bitboards[piece];
        while (pieces) {
          square_t square = pop_lsb(pieces);
          this->attacks[color] |= attack_array[piece](square, this->bitboards[piece::none]);
          this->attacks_no_king[color] |= attack_array[piece](square, this->bitboards[piece::none] & ~this->bitboards[opponent_king]);
        };
      };
    };

    template <color_t color>
    void update_pins() {
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t opponent_bishop = piece::compiletime::to_color(piece::bishop, opponent);
      constexpr piece_t opponent_rook = piece::compiletime::to_color(piece::rook, opponent);
      constexpr piece_t opponent_queen = piece::compiletime::to_color(piece::queen, opponent);
      square_t king_square = get_lsb(this->bitboards[king]);
      bitboard_t king_bishop_ray = attack_ray::bishop[king_square];
      bitboard_t king_rook_ray = attack_ray::rook[king_square];
      bitboard_t occupancy = this->bitboards[piece::none];
      bitboard_t king_bishop_attack = attack<piece::bishop>(king_square, occupancy);
      bitboard_t king_rook_attack = attack<piece::rook>(king_square, occupancy);
      // get all bishop pinned pieces
      this->bishop_pins[color] = bitboard::none;
      bitboard_t bishop_attackers = (this->bitboards[opponent_bishop] | this->bitboards[opponent_queen]) & king_bishop_ray;
      while (bishop_attackers) {
        square_t square = pop_lsb(bishop_attackers);
        bishop_pins[color] |= attack<opponent_bishop>(square, occupancy) & king_bishop_attack;
      };
      // get all rook pinned pieces
      this->rook_pins[color] = bitboard::none;
      bitboard_t rook_attackers = (this->bitboards[opponent_rook] | this->bitboards[opponent_queen]) & king_rook_ray;
      while (rook_attackers) {
        square_t square = pop_lsb(rook_attackers);
        this->rook_pins[color] |= attack<opponent_rook>(square, occupancy) & king_rook_attack;
      };
      // get all enpassant pinned pieces
      this->enpassant_pins[color] = bitboard::full;
      if (this->enpassant != square::none && color == this->turn) {
        this->enpassant_pins[color] = bitboard::none;
        bitboard_t enpassant_pawns = bitboard::none;
        if constexpr (color == color::white) {
          enpassant_pawns = bitboard(this->enpassant) << 8;
        } else {
          enpassant_pawns = bitboard(this->enpassant) >> 8;
        };
        bitboard_t enpassant_attackers = (this->bitboards[opponent_rook] | this->bitboards[opponent_queen]) & king_rook_ray;
        while (enpassant_attackers) {
          square_t square = pop_lsb(enpassant_attackers);
          this->enpassant_pins[color] |= attack<opponent_rook>(square, occupancy & ~enpassant_pawns) & attack<opponent_rook>(king_square, occupancy & ~enpassant_pawns);
        };
      };
    };

    template <color_t color>
    void update_king() {
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      bitboard_t occupancy = this->bitboards[piece::none];
      this->king_square[color] = get_lsb(this->bitboards[king]);
      this->king_bishop_ray[color] = attack_ray::bishop[this->king_square[color]];
      this->king_rook_ray[color] = attack_ray::rook[this->king_square[color]];
      this->king_bishop_attack[color] = attack<piece::bishop>(this->king_square[color], occupancy);
      this->king_rook_attack[color] = attack<piece::rook>(this->king_square[color], occupancy);
    };

    // generate all moves for a given generation type and color
    template <color_t color, typename T=move_stack_t>
    T generate() {
      // important constants and variables
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t opponent_bishop = piece::compiletime::to_color(piece::bishop, opponent);
      constexpr piece_t opponent_rook = piece::compiletime::to_color(piece::rook, opponent);
      constexpr piece_t opponent_queen = piece::compiletime::to_color(piece::queen, opponent);

      T moves;
      if constexpr (typeid(T) == typeid(move_stack_t)) {
        moves.clear();
      } else {
        moves = 0;
      };

      this->update_attacks<opponent>();
      this->update_pins<color>();
      this->update_king<color>();

      // add all possible moves to the move stack
      bitboard_t evasion_target = bitboard::none;
      bitboard_t checkers = this->attackers<opponent>(this->king_square[color]);
      if (popcount(checkers) > 1) {
        this->add_king_moves<color>(moves);
        return moves;
      } else if (checkers) {
        square_t checker_square = get_lsb(checkers);
        switch (this->pieces[checker_square]) {
          case opponent_bishop:
            evasion_target = bitboard(checker_square) | (this->king_bishop_attack[color] & attack<opponent_bishop>(checker_square, this->bitboards[color::none]));
            break;
          case opponent_rook:
            evasion_target = bitboard(checker_square) | (this->king_rook_attack[color] & attack<opponent_rook>(checker_square, this->bitboards[color::none]));
            break;
          case opponent_queen:
            if (this->king_bishop_attack[color] & this->bitboards[opponent_queen]) {
              evasion_target = bitboard(checker_square) | (this->king_bishop_attack[color] & attack<opponent_bishop>(checker_square, this->bitboards[color::none]));
            } else if (this->king_rook_attack[color] & this->bitboards[opponent_queen]) {
              evasion_target = bitboard(checker_square) | (this->king_rook_attack[color] & attack<opponent_rook>(checker_square, this->bitboards[color::none]));
            };
            break;
          default:
            evasion_target = bitboard(checker_square);
            break;
        };
      } else {
        evasion_target = bitboard::full;
        this->add_castling_moves<color>(moves);
      };
      this->add_king_moves<color>(moves);
      this->add_enpassant_moves<color>(moves, evasion_target);
      this->add_pawn_push_moves<color>(moves, evasion_target);
      this->add_pawn_capture_moves<color>(moves, evasion_target);
      this->add_knight_moves<color>(moves, evasion_target);
      this->add_bishop_moves<color>(moves, evasion_target);
      this->add_rook_moves<color>(moves, evasion_target);
      this->add_queen_moves<color>(moves, evasion_target);
      return moves;
    };

    // get all the legal moves for the current turn
    move_stack_t legal_moves() {
      if (this->turn == color::white) {
        return this->generate<color::white, move_stack_t>();
      } else {
        return this->generate<color::black, move_stack_t>();
      };
    };

    template <color_t color>
    u64_t perft(int depth) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      if (depth == 0) {
        return 1;
      } else if (depth == 1) {
        u64_t count = this->generate<color, int>();
        this->perft_hash_table.set(this->zobrist.hash, 1, count);
        return count;
      } else if (depth == 2) {
        u64_t count = 0;
        auto moves = this->generate<color>();
        for (auto move : moves) {
          this->make<color>(move);
          count += this->generate<opponent, int>();
          this->unmake<color>();
        };
        this->perft_hash_table.set(this->zobrist.hash, 2, count);
        return count;
      };
      if (this->perft_hash_table.contains(this->zobrist.hash, depth))
        return this->perft_hash_table.get(this->zobrist.hash);
      u64_t count = 0;
      auto moves = this->generate<color>();
      for (auto move : moves) {
        this->make<color>(move);
        count += this->perft<opponent>(depth - 1);
        this->unmake<color>();
      };
      this->perft_hash_table.set(this->zobrist.hash, depth, count);
      return count;
    };

    u64_t perft(int depth) {
      if (this->turn == color::white)
        return this->perft<color::white>(depth);
      else
        return this->perft<color::black>(depth);
    };

    // get the outcome of the game
    outcome_t outcome() {
      bool has_legal_moves = false;
      if (this->turn == color::white)
        has_legal_moves = this->generate<color::white, u64_t>();
      else
        has_legal_moves = this->generate<color::black, u64_t>();
      return this->outcome(has_legal_moves);
    };

    // get the outcome of the game given you know if there are any legal moves
    outcome_t outcome(bool has_legal_moves) {
      if (!has_legal_moves) {
        if (this->is_check()) {
          return outcome::checkmate_for(color::opponent(this->turn));
        } else {
          return outcome::stalemate;
        };
      } else if (this->halfmove_clock >= 100) {
        return outcome::fifty_move_rule;
      } else if (this->history.count([this](undo_t undo) {return undo.hash == this->zobrist.hash;}) >= 2) {
        return outcome::threefold_repetition;
      };
      return outcome::none;
    };

    // add legal castling moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_castling_moves(T& moves) {
      bitboard_t occupancy = this->bitboards[color::none];
      if constexpr (color == color::white) {
        bool castle_king = (
          !(this->attacks[color::black] & castling::white_king_attack_mask) &&
          !(occupancy & castling::white_king_piece_mask) &&
          (this->castling & castling::white_king)
        );
        bool castle_queen = (
          !(this->attacks[color::black] & castling::white_queen_attack_mask) &&
          !(occupancy & castling::white_queen_piece_mask) &&
          (this->castling & castling::white_queen)
        );
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          moves.push(move::move(square::e1, square::g1, piece::white_king, piece::white_king, piece::none, false, false, true, false, false));
          moves.pop(!castle_king);
          moves.push(move::move(square::e1, square::c1, piece::white_king, piece::white_king, piece::none, false, false, true, false, false));
          moves.pop(!castle_queen);
        } else {
          moves += castle_king + castle_queen;
        };
      } else if constexpr (color == color::black) {
        bool castle_king = (
          !(this->attacks[color::white] & castling::black_king_attack_mask) &&
          !(occupancy & castling::black_king_piece_mask) &&
          (this->castling & castling::black_king)
        );
        bool castle_queen = (
          !(this->attacks[color::white] & castling::black_queen_attack_mask) &&
          !(occupancy & castling::black_queen_piece_mask) &&
          (this->castling & castling::black_queen)
        );
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          moves.push(move::move(square::e8, square::g8, piece::black_king, piece::black_king, piece::none, false, false, true, false, false));
          moves.pop(!castle_king);
          moves.push(move::move(square::e8, square::c8, piece::black_king, piece::black_king, piece::none, false, false, true, false, false));
          moves.pop(!castle_queen);
        } else {
          moves += castle_king + castle_queen;
        };
      };
    };

    // add legal king moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_king_moves(T& moves) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t possible_to = attack<king>(this->king_square[color]) & ~occupancy_color & ~this->attacks_no_king[opponent];
      if constexpr (typeid(T) == typeid(move_stack_t)) {
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(this->king_square[color], to, king, king, this->pieces[to], false, false, false, false, false));
        };
      } else {
        moves += popcount(possible_to);
      };      
    };

    // add legal enpassant moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_enpassant_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
      constexpr int enpassant_offset = (color == color::white ? 8 : -8);
      square_t checker_square = get_lsb(this->attackers<opponent>(this->king_square[color]));
      if (this->enpassant == square::none) return;
      bitboard_t bishop_pinned_pawns = (
        this->bitboards[pawn] &
        attack<opponent_pawn>(this->enpassant) &
        ~this->enpassant_pins[color] &
        this->bishop_pins[color] &
        ~this->rook_pins[color]
      );
      while (bishop_pinned_pawns) {
        square_t from = pop_lsb(bishop_pinned_pawns);
        bool do_enpassant = (
          (this->king_bishop_ray[color] & bitboard(this->enpassant)) &&
          (
            (evasion_target & bitboard(this->enpassant)) ||
            (this->enpassant + enpassant_offset == checker_square)
          )
        );
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          moves.push(move::move(from, this->enpassant, pawn, pawn, piece::none, false, true, false, false, false));
          moves.pop(!do_enpassant);
        } else {
          moves += do_enpassant;
        };
      };
      bitboard_t free_pawns = (
        this->bitboards[pawn] &
        attack<opponent_pawn>(this->enpassant) &
        ~this->enpassant_pins[color] &
        ~this->bishop_pins[color] &
        ~this->rook_pins[color]
      );
      while (free_pawns) {
        bool do_enpassant = (
          (evasion_target & bitboard(this->enpassant)) ||
          (this->enpassant + enpassant_offset == checker_square)
        );
        square_t from = pop_lsb(free_pawns);
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          moves.push(move::move(from, this->enpassant, pawn, pawn, piece::none, false, true, false, false, false));
          moves.pop(!do_enpassant);
        } else {
          moves += do_enpassant;
        };
      };
    };

    // add legal pawn push moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_pawn_push_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr int push_offset = (color == color::white ? -8 : 8);
      constexpr int doublepush_offset = 2 * push_offset;
      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t free_pawns = this->bitboards[pawn] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      bitboard_t rook_pinned_pawns = this->bitboards[pawn] & ~this->bishop_pins[color] & this->rook_pins[color];
      bitboard_t pushable_pawns;
      bitboard_t doublepushable_pawns;
      bitboard_t promoting_pawns;
      if constexpr (color == color::white) {
        pushable_pawns = (
          (free_pawns & ~(occupancy << 8) & (evasion_target << 8) & ~bitboard::rank_7) |
          (rook_pinned_pawns & ~(occupancy << 8) & (evasion_target << 8) & (this->king_rook_ray[color] << 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(occupancy << 8) & ~(occupancy << 16) & (evasion_target << 16) & bitboard::rank_2) |
          (rook_pinned_pawns & ~(occupancy << 8) & ~(occupancy << 16) & (evasion_target << 16) & bitboard::rank_2 & (this->king_rook_ray[color] << 16))
        );
        promoting_pawns = free_pawns & ~(occupancy << 8) & (evasion_target << 8) & bitboard::rank_7;
      } else if constexpr (color == color::black) {
        pushable_pawns = (
          (free_pawns & ~(occupancy >> 8) & (evasion_target >> 8) & ~bitboard::rank_2) |
          (rook_pinned_pawns & ~(occupancy >> 8) & (evasion_target >> 8) & (this->king_rook_ray[color] >> 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(occupancy >> 8) & ~(occupancy >> 16) & (evasion_target >> 16) & bitboard::rank_7) |
          (rook_pinned_pawns & ~(occupancy >> 8) & ~(occupancy >> 16) & (evasion_target >> 16) & bitboard::rank_7 & (this->king_rook_ray[color] >> 16))
        );
        promoting_pawns = free_pawns & ~(occupancy >> 8) & (evasion_target >> 8) & bitboard::rank_2;
      } else return;
      if constexpr (typeid(T) == typeid(move_stack_t)) {
        while (pushable_pawns) {
          square_t from = pop_lsb(pushable_pawns);
          square_t to = from + push_offset;
          moves.push(move::move(from, to, pawn, pawn, piece::none, false, false, false, false, false));
        };
        while (doublepushable_pawns) {
          square_t from = pop_lsb(doublepushable_pawns);
          square_t to = from + doublepush_offset;
          moves.push(move::move(from, to, pawn, pawn, piece::none, true, false, false, false, false));
        };
        while (promoting_pawns) {
          square_t from = pop_lsb(promoting_pawns);
          square_t to = from + push_offset;
          moves.push(move::move(from, to, pawn, knight, piece::none, false, false, false, true, false));
          moves.push(move::move(from, to, pawn, bishop, piece::none, false, false, false, true, false));
          moves.push(move::move(from, to, pawn, rook, piece::none, false, false, false, true, false));
          moves.push(move::move(from, to, pawn, queen, piece::none, false, false, false, true, false));
        };
      } else {
        moves += popcount(pushable_pawns);
        moves += popcount(doublepushable_pawns);
        moves += popcount(promoting_pawns) << 2;
      };
    };

    // add legal pawn capture moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_pawn_capture_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr bitboard_t promotion_rank = (color == color::white) ? bitboard::rank_7 : bitboard::rank_2;
      bitboard_t occupancy_opponent = this->bitboards[opponent];
      bitboard_t free_pawns = this->bitboards[pawn] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      bitboard_t bishop_pinned_pawns = this->bitboards[pawn] & this->bishop_pins[color] & ~this->rook_pins[color];
      bitboard_t promoting_free_pawns = free_pawns & promotion_rank;
      bitboard_t not_promoting_free_pawns = free_pawns & ~promotion_rank;
      bitboard_t promoting_bishop_pinned_pawns = bishop_pinned_pawns & promotion_rank;
      bitboard_t not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~promotion_rank;
      while (promoting_free_pawns) {
        square_t from = pop_lsb(promoting_free_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false, true, false));
          };
        } else {
          moves += popcount(possible_to) << 2;
        };
      };
      while (not_promoting_free_pawns) {
        square_t from = pop_lsb(not_promoting_free_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, pawn, pawn, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
      while (promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & this->king_bishop_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false, true, false));
            moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false, true, false));
          };
        } else {
          moves += popcount(possible_to) << 2;
        };
      };
      while (not_promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(not_promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & this->king_bishop_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, pawn, pawn, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
    };

    // add legal knight moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_knight_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t free_knights = this->bitboards[knight] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      while (free_knights) {
        square_t from = pop_lsb(free_knights);
        bitboard_t possible_to = attack<knight>(from) & ~occupancy_color & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, knight, knight, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
    };

    // add legal bishop moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_bishop_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t bishop_pinned_bishops = this->bitboards[bishop] & this->bishop_pins[color] & ~this->rook_pins[color];
      while (bishop_pinned_bishops) {
        square_t from = pop_lsb(bishop_pinned_bishops);
        bitboard_t possible_to = attack<bishop>(from, occupancy) & ~occupancy_color & this->king_bishop_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, bishop, bishop, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
      bitboard_t free_bishops = this->bitboards[bishop] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = attack<bishop>(from, occupancy) & ~occupancy_color & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, bishop, bishop, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
    };

    // add legal rook moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_rook_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t rook_pinned_rooks = this->bitboards[rook] & ~this->bishop_pins[color] & this->rook_pins[color];
      while (rook_pinned_rooks) {
        square_t from = pop_lsb(rook_pinned_rooks);
        bitboard_t possible_to = attack<rook>(from, occupancy) & ~occupancy_color & this->king_rook_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, rook, rook, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
      bitboard_t free_rooks = this->bitboards[rook] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = attack<rook>(from, occupancy) & ~occupancy_color & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, rook, rook, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
    };

    // add legal queen moves to the move stack
    template <color_t color, typename T=move_stack_t>
    void add_queen_moves(T& moves, const bitboard_t& evasion_target=bitboard::full) {
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t bishop_pinned_queens = this->bitboards[queen] & this->bishop_pins[color] & ~this->rook_pins[color];
      while (bishop_pinned_queens) {
        square_t from = pop_lsb(bishop_pinned_queens);
        bitboard_t possible_to = attack<piece::bishop>(from, occupancy) & ~occupancy_color & this->king_bishop_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
      bitboard_t rook_pinned_queens = this->bitboards[queen] & ~this->bishop_pins[color] & this->rook_pins[color];
      while (rook_pinned_queens) {
        square_t from = pop_lsb(rook_pinned_queens);
        bitboard_t possible_to = attack<piece::rook>(from, occupancy) & ~occupancy_color & this->king_rook_ray[color] & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
      bitboard_t free_queens = this->bitboards[queen] & ~this->bishop_pins[color] & ~this->rook_pins[color];
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = attack<piece::queen>(from, occupancy) & ~occupancy_color & evasion_target;
        if constexpr (typeid(T) == typeid(move_stack_t)) {
          while (possible_to) {
            square_t to = pop_lsb(possible_to);
            moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
          };
        } else {
          moves += popcount(possible_to);
        };
      };
    };
};


#endif // __BOARD__MODULE__