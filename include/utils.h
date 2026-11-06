#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "chessboard.h"

int sign(int x);
int max(int x, int y);
int min(int x, int y);

Coords string_to_coords(const char *input);
char *coords_to_string(Coords pos);

void clear_stdin_buffer(void);
void clear_screen(void);
void write_move(FILE *moves_file, Coords src_pos, Coords dest_pos);
bool is_valid_string_pos(char *input);
void read_coords(char *prompt, Coords *pos);

uint64_t rand64();

#endif