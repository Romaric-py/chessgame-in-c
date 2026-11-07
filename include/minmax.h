#ifndef MINMAX_H
#define MINMAX_H

#include "chessboard.h"
#include "movelist.h"

#define INFINITY INT16_MAX
#define DEPTH 6

int evaluate_board(ChessBoard *board, ChessColor color);

int evaluate_move_ab(ChessBoard *board, ChessColor color_max, ChessColor curr_color, Move move,
                     unsigned int depth, int alpha, int beta);

int minmax_ab(ChessBoard *board, ChessColor color_max, ChessColor curr_color, Move *best_move,
              unsigned int depth, int alpha, int beta);

//! use openmp to parallelize the minmax_ab function
int minmax_ab_parallel(ChessBoard *board, ChessColor color_max, ChessColor curr_color,
                       Move *best_move, unsigned int depth);

#endif
