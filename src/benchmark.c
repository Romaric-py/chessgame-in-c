#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>

#include "play_chess.h"

int main(void) {
  const unsigned int move_count = 10;
  ChessBoard board;
  ChessColor color = CHESS_WHITE;
  double min_time = 0.0;
  double max_time = 0.0;
  double total_time = 0.0;

  init_board(&board);

  for (unsigned int i = 0; i < move_count; ++i) {
    struct timespec start, end;
    Move ai_move;

    clock_gettime(CLOCK_MONOTONIC, &start);
    get_ai_move(&board, color, &ai_move);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (double)(end.tv_sec - start.tv_sec) +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    if (i == 0 || elapsed < min_time) {
      min_time = elapsed;
    }
    if (i == 0 || elapsed > max_time) {
      max_time = elapsed;
    }
    total_time += elapsed;

    move(&board, ai_move.src, ai_move.dest);
    color = get_opponent_color(color);
  }

  printf("AI vs AI benchmark (%u moves):\n", move_count);
  printf("min: %.6f seconds\n", min_time);
  printf("max: %.6f seconds\n", max_time);
  printf("avg: %.6f seconds\n", total_time / move_count);
  return 0;
}
