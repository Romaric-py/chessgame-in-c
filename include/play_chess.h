#ifndef PLAY_CHESS_H
#define PLAY_CHESS_H

#include "chessboard.h"

#define DEPTH 5

typedef void (*FuncGetMove)(Board *, Color, Coords *, Coords *);

void get_human_move(Board *board, Color color, Coords *src_pos, Coords *dest_pos);
void get_ai_move(Board *board, Color color, Coords *src_pos, Coords *dest_pos);
void play_chess(Color *winner, bool *draw, FuncGetMove black_get_move, FuncGetMove white_get_move);

#endif