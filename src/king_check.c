#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "king_check.h"
#include "move_check.h"

bool is_square_attacked(Board *board, Coords pos) {
    unsigned int row, col;
    Coords curr_coords;
    bool target_is_empty = is_empty_square(*board, pos);
    Color target_color = get_piece(*board, pos).color;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            curr_coords = create_coords(row, col);
            if (!is_empty_square(*board, curr_coords)) {
                Piece attacker = get_piece(*board, curr_coords);
                // The target square shouldn't have the same color as the attacker
                if (!target_is_empty && attacker.color == target_color) {
                    continue;
                }
                // All pieces (different from the Pawn) valid moves can be an attack
                // So, we treat the pawn differently 
                if (attacker.name == PAWN) {
                    int delta_row = (attacker.color == WHITE) ? -1 : 1;
                    int row_diff = pos.row - curr_coords.row;
                    int col_diff = pos.col - curr_coords.col;

                    if (row_diff == delta_row && abs(col_diff) == 1) {
                        return true;
                    }
                } else if (is_valid_move(board, curr_coords, pos)) {
                    return true;
                }
            }
            
        }
    }
    return false;
}

void find_king(Board *board, Color color, Coords *coords, bool *found) {
    Coords curr_coords;
    Piece piece;
    unsigned int row, col;
    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            curr_coords = create_coords(row, col);
            piece = get_piece(*board, curr_coords);
            if (piece.color == color && piece.name == KING) {
                *coords = curr_coords;
                *found = true;
                return;
            }
        }
    }
    *coords = create_coords(0, 0);
    *found = false;
}

bool is_king_in_check(Board *board, Color color) {
    Coords king_pos;
    bool king_found;
    find_king(board, color, &king_pos, &king_found);
    return king_found && is_square_attacked(board, king_pos);
}
