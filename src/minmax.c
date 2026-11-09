#include <stdint.h>
#include "minmax.h"
#include "game_status.h"
#include "legal_move.h"
#include "piece_value.h"
#include "special_move.h"
#include "transposition_table.h"
#include "utils.h"
#include "position_history.h"

#define ENABLE_TRANS_TABLE 1

static MoveReport apply_search_move(ChessBoard *board, Move move_to_apply) {
  MoveReport report = make_move(board, move_to_apply.src, move_to_apply.dest);

  if (can_promote_pawn(board, move_to_apply.dest, move_to_apply.dest)) {
    Piece pawn = get_piece(board, move_to_apply.dest);
    PieceName promotion =
        move_to_apply.promotion == NONE ? QUEEN : move_to_apply.promotion;
    set_piece(board, move_to_apply.dest, create_piece(promotion, pawn.color));
  }
  return report;
}

/*
    It's preferred to use a clone of the board to avoid modifying the original
   board state during the search. However, we won't make clone of the passed
   board, as it would be a waste of memory and time. The user should manage the
   cloning of the board before calling this function, to ensure that his
   original board is not modified.
*/

/*
alpha: best score ensured by the max color
beta: best score ensured by the min color
 */

int32_t minmax_ab(ChessBoard *board, ChessColor color_max,
                  ChessColor curr_color, Move *best_move, unsigned int depth,
                  int32_t alpha, int32_t beta, GameContext *context) {
  GameStatus status;
  if (depth <= 0 || is_end_game(board, context, curr_color, &status)) {
    return evaluate_board(board, color_max, context, depth);
  }

  Move selected_move; // the best move found so far
  Move move;
  int32_t score;

#if ENABLE_TRANS_TABLE
  if (fetch_from_transposition_table(board->meta.hash, &score, &selected_move,
                                     alpha, beta, depth)) {
    *best_move = selected_move;
    return score;
  }

  int32_t alpha_orig = alpha;
  int32_t beta_orig = beta;
#endif

  // If not found in the transposition table, we will compute the score and add
  // it to the table later

  bool max_step = (curr_color == color_max);
  int32_t best_score = max_step ? -INFINITY : INFINITY;

  MoveList legal_moves;
  generate_legal_moves(board, curr_color, false, &legal_moves);
  order_captures_first(&legal_moves);

  for (unsigned int i = 0; i < legal_moves.count; i++) {
    move = legal_moves.moves[i];

    score = evaluate_move_ab(board, color_max, curr_color, move, depth, alpha,
                             beta, context);

    // Best score update
    if (
        // min
        (!max_step && (i == 0 || score < best_score)) ||
        // max
        (max_step && (i == 0 || score > best_score))) {
      best_score = score;
      selected_move = move;
    }

    // Alpha beta prunning
    // min
    if (!max_step && score <= alpha) {
      *best_move = selected_move;
      return best_score;
    }
    // max
    if (max_step && score >= beta) {
      *best_move = selected_move;
      return best_score;
    }

    // Alpha beta update
    if (max_step) {
      alpha = max(alpha, score);
    } else {
      beta = min(beta, score);
    }
  }

  *best_move = selected_move;

#if ENABLE_TRANS_TABLE
  add_to_transposition_table(board->meta.hash, best_score, depth, selected_move,
                             alpha_orig, beta_orig);
#endif

  return best_score;
}

/* This version that enables the use of many cpu at once, was written by GPT 5.6
 * Luna via VS Code Copilot SDK
 */
int32_t minmax_ab_parallel(ChessBoard *board, ChessColor color_max,
                           ChessColor curr_color, Move *best_move,
                           unsigned int depth, GameContext *context) {

  MoveList legal_moves;
  GameStatus status;
  generate_legal_moves(board, curr_color, false, &legal_moves);
  order_captures_first(&legal_moves);

  int32_t best_score = (curr_color == color_max) ? -INFINITY : INFINITY;
  Move selected_move;
  bool has_best = false;

  if (depth <= 0 || is_end_game(board, context, curr_color, &status)) {
    return evaluate_board(board, color_max, context, depth);
  }

#if ENABLE_TRANS_TABLE
  if (fetch_from_transposition_table(board->meta.hash, &best_score,
                                     &selected_move, -INFINITY, INFINITY,
                                     depth)) {
    *best_move = selected_move;
    return best_score;
  }
#endif

#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < (int)legal_moves.count; i++) {
    Move move = legal_moves.moves[i];
    ChessBoard board_copy = clone_board(board);
    int32_t score = evaluate_move_ab(&board_copy, color_max, curr_color, move,
                                     depth, -INFINITY, INFINITY, context);

#pragma omp critical
    {
      bool is_better =
          (curr_color == color_max) ? score > best_score : score < best_score;

      if (!has_best || is_better) {
        best_score = score;
        selected_move = move;
        has_best = true;
      }
    }
  }

  *best_move = selected_move;

