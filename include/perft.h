#ifndef PERFT_H
#define PERFT_H

#include "chessboard.h"

/*
 Performs a perft test on the given chessboard up to the specified depth. For
 each depth, it counts the number of legal moves and returns the total count.
 This function is useful for testing the move generation and search algorithms
 of a chess engine.
*/
unsigned int perft(ChessBoard *board, int depth);

#endif // PERFT_H