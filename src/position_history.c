
#include <stdlib.h>

#include "position_history.h"

PositionHistory create_position_history() {
  PositionHistory history = {.entries = NULL, .count = 0, .capacity = 0};
  return history;
}

PositionHistoryEntry create_position_history_entry(const ChessBoard *board,
                                                   ChessColor color_to_move) {
  PositionHistoryEntry entry = {.board = clone_board(board),
                                .color_to_move = color_to_move};
  return entry;
}

void free_position_history(PositionHistory *history) {
  free(history->entries);
  history->entries = NULL;
  history->count = 0;
  history->capacity = 0;
}

bool same_position(const PositionHistoryEntry *entry, const ChessBoard *board,
                   ChessColor color_to_move) {
  if (entry->color_to_move != color_to_move) {
    return false;
  }

  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      Coords pos = create_coords(row, col);
      Piece entry_piece = get_piece(&entry->board, pos);
      Piece board_piece = get_piece(board, pos);
      if (entry_piece.name != board_piece.name ||
          entry_piece.color != board_piece.color) {
        return false;
      }
    }
  }
  return true;
}

bool add_position_to_history(PositionHistory *history, const ChessBoard *board,
                             ChessColor color_to_move) {
  if (history->count == history->capacity) {
    size_t new_capacity =
        (history->capacity == 0) ? 128 : history->capacity * 2;
    PositionHistoryEntry *new_history =
        realloc(history->entries, new_capacity * sizeof(*new_history));
    if (new_history == NULL) {
      return false;
    }
    history->entries = new_history;
    history->capacity = new_capacity;
  }

  history->entries[history->count] = (PositionHistoryEntry){
      .board = clone_board(board), .color_to_move = color_to_move};
  history->count++;
  return true;
}

bool is_threefold_repetition(const PositionHistory *history, const ChessBoard *board,
                             ChessColor color_to_move) {
  unsigned int occurrences = 0;
  for (size_t i = 0; i < history->count; i++) {
    if (same_position(&history->entries[i], board, color_to_move)) {
      occurrences++;
    }
  }
  return occurrences >= 3;
}