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
#include "stack.cpp"
#include "time.cpp"
#include "zobrist.cpp"


#define MAX_GAME_LENGTH 512
#define MAX_MOVE_LENGTH 512


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
  
    Board(std::string fen=fen::starting) {
      this->set_fen(fen);
    };

    void set_fen(std::string fen=fen::starting) {
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
      enpassant = (fen[index] == '-') ? square::none : ((fen[index] - 'a') + 8 * (7 - (fen[++index] - '1')));
      this->zobrist.update_enpassant(enpassant);
      
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

    std::string to_string() {
      std::string string = "";
      for (square_t square = 0; square < 64; square++) {
        if (square % 8 == 0 && square != 0) string += "\n";
        if (square % 8 == 0) string +=  " " + std::to_string(8 - square / 8) + " ";
        string += escape::escape(this->pieces[square], square);
        string += " " + piece::unicode(this->pieces[square]) + " ";
        string += escape::escape();    
      };
      string += "\n    A  B  C  D  E  F  G  H\n\n";
      string += "  TURN:            " + color::to_string(this->turn) + "\n";
      string += "  CASTLING:        " + castling::to_string(this->castling) + "\n";
      string += "  EN PASSANT:      " + square::to_string(this->enpassant) + "\n";
      string += "  HALFMOVE CLOCK:  " + std::to_string(this->halfmove_clock) + "\n";
      string += "  FULLMOVE CLOCK:  " + std::to_string(this->fullmove_clock) + "\n";
      string += "  FEN:             " + this->fen() + "\n";
      string += "  ZOBRIST HASH:    " + std::to_string(this->zobrist.hash) + "\n";
      string += "  MOVES:           ";
      for (undo_t undo : this->history) {
        string += move::to_string(undo.move) + "\n                   ";
      };
      if (this->history.size() == 0) string += "-";
      return string;
    };

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

    void make(move_t move) {
      // add the move, castling rights, enpassant square, halfmove clock and hash to the history
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

      piece_t moved_piece = move::moved_piece(move);
      square_t from = move::from(move);
      square_t to = move::to(move);

      // remove castling rights if needed
      this->castling &= ~move::removed_castling(move);

      // remove enpassant square to be updated later
      this->enpassant = square::none;

      // change move clocks
      this->halfmove_clock = (this->halfmove_clock + 1) & (u16_t)((!(piece::type(moved_piece) == piece::pawn || move::capture(move))) * 0xFFFF);
      this->fullmove_clock += (this->turn == color::black);

      // do move
      if (move::enpassant(move)) {
        this->remove_piece(from);
        this->remove_piece(to - push_offset(this->turn));
        this->place_piece(moved_piece, to);
      } else if (move::castling(move)) {
        this->remove_piece(from);
        this->remove_piece(to + (to > from) * 3 - 2);
        this->place_piece(moved_piece, to);
        this->place_piece(piece::to_color(piece::rook, this->turn), to - (to > from) * 2 + 1);
      } else if (move::capture(move)) {
        if (move::promotion(move)) {
          this->remove_piece(from);
          this->remove_piece(to);
          this->place_piece(move::target_piece(move), to);
        } else {
          this->remove_piece(from);
          this->remove_piece(to);
          this->place_piece(moved_piece, to);   
        };
      } else {
        if (move::promotion(move)) {
          this->remove_piece(from);
          this->place_piece(move::target_piece(move), to);
        } else if (move::double_pawn_push(move)) {
          this->remove_piece(from);
          this->place_piece(moved_piece, to);
          this->enpassant = to - push_offset(this->turn);
        } else {
          this->remove_piece(from);
          this->place_piece(moved_piece, to);
        };
      };

      // change turn
      this->turn = color::opponent(this->turn);

      // add castling, enpassant square and the turn change to the hash
      this->zobrist.update_castling(this->castling);
      this->zobrist.update_enpassant(this->enpassant);
      this->zobrist.update_turn();
    };

    void unmake() {
      // change turn
      this->turn = color::opponent(this->turn);
      
      // restore history
      undo_t undo = this->history.pop();
      move_t move = undo.move;
      this->castling = undo.castling;
      this->enpassant = undo.enpassant;
      this->halfmove_clock = undo.halfmove_clock;

      // change move clocks
      this->fullmove_clock -= (this->turn == color::black);


      piece_t moved_piece = move::moved_piece(move);
      square_t from = move::from(move);
      square_t to = move::to(move);
      
      // undo move
      if (move::enpassant(move)) {
        this->remove_piece(to);
        this->place_piece(moved_piece, from);
        this->place_piece(piece::to_color(piece::pawn, color::opponent(this->turn)), to - push_offset(this->turn));
      } else if (move::castling(move)) {
        this->remove_piece(to);
        this->remove_piece(to - (to > from) * 2 + 1);
        this->place_piece(moved_piece, from);
        this->place_piece(piece::to_color(piece::rook, this->turn), to + (to > from) * 3 - 2);
      } else if (move::capture(move)) {
        this->remove_piece(to);
        this->place_piece(moved_piece, from);
        this->place_piece(move::captured_piece(move), to);
      } else {
        this->remove_piece(to);
        this->place_piece(moved_piece, from);
      };

      // restore hash
      this->zobrist.update(undo.hash);
    };

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

    void push_uci(std::string uci) {
      this->make(this->from_uci(uci));
    };

    Stack<move_t, MAX_MOVE_LENGTH> legal_moves() {
      if (this->turn == color::white) {
        return this->generate<legal, color::white>();
      } else {
        return this->generate<legal, color::black>();
      };
    };

    u64_t perft(int depth) {
      std::cout << "\tperft " << depth << " of " << std::endl;
      std::cout << "\t" << this->fen() << std::endl << std::endl;
      u64_t start = timing::ns();
      u64_t count = 0;
      if (this->turn == color::white) {
        auto moves = this->generate<legal, color::white>();
        for (auto move : moves) {
          u64_t local_start = timing::ns();
          u64_t local_count = 0;
          make(move);
          local_count += perft(depth - 1, true);
          unmake();
          u64_t local_end = timing::ns();
          std::cout << "\t" << move::to_string(move) << ": \t" << local_count << "\t\tMNps:\t" << 1000 * ((float)local_count) / ((float)(local_end - local_start)) << std::endl;
          count += local_count;
        };
      } else {
        auto moves = this->generate<legal, color::black>();
        for (auto move : moves) {
          u64_t local_start = timing::ns();
          u64_t local_count = 0;
          make(move);
          local_count += perft(depth - 1, true);
          unmake();
          u64_t local_end = timing::ns();
          std::cout << "\t" << move::to_string(move) << ": \t" << local_count << "\t\tMNps:\t" << 1000 * ((float)local_count) / ((float)(local_end - local_start)) << std::endl;
          count += local_count;
        };
      };
      
      u64_t end = timing::ns();
      std::cout << std::endl << "\tTotal:\t" << count << "\t\tMNps:\t" << 1000 * ((float)count) / ((float)(end - start))  << std::endl;
      return count;
    };

    u64_t perft(int depth, bool count_only) {
      if (depth == 0) return 1;
      u64_t count = 0;
      if (this->turn == color::white) {
        auto moves = this->generate<legal, color::white>();
        for (auto move : moves) {
          make(move);
          count += perft(depth - 1, true);
          unmake();
        };
      } else {
        auto moves = this->generate<legal, color::black>();
        for (auto move : moves) {
          make(move);
          count += perft(depth - 1, true);
          unmake();
        };
      };
      return count;
    };

  private:
    Stack<undo_t, MAX_GAME_LENGTH> history;

    void place_piece(piece_t piece, square_t square) {
      set_bit(this->bitboards[piece], square);
      set_bit(this->bitboards[piece::type(piece)], square);
      set_bit(this->bitboards[piece::color(piece)], square);
      set_bit(this->bitboards[piece::none], square);
      this->pieces[square] = piece;
      this->zobrist.update_piece(piece, square);
    };

    void remove_piece(square_t square) {
      piece_t piece = pieces[square];
      clear_bit(this->bitboards[piece], square);
      clear_bit(this->bitboards[piece::type(piece)], square);
      clear_bit(this->bitboards[piece::color(piece)], square);
      clear_bit(this->bitboards[piece::none], square);
      this->pieces[square] = piece::none;
      this->zobrist.update_piece(piece, square);
    };
  
    move_t from_uci(std::string uci) {
      square_t from = (uci[0] - 'a') + 8 * (7 - (uci[1] - '1'));
      square_t to = (uci[2] - 'a') + 8 * (7 - (uci[3] - '1'));
      piece_t moved_piece = this->pieces[from];
      piece_t target_piece = piece::none;
      if (uci.length() == 5) {
        target_piece = piece::to_color(piece::from_char(uci[4]), this->turn);
      };
      piece_t captured_piece = this->pieces[to];
      bool double_pawn_push = (piece::type(moved_piece) == piece::pawn) && (((from - to) == 16) || ((from - to) == -16));
      bool enpassant = (piece::type(moved_piece) == piece::pawn) && (to == this->enpassant);
      bool castling = (piece::type(moved_piece) == piece::king) && (((from - to) == 2) || ((from - to) == -2));
      return move::move(from, to, moved_piece, target_piece, captured_piece, double_pawn_push, enpassant, castling);
    };

    template <gen_t gen, color_t color>
    Stack<move_t, MAX_MOVE_LENGTH> generate() {
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

      Stack<move_t, MAX_MOVE_LENGTH> moves;
      moves.clear();

      square_t king_square = get_lsb(this->bitboards[king]);

      bitboard_t occupancy = this->bitboards[color::none];
      bitboard_t occupancy_color = this->bitboards[color];
      bitboard_t occupancy_opponent = this->bitboards[opponent];
      bitboard_t occupancy_no_king = occupancy & ~this->bitboards[king];

      bitboard_t king_bishop_ray = attack_ray::bishop[king_square];
      bitboard_t king_rook_ray = attack_ray::rook[king_square];

      bitboard_t king_bishop_attack = attack<piece::bishop>(king_square, occupancy);
      bitboard_t king_rook_attack = attack<piece::rook>(king_square, occupancy);

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

      bitboard_t bishop_pinned = bitboard::none;
      bitboard_t bishop_attackers = (this->bitboards[opponent_bishop] | this->bitboards[opponent_queen]) & king_bishop_ray;
      while (bishop_attackers) {
        square_t square = pop_lsb(bishop_attackers);
        bishop_pinned |= attack<piece::bishop>(square, occupancy) & king_bishop_attack;
      };

      bitboard_t rook_pinned = bitboard::none;
      bitboard_t rook_attackers = (this->bitboards[opponent_rook] | this->bitboards[opponent_queen]) & king_rook_ray;
      while (rook_attackers) {
        square_t square = pop_lsb(rook_attackers);
        rook_pinned |= attack<piece::rook>(square, occupancy) & king_rook_attack;
      };

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
          enpassant_pinned |= attack<piece::rook>(square, occupancy & ~enpassant_pawns) & attack<piece::rook>(king_square, occupancy & ~enpassant_pawns) & this->bitboards[pawn];
        };
      };
      
      bitboard_t checkers = this->attackers(king_square) & this->bitboards[opponent];
      square_t checker_square = get_lsb(checkers);
      if (checkers) {
        if (popcount(checkers) > 1) {
          this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color);
          return moves;
        };
        piece_t checker_type = piece::type(this->pieces[checker_square]);
        bitboard_t target = (
          bitboard(checker_square) |
          ((king_bishop_attack & attack<piece::bishop>(checker_square, occupancy)) * (checker_type == piece::bishop)) |
          ((king_rook_attack & attack<piece::rook>(checker_square, occupancy)) * (checker_type == piece::rook)) |
          ((king_bishop_attack & attack<piece::bishop>(checker_square, occupancy)) * (checker_type == piece::queen && (king_bishop_attack & this->bitboards[opponent_queen]))) |
          ((king_rook_attack & attack<piece::rook>(checker_square, occupancy)) * (checker_type == piece::queen && (king_rook_attack & this->bitboards[opponent_queen])))
        );
        this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color);
        this->add_enpassant_moves<color>(moves, bishop_pinned, rook_pinned, enpassant_pinned, king_bishop_ray, checker_square, target);
        this->add_pawn_push_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, target);
        this->add_pawn_capture_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy_opponent, target);
        this->add_knight_moves<color>(moves, bishop_pinned, rook_pinned, occupancy_color, target);
        this->add_bishop_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy, occupancy_color, target);
        this->add_rook_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, occupancy_color, target);
        this->add_queen_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, king_rook_ray, occupancy, occupancy_color, target);
        return moves;
      };
      this->add_castling_moves<color>(moves, attacked, occupancy);
      this->add_king_moves<color>(moves, attacked_no_king, king_square, occupancy_color);
      this->add_enpassant_moves<color>(moves, bishop_pinned, rook_pinned, enpassant_pinned, king_bishop_ray, checker_square);
      this->add_pawn_push_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy);
      this->add_pawn_capture_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy_opponent);
      this->add_knight_moves<color>(moves, bishop_pinned, rook_pinned, occupancy_color);
      this->add_bishop_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, occupancy, occupancy_color);
      this->add_rook_moves<color>(moves, bishop_pinned, rook_pinned, king_rook_ray, occupancy, occupancy_color);
      this->add_queen_moves<color>(moves, bishop_pinned, rook_pinned, king_bishop_ray, king_rook_ray, occupancy, occupancy_color);
      return moves;
    };

    template <color_t color>
    void add_castling_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& attacked,
      const bitboard_t& occupancy
    ) {
      if constexpr (color == color::white) {
        moves.push(move::move(square::e1, square::g1, piece::white_king, piece::none, piece::none, false, false, true));
        moves.pop(
          (attacked & castling::white_king_attack_mask) ||
          (occupancy & castling::white_king_piece_mask) ||
          !(this->castling & castling::white_king)
        );
        moves.push(move::move(square::e1, square::c1, piece::white_king, piece::none, piece::none, false, false, true));
        moves.pop(
          (attacked & castling::white_queen_attack_mask) ||
          (occupancy & castling::white_queen_piece_mask) ||
          !(this->castling & castling::white_queen)
        );
      } else if constexpr (color == color::black) {
        moves.push(move::move(square::e8, square::g8, piece::black_king, piece::none, piece::none, false, false, true));
        moves.pop(
          (attacked & castling::black_king_attack_mask) ||
          (occupancy & castling::black_king_piece_mask) ||
          !(this->castling & castling::black_king)
        );
        moves.push(move::move(square::e8, square::c8, piece::black_king, piece::none, piece::none, false, false, true));
        moves.pop(
          (attacked & castling::black_queen_attack_mask) ||
          (occupancy & castling::black_queen_piece_mask) ||
          !(this->castling & castling::black_queen)
        );
      };
    };

    template <color_t color>
    void add_king_moves(
      Stack<move_t, MAX_MOVE_LENGTH>& moves,
      const bitboard_t& attacked_no_king, const square_t& king_square,
      const bitboard_t& occupancy_color
    ) {
      constexpr piece_t king = piece::compiletime::to_color(piece::king, color);

      bitboard_t possible_to = attack<piece::king>(king_square) & ~occupancy_color & ~attacked_no_king;
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves.push(move::move(king_square, to, king, piece::none, this->pieces[to], false, false, false));
      };
    };

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

      constexpr int enpassant_offset = -push_offset(color);

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
        moves.push(move::move(from, this->enpassant, pawn, piece::none, piece::none, false, true, false));
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
        moves.push(move::move(from, this->enpassant, pawn, piece::none, piece::none, false, true, false));
        moves.pop(
          !get_bit(target, this->enpassant) &&
          (this->enpassant + enpassant_offset != checker_square)
        );
      };
    };

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

      constexpr int push_offset = push_offset(color);
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
        moves.push(move::move(from, to, pawn, piece::none, piece::none, false, false, false));
      };

      while (doublepushable_pawns) {
        square_t from = pop_lsb(doublepushable_pawns);
        square_t to = from + doublepush_offset;
        moves.push(move::move(from, to, pawn, piece::none, piece::none, true, false, false));
      };

      while (promoting_pawns) {
        square_t from = pop_lsb(promoting_pawns);
        square_t to = from + push_offset;
        moves.push(move::move(from, to, pawn, knight, piece::none, false, false, false));
        moves.push(move::move(from, to, pawn, bishop, piece::none, false, false, false));
        moves.push(move::move(from, to, pawn, rook, piece::none, false, false, false));
        moves.push(move::move(from, to, pawn, queen, piece::none, false, false, false));
      };
    };

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
          moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false));
        };
      };

      while (not_promoting_free_pawns) {
        square_t from = pop_lsb(not_promoting_free_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, piece::none, this->pieces[to], false, false, false));
        };
      };

      while (promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, knight, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, bishop, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, rook, this->pieces[to], false, false, false));
          moves.push(move::move(from, to, pawn, queen, this->pieces[to], false, false, false));
        };
      };

      while (not_promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(not_promoting_bishop_pinned_pawns);
        bitboard_t possible_to = attack<pawn>(from) & occupancy_opponent & king_bishop_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, pawn, piece::none, this->pieces[to], false, false, false));
        };
      };
    };

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
          moves.push(move::move(from, to, knight, piece::none, this->pieces[to], false, false, false));
        };
      };
    };

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
          moves.push(move::move(from, to, bishop, piece::none, this->pieces[to], false, false, false));
        };
      };

      bitboard_t free_bishops = this->bitboards[bishop] & ~bishop_pinned & ~rook_pinned;
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = attack<bishop>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, bishop, piece::none, this->pieces[to], false, false, false));
        };
      };
    };

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
          moves.push(move::move(from, to, rook, piece::none, this->pieces[to], false, false, false));
        };
      };

      bitboard_t free_rooks = this->bitboards[rook] & ~bishop_pinned & ~rook_pinned;
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = attack<rook>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, rook, piece::none, this->pieces[to], false, false, false));
        };
      };
    };

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
          moves.push(move::move(from, to, queen, piece::none, this->pieces[to], false, false, false));
        };
      };

      bitboard_t rook_pinned_queens = this->bitboards[queen] & ~bishop_pinned & rook_pinned;
      while (rook_pinned_queens) {
        square_t from = pop_lsb(rook_pinned_queens);
        bitboard_t possible_to = attack<piece::rook>(from, occupancy) & ~occupancy_color & king_rook_ray & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, queen, piece::none, this->pieces[to], false, false, false));
        };
      };

      bitboard_t free_queens = this->bitboards[queen] & ~bishop_pinned & ~rook_pinned;
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = attack<piece::queen>(from, occupancy) & ~occupancy_color & target;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves.push(move::move(from, to, queen, piece::none, this->pieces[to], false, false, false));
        };
      };
    };
};


std::ostream& operator<<(std::ostream& os, Board& board) { 
  return os << board.to_string();
};


#endif // __BOARD__MODULE__