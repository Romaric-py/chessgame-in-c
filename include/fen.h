#ifndef FEN_H
#define FEN_H

#include "chessboard.h"
#include <stdio.h>

// The return value is a boolean indicating whether the FEN string was valid or
// not
bool is_valid_fen_string(const char *fen_str);
bool fen_loads(Board *board, const char *fen_str);
bool fen_dumps(const Board *board, char *fen_str);
bool fen_load(Board *board, FILE *fen_file);
bool fen_dump(const Board *board, FILE *fen_file);

#endif