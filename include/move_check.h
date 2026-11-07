#ifndef MOVE_CHECK_H
#define MOVE_CHECK_H

#include "chessboard.h"

bool is_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_pawn_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_knight_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_rook_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_bishop_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_queen_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_king_valid_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_free_range(const ChessBoard *board, Coords src_pos, Coords dest_pos,
                   int delta_row, int delta_col);
bool is_pawn_2steps_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);

#endif