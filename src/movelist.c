#include <stdio.h>
#include "movelist.h"

Move create_move(Coords src, Coords dest) {
    return (Move){
        .src = src,
        .dest = dest,
        .is_capture = false,
        .promotion = NONE
    };
}

void create_movelist(MoveList *list) {
    list->count = 0;
}

void add_move(MoveList *list, Move move) {
    if (list->count < MAX_MOVES) {
        list->moves[list->count] = move;
        list->count++;
    } else {
        fprintf(stderr, "Error: MoveList is full. Cannot add more moves.\n");
    }
}

void extend_movelist(MoveList *list, MoveList *other_list) {
    for (unsigned int i = 0; i < other_list->count; i++) {
        add_move(list, other_list->moves[i]);
    }
}

void order_captures_first(MoveList *list) {
    Move ordered_moves[MAX_MOVES];
    unsigned int ordered_count = 0;

    for (unsigned int i = 0; i < list->count; i++) {
        if (list->moves[i].is_capture) {
            ordered_moves[ordered_count++] = list->moves[i];
        }
    }
    for (unsigned int i = 0; i < list->count; i++) {
        if (!list->moves[i].is_capture) {
            ordered_moves[ordered_count++] = list->moves[i];
        }
    }

    for (unsigned int i = 0; i < list->count; i++) {
        list->moves[i] = ordered_moves[i];
    }
}