#include "movelist.h"
#include <stdio.h>

Move create_move(Coords src, Coords dest) {
  return (Move){
      .src = src, .dest = dest, .is_capture = false, .promotion = NONE};
}

void create_movelist(MoveList *list) { list->count = 0; }

void add_move(MoveList *list, Move move) {
  if (list->count < MAX_MOVES) {
    list->moves[list->count] = move;
    list->count++;
  } else {
    fprintf(stderr, "Error: MoveList is full. Cannot add more moves.\n");
  }
}

void extend_movelist(MoveList *list, MoveList *other_list) {
  for (unsigned int i = 0; i < other_list->count; i++) {
    add_move(list, other_list->moves[i]);
  }
}

// Time: O(n), space: O(1).
void order_captures_first(MoveList *list) {
  int i = 0;
  int j = list->count - 1;

  while (i < j) {
    if (list->moves[i].is_capture)
      i++;
    if (!list->moves[j].is_capture)
      j--;

    if (i < j) {
      Move temp = list->moves[i];
      list->moves[i] = list->moves[j];
      list->moves[j] = temp;
    }
  }
}