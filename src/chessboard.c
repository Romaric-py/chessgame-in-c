#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "chessboard.h"
#include "move_check.h"


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
Board create_board() {
    Board board;
    unsigned int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            board.squares[row][col] = EMPTY_PIECE;
        }
    }
    return board;
}

Board clone_board(const Board *src_board) {
    if (src_board == NULL) {
        return create_board();
    }

    Board board;
    unsigned int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            board.squares[row][col] = src_board->squares[row][col];
        }
    }
    return board;
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
    if (is_valid_move(board, src_pos, dest_pos)) {
        set_piece(board, dest_pos, get_piece(*board, src_pos));
        set_piece(board, src_pos, EMPTY_PIECE);
    }
}
