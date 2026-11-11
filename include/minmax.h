#ifndef MINMAX_H
#define MINMAX_H

#include "chessboard.h"
#include "game_context.h"
#include "movelist.h"
#include <stdint.h>

#define INFINITY 10000000
#define MATE_SCORE 10000000
#define DEPTH 5

int32_t evaluate_board(ChessBoard *board, ChessColor color,
                       GameContext *context, unsigned int depth);

int32_t evaluate_move_ab(ChessBoard *board, ChessColor color_max,
                         ChessColor curr_color, Move move, unsigned int depth,
                         int32_t alpha, int32_t beta, GameContext *context);

int32_t minmax_ab(ChessBoard *board, ChessColor color_max,
                  ChessColor curr_color, Move *best_move, unsigned int depth,
                  int32_t alpha, int32_t beta, GameContext *context);

//! use openmp to parallelize the minmax_ab function
int32_t minmax_ab_parallel(ChessBoard *board, ChessColor color_max,
                           ChessColor curr_color, Move *best_move,
                           unsigned int depth, GameContext *context);

#endif
