#ifndef LEGAL_MOVE_H
#define LEGAL_MOVE_H

#include "chessboard.h"
#include "movelist.h"
#include <stdbool.h>

bool is_legal_move(Board *board, Coords src_pos, Coords dest_pos);
bool has_legal_move(Board *board, Color color);

void generate_legal_moves(Board *board, Color color, bool stop_at_first,
                          MoveList *movelist);

MoveList generate_legal_moves_for_piece(Board *board, Color color, Coords pos,
                                        bool stop_at_first);
MoveList generate_legal_moves_for_pawn(Board *board, Color color, Coords pos,
                                       bool stop_at_first);
MoveList generate_legal_moves_for_rook(Board *board, Color color, Coords pos,
                                       bool stop_at_first);
MoveList generate_legal_moves_for_knight(Board *board, Color color, Coords pos,
                                         bool stop_at_first);
MoveList generate_legal_moves_for_bishop(Board *board, Color color, Coords pos,
                                         bool stop_at_first);
MoveList generate_legal_moves_for_queen(Board *board, Color color, Coords pos,
                                        bool stop_at_first);
MoveList generate_legal_moves_for_king(Board *board, Color color, Coords pos,
                                       bool stop_at_first);

MoveList generate_legal_moves_in_direction(Board *board, Color color,
                                           Coords pos, int delta_row,
                                           int delta_col, bool stop_at_first);

#endif