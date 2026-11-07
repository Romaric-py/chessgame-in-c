#ifndef SPECIAL_MOVE_H
#define SPECIAL_MOVE_H

#include "chessboard.h"

bool is_enpassant_move(const ChessBoard *board, Coords src_pos,
                       Coords dest_pos);
bool can_promote_pawn(const ChessBoard *board, Coords src_pos, Coords dest_pos);
bool is_castling_move(const ChessBoard *board, Coords src_pos, Coords dest_pos);

#endif