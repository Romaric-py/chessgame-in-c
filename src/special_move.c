#include <assert.h>
#include <stdlib.h>

#include "king_check.h"
#include "move_check.h"
#include "special_move.h"
#include "utils.h"

static bool is_safe_range(const ChessBoard *board, Coords src_pos,
                          Coords dest_pos, int delta_row, int delta_col,
                          ChessColor color);

bool is_enpassant_move(const ChessBoard *board, Coords src_pos,
                       Coords dest_pos) {
  if (!ENPASSANT_ACTIVE(*board) ||
      !are_coords_equal(dest_pos, ENPASSANT_POS(*board)) ||
      is_empty_square(board, src_pos) || !is_empty_square(board, dest_pos)) {
    return false; // impossible to do "en passant" move in that configuration
  }

  // abs(src_pos.col - dest_col.col) = 1
  // if white: row_diff = 1 ; black: row_diff = -1
  Piece attacker = get_piece(board, src_pos);
  int delta_row = (attacker.color == CHESS_BLACK) ? 1 : -1;

  Piece attacked = get_piece(board, create_coords(src_pos.row, dest_pos.col));

  return attacker.color != attacked.color &&
         abs((int)dest_pos.col - (int)src_pos.col) == 1 &&
         ((int)dest_pos.row - (int)src_pos.row) == delta_row;
}

// assumes the move is valid
bool can_promote_pawn(const ChessBoard *board, Coords src_pos,
                      Coords dest_pos) {
  Piece pawn = get_piece(board, src_pos);

  return pawn.name == PAWN &&
         ((pawn.color == CHESS_WHITE && dest_pos.row == 0) ||
          (pawn.color == CHESS_BLACK && dest_pos.row == BOARD_SIZE - 1));
}

bool is_castling_move(const ChessBoard *board, Coords src_pos,
                      Coords dest_pos) {
  Piece src_piece = get_piece(board, src_pos);

  // src square should contain a king
  if (src_piece.name != KING)
    return false;

  ChessColor color = src_piece.color;

  int move_direction = sign(dest_pos.col - src_pos.col);
  unsigned int rook_col = (move_direction == -1) ? 0 : BOARD_SIZE - 1;
  Coords rook_coords = create_coords(src_pos.row, rook_col);
  bool castling_right = (move_direction == -1) ? CASTLE_Q(*board, color)
                                               : CASTLE_K(*board, color);

  return !are_coords_equal(src_pos, dest_pos) &&

         // dest_pos.col should be either 2 or 6
         (dest_pos.col == 2 || dest_pos.col == 6) &&

         // this castling move should be allowed by the castling state
         castling_right &&

         // the king should not be in check
         !is_square_attacked_by(board, src_pos, get_opponent_color(color)) &&

         // all squares between the targeted rook and the king should be empty
         // (bounds not included)
         is_free_range(board, src_pos, rook_coords, 0, move_direction) &&

         // all squares between src(not included) and dest (on the same row)
         // should not be attacked
         is_valid_coords(dest_pos.row, dest_pos.col + move_direction) &&
         is_safe_range(
             board, src_pos,
             create_coords(dest_pos.row, dest_pos.col + move_direction), 0,
             move_direction, color);
}

static bool is_safe_range(const ChessBoard *board, Coords src_pos,
                          Coords dest_pos, int delta_row, int delta_col,
                          ChessColor color) {
  assert(delta_col != 0 || delta_row != 0);

  unsigned int row = src_pos.row + delta_row;
  unsigned int col = src_pos.col + delta_col;
  bool result = true;
  ChessColor opponent_color = get_opponent_color(color);

  while (result && (row < BOARD_SIZE) && (col < BOARD_SIZE) &&
         !are_coords_equal(dest_pos, create_coords(row, col))) {
    result =
        !is_square_attacked_by(board, create_coords(row, col), opponent_color);
    row += delta_row;
    col += delta_col;
  }
  return result;
}