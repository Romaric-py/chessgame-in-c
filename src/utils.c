#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "utils.h"


int sign(int x) {
    if (x == 0) return 0;
    return (x > 0) ? 1 : -1;
}

int max(int x, int y) {
    return (x > y) ? x : y;
}

int min(int x, int y) {
    return (x < y) ? x : y;
}

Coords string_to_coords(const char *input) {
    assert(input != NULL && strlen(input) == 2);

    char col_char = input[0];
    char row_char = input[1];

    assert(col_char >= 'a' && col_char <= 'h' && row_char >= '1' && row_char <= '8');

    return create_coords(8 - (row_char - '0'), col_char - 'a');
}

char *coords_to_string(Coords pos) {
    static char buffer[3];
    snprintf(buffer, sizeof(buffer), "%c%d", 'a' + pos.col, 8 - pos.row);
    return buffer;
}