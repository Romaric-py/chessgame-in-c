#ifndef PLAY_CHESS_H
#define PLAY_CHESS_H

#include "chessboard.h"
#include "movelist.h"

#define MOVE_RECORD_FILE "output/moves.txt"

typedef void (*FuncGetMove)(ChessBoard *, ChessColor, Move *);

void get_human_move(ChessBoard *board, ChessColor color, Move *move);
void get_ai_move(ChessBoard *board, ChessColor color, Move *move);
void play_chess(ChessColor *winner, bool *draw, FuncGetMove black_get_move,
                FuncGetMove white_get_move);

#endif