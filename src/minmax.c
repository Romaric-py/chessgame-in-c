#include <stdint.h>
#include "minmax.h"
#include "legal_move.h"
#include "game_status.h"
#include "special_move.h"
#include "utils.h"
#include "transposition_table.h"
#include "piece_value.h"

uint64_t board_hash = 0ULL; // Global variable to store the current board hash

static MoveReport apply_search_move(Board *board, Move move_to_apply) {
    MoveReport report = make_move(board, move_to_apply.src, move_to_apply.dest);

    if (can_promote_pawn(board, move_to_apply.dest, move_to_apply.dest)) {
        Piece pawn = get_piece(board, move_to_apply.dest);
        PieceName promotion = move_to_apply.promotion == NONE
            ? QUEEN : move_to_apply.promotion;
        set_piece(board, move_to_apply.dest, create_piece(promotion, pawn.color));
    }
    return report;
}

/*
    It's preferred to use a clone of the board to avoid modifying the original board state during the search. 
    However, we won't make clone of the passed board, as it would be a waste of memory and time. 
    The user should manage the cloning of the board before calling this function, to ensure that his original board is not modified.
*/
//! (since we can't garantee yet that our functions are free of side effects)


/*
alpha: best score ensured by the max color
beta: best score ensured by the min color
 */

int minmax_ab(Board *board, Color color_max, Color curr_color, Move *best_move, unsigned int depth, int alpha, int beta) {
    Move selected_move; // the best move found so far
    Move move;
    int score;

    if (depth <= 0 || is_end_game(board)) {
        return evaluate_board(board, color_max);
    }

    if (board_hash == 0ULL) {
        board_hash = hash_board_state(board);
    }

    if (fetch_from_transposition_table(board_hash, &score, &selected_move, alpha, beta, depth)) {
        *best_move = selected_move;
        return score;
    }
    // If not found in the transposition table, we will compute the score and add it to the table later

    bool max_step = (curr_color == color_max);
    int best_score = max_step ? -INFINITY : INFINITY;

    MoveList legal_moves;
    generate_legal_moves(board, curr_color, false, &legal_moves);
    order_captures_first(&legal_moves);
    
    for (unsigned int i = 0; i < legal_moves.count; i++) {
        move = legal_moves.moves[i];

        score = evaluate_move_ab(board, color_max, curr_color, move, depth, alpha, beta);

        // Best score update                    
        if (
            // min
            (!max_step && (i == 0 || score < best_score)) ||
            // max
            (max_step && (i == 0 || score > best_score))
        ) {
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
    add_to_transposition_table(board_hash, best_score, depth, selected_move, alpha, beta);
    return best_score;
}


/* This version that enables the use of many cpu at once, was written by GPT 5.6 Luna via VS Code Copilot SDK
*/
int minmax_ab_parallel(Board *board, Color color_max, Color curr_color, Move *best_move, unsigned int depth) {
    
    MoveList legal_moves;
    generate_legal_moves(board, curr_color, false, &legal_moves);
    order_captures_first(&legal_moves);

    int best_score = (curr_color == color_max) ? -INFINITY : INFINITY;
    Move selected_move;
    bool has_best = false;

    if (depth <= 0 || is_end_game(board)) {
        return evaluate_board(board, color_max);
    } 

    if (fetch_from_transposition_table(board_hash, &best_score, &selected_move, -INFINITY, INFINITY, depth)) {
        *best_move = selected_move;
        return best_score;
    }


    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int) legal_moves.count; i++) {
        Move move = legal_moves.moves[i];
        Board board_copy = clone_board(board);
        int score = evaluate_move_ab(
            &board_copy,
            color_max,
            curr_color,
            move,
            depth,
            -INFINITY,
            INFINITY
        );

        #pragma omp critical
        {
            bool is_better = (curr_color == color_max)
                ? score > best_score
                : score < best_score;

            if (!has_best || is_better) {
                best_score = score;
                selected_move = move;
                has_best = true;
            }
        }
    }

    *best_move = selected_move;
    add_to_transposition_table(board_hash, best_score, depth, selected_move, -INFINITY, INFINITY);
    return best_score;
}


int evaluate_move_ab(Board *board, Color color_max, Color curr_color, Move move_to_apply, unsigned int depth, int alpha, int beta) {
    MoveReport report = apply_search_move(board, move_to_apply);
    increment_hash_board_state(&board_hash, board, &report);

    if (depth <= 0 || is_end_game(board)) {
        increment_hash_board_state(&board_hash, board, &report); // revert the hash back
        undo_move(board, &report);
        return evaluate_board(board, color_max);
    }

    Color opponent_color = get_opponent_color(curr_color);
    int score = minmax_ab(board, color_max, opponent_color,
                     &move_to_apply, depth - 1, alpha, beta);
    // undo move
    increment_hash_board_state(&board_hash, board, &report); // revert the hash back
    undo_move(board, &report);
    return score;
}


int _present_pieces_score(Board *board, Color color) {
    int score = 0;
    for (unsigned int piece_name = PAWN; piece_name <= QUEEN; piece_name++) {
        score += PIECE_COUNT(*board, color, piece_name) * get_piece_value(piece_name);
    }
    return score;
}

int _board_score(Board *board, Color color) {
    GameStatus color_game_status = get_game_status(board, color);
    if (color_game_status == GAME_CHECKMATE) {
        return -INFINITY; // loses
    }
    if (color_game_status == GAME_STALEMATE) {
        return 0;
    }

    int extra = 0;
    if (color_game_status == GAME_CHECK) {
        extra -= 200;
    }
    return _present_pieces_score(board, color) + extra;

}

int evaluate_board(Board *board, Color color) {
    return _board_score(board, color) - _board_score(board, get_opponent_color(color));
}
