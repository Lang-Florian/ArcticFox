// #ifdef __INTELLISENSE__
//   #pragma diag_suppress 28
// #endif
// #ifndef __SEARCH__MODULE__
// #define __SEARCH__MODULE__


// #include <string>


// /*
  
//   search
  
// */


// namespace search {
//   struct search_result_t {
//     pv_t pv;
//     score_t score;
//   };

//   template <color_t color>
//   score_t q_search(Board& board, int depth, score_t alpha, score_t beta) {
//     constexpr color_t opponent = color::compiletime::opponent(color);
//     if (depth == 0) {
//       return evaluation::evaluate<color>(board);
//     };
//     score_t score = evaluation::evaluate<color>(board);
//     if (score >= beta) {
//       return beta;
//     };
//     if (alpha < score) {
//       alpha = score;
//     };
//     move_stack_t moves = movegen::generate<color, check | capture, move_stack_t>(board);
//     moves.sort(move::comparison);
//     for (move_t move : moves) {
//       board.make(move);
//       score = evaluation::add_depth(q_search<opponent>(board, depth - 1, evaluation::remove_depth(beta), evaluation::remove_depth(alpha)));
//       board.unmake();
//       if (score >= beta) {
//         return beta;
//       };
//       if (score > alpha) {
//         alpha = score;
//       };
//     };
//     return alpha;
//   };

//   template <color_t color>
//   search_result_t search(Board& board, int depth, score_t alpha, score_t beta, pv_t old_pv=pv_t {}) {
//     constexpr color_t opponent = color::compiletime::opponent(color);
//     if (depth == 0) {
//       return search_result_t {pv_t {}, q_search<color>(board, MAX_Q_DEPTH, alpha, beta)};
//     };
//     if (board.position_existed()) {
//       return search_result_t {pv_t {}, evaluation::draw};
//     };
//     pv_t pv {};
//     transposition::entry_t& entry = transposition::get(board.zobrist.hash);
//     if (entry.is_valid(board.zobrist.hash, depth)) {
//       u8_t bound = entry.get_bound();
//       if (bound == transposition::exact_bound) {
//         pv.push(entry.move);
//         return search_result_t {pv, entry.get_score()};
//       } else if (bound == transposition::upper_bound && beta > entry.get_score()) {
//         beta = entry.get_score();
//       } else if (bound == transposition::lower_bound && alpha < entry.get_score()) {
//         alpha = entry.get_score();
//       };
//       if (alpha >= beta) {
//         return search_result_t {pv, entry.get_score()};
//       };
//     };
//     move_stack_t moves = movegen::generate<color, legal, move_stack_t>(board);
//     moves.sort(move::reverse_comparison);
//     if (old_pv.size() > 0) {
//       move_t move = old_pv.pop();
//       if (moves.contains(move)) {
//         moves.push(move);
//       } else {
//         pv.clear();
//       };
//     };
//     if (moves.contains(entry.move)) {
//       moves.push(entry.move);
//     };
//     moves.reverse();
//     u8_t bound = transposition::upper_bound;
//     for (move_t move : moves) {
//       board.make(move);
//       search_result_t search_result = search<opponent>(board, depth - 1, evaluation::remove_depth(beta), evaluation::remove_depth(alpha), old_pv);
//       board.unmake();
//       search_result.score = evaluation::add_depth(search_result.score);
//       if (search_result.score > alpha) {
//         pv = search_result.pv.copy();
//         pv.push(move);
//         if (search_result.score >= beta) {
//           entry.set(board.zobrist.hash, move, search_result.score, depth, transposition::lower_bound);
//           return search_result_t {pv, beta};
//         };
//         alpha = search_result.score;
//         bound = transposition::exact_bound;
//       };
//     };
//     entry.set(board.zobrist.hash, pv[0], alpha, depth, bound);
//     return search_result_t {pv, alpha};
//   };

//   search_result_t search (Board& board, int depth) {
//     search_result_t search_result;
//     pv_t pv {};
//     for (int i = 1; i <= depth; i++) {
//       if (board.turn == color::white) search_result = search<color::white>(board, i, -evaluation::inf, evaluation::inf, pv);
//       else search_result = search<color::black>(board, i, -evaluation::inf, evaluation::inf, pv);
//       if (board.turn == color::black) search_result.score = -search_result.score;
//       pv = search_result.pv.copy();
//       search_result.pv.reverse();
//     };
//     return search_result;
//   };
// };


// #endif // __SEARCH__MODULE__