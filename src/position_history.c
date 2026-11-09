#include <string.h>
#include "position_history.h"

#define INDEX_FOR(hash) ((hash) & (MAX_POSITION_HISTORY - 1))

PositionHistoryCounter create_position_history_counter() {
  PositionHistoryCounter counter = {.array = {0}};
  return counter;
}

void clear_position_history_counter(PositionHistoryCounter *counter) {
  memset(counter->array, 0, sizeof(counter->array));
}

void free_position_history_counter(PositionHistoryCounter *counter) {
  // Since we use a fixed-size array, there is no need to free any memory
  (void)counter; // For avoiding unused parameter warning
}

void increment_position_history_counter(PositionHistoryCounter *counter,
                                        uint64_t hash) {
  counter->array[INDEX_FOR(hash)]++;
}

unsigned int get_position_history_count(const PositionHistoryCounter *counter, uint64_t hash) {
  return counter->array[INDEX_FOR(hash)];
}

void decrement_position_history_counter(PositionHistoryCounter *counter,
                                        uint64_t hash) {
  unsigned int index = INDEX_FOR(hash);
  if (counter->array[index] > 0) {
    counter->array[index]--;
  }
}

bool is_threefold_repetition(const PositionHistoryCounter *counter,
                             uint64_t hash) {
  return counter->array[INDEX_FOR(hash)] >= 3;
}