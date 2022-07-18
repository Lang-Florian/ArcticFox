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


struct perft_t {
  move_t move;
  u64_t count;
  u64_t time;
  
  float mnps() {
    return 1000 * (float)count / (float)time;
  };
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

    // initialize the board
    Board(std::string fen=fen::starting) {
      this->set_fen(fen);
    };

    // set the board to a given fen string
    void set_fen(std::string fen=fen::starting) {
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
      for (square_t square = 0; square < 64; square++) {
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

    // get all the legal moves for the current turn
    Stack<move_t, MAX_MOVE_LENGTH> legal_moves() {
      if (this->turn == color::white) {
        return this->generate<legal, color::white>();
      } else {
        return this->generate<legal, color::black>();
      };
    };

    // do a perft
    Stack<perft_t, MAX_MOVE_LENGTH> perft(int depth, gen_t gen=legal) {
      if (this->turn == color::white && gen == legal) {
        return this->perft<legal, color::white>(depth);
      } else if (this->turn == color::white && gen == capture) {
        return this->perft<capture, color::white>(depth);
      } else if (this->turn == color::black && gen == legal) {
        return this->perft<legal, color::black>(depth);
      }  else if (this->turn == color::black && gen == capture) {
        return this->perft<capture, color::black>(depth);
      };
    };

    // see if current position is check
    bool is_check() {
      square_t king_square = get_lsb(this->bitboards[piece::to_color(piece::king, this->turn)]);
      return (
        this->attackers(king_square) &
        this->bitboards[color::opponent(this->turn)]
      );
    };

    // get the outcome of the game
    outcome_t outcome() {
      return this->outcome(!this->legal_moves().empty());
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

  private:
    
    // do a perft for a given color and generation type
    template <gen_t gen, color_t color>
    Stack<perft_t, MAX_MOVE_LENGTH> perft(int depth) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      Stack<perft_t, MAX_MOVE_LENGTH> perft_result;
      auto moves = this->generate<legal, color>();
      for (auto move : moves) {
        u64_t start = timing::nanoseconds();
        this->make(move);
        u64_t count = perft<gen, opponent, true>(depth - 1);
        this->unmake();
        u64_t end = timing::nanoseconds();
        perft_result.push(perft_t {move, count, end - start});
      };
      return perft_result;
    };

    // do a perft for a given color and generation type but only count
    template <gen_t gen, color_t color, bool count_only>
    u64_t perft(int depth) {
      constexpr color_t opponent = color::compiletime::opponent(color);
      if (depth == 1) return this->generate<gen, color>().size();
      u64_t count = 0;
      auto moves = this->generate<legal, color>();
      for (auto move : moves) {
        this->make(move);
        count += perft<gen, opponent, true>(depth - 1);
        this->unmake();
      };
      return count;
    };

    // place a piece on a square
    void place_piece(piece_t piece, square_t square) {
      set_bit(this->bitboards[piece], square);
      set_bit(this->bitboards[piece::type(piece)], square);
      set_bit(this->bitboards[piece::color(piece)], square);
      set_bit(this->bitboards[piece::none], square);
      this->pieces[square] = piece;
      this->zobrist.update_piece(piece, square);
    };

    // remove a piece from a square
    void remove_piece(square_t square) {
      piece_t piece = pieces[square];
      clear_bit(this->bitboards[piece], square);
      clear_bit(this->bitboards[piece::type(piece)], square);
      clear_bit(this->bitboards[piece::color(piece)], square);
      clear_bit(this->bitboards[piece::none], square);
      this->pieces[square] = piece::none;
      this->zobrist.update_piece(piece, square);
    };

     // make a move on the board
    void make(move_t move) {
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
      this->fullmove_clock += (this->turn == color::black);

      // do move
      square_t from = move::from(move);
      square_t to = move::to(move);

      this->remove_piece(from);
      this->remove_piece(to);
      this->place_piece(move::target_piece(move), to);
      if (move::enpassant(move)) {
        this->remove_piece(to - (8 - 16 * (this->turn == color::white)));
      } else if (move::castling(move)) {
        this->remove_piece(to + (to > from) * 3 - 2);
        this->place_piece(piece::to_color(piece::rook, this->turn), to - (to > from) * 2 + 1);
      } else if (move::double_pawn_push(move)) {
        this->enpassant = to - (8 - 16 * (this->turn == color::white));
      };

      // change turn
      this->turn = color::opponent(this->turn);

      // add castling, enpassant square and the turn change to the hash
      this->zobrist.update_castling(this->castling);
      this->zobrist.update_enpassant(this->enpassant);
      this->zobrist.update_turn();
    };

    // undo a move on the board
    void unmake() {
      // restore history
      undo_t undo = this->history.pop();
      move_t move = undo.move;
      this->castling = undo.castling;
      this->enpassant = undo.enpassant;
      this->halfmove_clock = undo.halfmove_clock;

      // change turn
      this->turn = color::opponent(this->turn);

      // change move clocks
      this->fullmove_clock -= (this->turn == color::black);
      
      // undo move
      square_t from = move::from(move);
      square_t to = move::to(move);

      this->remove_piece(to);
      this->place_piece(move::moved_piece(move), from);
      if (move::enpassant(move)) {
        this->place_piece(piece::to_color(piece::pawn, color::opponent(this->turn)), to - (8 - 16 * (this->turn == color::white)));
      } else if (move::castling(move)) {
        this->remove_piece(to - (to > from) * 2 + 1);
        this->place_piece(piece::to_color(piece::rook, this->turn), to + (to > from) * 3 - 2);
      } else if (move::capture(move)) {
        this->place_piece(move::captured_piece(move), to);
      };

      // restore hash
      this->zobrist.set(undo.hash);
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






    // generate all moves for a given generation type and color
    template <gen_t gen, color_t color>
    Stack<move_t, MAX_MOVE_LENGTH> generate() {
      Stack<move_t, MAX_MOVE_LENGTH> moves;
      moves.clear();

      // important constants and variables
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
      constexpr piece_t opponent_knight = piece::compiletime::to_color(piece::knight, opponent);
      constexpr piece_t opponent_bishop = piece::compiletime::to_color(piece::bishop, opponent);
      constexpr piece_t opponent_rook = piece::compiletime::to_color(piece::rook, opponent);
      constexpr piece_t opponent_queen = piece::compiletime::to_color(piece::queen, opponent);
      constexpr piece_t opponent_king = piece::compiletime::to_color(piece::king, opponent);
      square_t king_square = get_lsb(this->bitboards[king]);
      square_t opponent_king_square = get_lsb(this->bitboards[opponent_king]);
      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t occupancy_opponent = this->bitboards[opponent];
      bitboard_t occupancy_no_king = occupancy & ~this->bitboards[king];
      bitboard_t king_bishop_ray = attack_ray::bishop[king_square];
      bitboard_t king_rook_ray = attack_ray::rook[king_square];
      bitboard_t king_bishop_attack = attack<piece::bishop>(king_square, occupancy);
      bitboard_t king_rook_attack = attack<piece::rook>(king_square, occupancy);

      // get all attacked squares and squares which are attacked if you ignore your own king
      bitboard_t attacked = bitboard::none;
      bitboard_t attacked_no_king = bitboard::none;
      bitboard_t opponent_pawns = this->bitboards[opponent_pawn];
      while (opponent_pawns) {
        square_t square = pop_lsb(opponent_pawns);
        attacked |= attack<opponent_pawn>(square);
        attacked_no_king |= attack<opponent_pawn>(square);
      };
      bitboard_t opponent_knights = this->bitboards[opponent_knight];
      while (opponent_knights) {
        square_t square = pop_lsb(opponent_knights);
        attacked |= attack<opponent_knight>(square);
        attacked_no_king |= attack<opponent_knight>(square);
      };
      bitboard_t opponent_bishops = this->bitboards[opponent_bishop];
      while (opponent_bishops) {
        square_t square = pop_lsb(opponent_bishops);
        attacked |= attack<opponent_bishop>(square, occupancy);
        attacked_no_king |= attack<opponent_bishop>(square, occupancy_no_king);
      };
      bitboard_t opponent_rooks = this->bitboards[opponent_rook];
      while (opponent_rooks) {
        square_t square = pop_lsb(opponent_rooks);
        attacked |= attack<opponent_rook>(square, occupancy);
        attacked_no_king |= attack<opponent_rook>(square, occupancy_no_king);
      };
      bitboard_t opponent_queens = this->bitboards[opponent_queen];
      while (opponent_queens) {
        square_t square = pop_lsb(opponent_queens);
        attacked |= attack<opponent_queen>(square, occupancy);
        attacked_no_king |= attack<opponent_queen>(square, occupancy_no_king);
      };
      bitboard_t opponent_kings = this->bitboards[opponent_king];
      square_t square = pop_lsb(opponent_kings);
      attacked |= attack<opponent_king>(square);
      attacked_no_king |= attack<opponent_king>(square);

      // get all bishop pinned pieces
      bitboard_t bishop_pinned = bitboard::none;
      bitboard_t bishop_attackers = (this->bitboards[opponent_bishop] | this->bitboards[opponent_queen]) & king_bishop_ray;
      while (bishop_attackers) {
        square_t square = pop_lsb(bishop_attackers);
        bishop_pinned |= attack<opponent_bishop>(square, occupancy) & king_bishop_attack;
      };

      // get all rook pinned pieces
      bitboard_t rook_pinned = bitboard::none;
      bitboard_t rook_attackers = (this->bitboards[opponent_rook] | this->bitboards[opponent_queen]) & king_rook_ray;
      while (rook_attackers) {
        square_t square = pop_lsb(rook_attackers);
        rook_pinned |= attack<opponent_rook>(square, occupancy) & king_rook_attack;
      };

      // get all enpassant pinned pieces
      bitboard_t enpassant_pinned = bitboard::none;
      if (this->enpassant != square::none) {
        bitboard_t enpassant_pawns = bitboard::none;
        if constexpr (color == color::white) {
          enpassant_pawns = bitboard(this->enpassant) << 8;
        } else {
          enpassant_pawns = bitboard(this->enpassant) >> 8;
        };
        bitboard_t enpassant_attackers = (this->bitboards[opponent_rook] | this->bitboards[opponent_queen]) & king_rook_ray;
        while (enpassant_attackers) {
          square_t square = pop_lsb(enpassant_attackers);
          enpassant_pinned |= attack<opponent_rook>(square, occupancy & ~enpassant_pawns) & attack<opponent_rook>(king_square, occupancy & ~enpassant_pawns) & this->bitboards[pawn];
        };
      };

      // generate all possible targets for the given generation type
      bitboard_t pawn_target = bitboard::none;
      bitboard_t knight_target = bitboard::none;
      bitboard_t bishop_target = bitboard::none;
      bitboard_t rook_target = bitboard::none;
      bitboard_t queen_target = bitboard::none;
      bitboard_t king_target = bitboard::none;
      if constexpr (gen == capture) {
        pawn_target |= occupancy_opponent | bitboard(this->enpassant);
        knight_target |= occupancy_opponent;
        bishop_target |= occupancy_opponent;
        rook_target |= occupancy_opponent;
        queen_target |= occupancy_opponent;
        king_target |= occupancy_opponent;
      } else if constexpr (gen == legal) {
        pawn_target = bitboard::full;
        knight_target = bitboard::full;
        bishop_target = bitboard::full;
        rook_target = bitboard::full;
        queen_target = bitboard::full;
        king_target = bitboard::full;
      // checks need more work: discovered checks
      // } else if constexpr (gen == check) {
      //   pawn_target |= attack<opponent_pawn>(opponent_king_square);
      //   knight_target |= attack<knight>(opponent_king_square);
      //   bishop_target |= attack<bishop>(opponent_king_square, occupancy);
      //   rook_target |= attack<rook>(opponent_king_square, occupancy);
      //   queen_target |= attack<queen>(opponent_king_square, occupancy);
      // } else if constexpr (gen == check_or_capture) {
      //   pawn_target |= occupancy_opponent | attack<opponent_pawn>(opponent_king_square);
      //   knight_target |= occupancy_opponent | attack<knight>(opponent_king_square);
      //   bishop_target |= occupancy_opponent | attack<bishop>(opponent_king_square, occupancy);
      //   rook_target |= occupancy_opponent | attack<rook>(opponent_king_square, occupancy);
      //   queen_target |= occupancy_opponent | attack<queen>(opponent_king_square, occupancy);
      //   king_target |= occupancy_opponent;
      };

      // add all possible moves to the move stack
      bitboard_t checkers = this->attackers(king_square) & this->bitboards[opponent];
      square_t checker_square = get_lsb(checkers);
      if (checkers) {
        if (popcount(checkers) > 1) {
          this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color, king_target);
          return moves;
        };
        piece_t checker = this->pieces[checker_square];
        bitboard_t target = (
          bitboard(checker_square) |
          ((king_bishop_attack & attack<opponent_bishop>(checker_square, occupancy)) * (checker == opponent_bishop)) |
          ((king_rook_attack & attack<opponent_rook>(checker_square, occupancy)) * (checker == opponent_rook)) |
          ((king_bishop_attack & attack<opponent_bishop>(checker_square, occupancy)) * (checker == opponent_queen && (king_bishop_attack & this->bitboards[opponent_queen]))) |
          ((king_rook_attack & attack<opponent_rook>(checker_square, occupancy)) * (checker == opponent_queen && (king_rook_attack & this->bitboards[opponent_queen])))
        );
        this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color, king_target);
        this->add_enpassant_moves<color>(moves, bishop_pinned, rook_pinned, enpassant_pinned, king_bishop_ray, checker_square, target & pawn_target);
        this->add_pawn_push_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, target & pawn_target);
        this->add_pawn_capture_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy_opponent, target & pawn_target);
        this->add_knight_moves<color>(moves, bishop_pinned, rook_pinned, occupancy_color, target & knight_target);
        this->add_bishop_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy, occupancy_color, target & bishop_target);
        this->add_rook_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, occupancy_color, target & rook_target);
        this->add_queen_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, king_rook_ray, occupancy, occupancy_color, target & queen_target);
        return moves;
      };
      this->add_castling_moves<color>(moves, attacked, occupancy, rook_target);
      this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color, king_target);
      this->add_enpassant_moves<color>(moves, bishop_pinned, rook_pinned, enpassant_pinned, king_bishop_ray, checker_square, pawn_target);
      this->add_pawn_push_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, pawn_target);
      this->add_pawn_capture_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy_opponent, pawn_target);
      this->add_knight_moves<color>(moves, bishop_pinned, rook_pinned, occupancy_color, knight_target);
      this->add_bishop_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy, occupancy_color, bishop_target);
      this->add_rook_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, occupancy_color, rook_target);
      this->add_queen_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, king_rook_ray, occupancy, occupancy_color, queen_target);
      return moves;
    };

    // add legal castling moves to the move stack
    template <color_t color>
    void add_castling_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& attacked,
      const bitboard_t& occupancy,
      const bitboard_t& target=bitboard::full
    ) {
      if constexpr (color == color::white) {
        moves.push(move::move(square::e1, square::g1, piece::white_king, piece::white_king, piece::none, false, false, true, false, false));
        moves.pop(
          (attacked & castling::white_king_attack_mask) ||
          (occupancy & castling::white_king_piece_mask) ||
          !(this->castling & castling::white_king) ||
          !(get_bit(target, square::f1))
        );
        moves.push(move::move(square::e1, square::c1, piece::white_king, piece::white_king, piece::none, false, false, true, false, false));
        moves.pop(
          (attacked & castling::white_queen_attack_mask) ||
          (occupancy & castling::white_queen_piece_mask) ||
          !(this->castling & castling::white_queen) ||
          !(get_bit(target, square::d1))
        );
      } else if constexpr (color == color::black) {
        moves.push(move::move(square::e8, square::g8, piece::black_king, piece::black_king, piece::none, false, false, true, false, false));
        moves.pop(
          (attacked & castling::black_king_attack_mask) ||
          (occupancy & castling::black_king_piece_mask) ||
          !(this->castling & castling::black_king) ||
          !(get_bit(target, square::f8))
        );
        moves.push(move::move(square::e8, square::c8, piece::black_king, piece::black_king, piece::none, false, false, true, false, false));
        moves.pop(
          (attacked & castling::black_queen_attack_mask) ||
          (occupancy & castling::black_queen_piece_mask) ||
          !(this->castling & castling::black_queen) ||
          !(get_bit(target, square::d8))
        );
      };
    };

    // add legal king moves to the move stack
    template <color_t color>
    void add_king_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& attacked_no_king, const square_t& king_square,
      const bitboard_t& occupancy_color,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);
      bitboard_t possible_to = attack<king>(king_square) & ~occupancy_color & ~attacked_no_king & target;
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(king_square, to, king, king, this->pieces[to], false, false, false, false, false));
      };
    };

    // add legal enpassant moves to the move stack
    template <color_t color>
    void add_enpassant_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned, const bitboard_t& enpassant_pinned,
      const bitboard_t& king_bishop_ray,
      const square_t& checker_square,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr color_t opponent = color::compiletime::opponent(color);
      constexpr piece_t opponent_pawn = piece::compiletime::to_color(piece::pawn, opponent);
      constexpr int enpassant_offset = (color == color::white ? 8 : -8);
      if (this->enpassant == square::none) return;
      bitboard_t bishop_pinned_pawns = (
        this->bitboards[pawn] &
        attack<opponent_pawn>(this->enpassant) &
        ~enpassant_pinned &
        bishop_pinned &
        ~rook_pinned
      );
      while (bishop_pinned_pawns) {
        square_t from = pop_lsb(bishop_pinned_pawns);
        moves.push(move::move(from, this->enpassant, pawn, pawn, piece::none, false, true, false, false, false));
        moves.pop(
          !get_bit(king_bishop_ray, this->enpassant) ||
          (
            !get_bit(target, this->enpassant) &&
            (this->enpassant + enpassant_offset != checker_square)
          )
        );
      };
      bitboard_t free_pawns = (
        this->bitboards[pawn] &
        attack<opponent_pawn>(this->enpassant) &
        ~enpassant_pinned &
        ~bishop_pinned &
        ~rook_pinned
      );
      while (free_pawns) {
        square_t from = pop_lsb(free_pawns);
        moves.push(move::move(from, this->enpassant, pawn, pawn, piece::none, false, true, false, false, false));
        moves.pop(
          !get_bit(target, this->enpassant) &&
          (this->enpassant + enpassant_offset != checker_square)
        );
      };
    };

    // add legal pawn push moves to the move stack
    template <color_t color>
    void add_pawn_push_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& king_rook_ray,
      const bitboard_t& occupancy,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr int push_offset = (color == color::white ? -8 : 8);
      constexpr int doublepush_offset = 2 * push_offset;
      bitboard_t free_pawns = this->bitboards[pawn] & ~bishop_pinned & ~rook_pinned;
      bitboard_t rook_pinned_pawns = this->bitboards[pawn] & ~bishop_pinned & rook_pinned;
      bitboard_t pushable_pawns;
      bitboard_t doublepushable_pawns;
      bitboard_t promoting_pawns;
      if constexpr (color == color::white) {
        pushable_pawns = (
          (free_pawns & ~(occupancy << 8) & (target << 8) & ~bitboard::rank_7) |
          (rook_pinned_pawns & ~(occupancy << 8) & (target << 8) & (king_rook_ray << 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(occupancy << 8) & ~(occupancy << 16) & (target << 16) & bitboard::rank_2) |
          (rook_pinned_pawns & ~(occupancy << 8) & ~(occupancy << 16) & (target << 16) & bitboard::rank_2 & (king_rook_ray << 16))
        );
        promoting_pawns = free_pawns & ~(occupancy << 8) & (target << 8) & bitboard::rank_7;
      } else if constexpr (color == color::black) {
        pushable_pawns = (
          (free_pawns & ~(occupancy >> 8) & (target >> 8) & ~bitboard::rank_2) |
          (rook_pinned_pawns & ~(occupancy >> 8) & (target >> 8) & (king_rook_ray >> 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(occupancy >> 8) & ~(occupancy >> 16) & (target >> 16) & bitboard::rank_7) |
          (rook_pinned_pawns & ~(occupancy >> 8) & ~(occupancy >> 16) & (target >> 16) & bitboard::rank_7 & (king_rook_ray >> 16))
        );
        promoting_pawns = free_pawns & ~(occupancy >> 8) & (target >> 8) & bitboard::rank_2;
      } else return;
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
    };

    // add legal pawn capture moves to the move stack
    template <color_t color>
    void add_pawn_capture_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& king_bishop_ray,
      const bitboard_t& occupancy_opponent,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t pawn = piece::compiletime::to_color(piece::pawn, color);
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      constexpr bitboard_t promotion_rank = (color == color::white) ? bitboard::rank_7 : bitboard::rank_2;
      bitboard_t free_pawns = this->bitboards[pawn] & ~bishop_pinned & ~rook_pinned;
      bitboard_t bishop_pinned_pawns = this->bitboards[pawn] & bishop_pinned & ~rook_pinned;
      bitboard_t promoting_free_pawns = free_pawns & promotion_rank;
      bitboard_t not_promoting_free_pawns = free_pawns & ~promotion_rank;
      bitboard_t promoting_bishop_pinned_pawns = bishop_pinned_pawns & promotion_rank;
      bitboard_t not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~promotion_rank;
      while (promoting_free_pawns) {
        square_t from = pop_lsb(promoting_free_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false, true, false));
        };
      };
      while (not_promoting_free_pawns) {
        square_t from = pop_lsb(not_promoting_free_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, pawn, this->pieces[to], false, false, false, false, false));
        };
      };
      while (promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false, true, false));
          moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false, true, false));
        };
      };

      while (not_promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(not_promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, pawn, this->pieces[to], false, false, false, false, false));
        };
      };
    };

    // add legal knight moves to the move stack
    template <color_t color>
    void add_knight_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& occupancy_color,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t knight = piece::compiletime::to_color(piece::knight, color);
      bitboard_t free_knights = this->bitboards[knight] & ~bishop_pinned & ~rook_pinned;
      while (free_knights) {
        square_t from = pop_lsb(free_knights);
        bitboard_t possible_to = attack<knight>(from) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, knight, knight, this->pieces[to], false, false, false, false, false));
        };
      };
    };

    // add legal bishop moves to the move stack
    template <color_t color>
    void add_bishop_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& king_bishop_ray,
      const bitboard_t& occupancy, const bitboard_t& occupancy_color,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t bishop = piece::compiletime::to_color(piece::bishop, color);
      bitboard_t bishop_pinned_bishops = this->bitboards[bishop] & bishop_pinned & ~rook_pinned;
      while (bishop_pinned_bishops) {
        square_t from = pop_lsb(bishop_pinned_bishops);
        bitboard_t possible_to = attack<bishop>(from, occupancy) & ~occupancy_color & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, bishop, bishop, this->pieces[to], false, false, false, false, false));
        };
      };
      bitboard_t free_bishops = this->bitboards[bishop] & ~bishop_pinned & ~rook_pinned;
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = attack<bishop>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, bishop, bishop, this->pieces[to], false, false, false, false, false));
        };
      };
    };

    // add legal rook moves to the move stack
    template <color_t color>
    void add_rook_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& king_rook_ray,
      const bitboard_t& occupancy, const bitboard_t& occupancy_color,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t rook = piece::compiletime::to_color(piece::rook, color);
      bitboard_t rook_pinned_rooks = this->bitboards[rook] & ~bishop_pinned & rook_pinned;
      while (rook_pinned_rooks) {
        square_t from = pop_lsb(rook_pinned_rooks);
        bitboard_t possible_to = attack<rook>(from, occupancy) & ~occupancy_color & king_rook_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, rook, rook, this->pieces[to], false, false, false, false, false));
        };
      };
      bitboard_t free_rooks = this->bitboards[rook] & ~bishop_pinned & ~rook_pinned;
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = attack<rook>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, rook, rook, this->pieces[to], false, false, false, false, false));
        };
      };
    };

    // add legal queen moves to the move stack
    template <color_t color>
    void add_queen_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& bishop_pinned, const bitboard_t& rook_pinned,
      const bitboard_t& king_bishop_ray, const bitboard_t& king_rook_ray,
      const bitboard_t& occupancy, const bitboard_t& occupancy_color,
      const bitboard_t& target=bitboard::full
    ) {
      constexpr piece_t queen = piece::compiletime::to_color(piece::queen, color);
      bitboard_t bishop_pinned_queens = this->bitboards[queen] & bishop_pinned & ~rook_pinned;
      while (bishop_pinned_queens) {
        square_t from = pop_lsb(bishop_pinned_queens);
        bitboard_t possible_to = attack<piece::bishop>(from, occupancy) & ~occupancy_color & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
        };
      };
      bitboard_t rook_pinned_queens = this->bitboards[queen] & ~bishop_pinned & rook_pinned;
      while (rook_pinned_queens) {
        square_t from = pop_lsb(rook_pinned_queens);
        bitboard_t possible_to = attack<piece::rook>(from, occupancy) & ~occupancy_color & king_rook_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
        };
      };
      bitboard_t free_queens = this->bitboards[queen] & ~bishop_pinned & ~rook_pinned;
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = attack<piece::queen>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, queen, queen, this->pieces[to], false, false, false, false, false));
        };
      };
    };
};


#endif // __BOARD__MODULE__