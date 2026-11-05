#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "chessboard.h"
#include "move_check.h"
#include "special_move.h"

static void _update_roque_flags(Board *board, Coords src_pos);
static void _update_king_position(Board *board, Coords src_pos, Coords dest_pos);

// Color
Color get_opponent_color(Color color) {
    return (color == BLACK) ? WHITE : BLACK;
}

// Piece
Piece create_piece(PieceName name, Color color) {
    return (Piece){ .name = name, .color = color };
}
bool is_none_piece(Piece piece) {
    return piece.name == NONE;
}

// Coords
Coords create_coords(unsigned int row, unsigned int col) {
    assert(row < BOARD_SIZE && col < BOARD_SIZE);
    return (Coords){ .row = row, .col = col };
}

bool is_valid_coords(int row, int col) {
    return (0 <= row && row < BOARD_SIZE) && (0 <= col && col < BOARD_SIZE);
}

bool are_coords_equal(Coords coords1, Coords coords2) {
    return (coords1.row == coords2.row) && (coords1.col == coords2.col);
}


// Board
static void _init_flags(Board *board) {
    board->enpassant.flag = false;
    board->roque.black.king = true;
    board->roque.black.rook_a = true;
    board->roque.black.rook_h = true;
    board->roque.white.king = true;
    board->roque.white.rook_a = true;
    board->roque.white.rook_h = true;
}

Board create_board() {
    Board board;
    unsigned int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            board.squares[row][col] = EMPTY_PIECE;
        }
    }
    _init_flags(&board);
    
    return board;
}

Board clone_board(const Board *src_board) {
    if (src_board == NULL) {
        return create_board();
    }

    return *src_board; // shallow copy is enough since the board does not contain any pointer
}

void init_board(Board *board) {
    unsigned int i, row, col;
    Color color;
    // Empty squares
    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            if (1 < row && row < 6) {
                board->squares[row][col] = EMPTY_PIECE;
            }
        }
    }
    // Two lines of pawns
    for (col = 0; col < BOARD_SIZE; col++) {
        board->squares[6][col] = create_piece(PAWN, WHITE);
        board->squares[1][col] = create_piece(PAWN, BLACK);
    }
    // The others pieces
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            row = 0;
            color = BLACK;
        } else {
            row = 7;
            color = WHITE;
        }

        board->squares[row][0] = create_piece(ROOK, color);
        board->squares[row][7] = create_piece(ROOK, color);

        board->squares[row][1] = create_piece(KNIGHT, color);
        board->squares[row][6] = create_piece(KNIGHT, color);

        board->squares[row][2] = create_piece(BISHOP, color);
        board->squares[row][5] = create_piece(BISHOP, color);

        board->squares[row][3] = create_piece(QUEEN, color);
        board->squares[row][4] = create_piece(KING, color);

        board->king_pos.black = create_coords(0, 4);
        board->king_pos.white = create_coords(7, 4);
    }
    _init_flags(board);

}

Piece get_piece(const Board *board, Coords pos) {
    return board->squares[pos.row][pos.col];
}

void set_piece(Board *board, Coords pos, Piece piece) {
    board->squares[pos.row][pos.col] = piece;
    if (piece.name == KING) {
        if (piece.color == BLACK) {
            board->king_pos.black = pos;
        } else {
            board->king_pos.white = pos;
        }
    }
}

void switch_turn(Board *board) {
    board->current_turn = get_opponent_color(board->current_turn);
}

bool is_empty_square(const Board *board, Coords pos) {
    return is_none_piece(get_piece(board, pos));
}

void compute_rook_move_for_roque(Coords king_src_pos, Coords king_dest_pos, Coords *rook_src_pos, Coords *rook_dest_pos) {
    unsigned int rook_col;
    rook_col = (king_dest_pos.col < king_src_pos.col) ? 0 : BOARD_SIZE-1;
    *rook_src_pos = create_coords(king_dest_pos.row, rook_col);
    rook_col = (king_dest_pos.col < king_src_pos.col) ? 3 : 5;
    *rook_dest_pos = create_coords(king_dest_pos.row, rook_col);
}

