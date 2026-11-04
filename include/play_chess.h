#ifndef PLAY_CHESS_H
#define PLAY_CHESS_H

#include "chessboard.h"
#include "movelist.h"

#define MOVE_RECORD_FILE "src/moves.txt"

typedef void (*FuncGetMove)(Board *, Color, Move *);

void get_human_move(Board *board, Color color, Move *move);
void get_ai_move(Board *board, Color color, Move *move);
void play_chess(Color *winner, bool *draw, FuncGetMove black_get_move, FuncGetMove white_get_move);

#endif