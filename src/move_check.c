#include <stdlib.h>
#include <assert.h>

#include "chessboard.h"
#include "move_check.h"
#include "utils.h"
#include "special_move.h"


bool is_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    
    switch (get_piece(board, src_pos).name) {
        case PAWN:
            if (is_enpassant_move(board, src_pos, dest_pos)) {
                return true;
            }
            return is_pawn_valid_move(board, src_pos, dest_pos);
        case KNIGHT:
            return is_knight_valid_move(board, src_pos, dest_pos);
        case BISHOP:
            return is_bishop_valid_move(board, src_pos, dest_pos);
        case ROOK:
            return is_rook_valid_move(board, src_pos, dest_pos);
        case QUEEN:
            return is_queen_valid_move(board, src_pos, dest_pos);
        case KING:
            if (is_roque_move(board, src_pos, dest_pos)) {
                return true;
            }
            return is_king_valid_move(board, src_pos, dest_pos);
        default:
            return false;
    }
}


bool is_pawn_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    int delta_row = 1;
    unsigned int home_row = 1;
    Piece piece;

    piece = get_piece(board, src_pos);

    if (piece.color == WHITE) {
        delta_row = -1;
        home_row = 6;
    }

    return
    
    /* Forward move
      If the move is performed in the same columun, the dest_pos should be empty 
      and the pawn can move one square forward but also two if it is on the home row and the square in between is empty.
    */
    (
        (dest_pos.col == src_pos.col) && is_empty_square(board, dest_pos) && (
            (dest_pos.row == src_pos.row + delta_row) 
            || (
                src_pos.row == home_row &&
                dest_pos.row == src_pos.row + 2 * delta_row &&
                is_empty_square(board, create_coords(src_pos.row + delta_row, src_pos.col))
            ) 
        )
    ) || 

    /* Attack/Capture
    The dest_pos column is the one on left or on the right of the src_pos one; the row is 
    the next row (depending on the color). The dest_pos square should not be empty and must 
    contains an opponent's piece.
    Special: 
    */
    (
        (dest_pos.col == src_pos.col + 1 || dest_pos.col == src_pos.col - 1) &&
        (dest_pos.row == src_pos.row + delta_row) && (!is_empty_square(board, dest_pos)) && (get_piece(board, dest_pos).color == get_opponent_color(piece.color))
    );
}

bool is_pawn_2steps_move(const Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);
    int delta_row = (piece.color == BLACK) ? 1 : -1;
    return piece.name == PAWN && 
        dest_pos.row == src_pos.row + 2 * delta_row &&
        is_pawn_valid_move(board, src_pos, dest_pos);
}

bool is_knight_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);

    int row_diff = dest_pos.row - src_pos.row;
    int col_diff = dest_pos.col - src_pos.col;

    // Moves in L shape
    return (
        (abs(row_diff) == 2 && abs(col_diff) == 1) ||
        (abs(row_diff) == 1 && abs(col_diff) == 2)
    ) && (is_empty_square(board, dest_pos) || get_piece(board, dest_pos).color == get_opponent_color(piece.color)); // dest_pos square is not occuped by the same color
}

bool is_rook_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);

    // src_pos and dest_pos should have the same column or the same row (not both)
    if ((src_pos.row != dest_pos.row && src_pos.col != dest_pos.col) || 
        are_coords_equal(src_pos, dest_pos))  {
        return false;
    }
    
    // No pieces on the road to dest_pos
    bool range_free = is_free_range(
        board, src_pos, dest_pos,
        sign(dest_pos.row - src_pos.row),
        sign(dest_pos.col - src_pos.col)
    );
    return range_free && (is_empty_square(board, dest_pos) || (get_piece(board, dest_pos).color == get_opponent_color(piece.color)));
}

bool is_bishop_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);

    if (are_coords_equal(src_pos, dest_pos)) {
        return false;
    }

    int row_diff = dest_pos.row - src_pos.row;
    int col_diff = dest_pos.col - src_pos.col;

    if (abs(row_diff) != abs(col_diff)) {
        return false;
    }

    // No pieces on the road to dest_pos
    bool range_free = is_free_range(
        board, src_pos, dest_pos,
        sign(row_diff),
        sign(col_diff)
    );

    return range_free && (is_empty_square(board, dest_pos) || (get_piece(board, dest_pos).color == get_opponent_color(piece.color)));
}

bool is_queen_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    return is_bishop_valid_move(board, src_pos, dest_pos) || is_rook_valid_move(board, src_pos, dest_pos);
}

bool is_king_valid_move(const Board *board, Coords src_pos, Coords dest_pos) {
    Piece piece = get_piece(board, src_pos);

    int row_diff = dest_pos.row - src_pos.row;
    int col_diff = dest_pos.col - src_pos.col;

    return (
        max(abs(row_diff), abs(col_diff)) == 1 &&
        (is_empty_square(board, dest_pos) ||
        get_piece(board, dest_pos).color == get_opponent_color(piece.color))
    );
}


bool is_free_range(const Board *board, Coords src_pos, Coords dest_pos, int delta_row, int delta_col) {
    assert(delta_col != 0 || delta_row != 0);

    unsigned int row = src_pos.row + delta_row;
    unsigned int col = src_pos.col + delta_col;
    bool result = true;

    while (result && (row < BOARD_SIZE) && (col < BOARD_SIZE) &&
           !are_coords_equal(dest_pos, create_coords(row, col))
        ) {
        result = is_empty_square(board, create_coords(row, col));
        row += delta_row;
        col += delta_col;
    }
    return result;
}


