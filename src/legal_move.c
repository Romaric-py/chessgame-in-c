#include "legal_move.h"
#include "king_check.h"
#include "move_check.h"
#include "special_move.h"
#include "utils.h"

// This function adds a pawn move to the movelist taking into account pawn
// promotion.
static void add_pawn_move(MoveList *movelist, Move move, ChessColor color,
                          bool stop_at_first) {
  bool promotes =
      move.dest.row == ((color == CHESS_WHITE) ? 0 : BOARD_SIZE - 1);

  if (!promotes) {
    add_move(movelist, move);
    return;
  }
  // A pawn promotion corresponds to 4 different moves, one for each promotion
  // piece type
  const PieceName promotions[] = {QUEEN, ROOK, BISHOP, KNIGHT};
  for (unsigned int i = 0; i < 4; i++) {
    move.promotion = promotions[i];
    add_move(movelist, move);
    if (stop_at_first) {
      return;
    }
  }
}

bool piece_has_legal_move(ChessBoard *board, Coords pos) {
  Piece piece = get_piece(board, pos);
  MoveList moves;
  init_movelist(&moves);
  generate_legal_moves_for_piece(board, piece.color, pos, true, &moves);
  return moves.count > 0;
}

bool has_legal_move(ChessBoard *board, ChessColor color) {
  MoveList moves;
  generate_legal_moves(board, color, true, &moves);
  return moves.count > 0;
}

bool is_legal_move(ChessBoard *board, Coords src_pos, Coords dest_pos) {
  if (!is_valid_move(board, src_pos, dest_pos)) {
    return false;
  }

  // Impossible to capture a king
  if (!is_empty_square(board, dest_pos) &&
      get_piece(board, dest_pos).name == KING) {
    return false;
  }

  // Here, the move is valid (and the src_pos square is not empty)
  Piece piece = get_piece(board, src_pos);
  Piece captured_piece = get_piece(board, dest_pos);

  // Simulate the move
  set_piece(board, dest_pos, piece);
  set_piece(board, src_pos, EMPTY_PIECE);

  // Check if the king is in check after the move
  bool leaves_king_in_check = is_king_in_check(board, piece.color);

  // Undo the move
  set_piece(board, src_pos, piece);
  set_piece(board, dest_pos, captured_piece);

  return !leaves_king_in_check;
}

/*
The parameter stop_at_first is useful in case that we just want to ensure that
at least one legal move exists for a piece.
*/
void generate_legal_moves_for_piece(ChessBoard *board, ChessColor color,
                                    Coords pos, bool stop_at_first,
                                    MoveList *movelist) {
  Piece piece = get_piece(board, pos);

  if (piece.name == NONE || piece.color != color) {
    return; // No piece at the position or wrong color
  }
  switch (piece.name) {
  case PAWN:
    generate_legal_moves_for_pawn(board, color, pos, stop_at_first, movelist);
    break;
  case KNIGHT:
    generate_legal_moves_for_knight(board, color, pos, stop_at_first, movelist);
    break;
  case BISHOP:
    generate_legal_moves_for_bishop(board, color, pos, stop_at_first, movelist);
    break;
  case ROOK:
    generate_legal_moves_for_rook(board, color, pos, stop_at_first, movelist);
    break;
  case QUEEN:
    generate_legal_moves_for_queen(board, color, pos, stop_at_first, movelist);
    break;
  case KING:
    generate_legal_moves_for_king(board, color, pos, stop_at_first, movelist);
    break;
  default:
    break; // nothing to do for NONE piece
  }
}

void generate_legal_moves_for_pawn(ChessBoard *board, ChessColor color,
                                   Coords pos, bool stop_at_first,
                                   MoveList *movelist) {
  Coords dest_pos;
  Move move;
  // assumes pos contains a pawn

  int delta_row = (color == CHESS_BLACK) ? 1 : -1;
  int next_row = pos.row + delta_row;

  for (int col = (int)pos.col - 1; col <= (int)pos.col + 1; col++) {
    if (is_valid_coords(next_row, col)) {
      dest_pos = create_coords(next_row, col);
      if (is_legal_move(board, pos, dest_pos)) {
        move = create_move(pos, dest_pos);
        if (!is_empty_square(board, dest_pos) &&
            get_piece(board, dest_pos).color == get_opponent_color(color)) {
          move.is_capture = true;
        }
        if (is_enpassant_move(board, pos, dest_pos)) {
          move.is_capture = true;
        }
        add_pawn_move(movelist, move, color, stop_at_first);
        if (stop_at_first)
          return;
      }
    }
  }

  next_row = next_row + delta_row; // Check for the two-square initial move

  if (is_valid_coords(next_row, pos.col)) {
    dest_pos = create_coords(next_row, pos.col);
    if (is_legal_move(board, pos, dest_pos)) {
      move = create_move(pos, dest_pos);
      add_pawn_move(movelist, move, color, stop_at_first);
      if (stop_at_first)
        return;
    }
  }
}

