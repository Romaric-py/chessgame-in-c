#ifndef KING_CHECK_H
#define KING_CHECK_H

#include "chessboard.h"
#include <stdbool.h>

bool is_square_attacked(const ChessBoard *board, Coords pos);
bool is_square_attacked_by(const ChessBoard *board, Coords pos,
                           ChessColor attacker_color);
bool is_king_in_check(const ChessBoard *board, ChessColor color);

#endif