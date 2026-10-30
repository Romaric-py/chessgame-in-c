#include "legal_move.h"
#include "move_check.h"
#include "king_check.h"
#include "utils.h"


// Can be optimized: for example for a pawn, no need to test all squares but just the possible ones
/* LEGAGY
bool piece_has_legal_move(Board *board, Coords pos) {
    unsigned int row, col;
    // Brute force
    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            if (is_legal_move(board, pos, create_coords(row, col))) {
                return true;
            }
        } 
    }
    return false;
} */

bool piece_has_legal_move(Board *board, Coords pos) {
    Piece piece = get_piece(*board, pos);
    MoveList moves = generate_legal_moves_for_piece(board, piece.color, pos, true);
    return moves.count > 0;
}

bool has_legal_move(Board *board, Color color) {
    return generate_legal_moves(board, color, true).count > 0;
}

bool is_legal_move(Board *board, Coords src_pos, Coords dest_pos) {
    if (!is_valid_move(board, src_pos, dest_pos)) {
        return false;
    } 
    // Here, the move is valid (and the src_pos square is not empty)
    Board board_copy = clone_board(board);
    Color color = get_piece(board_copy, src_pos).color;
    move(&board_copy, src_pos, dest_pos);

    return !is_king_in_check(&board_copy, color);
}

/*
The parameter stop_at_first is useful in case that we just want to ensure that 
*/
MoveList generate_legal_moves_for_piece(Board *board, Color color, Coords pos, bool stop_at_first) {
    Piece piece = get_piece(*board, pos);
    
    if (piece.name == NONE || piece.color != color)
        return create_movelist();
    switch (piece.name)
    {
        case PAWN: return generate_legal_moves_for_pawn(board, color, pos, stop_at_first);
        case KNIGHT: return generate_legal_moves_for_knight(board, color, pos, stop_at_first);
        case BISHOP: return generate_legal_moves_for_bishop(board, color, pos, stop_at_first);
        case ROOK: return generate_legal_moves_for_rook(board, color, pos, stop_at_first);
        case QUEEN: return generate_legal_moves_for_queen(board, color, pos, stop_at_first);
        case KING: return generate_legal_moves_for_king(board, color, pos, stop_at_first);
        default: return create_movelist();
    }
}

MoveList generate_legal_moves_for_pawn(Board *board, Color color, Coords pos, bool stop_at_first) {
    Coords dest_pos;
    Move move;
    // assumes pos contains a pawn
    MoveList movelist = create_movelist();

    int delta_row = (color == BLACK) ? 1 : -1;
    int next_row = pos.row + delta_row;

    for (int col = (int) pos.col - 1; col <= (int) pos.col + 1; col++) {
        if (is_valid_coords(next_row, col)) {
            dest_pos = create_coords(next_row, col);
            if (is_legal_move(board, pos, dest_pos)) {
                move = create_move(pos, dest_pos);
                if (!is_empty_square(*board, dest_pos) && get_piece(*board, dest_pos).color == get_opponent_color(color)) {
                    move.is_capture = true;
                }
                add_move(&movelist, move);
                if (stop_at_first) return movelist;
            }
        }
    }

    next_row = next_row + delta_row;

    if (is_valid_coords(next_row, pos.col)) {
        dest_pos = create_coords(next_row, pos.col);
        if (is_legal_move(board, pos, dest_pos)) {
            move = create_move(pos, dest_pos);
            add_move(&movelist, move);
            if (stop_at_first) return movelist;
        }
    }

    return movelist;
}

MoveList generate_legal_moves_for_knight(Board *board, Color color, Coords pos, bool stop_at_first) {
    Coords dest_pos;
    Move move;
    int row, col;
    // assumes pos contains a knight
    MoveList movelist = create_movelist();
    int deltas[8][2] = {{-2, -1}, {-1, -2}, {-2, 1}, {1, -2}, {2, -1}, {-1, 2}, {1, 2}, {2, 1}};

    for (unsigned int i = 0; i < 8; i++) {
        row = pos.row + deltas[i][0];
        col = pos.col + deltas[i][1];

        if (is_valid_coords(row, col)) {
        dest_pos = create_coords(row, col);
            if (is_legal_move(board, pos, dest_pos)) {
                move = create_move(pos, dest_pos);
                if (!is_empty_square(*board, dest_pos) && get_piece(*board, dest_pos).color == get_opponent_color(color)) {
                    move.is_capture = true;
                }
                add_move(&movelist, move);
                if (stop_at_first) return movelist;
            }
        }
    }

    return movelist;
}

