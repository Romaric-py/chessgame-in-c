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


// Three fold repetition implemented by GPT 5.6 Luna via VS Code Copilot SDK
/* The structure is a dynamic array of PositionHistoryEntry */
// begin
typedef struct {
    Board board;
    Color color_to_move;
} PositionHistoryEntry;

static bool same_position(const PositionHistoryEntry *entry, const Board *board, Color color_to_move) {
    if (entry->color_to_move != color_to_move) {
        return false;
    }

    for (unsigned int row = 0; row < BOARD_SIZE; row++) {
        for (unsigned int col = 0; col < BOARD_SIZE; col++) {
            Piece entry_piece = entry->board.squares[row][col];
            Piece board_piece = board->squares[row][col];
            if (entry_piece.name != board_piece.name || entry_piece.color != board_piece.color) {
                return false;
            }
        }
    }
    return true;
}

static bool add_position_to_history(
    PositionHistoryEntry **history,
    size_t *history_count,
    size_t *history_capacity,
    const Board *board,
    Color color_to_move
) {
    if (*history_count == *history_capacity) {
        size_t new_capacity = (*history_capacity == 0) ? 128 : *history_capacity * 2;
        PositionHistoryEntry *new_history = realloc(
            *history,
            new_capacity * sizeof(*new_history)
        );
        if (new_history == NULL) {
            return false;
        }
        *history = new_history;
        *history_capacity = new_capacity;
    }

    (*history)[*history_count] = (PositionHistoryEntry){
        .board = clone_board(board),
        .color_to_move = color_to_move
    };
    (*history_count)++;
    return true;
}

static bool is_threefold_repetition(
    const PositionHistoryEntry *history,
    size_t history_count,
    const Board *board,
    Color color_to_move
) {
    unsigned int occurrences = 0;
    for (size_t i = 0; i < history_count; i++) {
        if (same_position(&history[i], board, color_to_move)) {
            occurrences++;
        }
    }
    return occurrences >= 3;
}
// end

/* The structure is a dynamic array of PositionHistoryEntry */

void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen() {
    printf("\033[2J\033[H");
}

void write_move(FILE *moves_file, Coords src_pos, Coords dest_pos) {
    fprintf(moves_file, "%s ", coords_to_string(src_pos));
    fprintf(moves_file, "%s\n", coords_to_string(dest_pos));
    fflush(moves_file);
}

// check if a position string is valid
bool is_valid_string_pos(char *input) {
    assert(strlen(input) == 2);

    char col_char = input[0];
    char row_char = input[1];

    return (col_char >= 'a' && col_char <= 'h' && row_char >= '1' && row_char <= '8');
}

void read_coords(char *prompt, Coords *pos) {
    char pos_str[3];

    do {
        printf("%s: ", prompt);
    }
    while (scanf("%2s", pos_str) != 1 ||
        strlen(pos_str) != 2 ||
        !is_valid_string_pos(pos_str));
    
    *pos = string_to_coords(pos_str);
    clear_stdin_buffer();
}

void get_human_move(Board *board, Color color, Coords *src_pos, Coords *dest_pos) {
    do {
        do {
            read_coords("From", src_pos);
        } while (get_piece(*board, *src_pos).color != color);

        clear_screen();
        display_board_with_hint(board, *src_pos);
        read_coords("To", dest_pos);
        clear_screen();
        display_board(board);

    } while (!is_legal_move(board, *src_pos, *dest_pos));
}

void get_ai_move(Board *board, Color color, Coords *src_pos, Coords *dest_pos) {
    minmax_ab_parallel(board, color, color, src_pos, dest_pos, DEPTH);
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

void make_pawn_promotion(Board *board, Coords pos, bool is_ai_turn) {
    // Promotion
    Piece piece = get_piece(*board, pos);
    bool can_promote_pawn = (
        piece.name == PAWN &&
        ((piece.color == BLACK && pos.row == BOARD_SIZE - 1) ||  (piece.color == WHITE && pos.row == 0))
    );

    if (can_promote_pawn) {
        PieceName new_name;
        if (is_ai_turn) {
            new_name = QUEEN;
        } else {
            choose_piece_name(&new_name);
        }
        piece = create_piece(new_name, piece.color);
        set_piece(board, pos, piece);
    }
}

void chess_turn(Board *board, Color color, FuncGetMove black_get_move, FuncGetMove white_get_move, FILE *moves_file) {
    //! DEBUG
    printf("roque_black: king=%d, rook_a=%d, rook_h=%d; roque_white: king=%d, rook_a=%d, rook_h=%d\n",
        board->roque.black.king, board->roque.black.rook_a, board->roque.black.rook_h,
        board->roque.white.king, board->roque.white.rook_a, board->roque.white.rook_h
    );

    
    printf("%s's turn\n", (color == BLACK) ? "Black" : "White");
    Coords src_pos, dest_pos;
    bool is_ai_turn = (color == BLACK);

    FuncGetMove get_move = (color == BLACK) ? black_get_move : white_get_move;
    get_move(board, color, &src_pos, &dest_pos);

    write_move(moves_file, src_pos, dest_pos);
    move(board, src_pos, dest_pos);
    make_pawn_promotion(board, dest_pos, is_ai_turn);
}


void play_chess(Color *winner, bool *draw, FuncGetMove black_get_move, FuncGetMove white_get_move) {
    Board board;
    FILE *moves_file = fopen("src/moves.txt", "w");
    PositionHistoryEntry *history = NULL;
    size_t history_count = 0;
    size_t history_capacity = 0;

    if (moves_file == NULL) {
        perror("src/moves.txt");
        *draw = true;
        return;
    }

    init_board(&board);
    //! DEBUG: Set the board in a configuration where the white king can roque
    board.squares[7][5] = EMPTY_PIECE;
    board.squares[7][6] = EMPTY_PIECE;
    board.squares[7][3] = EMPTY_PIECE;
    board.squares[7][2] = EMPTY_PIECE;
    board.squares[7][1] = EMPTY_PIECE;

    if (!add_position_to_history(&history, &history_count, &history_capacity, &board, WHITE)) {
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
        } else if (!add_position_to_history(&history, &history_count, &history_capacity, &board, opponent_color)) {
            fprintf(stderr, "Unable to record the position history\n");
            *draw = true;
            end_game = true;
        } else if (is_threefold_repetition(history, history_count, &board, opponent_color)) {
            *draw = true;
            end_game = true;
        } else {
             curr_color = opponent_color;
        }
    }

    free(history);
    fclose(moves_file);
}
