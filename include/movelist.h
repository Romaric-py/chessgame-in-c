#ifndef MOVELIST_H
#define MOVELIST_H

#include "chessboard.h"
#define MAX_MOVES 128

typedef struct Move {
    Coords src;
    Coords dest;
    bool is_capture;
} Move;

typedef struct {
    Move moves[MAX_MOVES];
    unsigned int count;
} MoveList;

Move create_move(Coords src, Coords dest);
MoveList create_movelist();
void add_move(MoveList *list, Move move);
void extend_movelist(MoveList *list, MoveList *other_list);
void order_captures_first(MoveList *list);

#endif