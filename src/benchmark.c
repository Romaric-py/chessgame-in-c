#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>

#include "fen.h"
#include "minmax.h"
#include "position_history.h"
#include "transposition_table.h"

#define MOVE_COUNT 10

static void benchmark_minmax(void) {
  ChessBoard board;
  ChessColor color = CHESS_WHITE;
  double min_time = 0.0;
  double max_time = 0.0;
  double total_time = 0.0;
  char fen_strs[MOVE_COUNT][128] = {
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnr/pppp1ppp/4p3/8/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2",
      "rnbqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3",
      "rnbqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPPBPPP/RNBQK2R b KQkq - 3 4",
      "rnbqkb1r/p1pp1ppp/5n2/1p2p3/4P3/5N2/PBPPBPPP/RN1QK2R w KQkq - 0 5",
      "rnbqkb1r/p1pp1ppp/5n2/1p2p3/P3P3/5N2/BbPPBPPP/RN1QK2R b KQkq - 0 6",
      "rnbqkb1r/p1pp1ppp/5n2/Pp2p3/P3P3/B4N2/bbPPBPPP/RN1QK2R w KQkq - 0 7",
      "rnbqkb1r/p1pp1ppp/P4n2/Pp2p3/P3P3/B4N2/bbPPBPPP/RN1QK2R b KQkq - 0 8",
      "rnBQkb1r/p1pp1ppp/P4n2/Pp2p3/P3P3/b4N2/bbPPBPPP/RN2K2R w KQkq - 0 9",
      "rnBQkb1r/p4ppp/Pp3n2/Pp2p3/P3P3/b4N2/bbPPBPPP/RN2K2R b KQkq - 0 10"};

  init_board(&board);

  printf("Minmax benchmark (%u moves):\n\n", MOVE_COUNT);
  printf("Depth | Min Time (s) | Max Time (s) | Avg Time (s)\n");

  for (unsigned int depth = 1; depth < 8; depth++) {
    for (unsigned int i = 0; i < MOVE_COUNT; ++i) {
      struct timespec start, end;
      Move ai_move;
      GameContext context = {.is_timed = false,
                             .history_counter =
                                 create_position_history_counter()};
      fen_loads(&board, fen_strs[i]);

      clock_gettime(CLOCK_MONOTONIC, &start);
      minmax_ab(&board, color, color, &ai_move, depth, -INFINITY, INFINITY,
                &context);
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

      make_move(&board, ai_move.src, ai_move.dest);
      // color = get_opponent_color(color);
    }

    printf("%5u | %12.6f | %12.6f | %12.6f\n", depth, min_time, max_time,
           total_time / MOVE_COUNT);
  }
}

int main(void) {
  init_zobrist_hashes();
  benchmark_minmax();

  return 0;
}
