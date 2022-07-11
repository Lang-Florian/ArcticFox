#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "attacks.cpp"
#include "constants.cpp"
#include "moves.cpp"
#include "time.cpp"
#include "zobrist.cpp"

#include "array"
#include "iostream"
#include "string"


#ifndef __BOARD__MODULE__
#define __BOARD__MODULE__


#define HISTORY_SIZE 512
#define MAX_LEGAL_MOVES 1024


struct history_t {
  move_t move;
  castling_t castling_rights;
  square_t enpassant_square;
  U16 halfmove_clock;
};


class Board {
  public:
    std::array<bitboard_t, 32> bitboards;
    std::array<piece_t, 64> pieces;

    color_t turn;
    castling_t castling_rights;
    square_t enpassant_square;
    U16 halfmove_clock;
    U16 fullmove_clock;

    hash_t hash;

    Board(string_t fen=FEN::initial) {
      set_fen(fen);
    };

    // parse a fen
    void set_fen(string_t fen) {
      bitboards.fill(BITBOARD::none);
      pieces.fill(PIECE::none);

      hash = ZOBRIST::none;

      int index = 0;
      for (square_t square = 0; index < fen.length() && fen[index] != ' '; index++) {
        if (fen[index] == '/') continue;
        piece_t piece = PIECE::from_char(fen[index]);
        if (piece != PIECE::none) {
          place_piece(piece, square);
          square++;
        } else {
          square += fen[index] - '0';
        };
      };

      index++;

      turn = COLOR::from_char(fen[index]);
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, CASTLING::none, SQUARE::none, turn != COLOR::white);

      index++;

