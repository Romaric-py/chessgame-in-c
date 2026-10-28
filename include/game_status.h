#ifndef GAME_STATUS_H
#define GAME_STATUS_H

#include <stdbool.h>
#include "chessboard.h"

typedef enum GameStatus {
    GAME_IN_PROGRESS,
    GAME_CHECK,
    GAME_CHECKMATE,
    GAME_STALEMATE
} GameStatus;

// The argument `color` indicates the color that it is the turn
GameStatus get_game_status(Board *board, Color color);
bool is_end_game(Board *board);

#endif
