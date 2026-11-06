#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "king_check.h"
#include "legal_move.h"
#include "move_check.h"

bool is_square_attacked_by(const Board *board, Coords pos,
                           Color attacker_color) {
  unsigned int row, col;

  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      Coords src_pos = create_coords(row, col);
      Piece attacker = get_piece(board, src_pos);

      if (attacker.name == NONE || attacker.color != attacker_color) {
        continue;
      }

      if (attacker.name == PAWN) {
        int delta_row = (attacker.color == WHITE) ? -1 : 1;
        int row_diff = (int)pos.row - (int)src_pos.row;
        int col_diff = (int)pos.col - (int)src_pos.col;

        if (row_diff == delta_row && abs(col_diff) == 1) {
          return true;
        }

      } else if ((attacker.name == KNIGHT &&
                  is_knight_valid_move(board, src_pos, pos)) ||
                 (attacker.name == BISHOP &&
                  is_bishop_valid_move(board, src_pos, pos)) ||
                 (attacker.name == ROOK &&
                  is_rook_valid_move(board, src_pos, pos)) ||
                 (attacker.name == QUEEN &&
                  is_queen_valid_move(board, src_pos, pos)) ||
                 (attacker.name == KING &&
                  is_king_valid_move(board, src_pos, pos))) {
        return true;
      }
    }
  }
  return false;
}

bool is_square_attacked(const Board *board, Coords pos) {
  if (!is_empty_square(board, pos)) {
    return is_square_attacked_by(
        board, pos, get_opponent_color(get_piece(board, pos).color));
  }

  return is_square_attacked_by(board, pos, BLACK) ||
         is_square_attacked_by(board, pos, WHITE);
}

// WARNING: This function assumes that the color's king position field is
// initialized correctly in the board structure. If the king's position is not
// set, this can lead to undefined behavior.
bool is_king_in_check(const Board *board, Color color) {
  return is_square_attacked(board, KING_POS(*board, color));
}
