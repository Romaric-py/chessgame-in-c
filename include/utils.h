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
PieceName piece_name_from_char(char c);
char piece_to_fen(Piece piece);
Piece fen_to_piece(char c);
bool is_valid_piece_char(char c);
bool is_lowercase(char c);
bool is_digit(char c);

uint64_t rand64();

#endif