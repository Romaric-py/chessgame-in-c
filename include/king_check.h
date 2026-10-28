#ifndef KING_CHECK_H
#define KING_CHECK_H

#include <stdbool.h>
#include "chessboard.h"

bool is_square_attacked(Board *board, Coords pos);
bool is_king_in_check(Board *board, Color color);

#endif