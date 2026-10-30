#ifndef KING_CHECK_H
#define KING_CHECK_H

#include <stdbool.h>
#include "chessboard.h"

bool is_square_attacked(const Board *board, Coords pos);
bool is_square_attacked_by(const Board *board, Coords pos, Color attacker_color);
bool is_king_in_check(const Board *board, Color color);

#endif