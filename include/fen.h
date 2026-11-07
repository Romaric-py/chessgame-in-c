#ifndef FEN_H
#define FEN_H

#include "chessboard.h"
#include <stdio.h>

// The return value is a boolean indicating whether the FEN string was valid or
// not
bool is_valid_fen_string(const char *fen_str);
bool fen_loads(ChessBoard *board, const char *fen_str);
bool fen_dumps(const ChessBoard *board, char *fen_str);
bool fen_load(ChessBoard *board, FILE *fen_file);
bool fen_dump(const ChessBoard *board, FILE *fen_file);

#endif