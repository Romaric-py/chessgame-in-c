#include "minmax.h"
#include "legal_move.h"
#include "game_status.h"
#include "special_move.h"
#include "utils.h"

static void apply_search_move(Board *board, Move move_to_apply) {
    move(board, move_to_apply.src, move_to_apply.dest);

    if (can_promote_pawn(board, move_to_apply.dest, move_to_apply.dest)) {
        Piece pawn = get_piece(*board, move_to_apply.dest);
        PieceName promotion = move_to_apply.promotion == NONE
            ? QUEEN : move_to_apply.promotion;
        set_piece(board, move_to_apply.dest, create_piece(promotion, pawn.color));
    }
}

int minmax(Board *board, Color color_max, Color curr_color, Coords *src_pos, Coords *dest_pos, unsigned int depth) {
    int best_score = -INFINITY, score;
    Coords from_pos, to_pos, best_src_pos, best_dest_pos;

    MoveList legal_moves = generate_legal_moves(board, curr_color, false);
    order_captures_first(&legal_moves);

    for (unsigned int i = 0; i < legal_moves.count; i++) {
        from_pos = legal_moves.moves[i].src;
        to_pos = legal_moves.moves[i].dest;

        score = evaluate_move(board, color_max, curr_color, from_pos, to_pos, depth);

        if (
            // min
            ((curr_color != color_max) && (i == 0 || score < best_score)) ||
            // max
            ((curr_color == color_max) && (i == 0 || score > best_score))
        ) {
            best_score = score;
            best_src_pos = from_pos;
            best_dest_pos = to_pos;
        }
     
    }
    
    *src_pos = best_src_pos;
    *dest_pos = best_dest_pos;
    return best_score;
}


int evaluate_move(Board *board, Color color_max, Color curr_color, Coords src_pos, Coords dest_pos, unsigned int depth) {
    
    Board board_copy = clone_board(board);
    apply_search_move(&board_copy, create_move(src_pos, dest_pos));

    //* Note: decrementing depth
    if (depth <= 0 || is_end_game(&board_copy)) {
        return evaluate_board(&board_copy, color_max);
    }

    Color opponent_color = get_opponent_color(curr_color);
    return minmax(&board_copy, color_max, opponent_color, &src_pos, &dest_pos, depth-1);
}

int _present_pieces_score(Board *board, Color color) {
    Piece piece;
    unsigned int row, col;
    unsigned int score = 0;
    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            piece = get_piece(*board, create_coords(row, col));
            if (piece.color == color) {
                if (piece.name == PAWN) score += 1;
                else if (piece.name == ROOK) score += 5;
                else if (piece.name == BISHOP) score += 3;
                else if (piece.name == KNIGHT) score += 3;
                else if (piece.name == QUEEN) score += 9;
            }
        }
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


/*
alpha: best score ensured by the max color
beta: best score ensured by the min color
 */

int minmax_ab(Board *board, Color color_max, Color curr_color, Coords *src_pos, Coords *dest_pos, unsigned int depth, int alpha, int beta) {
    int best_score = -INFINITY, score;
    Coords from_pos, to_pos, best_src_pos, best_dest_pos;

    MoveList legal_moves = generate_legal_moves(board, curr_color, false);
    order_captures_first(&legal_moves);
    bool max_step = (curr_color == color_max);

    for (unsigned int i = 0; i < legal_moves.count; i++) {
        from_pos = legal_moves.moves[i].src;
        to_pos = legal_moves.moves[i].dest;

        score = evaluate_move_ab(board, color_max, curr_color,
                                 legal_moves.moves[i], depth, alpha, beta);

        // Best score update                    
        if (
            // min
            (!max_step && (i == 0 || score < best_score)) ||
            // max
            (max_step && (i == 0 || score > best_score))
        ) {
            best_score = score;
            best_src_pos = from_pos;
            best_dest_pos = to_pos;
        }

        // Alpha beta prunning
        // min
        if (!max_step && score <= alpha) {
            *src_pos = best_src_pos;
            *dest_pos = best_dest_pos;
            return best_score;
            
        }
        // max
        if (max_step && score >= beta) {
            *src_pos = best_src_pos;
            *dest_pos = best_dest_pos;
            return best_score;
            
        }

        // Alpha beta update
        if (max_step) {
            alpha = max(alpha, score);
        } else {
            beta = min(beta, score);
        }

     
    }
    
    *src_pos = best_src_pos;
    *dest_pos = best_dest_pos;
    return best_score;
}


/* This version that enables the use of many cpu at once, was written by GPT 5.5 Luna via VS Code Copilot SDK
*/
int minmax_ab_parallel(Board *board, Color color_max, Color curr_color,
                       Move *best_move, unsigned int depth) {
    MoveList legal_moves = generate_legal_moves(board, curr_color, false);
    order_captures_first(&legal_moves);

    int best_score = (curr_color == color_max) ? -INFINITY : INFINITY;
    Move selected_move;
    bool has_best = false;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int) legal_moves.count; i++) {
        Move move = legal_moves.moves[i];
        int score = evaluate_move_ab(
            board,
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
    return best_score;
}


int evaluate_move_ab(Board *board, Color color_max, Color curr_color,
                     Move move_to_apply, unsigned int depth, int alpha,
                     int beta) {
    
    Board board_copy = clone_board(board);
    apply_search_move(&board_copy, move_to_apply);

    //* Note: decrementing depth
    if (depth <= 0 || is_end_game(&board_copy)) {
        return evaluate_board(&board_copy, color_max);
    }

    Color opponent_color = get_opponent_color(curr_color);
    Coords src_pos = move_to_apply.src;
    Coords dest_pos = move_to_apply.dest;
    return minmax_ab(&board_copy, color_max, opponent_color,
                     &src_pos, &dest_pos, depth-1, alpha, beta);
}