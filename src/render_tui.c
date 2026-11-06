#include <stdio.h>

#include "chessboard.h"
#include "legal_move.h"
#include "render_tui.h"

// The static buffer was an Gemini 3.5 Flash-Lite's idea

const char *repr_piece(Piece piece) {
  static char buffer[3];

  if (is_none_piece(piece)) {
    return "  ";
  }

  char piece_char;
  switch (piece.name) {
  case PAWN:
    piece_char = 'P';
    break;
  case KNIGHT:
    piece_char = 'N';
    break;
  case BISHOP:
    piece_char = 'B';
    break;
  case ROOK:
    piece_char = 'R';
    break;
  case QUEEN:
    piece_char = 'Q';
    break;
  case KING:
    piece_char = 'K';
    break;
  default:
    piece_char = '?';
    break;
  }

  char color_char = (piece.color == WHITE) ? 'W' : 'B';

  sprintf(buffer, "%c%c", piece_char, color_char);

  return buffer;
}

void display_board(Board *board) {

  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    printf(" %d ", BOARD_SIZE - row);
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      Coords pos = create_coords(row, col);
      if ((row + col) % 2 == 0) {
        printf("\e[0;107;30m"); // black on white
      } else {
        printf("\e[0;100m"); // white on black
      }
      printf(" %s " reset, repr_piece(get_piece(board, pos)));
    }
    printf(" \n");
  }
  printf("   ");
  for (char col = 0; col < BOARD_SIZE; col++) {
    printf("  %c ", 'a' + col);
  }
  printf("\n\n");
}

void display_board_with_hint(Board *board, Coords src_pos) {
  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    printf(" %d ", BOARD_SIZE - row);
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      Coords pos = create_coords(row, col);
      if (is_legal_move(board, src_pos, pos) ||
          are_coords_equal(pos, src_pos)) {
        printf("\e[0;46;30m"); // black on cyan
      } else if ((row + col) % 2 == 0) {
        printf("\e[0;107;30m"); // black on white
      } else {
        printf("\e[0;100m"); // white on black
      }
      printf(" %s " reset, repr_piece(get_piece(board, pos)));
    }
    printf(" \n");
  }
  printf("   ");
  for (char col = 0; col < BOARD_SIZE; col++) {
    printf("  %c ", 'a' + col);
  }
  printf("\n\n");
}
