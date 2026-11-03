#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "play_chess.h"
#include "legal_move.h"
#include "render_tui.h"
#include "game_status.h"
#include "minmax.h"
#include "utils.h"
#include "position_history.h"


void get_human_move(Board *board, Color color, Move *move) {
    do {
        do {
            read_coords("From", &move->src);
        } while (get_piece(*board, move->src).color != color);

        clear_screen();
        display_board_with_hint(board, move->src);
        read_coords("To", &move->dest);
        clear_screen();
        display_board(board);

    } while (!is_legal_move(board, move->src, move->dest));

    // The following field values can be wrong but that doesn't matter since they are only used for ai move.
    move->is_capture = false;
    move->promotion = NONE;
}

void choose_piece_name(PieceName *name) {
    char pn;
    do {
        printf("Promote pawn to (N, R, Q, B): ");
        pn = getchar();
        clear_stdin_buffer();
    } while (pn != 'N' && pn != 'R' &&
            pn != 'Q' && pn != 'B');

    switch (pn) {
        case 'N':
            *name = KNIGHT;
            break;
        case 'B':
            *name = BISHOP;
            break;
        case 'Q':
            *name = QUEEN;
            break;
        case 'R':
            *name = ROOK;
            break;
        default:
            *name = NONE;
            break;
    }
}


void get_ai_move(Board *board, Color color, Move *move) {
    // minmax_ab_parallel(board, color, color, move, DEPTH);
    minmax_ab(board, color, color, move, DEPTH, -INFINITY, INFINITY);
}

void make_pawn_promotion(Board *board, Coords pos, PieceName promotion, bool is_ai_turn) {
    // Promotion
    Piece piece = get_piece(*board, pos);
    bool can_promote_pawn = (
        piece.name == PAWN &&
        ((piece.color == BLACK && pos.row == BOARD_SIZE - 1) ||  (piece.color == WHITE && pos.row == 0))
    );

    if (can_promote_pawn) {
        PieceName new_name;
        if (promotion != NONE) {
            new_name = promotion;
        } else if (is_ai_turn) {
            new_name = QUEEN;
        } else {
            choose_piece_name(&new_name);
        }
        piece = create_piece(new_name, piece.color);
        set_piece(board, pos, piece);
    }
}

void chess_turn(Board *board, Color color, FuncGetMove black_get_move, FuncGetMove white_get_move, FILE *moves_file) {    
    printf("%s's turn\n", (color == BLACK) ? "Black" : "White");
    Move move_to_play;
    bool is_ai_turn = (color == BLACK);

    FuncGetMove get_move = (color == BLACK) ? black_get_move : white_get_move;
    get_move(board, color, &move_to_play);

    write_move(moves_file, move_to_play.src, move_to_play.dest);
    move(board, move_to_play.src, move_to_play.dest);
    make_pawn_promotion(board, move_to_play.dest, move_to_play.promotion, is_ai_turn);
}


void play_chess(Color *winner, bool *draw, FuncGetMove black_get_move, FuncGetMove white_get_move) {
    Board board;
    FILE *moves_file = fopen("src/moves.txt", "w");
    PositionHistory history = create_position_history();

    if (moves_file == NULL) {
        perror("src/moves.txt");
        *draw = true;
        return;
    }

    init_board(&board);

    if (!add_position_to_history(&history, &board, WHITE)) {
        fclose(moves_file);
        *draw = true;
        return;
    }

    Color opponent_color, curr_color = WHITE;
    bool end_game = false;
    GameStatus game_status = GAME_IN_PROGRESS;

    clear_screen();
    display_board(&board);

    while (!end_game) {
        chess_turn(&board, curr_color, black_get_move, white_get_move, moves_file);
        clear_screen();
        display_board(&board);

        opponent_color = get_opponent_color(curr_color);
        game_status = get_game_status(&board, opponent_color);

        if (game_status == GAME_CHECKMATE) {
            *winner = curr_color;
            *draw = false;
            end_game = true;
        } else if (game_status == GAME_STALEMATE) {
            *draw = true;
            end_game = true;
        } 
        
        // History management
        else if (!add_position_to_history(&history, &board, opponent_color)) {
            fprintf(stderr, "Unable to record the position history\n");
            *draw = true;
            end_game = true;
        } else if (is_threefold_repetition(&history, &board, opponent_color)) {
            *draw = true;
            end_game = true;
        } else {
             curr_color = opponent_color;
        }
    }

    free_position_history(&history);
    fclose(moves_file);
}
