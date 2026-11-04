#include <stdio.h>
#include <stdlib.h>

#include "chessboard.h"
#include "utils.h"
#include "play_chess.h"



int main(void) {
    Color winner;
    bool draw;

    play_chess(&winner, &draw, get_human_move, get_human_move);
    if (!draw) {
        printf("The winner is %s\n", (winner == BLACK) ? "BLACK" : "WHITE");
    } else {
        printf("It's a draw");
    }
    return EXIT_SUCCESS;
}