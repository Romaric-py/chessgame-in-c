#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <stdbool.h>

#include "position_history.h"

typedef struct GameContext {
  PositionHistoryCounter history_counter;
  unsigned int remaining_seconds[2]; // CHESS_BLACK & CHESS_WHITE
  bool is_timed;
} GameContext;

#endif