MoveList generate_legal_moves_for_king(Board *board, Color color, Coords pos, bool stop_at_first) {
    Coords dest_pos;
    Move move;
    int row, col;
    // assumes pos contains a king
    MoveList movelist = create_movelist();
    
    int deltas[10][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {1, -1}, {1, 0}, {1, 1}, {0, -1}, {0, 1}, {0, 2}, {0, -2}}; // last two are for roque

    for (unsigned int i = 0; i < 10; i++) {
        row = pos.row + deltas[i][0];
        col = pos.col + deltas[i][1];

        if (is_valid_coords(row, col)) {
        dest_pos = create_coords(row, col);
            if (is_legal_move(board, pos, dest_pos)) {
                move = create_move(pos, dest_pos);
                if (!is_empty_square(*board, dest_pos) && get_piece(*board, dest_pos).color == get_opponent_color(color)) {
                    move.is_capture = true;
                }
                add_move(&movelist, move);
                if (stop_at_first) return movelist;
            }
        }
    }

    return movelist;
}

MoveList generate_legal_moves_for_bishop(Board *board, Color color, Coords pos, bool stop_at_first) {
    // assumes pos contains a bishop
    MoveList tmp;

    int dirs[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    MoveList movelist = create_movelist();

    for (unsigned int i = 0; i < 4; i++) {
        tmp = generate_legal_moves_in_direction(board, color, pos, dirs[i][0], dirs[i][1], stop_at_first);
        extend_movelist(&movelist, &tmp);
        if (stop_at_first && movelist.count > 0) return movelist;
    }

    return movelist;
}

MoveList generate_legal_moves_for_rook(Board *board, Color color, Coords pos, bool stop_at_first) {
    // assumes pos contains a rook
    MoveList tmp;

    int dirs[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    MoveList movelist = create_movelist();

    for (unsigned int i = 0; i < 4; i++) {
        tmp = generate_legal_moves_in_direction(board, color, pos, dirs[i][0], dirs[i][1], stop_at_first);
        extend_movelist(&movelist, &tmp);
        if (stop_at_first && movelist.count > 0) return movelist;
    }

    return movelist;
}

MoveList generate_legal_moves_for_queen(Board *board, Color color, Coords pos, bool stop_at_first) {
    // assumes pos contains a queen
    MoveList tmp;

    MoveList movelist = create_movelist();
    tmp = generate_legal_moves_for_bishop(board, color, pos, stop_at_first);
    extend_movelist(&movelist, &tmp);
    if (stop_at_first && movelist.count > 0) return movelist;
    tmp = generate_legal_moves_for_rook(board, color, pos, stop_at_first);
    extend_movelist(&movelist, &tmp);

    return movelist;
}



MoveList generate_legal_moves_in_direction(Board *board, Color color, Coords pos, int delta_row, int delta_col, bool stop_at_first) {
    Coords dest_pos;
    Move move;
    MoveList movelist = create_movelist();

    int row = pos.row + delta_row;
    int col = pos.col + delta_col;

    while (is_valid_coords(row, col)) {

        dest_pos = create_coords(row, col);
        if (is_legal_move(board, pos, dest_pos)) {
            move = create_move(pos, dest_pos);
            if (!is_empty_square(*board, dest_pos) && get_piece(*board, dest_pos).color == get_opponent_color(color)) {
                move.is_capture = true;
            }
            add_move(&movelist, move);
            if (stop_at_first) return movelist;
        }
        if (!is_empty_square(*board, dest_pos)) break;
        
        row += delta_row;
        col += delta_col;
    }
    return movelist;
}

MoveList generate_legal_moves(Board *board, Color color, bool stop_at_first) {
    unsigned int row, col;
    Coords pos;
    MoveList tmp;
    MoveList movelist = create_movelist();

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            pos = create_coords(row, col);
            if (!is_empty_square(*board, pos) && get_piece(*board, pos).color == color) {
                tmp = generate_legal_moves_for_piece(board, color, pos, stop_at_first);
                extend_movelist(&movelist, &tmp);
                if (stop_at_first && tmp.count > 0) {
                    return movelist;
                }
            }
        }
    }
    return movelist;
}