void generate_legal_moves_for_knight(ChessBoard *board, ChessColor color,
                                     Coords pos, bool stop_at_first,
                                     MoveList *movelist) {
  Coords dest_pos;
  Move move;
  int row, col;
  // assumes pos contains a knight

  int deltas[8][2] = {{-2, -1}, {-1, -2}, {-2, 1}, {1, -2},
                      {2, -1},  {-1, 2},  {1, 2},  {2, 1}};

  for (unsigned int i = 0; i < 8; i++) {
    row = pos.row + deltas[i][0];
    col = pos.col + deltas[i][1];

    if (is_valid_coords(row, col)) {
      dest_pos = create_coords(row, col);
      if (is_legal_move(board, pos, dest_pos)) {
        move = create_move(pos, dest_pos);
        if (!is_empty_square(board, dest_pos) &&
            get_piece(board, dest_pos).color == get_opponent_color(color)) {
          move.is_capture = true;
        }
        add_move(movelist, move);
        if (stop_at_first)
          return;
      }
    }
  }
}

void generate_legal_moves_for_king(ChessBoard *board, ChessColor color,
                                   Coords pos, bool stop_at_first,
                                   MoveList *movelist) {
  Coords dest_pos;
  Move move;
  int row, col;
  // assumes pos contains a king

  int deltas[10][2] = {
      {-1, -1}, {-1, 0}, {-1, 1}, {1, -1}, {1, 0},
      {1, 1},   {0, -1}, {0, 1},  {0, 2},  {0, -2}}; // last two are for roque

  for (unsigned int i = 0; i < 10; i++) {
    row = pos.row + deltas[i][0];
    col = pos.col + deltas[i][1];

    if (is_valid_coords(row, col)) {
      dest_pos = create_coords(row, col);
      if (is_legal_move(board, pos, dest_pos)) {
        move = create_move(pos, dest_pos);
        if (!is_empty_square(board, dest_pos) &&
            get_piece(board, dest_pos).color == get_opponent_color(color)) {
          move.is_capture = true;
        }
        add_move(movelist, move);
        if (stop_at_first)
          return;
      }
    }
  }
}

void generate_legal_moves_for_bishop(ChessBoard *board, ChessColor color,
                                     Coords pos, bool stop_at_first,
                                     MoveList *movelist) {
  // assumes pos contains a bishop
  int dirs[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

  for (unsigned int i = 0; i < 4; i++) {
    generate_legal_moves_in_direction(board, color, pos, dirs[i][0], dirs[i][1],
                                      stop_at_first, movelist);
    if (stop_at_first && movelist->count > 0)
      return;
  }
}

void generate_legal_moves_for_rook(ChessBoard *board, ChessColor color,
                                   Coords pos, bool stop_at_first,
                                   MoveList *movelist) {
  // assumes pos contains a rook
  int dirs[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  for (unsigned int i = 0; i < 4; i++) {
    generate_legal_moves_in_direction(board, color, pos, dirs[i][0], dirs[i][1],
                                      stop_at_first, movelist);
    if (stop_at_first && movelist->count > 0)
      return;
  }
}

void generate_legal_moves_for_queen(ChessBoard *board, ChessColor color,
                                    Coords pos, bool stop_at_first,
                                    MoveList *movelist) {
  // assumes pos contains a queen
  generate_legal_moves_for_bishop(board, color, pos, stop_at_first, movelist);
  if (stop_at_first && movelist->count > 0)
    return;

  generate_legal_moves_for_rook(board, color, pos, stop_at_first, movelist);
}

void generate_legal_moves_in_direction(ChessBoard *board, ChessColor color,
                                       Coords pos, int delta_row, int delta_col,
                                       bool stop_at_first, MoveList *movelist) {
  Coords dest_pos;
  Move move;

  int row = pos.row + delta_row;
  int col = pos.col + delta_col;

  while (is_valid_coords(row, col)) {

    dest_pos = create_coords(row, col);
    if (is_legal_move(board, pos, dest_pos)) {
      move = create_move(pos, dest_pos);
      if (!is_empty_square(board, dest_pos) &&
          get_piece(board, dest_pos).color == get_opponent_color(color)) {
        move.is_capture = true;
      }
      add_move(movelist, move);
      if (stop_at_first)
        return;
    }
    if (!is_empty_square(board, dest_pos))
      break;

    row += delta_row;
    col += delta_col;
  }
}

void generate_legal_moves(ChessBoard *board, ChessColor color,
                          bool stop_at_first, MoveList *movelist) {
  unsigned int row, col;
  Coords pos;
  init_movelist(movelist);

  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      pos = create_coords(row, col);
      if (!is_empty_square(board, pos) &&
          get_piece(board, pos).color == color) {
        generate_legal_moves_for_piece(board, color, pos, stop_at_first,
                                       movelist);
        if (stop_at_first && movelist->count > 0)
          return;
      }
    }
  }
}
