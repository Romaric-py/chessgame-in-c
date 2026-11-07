#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int sign(int x) {
  if (x == 0)
    return 0;
  return (x > 0) ? 1 : -1;
}

int max(int x, int y) { return (x > y) ? x : y; }

int min(int x, int y) { return (x < y) ? x : y; }

Coords string_to_coords(const char *input) {
  assert(input != NULL && strlen(input) == 2);

  char col_char = input[0];
  char row_char = input[1];

  assert(col_char >= 'a' && col_char <= 'h' && row_char >= '1' &&
         row_char <= '8');

  return create_coords(8 - (row_char - '0'), col_char - 'a');
}

char *coords_to_string(Coords pos) {
  static char buffer[3];
  snprintf(buffer, sizeof(buffer), "%c%d", 'a' + pos.col, 8 - pos.row);
  return buffer;
}

void clear_stdin_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void clear_screen() { printf("\033[2J\033[H"); }

void write_move(FILE *moves_file, Coords src_pos, Coords dest_pos) {
  fprintf(moves_file, "%s ", coords_to_string(src_pos));
  fprintf(moves_file, "%s\n", coords_to_string(dest_pos));
  fflush(moves_file);
}

// check if a position string is valid
bool is_valid_string_pos(char *input) {
  assert(strlen(input) == 2);

  char col_char = input[0];
  char row_char = input[1];

  return (col_char >= 'a' && col_char <= 'h' && row_char >= '1' &&
          row_char <= '8');
}

void read_coords(char *prompt, Coords *pos) {
  char pos_str[3];

  do {
    printf("%s: ", prompt);
  } while (scanf("%2s", pos_str) != 1 || strlen(pos_str) != 2 ||
           !is_valid_string_pos(pos_str));

  *pos = string_to_coords(pos_str);
  clear_stdin_buffer();
}

bool is_lowercase(char c) { return (c >= 'a' && c <= 'z'); }

bool is_digit(char c) { return (c >= '0' && c <= '9'); }

PieceName piece_name_from_char(char c) {
  switch (c) {
  case 'p':
  case 'P':
    return PAWN;
  case 'r':
  case 'R':
    return ROOK;
  case 'n':
  case 'N':
    return KNIGHT;
  case 'b':
  case 'B':
    return BISHOP;
  case 'q':
  case 'Q':
    return QUEEN;
  case 'k':
  case 'K':
    return KING;
  default:
    return NONE;
  }
}

bool is_valid_piece_char(char c) {
  return (c == 'p' || c == 'r' || c == 'n' || c == 'b' || c == 'q' ||
          c == 'k' || c == 'P' || c == 'R' || c == 'N' || c == 'B' ||
          c == 'Q' || c == 'K');
}

uint64_t rand64() {
  uint64_t r1 = (uint64_t)rand();
  uint64_t r2 = (uint64_t)rand();
  uint64_t r3 = (uint64_t)rand();
  uint64_t r4 = (uint64_t)rand();

  return r1 | (r2 << 16) | (r3 << 32) | (r4 << 48);
}