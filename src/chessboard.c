#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "chessboard.h"
#include "move_check.h"
#include "special_move.h"

static void _update_roque_flags(Board *board, Coords src_pos);

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
    }
    _init_flags(board);

}

Piece get_piece(Board board, Coords pos) {
    return board.squares[pos.row][pos.col];
}

void set_piece(Board *board, Coords pos, Piece piece) {
    board->squares[pos.row][pos.col] = piece;
}

bool is_empty_square(Board board, Coords pos) {
    return is_none_piece(get_piece(board, pos));
}

void move(Board *board, Coords src_pos, Coords dest_pos) {
    bool valid = false;

    if (is_enpassant_move(board, src_pos, dest_pos)) {
        set_piece(board, board->enpassant.pos, EMPTY_PIECE);
        valid = true;
    }

    /* since the move down here, just make it for one piece, we 
        to manually move the rook for roque move */
    if (is_roque_move(board, src_pos, dest_pos)) {
        unsigned int rook_col;
        Coords rook_src_pos, rook_dest_pos;

        rook_col = (dest_pos.col < src_pos.col) ? 0 : BOARD_SIZE-1;
        rook_src_pos = create_coords(dest_pos.row, rook_col);
        rook_col = (dest_pos.col < src_pos.col) ? 3 : 5;
        rook_dest_pos = create_coords(dest_pos.row, rook_col);

        set_piece(board, rook_dest_pos, get_piece(*board, rook_src_pos));
        set_piece(board, rook_src_pos, EMPTY_PIECE);
        valid = true;
    }

    // "en passant" flag managing
    if (is_pawn_2steps_move(board, src_pos, dest_pos)) {
        board->enpassant.flag = true;
        board->enpassant.pos = dest_pos;
        valid = true;
    } else {
        board->enpassant.flag = false;
    }
    
    if (valid || is_valid_move(board, src_pos, dest_pos)) {
        _update_roque_flags(board, src_pos);
        set_piece(board, dest_pos, get_piece(*board, src_pos));
        set_piece(board, src_pos, EMPTY_PIECE);
    }
   
}

// Ensure the move is valid before calling this function
static void _update_roque_flags(Board *board, Coords src_pos) {
    Piece piece = get_piece(*board, src_pos);

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
