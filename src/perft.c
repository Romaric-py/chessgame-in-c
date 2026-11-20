#include "perft.h"
#include "legal_move.h"
#include "utils.h"
#include <stdio.h>

// Generated using GPT 5.6 Luna via VS Code Copilot SDK
unsigned int perft_rec(ChessBoard *board, int depth) {
  if (depth == 0) {
    return 1; // Base case: one position at depth 0
  }

  unsigned int total_nodes = 0;
  MoveList moves;
  init_movelist(&moves);

  generate_legal_moves(board, board->meta.current_turn, false, &moves);

  for (unsigned int i = 0; i < moves.count; i++) {
    MoveReport report =
        make_move(board, moves.moves[i].src, moves.moves[i].dest);
    unsigned int nodes = perft_rec(board, depth - 1);
    total_nodes += nodes;
    undo_move(board, &report);
  }

  return total_nodes;
}

unsigned int perft(ChessBoard *board, int depth) {
  if (depth < 0) {
    fprintf(stderr, "Error: Depth must be non-negative.\n");
    return 0;
  }
  if (depth == 0) {
    return 1; // Base case: one position at depth 0
  }
  unsigned int total_nodes = 0;
  MoveList moves;
  init_movelist(&moves);
  generate_legal_moves(board, board->meta.current_turn, false, &moves);
  for (unsigned int i = 0; i < moves.count; i++) {
    MoveReport report =
        make_move(board, moves.moves[i].src, moves.moves[i].dest);
    unsigned int nodes = perft_rec(board, depth - 1);
    total_nodes += nodes;
    undo_move(board, &report);

    char move_str[5];
    move_to_string(moves.moves[i], move_str);
    printf("Move %s: %u nodes\n", move_str, nodes);
  }
  return total_nodes;
}
