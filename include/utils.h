#ifndef UTILS_H
#define UTILS_H

#include "chessboard.h"

int sign(int x);
int max(int x, int y);
int min(int x, int y);

Coords string_to_coords(const char *input);
char *coords_to_string(Coords pos);

#endif