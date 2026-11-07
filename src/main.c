#include <stdio.h>
#include <stdlib.h>

#include "chessboard.h"
#include "play_chess.h"
#include "transposition_table.h"
#include "utils.h"

int main(void) {
  ChessColor winner;
  bool draw;

  init_zobrist_hashes();
  clear_transposition_table();

  play_chess(&winner, &draw, get_ai_move, get_ai_move);
  if (!draw) {
    printf("The winner is %s\n",
           (winner == CHESS_BLACK) ? "CHESS_BLACK" : "CHESS_WHITE");
  } else {
    printf("It's a draw");
  }
  return EXIT_SUCCESS;
}
