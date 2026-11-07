#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <stdbool.h>

#include "position_history.h"

typedef struct GameContext {
  PositionHistory history;
  unsigned int remaining_seconds[2]; // BLACK & WHITE
  bool is_timed;
} GameContext;

#endif