      castling_rights = CASTLING::none;
      while (fen[++index] != ' ') {
        castling_rights |= CASTLING::from_char(fen[index]);
      };
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, castling_rights, SQUARE::none, false);

      enpassant_square = (fen[++index] == '-') ? SQUARE::none : ((fen[index] - 'a') + 8 * (7 - (fen[++index] - '1')));
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, CASTLING::none, enpassant_square, false);

      index++;

      halfmove_clock = 0;
      while (fen[++index] != ' ') {
        halfmove_clock = 10 * halfmove_clock + (fen[index] - '0');
      };      

      fullmove_clock = 0;
      while (++index < fen.length() && fen[index] != ' ') {
        fullmove_clock = 10 * fullmove_clock + (fen[index] - '0');
      };

      history_index = 0;

      uptodate = false;
    };

    // create a string representation of the board
    inline string_t to_string() {
      string_t string = "";
      for (square_t square = 0; square < 64; square++) {
        if (square % 8 == 0 && square != 0) string += "\n";
        if (square % 8 == 0) string +=  " " + std::to_string(8 - square / 8) + " ";
        string += ESCAPE::escape(square, pieces[square]);
        string += " " + PIECE::unicode(pieces[square]) + " ";
        string += ESCAPE::escape();    
      };
      string += "\n    A  B  C  D  E  F  G  H\n\n";
      string += "  TURN:            " + COLOR::to_string(turn) + "\n";
      string += "  CASTLING:        " + CASTLING::to_string(castling_rights) + "\n";
      string += "  EN PASSANT:      " + SQUARE::to_string(enpassant_square) + "\n";
      string += "  HALFMOVE CLOCK:  " + std::to_string(halfmove_clock) + "\n";
      string += "  FULLMOVE CLOCK:  " + std::to_string(fullmove_clock) + "\n";
      string += "  FEN:             " + fen() + "\n";
      string += "  ZOBRIST HASH:    " + std::to_string(hash) + "\n";
      string += "  MOVES:           ";
      for (int index = 0; index < history_index; index++) {
        if (index % 4 == 0 && index != 0) string += "\n                   ";
        string += MOVE::to_string(history[index].move) + " ";
      };
      if (history_index == 0) string += "-";
      return string;
    };

    // return a fen representation of the board
    inline string_t fen() {
      string_t string = "";
      int index = 0;
      for (square_t square = 0; square < 64; square++) {
        if (square % 8 == 0 && square != 0) {
          if (index != 0) {
            string += std::to_string(index);
            index = 0;
          };
          string += "/";
        };
        if (pieces[square] == PIECE::none) {
          index++;
        } else {
          if (index != 0) {
            string += std::to_string(index);
            index = 0;
          };
          string += PIECE::to_string(pieces[square]);
        };
      };
      if (index != 0) string += std::to_string(index);
      string += " " + COLOR::to_string(turn);
      string += " " + CASTLING::to_string(castling_rights);
      string += " " + SQUARE::to_string(enpassant_square);
      string += " " + std::to_string(halfmove_clock);
      string += " " + std::to_string(fullmove_clock);
      return string;
    };

    // make a move
    inline void make(move_t move) {
      // remove castling and enpassant square from the hash
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, castling_rights, enpassant_square, false);
      
      // add the move, castling_rights and enpassant square to the history
      history[history_index++] = history_t {
        move,
        castling_rights,
        enpassant_square,
        halfmove_clock,
      };

      piece_t moved_piece = MOVE::moved_piece(move);
      square_t from = MOVE::from(move);
      square_t to = MOVE::to(move);

      // remove castling rights if needed
      castling_rights &= ~MOVE::removed_castling(move);

      enpassant_square = SQUARE::none;

      // change move clocks
      halfmove_clock = (halfmove_clock + 1) & (U16)((!(PIECE::type(moved_piece) == PIECE::pawn || MOVE::capture(move))) * 0xFFFF);
      fullmove_clock += (turn == COLOR::black);

      // do move
      if (MOVE::enpassant(move)) {
        remove_piece(from);
        remove_piece(to + (turn == COLOR::white) * 16 - 8);
        place_piece(moved_piece, to);
      } else if (MOVE::castling(move)) {
        remove_piece(from);
        remove_piece(to + (to > from) * 3 - 2);
        place_piece(moved_piece, to);
        place_piece(PIECE::to_color(PIECE::rook, turn), to - (to > from) * 2 + 1);
      } else if (MOVE::capture(move)) {
        if (MOVE::promotion(move)) {
          remove_piece(from);
          remove_piece(to);
          place_piece(MOVE::promoted_piece(move), to);
        } else {
          remove_piece(from);
          remove_piece(to);
          place_piece(moved_piece, to);   
        };
      } else {
        if (MOVE::promotion(move)) {
          remove_piece(from);
          place_piece(MOVE::promoted_piece(move), to);
        } else if (MOVE::double_pawn_push(move)) {
          remove_piece(from);
          place_piece(moved_piece, to);
          enpassant_square = to + (turn == COLOR::white ? 8 : -8);
        } else {
          remove_piece(from);
          place_piece(moved_piece, to);
        };
      };

      // change turn
      turn = COLOR::opponent(turn);

      // add castling, enpassant square and the turn change to the hash
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, castling_rights, enpassant_square, true);

      uptodate = false;
    };

    // unmake a move
    inline void unmake() {
      // remove castling and enpassant square from the hash
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, castling_rights, enpassant_square, false);

      // change turn
      turn = COLOR::opponent(turn);
      
      // restore history
      history_t element = history[--history_index];
      move_t move = element.move;
      castling_rights = element.castling_rights;
      enpassant_square = element.enpassant_square;
      halfmove_clock = element.halfmove_clock;
      fullmove_clock -= (turn == COLOR::black);


      piece_t moved_piece = MOVE::moved_piece(move);
      square_t from = MOVE::from(move);
      square_t to = MOVE::to(move);
      
      // undo move
      if (MOVE::enpassant(move)) {
        remove_piece(to);
        place_piece(moved_piece, from);
        place_piece(PIECE::to_color(PIECE::pawn, COLOR::opponent(turn)), to + (turn == COLOR::white) * 16 - 8);
      } else if (MOVE::castling(move)) {
        remove_piece(to);
        remove_piece(to - (to > from) * 2 + 1);
        place_piece(moved_piece, from);
        place_piece(PIECE::to_color(PIECE::rook, turn), to + (to > from) * 3 - 2);
      } else if (MOVE::capture(move)) {
        remove_piece(to);
        place_piece(moved_piece, from);
        place_piece(MOVE::captured_piece(move), to);
      } else {
        remove_piece(to);
        place_piece(moved_piece, from);
      };

      // add castling, enpassant square and the turn change to the hash
      hash = ZOBRIST::hash(hash, PIECE::none, SQUARE::none, castling_rights, enpassant_square, true);

      uptodate = false;
    };

    // print a perft
    inline U64 perft(int depth) {
      std::cout << "\tperft " << depth << " of " << fen() << std::endl << std::endl;
      U64 start = TIME::ms();
      U64 count = 0;
      auto moves = get_legal_moves();
      for (int i = 1; i <= moves[0]; i++) {
        U64 local_start = TIME::ms();
        U64 local_count = 0;
        make(moves[i]);
        local_count += perft(depth - 1, true);
        unmake();
        U64 local_end = TIME::ms();
        std::cout << "  " << MOVE::to_string(moves[i]) << ": \t" << local_count << "\t\tMNps:\t" << ((float)local_count) / (1000 * (float)(local_end - local_start)) << std::endl;
        count += local_count;
      };
      U64 end = TIME::ms();
      std::cout << std::endl << "  Total:\t" << count << "\t\tMNps:\t" << ((float)count) / (1000 * (float)(end - start))  << std::endl;
      return count;
    };

    // do a perft but only counting the positions
    inline U64 perft(int depth, bool count_only) {
      if (depth == 0) return 1;
      U64 count = 0;
      auto moves = get_legal_moves();
      for (int i = 1; i <= moves[0]; i++) {
        make(moves[i]);
        count += perft(depth - 1, true);
        unmake();
      };
      return count;
    };

    // return a bitboard of all the attackers of a given square
    inline bitboard_t attackers(square_t square) {
      return (
        (ATTACK::pawn(COLOR::black, square) & bitboards[PIECE::white_pawn]) |
        (ATTACK::pawn(COLOR::white, square) & bitboards[PIECE::black_pawn]) |
        (ATTACK::knight(square) & bitboards[PIECE::both_knight]) |
        (ATTACK::bishop(square, bitboards[PIECE::both]) & bitboards[PIECE::both_bishop]) |
        (ATTACK::rook(square, bitboards[PIECE::both]) & bitboards[PIECE::both_rook]) |
        (ATTACK::queen(square, bitboards[PIECE::both]) & bitboards[PIECE::both_queen]) |
        (ATTACK::king(square) & bitboards[PIECE::both_king])
      );
    };

    inline std::array<move_t, MAX_LEGAL_MOVES> get_legal_moves() {
      update();
      return legal_moves;
    };

    inline void push_uci(string_t uci) {
      make(from_uci(uci));
    };

  private:
    bool uptodate;

    outcome_t outcome;//
    bool in_check;
    bool in_multicheck;
    bitboard_t checkers;
    std::array<bitboard_t, 2> attacks;
    std::array<bitboard_t, 2> safe_king_squares;
    std::array<bitboard_t, 2> bishop_pins;
    std::array<bitboard_t, 2> rook_pins;
    bitboard_t enpassant_pins;
    std::array<bitboard_t, 2> king_bishop_rays;
    std::array<bitboard_t, 2> king_rook_rays;
    std::array<bitboard_t, 2> king_bishop_attacks;
    std::array<bitboard_t, 2> king_rook_attacks;
    bitboard_t opponent_attacks;

    std::array<move_t, MAX_LEGAL_MOVES> legal_moves;

    U16 history_index;
    std::array<history_t, HISTORY_SIZE> history{};

    inline void add_castling_moves() {
      legal_moves[++legal_moves[0]] = MOVE::move(SQUARE::E1, SQUARE::G1, PIECE::white_king, PIECE::none, PIECE::none, false, false, true);
      legal_moves[0] -= !(
        (turn == COLOR::white) &&
        !(opponent_attacks & CASTLING::white_king_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::white_king_piece_mask) &&
        (castling_rights & CASTLING::white_king)
      );
      legal_moves[++legal_moves[0]] = MOVE::move(SQUARE::E1, SQUARE::C1, PIECE::white_king, PIECE::none, PIECE::none, false, false, true);
      legal_moves[0] -= !(
        (turn == COLOR::white) &&
        !(opponent_attacks & CASTLING::white_queen_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::white_queen_piece_mask) &&
        (castling_rights & CASTLING::white_queen)
      );
      legal_moves[++legal_moves[0]] = MOVE::move(SQUARE::E8, SQUARE::G8, PIECE::black_king, PIECE::none, PIECE::none, false, false, true);
      legal_moves[0] -= !(
        (turn == COLOR::black) &&
        !(opponent_attacks & CASTLING::black_king_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::black_king_piece_mask) &&
        (castling_rights & CASTLING::black_king)
      );
      legal_moves[++legal_moves[0]] = MOVE::move(SQUARE::E8, SQUARE::C8, PIECE::black_king, PIECE::none, PIECE::none, false, false, true);
      legal_moves[0] -= !(
        (turn == COLOR::black) &&
        !(opponent_attacks & CASTLING::black_queen_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::black_queen_piece_mask) &&
        (castling_rights & CASTLING::black_queen)
      );
    };

    inline void add_enpassant_moves(bitboard_t targets=BITBOARD::full) {
      if (enpassant_square == SQUARE::none) return;

      bitboard_t bishop_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pins & bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (bishop_pinned_pawns) {
        square_t from = pop_lsb(bishop_pinned_pawns);
        legal_moves[++legal_moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
        legal_moves[0] -= !get_bit(king_bishop_rays[turn & 0b1], enpassant_square) || (!get_bit(targets, enpassant_square) && (enpassant_square + 16 * (turn == COLOR::white) - 8 != get_lsb(checkers)));
      };

      bitboard_t free_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pins & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (free_pawns) {
        square_t from = pop_lsb(free_pawns);
        legal_moves[++legal_moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
        legal_moves[0] -= !get_bit(targets, enpassant_square) && (enpassant_square + 16 * (turn == COLOR::white) - 8 != get_lsb(checkers));
      };
    };

    inline void add_pawn_push_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t free_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      bitboard_t rook_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ~bishop_pins[turn & 0b1] & rook_pins[turn & 0b1];

      bitboard_t pushable_pawns;
      bitboard_t doublepushable_pawns;
      bitboard_t promoting_pawns;

      if (turn == COLOR::white) {
        pushable_pawns = (
          (free_pawns & ~(bitboards[PIECE::both] << 8) & (targets << 8) & ~BITBOARD::rank_7) |
          (rook_pinned_pawns & ~(bitboards[PIECE::both] << 8) & (targets << 8) & (king_rook_rays[turn & 0b1] << 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(bitboards[PIECE::both] << 8) & ~(bitboards[PIECE::both] << 16) & (targets << 16) & BITBOARD::rank_2) |
          (rook_pinned_pawns & ~(bitboards[PIECE::both] << 8) & ~(bitboards[PIECE::both] << 16) & (targets << 16) & BITBOARD::rank_2 & (king_rook_rays[turn & 0b1] << 16))
        );
        promoting_pawns = free_pawns & ~(bitboards[PIECE::both] << 8) & (targets << 8) & BITBOARD::rank_7;
      } else {
        pushable_pawns = (
          (free_pawns & ~(bitboards[PIECE::both] >> 8) & (targets >> 8) & ~BITBOARD::rank_2) |
          (rook_pinned_pawns & ~(bitboards[PIECE::both] >> 8) & (targets >> 8) & (king_rook_rays[turn & 0b1] >> 8))
        );
        doublepushable_pawns = (
          (free_pawns & ~(bitboards[PIECE::both] >> 8) & ~(bitboards[PIECE::both] >> 16) & (targets >> 16) & BITBOARD::rank_7) |
          (rook_pinned_pawns & ~(bitboards[PIECE::both] >> 8) & ~(bitboards[PIECE::both] >> 16) & (targets >> 16) & BITBOARD::rank_7 & (king_rook_rays[turn & 0b1] >> 16))
        );
        promoting_pawns = free_pawns & ~(bitboards[PIECE::both] >> 8) & (targets >> 8) & BITBOARD::rank_2;
      };

      while (pushable_pawns) {
        square_t from = pop_lsb(pushable_pawns);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, false, false);
      };
      while (doublepushable_pawns) {
        square_t from = pop_lsb(doublepushable_pawns);
        square_t to = from - 32 * (turn == COLOR::white) + 16;
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, true, false, false);
      };
      while (promoting_pawns) {
        square_t from = pop_lsb(promoting_pawns);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), PIECE::none, false, false, false);
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), PIECE::none, false, false, false);
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), PIECE::none, false, false, false);
        legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), PIECE::none, false, false, false);
      };
    };

    inline void add_pawn_capture_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t free_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      bitboard_t bishop_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];

      bitboard_t promotion_rank = (turn == COLOR::white) ? BITBOARD::rank_7 : BITBOARD::rank_2;

      bitboard_t promoting_free_pawns = free_pawns & promotion_rank;
      bitboard_t not_promoting_free_pawns = free_pawns & ~promotion_rank;
      bitboard_t promoting_bishop_pinned_pawns = bishop_pinned_pawns & promotion_rank;
      bitboard_t not_promoting_bishop_pinned_pawns = bishop_pinned_pawns & ~promotion_rank;
      
      while (promoting_free_pawns) {
        square_t from = pop_lsb(promoting_free_pawns);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };
      while (not_promoting_free_pawns) {
        square_t from = pop_lsb(not_promoting_free_pawns);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
      while (promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(promoting_bishop_pinned_pawns);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_bishop_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };
      while (not_promoting_bishop_pinned_pawns) {
        square_t from = pop_lsb(not_promoting_bishop_pinned_pawns);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_bishop_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
    };

    inline void add_knight_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t free_knights = bitboards[PIECE::to_color(PIECE::knight, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (free_knights) {
        square_t from = pop_lsb(free_knights);
        bitboard_t possible_to = ATTACK::knight(from) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::knight, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
    };

    inline void add_bishop_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t bishop_pinned_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (bishop_pinned_bishops) {
        square_t from = pop_lsb(bishop_pinned_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_bishop_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
    };

    inline void add_rook_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t rook_pinned_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~bishop_pins[turn & 0b1] & rook_pins[turn & 0b1];
      while (rook_pinned_rooks) {
        square_t from = pop_lsb(rook_pinned_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_rook_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
    };

    inline void add_queen_moves(bitboard_t targets=BITBOARD::full) {
      bitboard_t bishop_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (bishop_pinned_queens) {
        square_t from = pop_lsb(bishop_pinned_queens);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_bishop_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t rook_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~bishop_pins[turn & 0b1] & rook_pins[turn & 0b1];
      while (rook_pinned_queens) {
        square_t from = pop_lsb(rook_pinned_queens);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_rook_rays[turn & 0b1] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~bishop_pins[turn & 0b1] & ~rook_pins[turn & 0b1];
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = ATTACK::queen(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          legal_moves[++legal_moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
    };

    inline void add_king_moves() {
      bitboard_t possible_to = (
        ATTACK::king(get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)])) &
        ~bitboards[PIECE::to_color(PIECE::none, turn)] &
        safe_king_squares[turn & 0b1]
      );
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        legal_moves[++legal_moves[0]] = MOVE::move(get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]), to, PIECE::to_color(PIECE::king, turn), PIECE::none, pieces[to], false, false, false);
      };
    };

    inline void add_check_evading_moves() {
      square_t checker_square = get_lsb(checkers);
      piece_t checker_type = PIECE::type(pieces[checker_square]);
      bitboard_t targets = (
        bitboard(checker_square) |
        ((king_bishop_attacks[turn & 0b1] & ATTACK::bishop(checker_square, bitboards[PIECE::both])) * (checker_type == PIECE::bishop)) |
        ((king_rook_attacks[turn & 0b1] & ATTACK::rook(checker_square, bitboards[PIECE::both])) * (checker_type == PIECE::rook)) |
        ((king_bishop_attacks[turn & 0b1] & ATTACK::bishop(checker_square, bitboards[PIECE::both])) * (checker_type == PIECE::queen && (king_bishop_attacks[turn & 0b1] & bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))]))) |
        ((king_rook_attacks[turn & 0b1] & ATTACK::rook(checker_square, bitboards[PIECE::both])) * (checker_type == PIECE::queen && (king_rook_attacks[turn & 0b1] & bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))])))
      );

      add_pawn_push_moves(targets);
      add_pawn_capture_moves(targets);
      add_enpassant_moves(targets);
      add_knight_moves(targets);
      add_bishop_moves(targets);
      add_rook_moves(targets);
      add_queen_moves(targets);
      add_king_moves();
    };

    inline void update_legal_moves() {
      legal_moves[0] = 0;

      if (in_check) {
        if (in_multicheck) {
          add_king_moves();
          return;
        }
        add_check_evading_moves();
        return;
      }
      add_pawn_push_moves();
      add_pawn_capture_moves();
      add_enpassant_moves();
      add_knight_moves();
      add_bishop_moves();
      add_rook_moves();
      add_queen_moves();
      add_king_moves();
      add_castling_moves();
    };

    // place a piece on the board without changing the turn
    inline void place_piece(piece_t piece, square_t square) {
      set_bit(bitboards[piece], square);
      set_bit(bitboards[PIECE::to_color(piece, COLOR::both)], square);
      set_bit(bitboards[PIECE::piece_color(piece)], square);
      set_bit(bitboards[PIECE::both], square);
      pieces[square] = piece;
      hash = ZOBRIST::hash(hash, piece, square, CASTLING::none, SQUARE::none, false);
    };

    // remove a piece from the board without changing the turn
    inline void remove_piece(square_t square) {
      piece_t piece = pieces[square];
      clear_bit(bitboards[piece], square);
      clear_bit(bitboards[PIECE::to_color(piece, COLOR::both)], square);
      clear_bit(bitboards[PIECE::piece_color(piece)], square);
      clear_bit(bitboards[PIECE::both], square);
      pieces[square] = PIECE::none;
      hash = ZOBRIST::hash(hash, piece, square, CASTLING::none, SQUARE::none, false);
    };

    inline void update() {
      if (uptodate) return;

      bitboard_t white_king = bitboards[PIECE::white_king];
      bitboard_t black_king = bitboards[PIECE::black_king];
      square_t white_king_square = get_lsb(white_king);
      square_t black_king_square = get_lsb(black_king);

      // update the attack maps and the safe king squares
      attacks.fill(BITBOARD::none);
      safe_king_squares.fill(BITBOARD::none);

      bitboard_t white_pawns = bitboards[PIECE::white_pawn];
      bitboard_t white_knights = bitboards[PIECE::white_knight];
      bitboard_t white_bishops = bitboards[PIECE::white_bishop];
      bitboard_t white_rooks = bitboards[PIECE::white_rook];
      bitboard_t white_queens = bitboards[PIECE::white_queen];
      while (white_pawns) {
        square_t square = pop_lsb(white_pawns);
        attacks[COLOR::white & 0b1] |= ATTACK::pawn(COLOR::white, square);
        safe_king_squares[COLOR::black & 0b1] |= ATTACK::pawn(COLOR::white, square);
      };
      while (white_knights) {
        square_t square = pop_lsb(white_knights);
        attacks[COLOR::white & 0b1] |= ATTACK::knight(square);
        safe_king_squares[COLOR::black & 0b1] |= ATTACK::knight(square);
      };
      while (white_bishops) {
        square_t square = pop_lsb(white_bishops);
        attacks[COLOR::white & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::black & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both] & ~black_king);
      };
      while (white_rooks) {
        square_t square = pop_lsb(white_rooks);
        attacks[COLOR::white & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::black & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both] & ~black_king);
      };
      while (white_queens) {
        square_t square = pop_lsb(white_queens);
        attacks[COLOR::white & 0b1] |= ATTACK::queen(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::black & 0b1] |= ATTACK::queen(square, bitboards[PIECE::both] & ~black_king);
      };
      attacks[COLOR::white & 0b1] |= ATTACK::king(white_king_square);
      safe_king_squares[COLOR::black & 0b1] |= ATTACK::king(white_king_square);
      safe_king_squares[COLOR::black & 0b1] = ~safe_king_squares[COLOR::black & 0b1];

      bitboard_t black_pawns = bitboards[PIECE::black_pawn];
      bitboard_t black_knights = bitboards[PIECE::black_knight];
      bitboard_t black_bishops = bitboards[PIECE::black_bishop];
      bitboard_t black_rooks = bitboards[PIECE::black_rook];
      bitboard_t black_queens = bitboards[PIECE::black_queen];
      while (black_pawns) {
        square_t square = pop_lsb(black_pawns);
        attacks[COLOR::black & 0b1] |= ATTACK::pawn(COLOR::black, square);
        safe_king_squares[COLOR::white & 0b1] |= ATTACK::pawn(COLOR::black, square);
      };
      while (black_knights) {
        square_t square = pop_lsb(black_knights);
        attacks[COLOR::black & 0b1] |= ATTACK::knight(square);
        safe_king_squares[COLOR::white & 0b1] |= ATTACK::knight(square);
      };
      while (black_bishops) {
        square_t square = pop_lsb(black_bishops);
        attacks[COLOR::black & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::white & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both] & ~white_king);
      };
      while (black_rooks) {
        square_t square = pop_lsb(black_rooks);
        attacks[COLOR::black & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::white & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both] & ~white_king);
      };
      while (black_queens) {
        square_t square = pop_lsb(black_queens);
        attacks[COLOR::black & 0b1] |= ATTACK::queen(square, bitboards[PIECE::both]);
        safe_king_squares[COLOR::white & 0b1] |= ATTACK::queen(square, bitboards[PIECE::both] & ~white_king);
      };
      attacks[COLOR::black & 0b1] |= ATTACK::king(black_king_square);
      safe_king_squares[COLOR::white & 0b1] |= ATTACK::king(black_king_square);
      safe_king_squares[COLOR::white & 0b1] = ~safe_king_squares[COLOR::white & 0b1];

      opponent_attacks = attacks[COLOR::opponent(turn) & 0b1];

      // update the bitboard of all the checkers
      checkers = attackers(get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)])) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))];

      // check if the current player is in check
      in_check = checkers;

      // check if the current player is in multi check
      in_multicheck = popcount(checkers) > 1;

      // get the king bishop rays
      king_bishop_rays[COLOR::white & 0b1] = ATTACK_RAY::bishop[white_king_square];
      king_bishop_rays[COLOR::black & 0b1] = ATTACK_RAY::bishop[black_king_square];

      // get the king rook rays
      king_rook_rays[COLOR::white & 0b1] = ATTACK_RAY::rook[white_king_square];
      king_rook_rays[COLOR::black & 0b1] = ATTACK_RAY::rook[black_king_square];

      // get the king bishop attacks
      king_bishop_attacks[COLOR::white & 0b1] = ATTACK::bishop(white_king_square, bitboards[PIECE::both]);
      king_bishop_attacks[COLOR::black & 0b1] = ATTACK::bishop(black_king_square, bitboards[PIECE::both]);

      // get the king rook attacks
      king_rook_attacks[COLOR::white & 0b1] = ATTACK::rook(white_king_square, bitboards[PIECE::both]);
      king_rook_attacks[COLOR::black & 0b1] = ATTACK::rook(black_king_square, bitboards[PIECE::both]);

      // get all the bishop pinned pieces
      bishop_pins.fill(BITBOARD::none);

      bitboard_t black_bishop_attackers = (bitboards[PIECE::black_bishop] | bitboards[PIECE::black_queen]) & king_bishop_rays[COLOR::white & 0b1];
      while (black_bishop_attackers) {
        square_t square = pop_lsb(black_bishop_attackers);
        bishop_pins[COLOR::white & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both]) & king_bishop_attacks[COLOR::white & 0b1];
      };
      
      bitboard_t white_bishop_attackers = (bitboards[PIECE::white_bishop] | bitboards[PIECE::white_queen]) & king_bishop_rays[COLOR::black & 0b1];
      while (white_bishop_attackers) {
        square_t square = pop_lsb(white_bishop_attackers);
        bishop_pins[COLOR::black & 0b1] |= ATTACK::bishop(square, bitboards[PIECE::both]) & king_bishop_attacks[COLOR::black & 0b1];
      };

      // get all the rook pinned pieces
      rook_pins.fill(BITBOARD::none);

      bitboard_t black_rook_attackers = (bitboards[PIECE::black_rook] | bitboards[PIECE::black_queen]) & king_rook_rays[COLOR::white & 0b1];
      while (black_rook_attackers) {
        square_t square = pop_lsb(black_rook_attackers);
        rook_pins[COLOR::white & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both]) & king_rook_attacks[COLOR::white & 0b1];
      };

      bitboard_t white_rook_attackers = (bitboards[PIECE::white_rook] | bitboards[PIECE::white_queen]) & king_rook_rays[COLOR::black & 0b1];
      while (white_rook_attackers) {
        square_t square = pop_lsb(white_rook_attackers);
        rook_pins[COLOR::black & 0b1] |= ATTACK::rook(square, bitboards[PIECE::both]) & king_rook_attacks[COLOR::black & 0b1];
      };

      // get all the enpassant pinned pawns
      enpassant_pins = BITBOARD::none;
      if (enpassant_square != SQUARE::none) {
        bitboard_t opponent_attackers = (bitboards[PIECE::to_color(PIECE::rook, COLOR::opponent(turn))] | bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))]) & king_rook_rays[turn & 0b1];
        bitboard_t double_pushed_pawn = bitboard(enpassant_square + 16 * (turn == COLOR::white) - 8);
        while (opponent_attackers) {
          square_t square = pop_lsb(opponent_attackers);
          enpassant_pins |= (
            ATTACK::rook(square, bitboards[PIECE::both] & ~double_pushed_pawn) &
            ATTACK::rook(get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]), bitboards[PIECE::both] & ~double_pushed_pawn) &
            bitboards[PIECE::to_color(PIECE::pawn, turn)]
          );
        };
      };

      update_legal_moves();

      uptodate = true;
    };

    // generate a move from a uci string
    inline move_t from_uci(string_t uci) {
      square_t from = (uci[0] - 'a') + 8 * (7 - (uci[1] - '1'));
      square_t to = (uci[2] - 'a') + 8 * (7 - (uci[3] - '1'));
      piece_t moved_piece = pieces[from];
      piece_t promoted_piece = PIECE::none;
      if (uci.length() == 5) {
        promoted_piece = PIECE::to_color(PIECE::from_char(uci[4]), turn);
      };
      piece_t captured_piece = pieces[to];
      bool double_pawn_push = (PIECE::type(moved_piece) == PIECE::pawn) && (((from - to) == 16) || ((from - to) == -16));
      bool enpassant = (PIECE::type(moved_piece) == PIECE::pawn) && (to == enpassant_square);
      bool castling = (PIECE::type(moved_piece) == PIECE::king) && (((from - to) == 2) || ((from - to) == -2));
      return MOVE::move(from, to, moved_piece, promoted_piece, captured_piece, double_pawn_push, enpassant, castling);
    };
};


std::ostream& operator<<(std::ostream& os, Board& board) { 
  return os << board.to_string();
};


#endif