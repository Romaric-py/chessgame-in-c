#ifndef MINMAX_H
#define MINMAX_H

#include <stdint.h>
#include "chessboard.h"
#include "movelist.h"

#define INFINITY INT32_MAX
#define DEPTH 6

int32_t evaluate_board(ChessBoard *board, ChessColor color);

int32_t evaluate_move_ab(ChessBoard *board, ChessColor color_max,
                     ChessColor curr_color, Move move, unsigned int depth,
                     int32_t alpha, int32_t beta);

int32_t minmax_ab(ChessBoard *board, ChessColor color_max, ChessColor curr_color,
              Move *best_move, unsigned int depth, int32_t alpha, int32_t beta);

//! use openmp to parallelize the minmax_ab function
int32_t minmax_ab_parallel(ChessBoard *board, ChessColor color_max,
                       ChessColor curr_color, Move *best_move,
                       unsigned int depth);

#endif
