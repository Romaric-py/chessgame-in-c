#ifndef PLAY_CHESS_H
#define PLAY_CHESS_H

#include "chessboard.h"
#include "movelist.h"
#include "game_context.h"

#define MOVE_RECORD_FILE "output/moves.txt"

typedef void (*FuncGetMove)(ChessBoard *, ChessColor, Move *, GameContext *);

void get_human_move(ChessBoard *board, ChessColor color, Move *move, GameContext *context);
void get_ai_move(ChessBoard *board, ChessColor color, Move *move, GameContext *context);
void play_chess(ChessColor *winner, bool *draw, FuncGetMove black_get_move,
                FuncGetMove white_get_move);

#endif