#ifndef MINMAX_H
#define MINMAX_H

#include "chessboard.h"
#include "movelist.h"

#define INFINITY INT16_MAX
#define DEPTH 5

int evaluate_board(Board *board, Color color);

int evaluate_move_ab(Board *board, Color color_max, Color curr_color, Move move,
                     unsigned int depth, int alpha, int beta);

int minmax_ab(Board *board, Color color_max, Color curr_color, Move *best_move,
              unsigned int depth, int alpha, int beta);

//! use openmp to parallelize the minmax_ab function
int minmax_ab_parallel(Board *board, Color color_max, Color curr_color,
                       Move *best_move, unsigned int depth);

#endif
