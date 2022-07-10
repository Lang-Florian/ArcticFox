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

    U16 history_index;
    std::array<history_t, HISTORY_SIZE> history{};

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
      // if (pieces[SQUARE::A1] == PIECE::white_pawn) {
      //   std::cout << to_string() << std::endl;
      //   std::cout << SQUARE::to_string(MOVE::from(move))
      //             << " " << SQUARE::to_string(MOVE::to(move))
      //             << " " << PIECE::to_string(MOVE::moved_piece(move))
      //             << " " << PIECE::to_string(MOVE::promoted_piece(move))
      //             << " " << PIECE::to_string(MOVE::captured_piece(move))
      //             << " " << MOVE::double_pawn_push(move)
      //             << " " << MOVE::enpassant(move)
      //             << " " << MOVE::castling(move) << std::endl;
      //   std::cout << BITBOARD::to_string(bitboards[PIECE::white_pawn]) << std::endl;
      //   std::cout << "A1" << std::endl;
      // };
    };

    // check if the current player is in check
    inline bool is_check() {
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      return attackers(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
    };

    // check if the current player is in multi check
    inline bool is_multicheck() {
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      return popcount(attackers(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)]) > 1;
    };

    // get a bitboard of all the checkers
    inline bitboard_t checkers() {
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      return attackers(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
    };

    // get a bitboard of all the attacked squares of a given color
    inline bitboard_t attacks(color_t color) {
      bitboard_t attacks = BITBOARD::none;
      bitboard_t pawns = bitboards[PIECE::to_color(PIECE::pawn, color)];
      bitboard_t knights = bitboards[PIECE::to_color(PIECE::knight, color)];
      bitboard_t bishops = bitboards[PIECE::to_color(PIECE::bishop, color)];
      bitboard_t rooks = bitboards[PIECE::to_color(PIECE::rook, color)];
      bitboard_t queens = bitboards[PIECE::to_color(PIECE::queen, color)];
      while (pawns) {
        square_t square = pop_lsb(pawns);
        attacks |= ATTACK::pawn(color, square);
      };
      while (knights) {
        square_t square = pop_lsb(knights);
        attacks |= ATTACK::knight(square);
      };
      while (bishops) {
        square_t square = pop_lsb(bishops);
        attacks |= ATTACK::bishop(square, bitboards[PIECE::both]);
      };
      while (rooks) {
        square_t square = pop_lsb(rooks);
        attacks |= ATTACK::rook(square, bitboards[PIECE::both]);
      };
      while (queens) {
        square_t square = pop_lsb(queens);
        attacks |= ATTACK::queen(square, bitboards[PIECE::both]);
      };
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, color)]);
      attacks |= ATTACK::king(king_square);
      return attacks;
    };

    // get a bitboard of all safe squares for the king
    inline bitboard_t safe_king_squares() {
      bitboard_t attacks = BITBOARD::none;
      bitboard_t pawns = bitboards[PIECE::to_color(PIECE::pawn, COLOR::opponent(turn))];
      bitboard_t knights = bitboards[PIECE::to_color(PIECE::knight, COLOR::opponent(turn))];
      bitboard_t bishops = bitboards[PIECE::to_color(PIECE::bishop, COLOR::opponent(turn))];
      bitboard_t rooks = bitboards[PIECE::to_color(PIECE::rook, COLOR::opponent(turn))];
      bitboard_t queens = bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))];
      bitboard_t king = bitboards[PIECE::to_color(PIECE::king, turn)];
      while (pawns) {
        square_t square = pop_lsb(pawns);
        attacks |= ATTACK::pawn(COLOR::opponent(turn), square);
      };
      while (knights) {
        square_t square = pop_lsb(knights);
        attacks |= ATTACK::knight(square);
      };
      while (bishops) {
        square_t square = pop_lsb(bishops);
        attacks |= ATTACK::bishop(square, bitboards[PIECE::both] & ~king);
      };
      while (rooks) {
        square_t square = pop_lsb(rooks);
        attacks |= ATTACK::rook(square, bitboards[PIECE::both] & ~king);
      };
      while (queens) {
        square_t square = pop_lsb(queens);
        attacks |= ATTACK::queen(square, bitboards[PIECE::both] & ~king);
      };
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, COLOR::opponent(turn))]);
      attacks |= ATTACK::king(king_square);
      return ~attacks;
    };

    // get all the diagonal pinned pieces of a given color
    inline bitboard_t diagonal_pins(color_t color) {
      bitboard_t pins = BITBOARD::none;
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, color)]);
      bitboard_t attack_rays = ATTACK_RAY::bishop[king_square];
      bitboard_t bishops = bitboards[PIECE::to_color(PIECE::bishop, COLOR::opponent(color))] & attack_rays;
      bitboard_t queens = bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(color))] & attack_rays;
      while (bishops) {
        square_t square = pop_lsb(bishops);
        pins |= ATTACK::bishop(square, bitboards[PIECE::both]) & ATTACK::bishop(king_square, bitboards[PIECE::both]);
      };
      while (queens) {
        square_t square = pop_lsb(queens);
        pins |= ATTACK::bishop(square, bitboards[PIECE::both]) & ATTACK::bishop(king_square, bitboards[PIECE::both]);
      };
      return pins;
    };

    // get all the coordinate pinned pieces of a given color
    inline bitboard_t coordinate_pins(color_t color) {
      bitboard_t pins = BITBOARD::none;
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, color)]);
      bitboard_t attack_rays = ATTACK_RAY::rook[king_square];
      bitboard_t rooks = bitboards[PIECE::to_color(PIECE::rook, COLOR::opponent(color))] & attack_rays;
      bitboard_t queens = bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(color))] & attack_rays;
      while (rooks) {
        square_t square = pop_lsb(rooks);
        pins |= ATTACK::rook(square, bitboards[PIECE::both]) & ATTACK::rook(king_square, bitboards[PIECE::both]);
      };
      while (queens) {
        square_t square = pop_lsb(queens);
        pins |= ATTACK::rook(square, bitboards[PIECE::both]) & ATTACK::rook(king_square, bitboards[PIECE::both]);
      };
      return pins;
    };

    // get all the enpassant pinned pawns
    inline bitboard_t enpassant_pins() {
      bitboard_t pins = BITBOARD::none;
      if (enpassant_square == SQUARE::none) return pins;
      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      bitboard_t attack_rays = ATTACK_RAY::rook[king_square];
      bitboard_t rooks = bitboards[PIECE::to_color(PIECE::rook, COLOR::opponent(turn))] & attack_rays;
      bitboard_t queens = bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))] & attack_rays;
      remove_piece(enpassant_square + 16 * (turn == COLOR::white) - 8);
      while (rooks) {
        square_t square = pop_lsb(rooks);
        pins |= ATTACK::rook(square, bitboards[PIECE::both]) & ATTACK::rook(king_square, bitboards[PIECE::both]) & bitboards[PIECE::to_color(PIECE::pawn, turn)];
      };
      while (queens) {
        square_t square = pop_lsb(queens);
        pins |= ATTACK::rook(square, bitboards[PIECE::both]) & ATTACK::rook(king_square, bitboards[PIECE::both]) & bitboards[PIECE::to_color(PIECE::pawn, turn)];
      };
      place_piece(PIECE::to_color(PIECE::pawn, COLOR::opponent(turn)), enpassant_square + 16 * (turn == COLOR::white) - 8);
      return pins;
    };

    // get all legal moves
    inline std::array<move_t, MAX_LEGAL_MOVES> legal_moves() {
      std::array<move_t, MAX_LEGAL_MOVES> moves;
      moves[0] = 0;

      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      bitboard_t opponent_attacks = attacks(COLOR::opponent(turn));
      bitboard_t diagonal_pinned = diagonal_pins(turn);
      bitboard_t coordinate_pinned = coordinate_pins(turn);
      bitboard_t enpassant_pinned = enpassant_pins();
      bitboard_t king_diagonals = ATTACK_RAY::bishop[king_square];
      bitboard_t king_coordinates = ATTACK_RAY::rook[king_square];
      bitboard_t king_diagonals_with_occupancy = ATTACK::bishop(king_square, bitboards[PIECE::both]);
      bitboard_t king_coordinates_with_occupancy = ATTACK::rook(king_square, bitboards[PIECE::both]);

      // move king in case of multicheck
      if (is_multicheck()) {
        bitboard_t possible_to = ATTACK::king(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & safe_king_squares();
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(king_square, to, PIECE::to_color(PIECE::king, turn), PIECE::none, pieces[to], false, false, false);
        };
        return moves;
      };

      // evade a check
      if (is_check()) {
        bitboard_t attacker_square = get_lsb(checkers());
        piece_t attacker_type = PIECE::type(pieces[attacker_square]);
        bitboard_t targets = (
          bitboard(attacker_square) |
          ((king_diagonals_with_occupancy & ATTACK::bishop(attacker_square, bitboards[PIECE::both])) * (attacker_type == PIECE::bishop)) |
          ((king_coordinates_with_occupancy & ATTACK::rook(attacker_square, bitboards[PIECE::both])) * (attacker_type == PIECE::rook)) |
          ((king_diagonals_with_occupancy & ATTACK::bishop(attacker_square, bitboards[PIECE::both])) * (attacker_type == PIECE::queen && (king_diagonals_with_occupancy & bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))]))) |
          ((king_coordinates_with_occupancy & ATTACK::rook(attacker_square, bitboards[PIECE::both])) * (attacker_type == PIECE::queen && (king_coordinates_with_occupancy & bitboards[PIECE::to_color(PIECE::queen, COLOR::opponent(turn))])))
        );
        return check_evasion(targets);
      }

      // pawn moves
      bitboard_t diagonal_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)]   &  diagonal_pinned & ~coordinate_pinned;
      bitboard_t coordinate_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ~diagonal_pinned &  coordinate_pinned;
      bitboard_t free_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)]              & ~diagonal_pinned & ~coordinate_pinned;
      bitboard_t enpassant_pinned_pawns = enpassant_pins();

      bitboard_t pushable_free_pawns;
      bitboard_t double_pushable_free_pawns; //

      bitboard_t pushable_free_pawns_promoting; //
      bitboard_t pushable_free_pawns_not_promoting; //

      bitboard_t free_pawns_promoting; //
      bitboard_t free_pawns_not_promoting; //

      bitboard_t diagonal_pinned_pawns_promoting; //
      bitboard_t diagonal_pinned_pawns_not_promoting; //

      bitboard_t pushable_coordinate_pinned_pawns;
      bitboard_t double_pushable_coordinate_pinned_pawns;

      if (turn == COLOR::white) {
        pushable_free_pawns = free_pawns & ~(bitboards[PIECE::both] << 8);
        double_pushable_free_pawns = pushable_free_pawns & ~(bitboards[PIECE::both] << 16) & BITBOARD::rank_2;

        pushable_free_pawns_promoting = pushable_free_pawns & BITBOARD::rank_7;
        pushable_free_pawns_not_promoting = pushable_free_pawns & ~BITBOARD::rank_7;

        free_pawns_promoting = free_pawns & BITBOARD::rank_7;
        free_pawns_not_promoting = free_pawns & ~BITBOARD::rank_7;

        diagonal_pinned_pawns_promoting = diagonal_pinned_pawns & BITBOARD::rank_7;
        diagonal_pinned_pawns_not_promoting = diagonal_pinned_pawns & ~BITBOARD::rank_7;

        pushable_coordinate_pinned_pawns = coordinate_pinned_pawns & ~(bitboards[PIECE::both] << 8);
        double_pushable_coordinate_pinned_pawns = pushable_coordinate_pinned_pawns & ~(bitboards[PIECE::both] << 16) & BITBOARD::rank_2;
      } else {
        pushable_free_pawns = free_pawns & ~(bitboards[PIECE::both] >> 8);
        double_pushable_free_pawns = pushable_free_pawns & ~(bitboards[PIECE::both] >> 16) & BITBOARD::rank_7;

        pushable_free_pawns_promoting = pushable_free_pawns & BITBOARD::rank_2;
        pushable_free_pawns_not_promoting = pushable_free_pawns & ~BITBOARD::rank_2;

        free_pawns_promoting = free_pawns & BITBOARD::rank_2;
        free_pawns_not_promoting = free_pawns & ~BITBOARD::rank_2;

        diagonal_pinned_pawns_promoting = diagonal_pinned_pawns & BITBOARD::rank_2;
        diagonal_pinned_pawns_not_promoting = diagonal_pinned_pawns & ~BITBOARD::rank_2;

        pushable_coordinate_pinned_pawns = coordinate_pinned_pawns & ~(bitboards[PIECE::both] >> 8);
        double_pushable_coordinate_pinned_pawns = pushable_coordinate_pinned_pawns & ~(bitboards[PIECE::both] >> 16) & BITBOARD::rank_7;
      };

      while (double_pushable_free_pawns) {
        square_t from = pop_lsb(double_pushable_free_pawns);
        square_t to = from - 32 * (turn == COLOR::white) + 16;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, true, false, false);
      };

      while (pushable_free_pawns_promoting) {
        square_t from = pop_lsb(pushable_free_pawns_promoting);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), PIECE::none, false, false, false);
      };

      while (pushable_free_pawns_not_promoting) {
        square_t from = pop_lsb(pushable_free_pawns_not_promoting);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, false, false);
      };

      while (free_pawns_promoting) {
        square_t from = pop_lsb(free_pawns_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };

      while (free_pawns_not_promoting) {
        square_t from = pop_lsb(free_pawns_not_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      while (diagonal_pinned_pawns_promoting) {
        square_t from = pop_lsb(diagonal_pinned_pawns_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_diagonals;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };

      while (diagonal_pinned_pawns_not_promoting) {
        square_t from = pop_lsb(diagonal_pinned_pawns_not_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_diagonals;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      while (pushable_coordinate_pinned_pawns) {
        square_t from = pop_lsb(pushable_coordinate_pinned_pawns);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, false, false);
        moves[0] -= !get_bit(king_coordinates, to);
      };

      while (double_pushable_coordinate_pinned_pawns) {
        square_t from = pop_lsb(double_pushable_coordinate_pinned_pawns);
        square_t to = from - 32 * (turn == COLOR::white) + 16;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, true, false, false);
        moves[0] -= !get_bit(king_coordinates, to);
      };

      // enpassant moves
      if (enpassant_square != SQUARE::none) {
        bitboard_t diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & diagonal_pinned & ~coordinate_pinned;
        while (diagonal_pinned_enpassant_pawns) {
          square_t from = pop_lsb(diagonal_pinned_enpassant_pawns);
          moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
          moves[0] -= !get_bit(king_diagonals, enpassant_square);
        };

        bitboard_t not_diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & ~diagonal_pinned & ~coordinate_pinned;
        while (not_diagonal_pinned_enpassant_pawns) {
          square_t from = pop_lsb(not_diagonal_pinned_enpassant_pawns);
          moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
        };
      };

      // standard piece moves
      bitboard_t free_knights = bitboards[PIECE::to_color(PIECE::knight, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_knights) {
        square_t from = pop_lsb(free_knights);
        bitboard_t possible_to = ATTACK::knight(from) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::knight, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
      
      bitboard_t diagonal_pinned_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & diagonal_pinned & ~coordinate_pinned;
      while (diagonal_pinned_bishops) {
        square_t from = pop_lsb(diagonal_pinned_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_diagonals;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t coordinate_pinned_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~diagonal_pinned & coordinate_pinned;
      while (coordinate_pinned_rooks) {
        square_t from = pop_lsb(coordinate_pinned_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_coordinates;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t diagonal_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & diagonal_pinned & ~coordinate_pinned;
      while (diagonal_pinned_queens) {
        square_t from = pop_lsb(diagonal_pinned_queens);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_diagonals;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t coordinate_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~diagonal_pinned & coordinate_pinned;
      while (coordinate_pinned_queens) {
        square_t from = pop_lsb(coordinate_pinned_queens);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_coordinates;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = ATTACK::queen(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)];
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      // king moves
      bitboard_t possible_to = ATTACK::king(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & safe_king_squares();
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves[++moves[0]] = MOVE::move(king_square, to, PIECE::to_color(PIECE::king, turn), PIECE::none, pieces[to], false, false, false);
      };

      // castling moves
      moves[++moves[0]] = MOVE::move(SQUARE::E1, SQUARE::G1, PIECE::white_king, PIECE::none, PIECE::none, false, false, true);
      moves[0] -= !(
        (turn == COLOR::white) &&
        !(opponent_attacks & CASTLING::white_king_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::white_king_piece_mask) &&
        (castling_rights & CASTLING::white_king)
      );
      moves[++moves[0]] = MOVE::move(SQUARE::E1, SQUARE::C1, PIECE::white_king, PIECE::none, PIECE::none, false, false, true);
      moves[0] -= !(
        (turn == COLOR::white) &&
        !(opponent_attacks & CASTLING::white_queen_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::white_queen_piece_mask) &&
        (castling_rights & CASTLING::white_queen)
      );
      moves[++moves[0]] = MOVE::move(SQUARE::E8, SQUARE::G8, PIECE::black_king, PIECE::none, PIECE::none, false, false, true);
      moves[0] -= !(
        (turn == COLOR::black) &&
        !(opponent_attacks & CASTLING::black_king_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::black_king_piece_mask) &&
        (castling_rights & CASTLING::black_king)
      );
      moves[++moves[0]] = MOVE::move(SQUARE::E8, SQUARE::C8, PIECE::black_king, PIECE::none, PIECE::none, false, false, true);
      moves[0] -= !(
        (turn == COLOR::black) &&
        !(opponent_attacks & CASTLING::black_queen_attack_mask) &&
        !(bitboards[PIECE::both] & CASTLING::black_queen_piece_mask) &&
        (castling_rights & CASTLING::black_queen)
      );

      return moves;
    };

    // get all check evading moves
    inline std::array<move_t, MAX_LEGAL_MOVES> check_evasion(bitboard_t targets) {
      std::array<move_t, MAX_LEGAL_MOVES> moves;
      moves[0] = 0;

      square_t king_square = get_lsb(bitboards[PIECE::to_color(PIECE::king, turn)]);
      bitboard_t opponent_attacks = attacks(COLOR::opponent(turn));
      bitboard_t diagonal_pinned = diagonal_pins(turn);
      bitboard_t coordinate_pinned = coordinate_pins(turn);
      bitboard_t enpassant_pinned = enpassant_pins();
      bitboard_t king_diagonals = ATTACK_RAY::bishop[king_square];
      bitboard_t king_coordinates = ATTACK_RAY::rook[king_square];

      // pawn moves
      bitboard_t diagonal_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)]      &  diagonal_pinned & ~coordinate_pinned;
      bitboard_t coordinate_pinned_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)]    & ~diagonal_pinned &  coordinate_pinned;
      bitboard_t free_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)]                 & ~diagonal_pinned & ~coordinate_pinned;
      bitboard_t enpassant_pinned_pawns = enpassant_pins();

      bitboard_t pushable_free_pawns;
      bitboard_t double_pushable_free_pawns; //

      bitboard_t pushable_free_pawns_promoting; //
      bitboard_t pushable_free_pawns_not_promoting; //

      bitboard_t free_pawns_promoting; //
      bitboard_t free_pawns_not_promoting; //

      bitboard_t diagonal_pinned_pawns_promoting; //
      bitboard_t diagonal_pinned_pawns_not_promoting; //

      bitboard_t pushable_coordinate_pinned_pawns;
      bitboard_t double_pushable_coordinate_pinned_pawns;

      if (turn == COLOR::white) {
        pushable_free_pawns = free_pawns & ~(bitboards[PIECE::both] << 8);
        double_pushable_free_pawns = pushable_free_pawns & ~(bitboards[PIECE::both] << 16) & BITBOARD::rank_2;

        pushable_free_pawns_promoting = pushable_free_pawns & BITBOARD::rank_7;
        pushable_free_pawns_not_promoting = pushable_free_pawns & ~BITBOARD::rank_7;

        free_pawns_promoting = free_pawns & BITBOARD::rank_7;
        free_pawns_not_promoting = free_pawns & ~BITBOARD::rank_7;

        diagonal_pinned_pawns_promoting = diagonal_pinned_pawns & BITBOARD::rank_7;
        diagonal_pinned_pawns_not_promoting = diagonal_pinned_pawns & ~BITBOARD::rank_7;

        pushable_coordinate_pinned_pawns = coordinate_pinned_pawns & ~(bitboards[PIECE::both] << 8);
        double_pushable_coordinate_pinned_pawns = pushable_coordinate_pinned_pawns & ~(bitboards[PIECE::both] << 16) & BITBOARD::rank_2;
      } else {
        pushable_free_pawns = free_pawns & ~(bitboards[PIECE::both] >> 8);
        double_pushable_free_pawns = pushable_free_pawns & ~(bitboards[PIECE::both] >> 16) & BITBOARD::rank_7;

        pushable_free_pawns_promoting = pushable_free_pawns & BITBOARD::rank_2;
        pushable_free_pawns_not_promoting = pushable_free_pawns & ~BITBOARD::rank_2;

        free_pawns_promoting = free_pawns & BITBOARD::rank_2;
        free_pawns_not_promoting = free_pawns & ~BITBOARD::rank_2;

        diagonal_pinned_pawns_promoting = diagonal_pinned_pawns & BITBOARD::rank_2;
        diagonal_pinned_pawns_not_promoting = diagonal_pinned_pawns & ~BITBOARD::rank_2;

        pushable_coordinate_pinned_pawns = coordinate_pinned_pawns & ~(bitboards[PIECE::both] >> 8);
        double_pushable_coordinate_pinned_pawns = pushable_coordinate_pinned_pawns & ~(bitboards[PIECE::both] >> 16) & BITBOARD::rank_7;
      };

      while (double_pushable_free_pawns) {
        square_t from = pop_lsb(double_pushable_free_pawns);
        square_t to = from - 32 * (turn == COLOR::white) + 16;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, true, false, false);
        moves[0] -= !get_bit(targets, to);
      };

      while (pushable_free_pawns_promoting) {
        square_t from = pop_lsb(pushable_free_pawns_promoting);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), PIECE::none, false, false, false);
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), PIECE::none, false, false, false);
        moves[0] -= 4 * !get_bit(targets, to);
      };

      while (pushable_free_pawns_not_promoting) {
        square_t from = pop_lsb(pushable_free_pawns_not_promoting);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, false, false);
        moves[0] -= !get_bit(targets, to);
      };

      while (free_pawns_promoting) {
        square_t from = pop_lsb(free_pawns_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };

      while (free_pawns_not_promoting) {
        square_t from = pop_lsb(free_pawns_not_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      while (diagonal_pinned_pawns_promoting) {
        square_t from = pop_lsb(diagonal_pinned_pawns_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_diagonals & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::knight, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::bishop, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::rook, turn), pieces[to], false, false, false);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::to_color(PIECE::queen, turn), pieces[to], false, false, false);
        };
      };

      while (diagonal_pinned_pawns_not_promoting) {
        square_t from = pop_lsb(diagonal_pinned_pawns_not_promoting);
        bitboard_t possible_to = ATTACK::pawn(turn, from) & bitboards[PIECE::to_color(PIECE::none, COLOR::opponent(turn))] & king_diagonals & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      while (pushable_coordinate_pinned_pawns) {
        square_t from = pop_lsb(pushable_coordinate_pinned_pawns);
        square_t to = from - 16 * (turn == COLOR::white) + 8;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, false, false);
        moves[0] -= !get_bit(king_coordinates, to) | !get_bit(targets, to);
      };

      while (double_pushable_coordinate_pinned_pawns) {
        square_t from = pop_lsb(double_pushable_coordinate_pinned_pawns);
        square_t to = from - 32 * (turn == COLOR::white) + 16;
        moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, true, false, false);
        moves[0] -= !get_bit(king_coordinates, to) | !get_bit(targets, to);
      };

      // enpassant moves
      if (enpassant_square != SQUARE::none) {
        bitboard_t diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & diagonal_pinned & ~coordinate_pinned;
        while (diagonal_pinned_enpassant_pawns) {
          square_t from = pop_lsb(diagonal_pinned_enpassant_pawns);
          moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
          moves[0] -= !get_bit(king_diagonals, enpassant_square) | !get_bit(targets, enpassant_square);
        };

        bitboard_t not_diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & ~diagonal_pinned & ~coordinate_pinned;
        while (not_diagonal_pinned_enpassant_pawns) {
          square_t from = pop_lsb(not_diagonal_pinned_enpassant_pawns);
          moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
          moves[0] -= !get_bit(targets, enpassant_square);
        };

        if (enpassant_square + 16 * (turn == COLOR::white) - 8 == get_lsb(checkers())) {
          bitboard_t diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & diagonal_pinned & ~coordinate_pinned;
          while (diagonal_pinned_enpassant_pawns) {
            square_t from = pop_lsb(diagonal_pinned_enpassant_pawns);
            moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
            moves[0] -= !get_bit(king_diagonals, enpassant_square);
          };

          bitboard_t not_diagonal_pinned_enpassant_pawns = bitboards[PIECE::to_color(PIECE::pawn, turn)] & ATTACK::pawn(COLOR::opponent(turn), enpassant_square) & ~enpassant_pinned_pawns & ~diagonal_pinned & ~coordinate_pinned;
          while (not_diagonal_pinned_enpassant_pawns) {
            square_t from = pop_lsb(not_diagonal_pinned_enpassant_pawns);
            moves[++moves[0]] = MOVE::move(from, enpassant_square, PIECE::to_color(PIECE::pawn, turn), PIECE::none, PIECE::none, false, true, false);
          };
        };
      };

      // standard piece moves
      bitboard_t free_knights = bitboards[PIECE::to_color(PIECE::knight, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_knights) {
        square_t from = pop_lsb(free_knights);
        bitboard_t possible_to = ATTACK::knight(from) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::knight, turn), PIECE::none, pieces[to], false, false, false);
        };
      };
      
      bitboard_t diagonal_pinned_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & diagonal_pinned & ~coordinate_pinned;
      while (diagonal_pinned_bishops) {
        square_t from = pop_lsb(diagonal_pinned_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_diagonals & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_bishops = bitboards[PIECE::to_color(PIECE::bishop, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_bishops) {
        square_t from = pop_lsb(free_bishops);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::bishop, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t coordinate_pinned_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~diagonal_pinned & coordinate_pinned;
      while (coordinate_pinned_rooks) {
        square_t from = pop_lsb(coordinate_pinned_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_coordinates & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_rooks = bitboards[PIECE::to_color(PIECE::rook, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_rooks) {
        square_t from = pop_lsb(free_rooks);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::rook, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t diagonal_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & diagonal_pinned & ~coordinate_pinned;
      while (diagonal_pinned_queens) {
        square_t from = pop_lsb(diagonal_pinned_queens);
        bitboard_t possible_to = ATTACK::bishop(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_diagonals & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t coordinate_pinned_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~diagonal_pinned & coordinate_pinned;
      while (coordinate_pinned_queens) {
        square_t from = pop_lsb(coordinate_pinned_queens);
        bitboard_t possible_to = ATTACK::rook(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & king_coordinates & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      bitboard_t free_queens = bitboards[PIECE::to_color(PIECE::queen, turn)] & ~diagonal_pinned & ~coordinate_pinned;
      while (free_queens) {
        square_t from = pop_lsb(free_queens);
        bitboard_t possible_to = ATTACK::queen(from, bitboards[PIECE::both]) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & targets;
        while (possible_to) {
          square_t to = pop_lsb(possible_to);
          moves[++moves[0]] = MOVE::move(from, to, PIECE::to_color(PIECE::queen, turn), PIECE::none, pieces[to], false, false, false);
        };
      };

      // king moves
      bitboard_t possible_to = ATTACK::king(king_square) & ~bitboards[PIECE::to_color(PIECE::none, turn)] & safe_king_squares();
      while (possible_to) {
        square_t to = pop_lsb(possible_to);
        moves[++moves[0]] = MOVE::move(king_square, to, PIECE::to_color(PIECE::king, turn), PIECE::none, pieces[to], false, false, false);
      };

      return moves;
    };

    // print a perft
    inline U64 perft(int depth) {
      std::cout << "\tperft " << depth << " of " << fen() << std::endl << std::endl;
      U64 start = TIME::ms();
      U64 count = 0;
      std::array<move_t, MAX_LEGAL_MOVES> moves = legal_moves();
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
      std::array<move_t, MAX_LEGAL_MOVES> moves = legal_moves();
      for (int i = 1; i <= moves[0]; i++) {
        make(moves[i]);
        count += perft(depth - 1, true);
        unmake();
      };
      return count;
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