#ifndef MINMAX_H
#define MINMAX_H

#include "chessboard.h"

#define INFINITY 6000

int minmax(Board *board, Color color_max, Color curr_color, Coords *src_pos, Coords *dest_pos, unsigned int depth);

int evaluate_move(Board *board, Color color_max, Color curr_color, Coords src_pos, Coords dest_pos, unsigned int depth);

int evaluate_board(Board *board, Color color);

int evaluate_move_ab(Board *board, Color color_max, Color curr_color, Coords src_pos, Coords dest_pos, unsigned int depth, int alpha, int beta);

int minmax_ab(Board *board, Color color_max, Color curr_color, Coords *src_pos, Coords *dest_pos, unsigned int depth, int alpha, int beta);

int minmax_ab_parallel(Board *board, Color color_max, Color curr_color, Coords *src_pos, Coords *dest_pos, unsigned int depth);

#endif