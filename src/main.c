#include <stdlib.h>
#include <stdio.h>

#include "chessboard.h"
#include "render_tui.h"
#include "utils.h"
#include "play_chess.h"



int main(void) {
    /* Board tmp = create_board();
    Board *board = &tmp;
    init_board(board);
    display_board(board);

    FILE *file = fopen("src/moves.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening moves.txt\n");
        return EXIT_FAILURE;
    }

    while (!feof(file)) {
        char src[3], dest[3];
        if (fscanf(file, "%2s %2s", src, dest) != 2) {
            break;
        }

        Coords src_pos = string_to_coords(src);
        Coords dest_pos = string_to_coords(dest);

        move(board, src_pos, dest_pos);
    }


    display_board(board);

    fclose(file); */
    Color winner;
    bool draw;
    play_chess(&winner, &draw, get_ai_move, get_human_move);

    if (!draw) {
        printf("The winner is %s\n", (winner == BLACK) ? "BLACK" : "WHITE");
    } else {
        printf("It's a draw");
    }
    return 0;
}