#if ENABLE_TRANS_TABLE
  add_to_transposition_table(board->meta.hash, best_score, depth, selected_move,
                             -INFINITY, INFINITY);
#endif

  return best_score;
}

int32_t evaluate_move_ab(ChessBoard *board, ChessColor color_max,
                         ChessColor curr_color, Move move_to_apply,
                         unsigned int depth, int32_t alpha, int32_t beta, GameContext *context) {
  GameStatus status;
  MoveReport report = apply_search_move(board, move_to_apply);
  if (context != NULL) {
    increment_position_history_counter(&context->history_counter, board->meta.hash);
  }

  if (depth <= 0 || is_end_game(board, context, curr_color, &status)) {
    int32_t score = evaluate_board(board, color_max, context, depth);
    if (context != NULL) {
      decrement_position_history_counter(&context->history_counter, board->meta.hash);
    }
    undo_move(board, &report);
    return score;
  }

  ChessColor opponent_color = get_opponent_color(curr_color);
  int32_t score = minmax_ab(board, color_max, opponent_color, &move_to_apply,
                            depth - 1, alpha, beta, context);
  // undo move
  if (context != NULL) {
    decrement_position_history_counter(&context->history_counter, board->meta.hash);
  }
  undo_move(board, &report);
  return score;
}

static bool _is_endgame_phase(const ChessBoard *board) {
  int total = 0;
  for (int color = 0; color < 2; color++) {
    for (int name = BISHOP; name <= QUEEN; name++) {
      total += PIECE_COUNT(*board, color, name) *get_piece_value(name);
    }
  }
  return total <= 2400;
}
 

static int _positional_score(const ChessBoard *board, ChessColor color) {
  bool endgame = _is_endgame_phase(board);
  int score = 0;
 
  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      Piece piece = board->squares[row][col];
      if (piece.name == NONE || piece.color != color) {
        continue;
      }
      score += heatmap_value(piece, create_coords(row, col), endgame);
    }
  }
  return score;
}

int32_t _present_pieces_score(ChessBoard *board, ChessColor color) {
  int32_t score = 0;
  for (unsigned int piece_name = PAWN; piece_name <= QUEEN; piece_name++) {
    score +=
        PIECE_COUNT(*board, color, piece_name) * get_piece_value(piece_name);
  }
  return score;
}

int32_t evaluate_board(ChessBoard *board, ChessColor color_max,
                       GameContext *context, unsigned int depth) {
  ChessColor to_move = board->meta.current_turn;
  ChessColor other = get_opponent_color(color_max);
  GameStatus st = get_game_status(board, to_move, context);

  if (st == GAME_CHECKMATE) {
    int32_t mate = INFINITY + (int32_t)depth * 100;
    return (to_move == color_max) ? -mate : mate;
  }
  if (st == GAME_STALEMATE || st == GAME_DRAW_50_MOVE_RULE ||
      st == GAME_DRAW_INSUFFICIENT_MATERIAL ||
      st == GAME_DRAW_3FOLD_REPETITION) {
    return 0;
  }

  int32_t score = _present_pieces_score(board, color_max) +
                  _positional_score(board, color_max) -
                  _present_pieces_score(board, other) -
                  _positional_score(board, other);

  if (st == GAME_CHECK) {
    score += (to_move == color_max) ? -CHECK_BONUS : CHECK_BONUS;
  }

  if (context != NULL) {
    int rep = (int)get_position_history_count(&context->history_counter,
                                              board->meta.hash) - 1;
    if (rep > 0) {
      score -= rep * POSITION_REPETITION_PENALTY;
    }
  }
  return score;
}