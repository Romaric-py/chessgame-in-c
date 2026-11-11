#ifndef POSITION_HISTORY_H
#define POSITION_HISTORY_H

#include <stdbool.h>
#include <stdint.h>

#include "chessboard.h"

#define MAX_POSITION_HISTORY                                                   \
  1048576 // 2^20 (use a power of 2 for efficient indexing with bitwise AND)

typedef struct {
  unsigned int array[MAX_POSITION_HISTORY];
} PositionHistoryCounter;

PositionHistoryCounter create_position_history_counter();
// Since we use a fixed-size array, this function does not need to free any
// memory
void clear_position_history_counter(PositionHistoryCounter *counter);
void free_position_history_counter(PositionHistoryCounter *counter);
unsigned int get_position_history_count(const PositionHistoryCounter *counter,
                                        uint64_t hash);
void increment_position_history_counter(PositionHistoryCounter *counter,
                                        uint64_t hash);
void decrement_position_history_counter(PositionHistoryCounter *counter,
                                        uint64_t hash);

// Check if the position represented by the hash has occurred three times in the
// game history Be aware to add the current position to the history before
// calling this function, otherwise it will return false
bool is_threefold_repetition(const PositionHistoryCounter *counter,
                             uint64_t hash);

#endif