// Assumes the move is legal before calling this function
MoveReport move(Board *board, Coords src_pos, Coords dest_pos) {
    MoveReport report = {
        .moved_color = get_piece(board, src_pos).color,
        .src_pos = src_pos,
        .dest_pos = dest_pos,
        .moved_piece = get_piece(board, src_pos),
        .captured_piece = get_piece(board, dest_pos),
        .old_state = {
            .enpassant = board->enpassant,
            .roque = board->roque,
            .king_pos = board->king_pos
        },
        .is_roque_move = is_roque_move(board, src_pos, dest_pos),
        .is_enpassant_move = is_enpassant_move(board, src_pos, dest_pos)
    };

    /* if (!is_valid_move(board, src_pos, dest_pos)) {
        return report;
    } */

    if (report.is_enpassant_move) {
        report.captured_piece = get_piece(board, board->enpassant.pos);
        set_piece(board, board->enpassant.pos, EMPTY_PIECE);
    }

    /* since the move down here, just make it for one piece, we 
        to manually move the rook for roque move */
    if (report.is_roque_move) {
        Coords rook_src_pos, rook_dest_pos;
        compute_rook_move_for_roque(src_pos, dest_pos, &rook_src_pos, &rook_dest_pos);

        set_piece(board, rook_dest_pos, get_piece(board, rook_src_pos));
        set_piece(board, rook_src_pos, EMPTY_PIECE);
    }

    // "en passant" flag managing
    if (is_pawn_2steps_move(board, src_pos, dest_pos)) {
        board->enpassant.flag = true;
        board->enpassant.pos = dest_pos;
    } else {
        board->enpassant.flag = false;
    }
    
    _update_roque_flags(board, dest_pos); // to invalidate the roque flag if a rook has been captured
    _update_roque_flags(board, src_pos); // to invalidate the roque flag if a rook or a king has been moved
    _update_king_position(board, src_pos, dest_pos);
    set_piece(board, dest_pos, get_piece(board, src_pos));
    set_piece(board, src_pos, EMPTY_PIECE);
    switch_turn(board);
    return report;
}

void undo_move(Board *board, MoveReport move_report) {
    // Restore the board state to what it was before the move
    board->enpassant = move_report.old_state.enpassant;
    board->roque = move_report.old_state.roque;
    board->king_pos = move_report.old_state.king_pos;

    if (move_report.is_enpassant_move) {
        set_piece(board, board->enpassant.pos, move_report.captured_piece);
    }
    if (move_report.is_roque_move) {
        Coords rook_src_pos, rook_dest_pos;
        compute_rook_move_for_roque(move_report.src_pos, move_report.dest_pos, &rook_src_pos, &rook_dest_pos);

        set_piece(board, rook_src_pos, get_piece(board, rook_dest_pos));
        set_piece(board, rook_dest_pos, EMPTY_PIECE);
    }

    set_piece(board, move_report.src_pos, move_report.moved_piece);
    set_piece(board, move_report.dest_pos, 
        move_report.is_enpassant_move ? EMPTY_PIECE : move_report.captured_piece);
}

// Ensure the move is valid before calling this function
static void _update_roque_flags(Board *board, Coords src_pos) {
    Piece piece = get_piece(board, src_pos);

    if (piece.name != KING && piece.name != ROOK) return;
    // Starting here, we ensure that it's a rook or a king

    struct _oneColorRoqueState *roque_state = (piece.color == BLACK) ? &board->roque.black : &board->roque.white;

    if (piece.name == KING) {
        roque_state->king = false;
    } else {
        if (src_pos.col == 0) {
            roque_state->rook_a = false;
        } else if (src_pos.col == BOARD_SIZE - 1) {
            roque_state->rook_h = false;
        }
    }
}

// Ensure the move is valid before calling this function
static void _update_king_position(Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);

    if (piece.name != KING) return;

    if (piece.color == BLACK) {
        board->king_pos.black = dest_pos;
    } else {
        board->king_pos.white = dest_pos;
    }
}
