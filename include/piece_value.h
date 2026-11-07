#ifndef PIECE_VALUE_H
#define PIECE_VALUE_H

#include "chessboard.h"


#define CHECK_BONUS 20 // bonus for putting the opponent in check
// I assume that it's will not good to choose losing a valuable piece to put the opponent in check, so the bonus is not too high. The bonus is not too low either, because putting the opponent in check is a good thing. I decide it's better than losing a pawn, but not better than losing a knight. So I choose 20 as the bonus, which is less than the value of a knight (30) and more than the value of a pawn (10).

int pieces_values[6] = {
    10, 30, 30, 50, 90, 100}; // PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING

static inline int get_piece_value(PieceName piece_name) {
  return pieces_values[piece_name];
}

#endif