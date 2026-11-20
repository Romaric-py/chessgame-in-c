#include <stdbool.h>
#include <stdlib.h>

#include "king_check.h"
#include "legal_move.h"
#include "move_check.h"

bool is_square_attacked_by_pawn(const ChessBoard *board, Coords pos,
                                ChessColor attacker_color) {
  int delta_row = (attacker_color == CHESS_WHITE) ? 1 : -1;

  // Attacks can come from the left or right diagonal squares
  if (is_valid_coords(pos.row + delta_row, pos.col - 1)) {
    Coords left_diagonal = create_coords(pos.row + delta_row, pos.col - 1);
    if (get_piece(board, left_diagonal).name == PAWN &&
        get_piece(board, left_diagonal).color == attacker_color) {
      return true;
    }
  }
  if (is_valid_coords(pos.row + delta_row, pos.col + 1)) {
    Coords right_diagonal = create_coords(pos.row + delta_row, pos.col + 1);
    if (get_piece(board, right_diagonal).name == PAWN &&
        get_piece(board, right_diagonal).color == attacker_color) {
      return true;
    }
  }
  return false;
}

bool is_square_attacked_by_knight(const ChessBoard *board, Coords pos,
                                  ChessColor attacker_color) {
  int knight_dirs[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                           {1, -2},  {1, 2},  {2, -1},  {2, 1}};

  for (int i = 0; i < 8; i++) {
    int new_row = pos.row + knight_dirs[i][0];
    int new_col = pos.col + knight_dirs[i][1];

    if (is_valid_coords(new_row, new_col)) {
      Coords new_pos = create_coords(new_row, new_col);
      Piece piece = get_piece(board, new_pos);
      if (piece.name == KNIGHT && piece.color == attacker_color) {
        return true;
      }
    }
  }
  return false;
}

bool is_square_attacked_by_king(const ChessBoard *board, Coords pos,
                                ChessColor attacker_color) {
  int king_dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  for (int i = 0; i < 8; i++) {
    int new_row = pos.row + king_dirs[i][0];
    int new_col = pos.col + king_dirs[i][1];

    if (is_valid_coords(new_row, new_col)) {
      Coords new_pos = create_coords(new_row, new_col);
      Piece piece = get_piece(board, new_pos);
      if (piece.name == KING && piece.color == attacker_color) {
        return true;
      }
    }
  }
  return false;
}

bool walk_til_reach_or_block(const ChessBoard *board, Coords start,
                             int row_step, int col_step,
                             PieceName attacker_name,
                             ChessColor attacker_color) {
  int current_row = start.row + row_step;
  int current_col = start.col + col_step;

  while (is_valid_coords(current_row, current_col)) {
    Coords current_pos = create_coords(current_row, current_col);
    Piece piece = get_piece(board, current_pos);
    if (piece.name != NONE) {
      if (piece.name == attacker_name && piece.color == attacker_color) {
        return true; // Found the attacking piece
      } else {
        return false; // Blocked by another piece
      }
    }
    current_row += row_step;
    current_col += col_step;
  }
  return false; // Went out of bounds without reaching the target
}

bool is_square_attacked_by_rook(const ChessBoard *board, Coords pos,
                                ChessColor attacker_color) {
  int rook_dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (int i = 0; i < 4; i++) {
    if (walk_til_reach_or_block(board, pos, rook_dirs[i][0], rook_dirs[i][1],
                                ROOK, attacker_color)) {
      return true;
    }
  }
  return false;
}

bool is_square_attacked_by_bishop(const ChessBoard *board, Coords pos,
                                  ChessColor attacker_color) {
  int bishop_dirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  for (int i = 0; i < 4; i++) {
    if (walk_til_reach_or_block(board, pos, bishop_dirs[i][0],
                                bishop_dirs[i][1], BISHOP, attacker_color)) {
      return true;
    }
  }
  return false;
}

bool is_square_attacked_by_queen(const ChessBoard *board, Coords pos,
                                 ChessColor attacker_color) {
  int queen_dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                          {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  for (int i = 0; i < 8; i++) {
    if (walk_til_reach_or_block(board, pos, queen_dirs[i][0], queen_dirs[i][1],
                                QUEEN, attacker_color)) {
      return true;
    }
  }
  return false;
}

bool is_square_attacked_by(const ChessBoard *board, Coords pos,
                           ChessColor attacker_color) {
  return is_square_attacked_by_pawn(board, pos, attacker_color) ||
         is_square_attacked_by_knight(board, pos, attacker_color) ||
         is_square_attacked_by_bishop(board, pos, attacker_color) ||
         is_square_attacked_by_rook(board, pos, attacker_color) ||
         is_square_attacked_by_queen(board, pos, attacker_color) ||
         is_square_attacked_by_king(board, pos, attacker_color);
}

bool is_square_attacked(const ChessBoard *board, Coords pos) {
  if (!is_empty_square(board, pos)) {
    return is_square_attacked_by(
        board, pos, get_opponent_color(get_piece(board, pos).color));
  }

  return is_square_attacked_by(board, pos, CHESS_BLACK) ||
         is_square_attacked_by(board, pos, CHESS_WHITE);
}

// WARNING: This function assumes that the color's king position field is
// initialized correctly in the board structure. If the king's position is not
// set, this can lead to undefined behavior.
bool is_king_in_check(const ChessBoard *board, ChessColor color) {
  // King in check =  only if king squrare is attacked by opponent's pieces
  return is_square_attacked_by(board, KING_POS(*board, color),
                               get_opponent_color(color));
}
