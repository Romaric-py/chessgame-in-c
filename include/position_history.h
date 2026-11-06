#ifndef POSITION_HISTORY_H
#define POSITION_HISTORY_H

#include <stdbool.h>
#include <stddef.h>

#include "chessboard.h"

typedef struct {
  Board board;
  Color color_to_move;
} PositionHistoryEntry;

typedef struct {
  PositionHistoryEntry *entries;
  size_t count;
  size_t capacity;
} PositionHistory;

PositionHistory create_position_history();

PositionHistoryEntry create_position_history_entry(const Board *board,
                                                   Color color_to_move);

void free_position_history(PositionHistory *history);

bool same_position(const PositionHistoryEntry *entry, const Board *board,
                   Color color_to_move);

bool add_position_to_history(PositionHistory *history, const Board *board,
                             Color color_to_move);

bool is_threefold_repetition(const PositionHistory *history, const Board *board,
                             Color color_to_move);

#endif
