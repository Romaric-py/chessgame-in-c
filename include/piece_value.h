#ifndef PIECE_VALUE_H
#define PIECE_VALUE_H

#include "chessboard.h"

int pieces_values[6] = {
    10, 30, 30, 50, 90, 100}; // PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING, NONE

static inline int get_piece_value(PieceName piece_name) {
  return pieces_values[piece_name];
}

#endif