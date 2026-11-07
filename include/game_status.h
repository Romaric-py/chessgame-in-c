#ifndef GAME_STATUS_H
#define GAME_STATUS_H

#include "chessboard.h"
#include "game_context.h"
#include <stdbool.h>

typedef enum GameStatus {
  GAME_IN_PROGRESS,
  GAME_CHECK,
  GAME_CHECKMATE,
  GAME_STALEMATE,
  GAME_DRAW_50_MOVE_RULE,
  GAME_DRAW_INSUFFICIENT_MATERIAL,
  GAME_LOSS_TIMEOUT,
  GAME_DRAW_3FOLD_REPETITION,
} GameStatus;

// The argument `color` indicates the color that it is the turn
GameStatus get_game_status(ChessBoard *board, ChessColor color,
                           const GameContext *context);
bool is_end_game(ChessBoard *board);
bool is_insufficient_material(const ChessBoard *board);

#endif
