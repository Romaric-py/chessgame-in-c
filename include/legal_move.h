#ifndef LEGAL_MOVE_H
#define LEGAL_MOVE_H

#include "chessboard.h"
#include "movelist.h"
#include <stdbool.h>

bool is_legal_move(ChessBoard *board, Coords src_pos, Coords dest_pos);
bool has_legal_move(ChessBoard *board, ChessColor color);

void generate_legal_moves(ChessBoard *board, ChessColor color, bool stop_at_first,
                          MoveList *movelist);

MoveList generate_legal_moves_for_piece(ChessBoard *board, ChessColor color, Coords pos,
                                        bool stop_at_first);
MoveList generate_legal_moves_for_pawn(ChessBoard *board, ChessColor color, Coords pos,
                                       bool stop_at_first);
MoveList generate_legal_moves_for_rook(ChessBoard *board, ChessColor color, Coords pos,
                                       bool stop_at_first);
MoveList generate_legal_moves_for_knight(ChessBoard *board, ChessColor color, Coords pos,
                                         bool stop_at_first);
MoveList generate_legal_moves_for_bishop(ChessBoard *board, ChessColor color, Coords pos,
                                         bool stop_at_first);
MoveList generate_legal_moves_for_queen(ChessBoard *board, ChessColor color, Coords pos,
                                        bool stop_at_first);
MoveList generate_legal_moves_for_king(ChessBoard *board, ChessColor color, Coords pos,
                                       bool stop_at_first);

MoveList generate_legal_moves_in_direction(ChessBoard *board, ChessColor color,
                                           Coords pos, int delta_row,
                                           int delta_col, bool stop_at_first);

#endif