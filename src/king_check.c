#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "king_check.h"
#include "move_check.h"

bool is_square_attacked_by(const Board *board, Coords pos, Color attacker_color) {
    unsigned int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            Coords source = create_coords(row, col);
            Piece attacker = get_piece(*board, source);

            if (attacker.name == NONE || attacker.color != attacker_color) {
                continue;
            }

            if (attacker.name == PAWN) {
                int delta_row = (attacker.color == WHITE) ? -1 : 1;
                int row_diff = (int)pos.row - (int)source.row;
                int col_diff = (int)pos.col - (int)source.col;

                if (row_diff == delta_row && abs(col_diff) == 1) {
                    return true;
                }
            } else if(
                (attacker.name == KNIGHT &&
                is_knight_valid_move(board, source, pos)) ||
                (attacker.name == BISHOP &&
                is_bishop_valid_move(board, source, pos)) ||
                (attacker.name == ROOK &&
                is_rook_valid_move(board, source, pos)) ||
                (attacker.name == QUEEN &&
                is_queen_valid_move(board, source, pos)) ||
                (attacker.name == KING &&
                is_king_valid_move(board, source, pos))
            ) {
                return true;
            }
        }
    }
    return false;
}

bool is_square_attacked(const Board *board, Coords pos) {
    if (!is_empty_square(*board, pos)) {
        return is_square_attacked_by(
            board, pos, get_opponent_color(get_piece(*board, pos).color));
    }

    return is_square_attacked_by(board, pos, BLACK) ||
           is_square_attacked_by(board, pos, WHITE);
}

void find_king(const Board *board, Color color, Coords *coords, bool *found) {
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

bool is_king_in_check(const Board *board, Color color) {
    Coords king_pos;
    bool king_found;
    find_king(board, color, &king_pos, &king_found);
    return king_found && is_square_attacked(board, king_pos);
}
