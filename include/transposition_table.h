#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>
#include "chessboard.h"
#include "movelist.h"

#define TT_SIZE 1048576 // Transposition Table Size (2^20)

typedef enum {
    EXACT, // Exact score
    LOWER_BOUND, // Score is a lower bound 
    // that means that the actual score is greater than or equal to the stored score. And this is due to the beta cut-off in the alpha-beta pruning algorithm.
    UPPER_BOUND // Score is an upper bound
    // that means that the actual score is less than or equal to the stored score. And this is due to the alpha cut-off in the alpha-beta pruning algorithm.
} TTEntryFlag;

typedef struct {
    uint64_t zobrist_key;
    int score;
    unsigned int depth;
    TTEntryFlag flag;
    Move best_move;
} TTEntry; // Transposition Table Entry


extern uint64_t position_piece_hashes[8][8][2][6];
extern TTEntry transposition_table[TT_SIZE];


void init_zobrist_hashes();
uint64_t get_position_piece_hash(Coords pos, Piece piece);
uint64_t hash_board_state(Board *board);

//! When move_report will include the color who played the move, we can use it to update the hash without needing the board state
void increment_hash_board_state(uint64_t *hash, Board *board, const MoveReport *move_report);

void add_to_transposition_table(uint64_t hash, int score, unsigned int depth, Move best_move, int alpha, int beta);
bool fetch_from_transposition_table(uint64_t hash, int *score, Move *best_move, int alpha, int beta, unsigned int depth);
void clear_transposition_table();

#endif