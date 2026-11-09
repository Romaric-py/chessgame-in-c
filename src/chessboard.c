#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "chessboard.h"
#include "move_check.h"
#include "special_move.h"
#include "transposition_table.h"

static void _init_meta(ChessBoard *board);
static void _init_piece_counts(ChessBoard *board);
static void _count_pieces(ChessBoard *board);
static void _update_roque_flags(ChessBoard *board, Coords src_pos);
static void _update_king_position(ChessBoard *board, Coords src_pos,
                                  Coords dest_pos);
static void _update_move_counters(ChessBoard *board, Piece moved_piece,
                                  Piece captured_piece);
static void _update_board_hash(ChessBoard *board, Piece piece, Coords pos);
static void _update_metadata_hash(uint64_t *hash, const BoardMetaData *old_meta,
                                  const BoardMetaData *new_meta);

// ChessColor
ChessColor get_opponent_color(ChessColor color) {
  return (color == CHESS_BLACK) ? CHESS_WHITE : CHESS_BLACK;
}

// Piece
Piece create_piece(PieceName name, ChessColor color) {
  return (Piece){.name = name, .color = color};
}

bool is_none_piece(Piece piece) { return piece.name == NONE; }

bool are_pieces_equal(Piece piece1, Piece piece2) {
  return (piece1.name == piece2.name) && (piece1.color == piece2.color);
}

// Coords
Coords create_coords(unsigned int row, unsigned int col) {
  assert(row < BOARD_SIZE && col < BOARD_SIZE);
  return (Coords){.row = row, .col = col};
}

bool is_valid_coords(int row, int col) {
  return (0 <= row && row < BOARD_SIZE) && (0 <= col && col < BOARD_SIZE);
}

bool are_coords_equal(Coords coords1, Coords coords2) {
  return (coords1.row == coords2.row) && (coords1.col == coords2.col);
}

// ChessBoard
ChessBoard create_empty_board() {
  ChessBoard board;
  unsigned int row, col;

  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      board.squares[row][col] = EMPTY_PIECE;
    }
  }

  _init_meta(&board);

  return board;
}

ChessBoard clone_board(const ChessBoard *src_board) {
  if (src_board == NULL) {
    return create_empty_board();
  }

  return *src_board; // shallow copy is enough since the board does not contain
                     // any pointer
}

void init_board(ChessBoard *board) {
  unsigned int i, row, col;
  ChessColor color;
  // Empty squares
  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      if (1 < row && row < 6) {
        board->squares[row][col] = EMPTY_PIECE;
      }
    }
  }
  // Two lines of pawns
  for (col = 0; col < BOARD_SIZE; col++) {
    board->squares[6][col] = create_piece(PAWN, CHESS_WHITE);
    board->squares[1][col] = create_piece(PAWN, CHESS_BLACK);
  }
  // The others pieces
  for (i = 0; i < 2; i++) {
    if (i == 0) {
      row = 0;
      color = CHESS_BLACK;
    } else {
      row = 7;
      color = CHESS_WHITE;
    }

    board->squares[row][0] = create_piece(ROOK, color);
    board->squares[row][7] = create_piece(ROOK, color);

    board->squares[row][1] = create_piece(KNIGHT, color);
    board->squares[row][6] = create_piece(KNIGHT, color);

    board->squares[row][2] = create_piece(BISHOP, color);
    board->squares[row][5] = create_piece(BISHOP, color);

    board->squares[row][3] = create_piece(QUEEN, color);
    board->squares[row][4] = create_piece(KING, color);

    KING_POS(*board, color) = create_coords(row, 4);
  }

  _init_meta(board);
  sync_board_meta_data(board);
}

Piece get_piece(const ChessBoard *board, Coords pos) {
  return board->squares[pos.row][pos.col];
}

void set_piece(ChessBoard *board, Coords pos, Piece piece) {
  if (!is_empty_square(board, pos)) {
    Piece p = get_piece(board, pos);
    _update_board_hash(board, p, pos);
    PIECE_COUNT(*board, p.color, p.name)--;
    if (p.name == KING && are_coords_equal(KING_POS(*board, p.color), pos)) {
      KING_POS(*board, p.color) =
          (Coords){.row = BOARD_SIZE, .col = BOARD_SIZE};
    }
  }

  board->squares[pos.row][pos.col] = piece;

  if (piece.name == KING) {
    KING_POS(*board, piece.color) = pos;
  }

  if (!is_none_piece(piece)) {
    PIECE_COUNT(*board, piece.color, piece.name)++;
    _update_board_hash(board, piece, pos);
  }
}

void switch_turn(ChessBoard *board) {
  board->meta.current_turn = get_opponent_color(board->meta.current_turn);
}

bool is_empty_square(const ChessBoard *board, Coords pos) {
  return is_none_piece(get_piece(board, pos));
}

void compute_rook_move_for_castling(Coords king_src_pos, Coords king_dest_pos,
                                    Coords *rook_src_pos,
                                    Coords *rook_dest_pos) {
  unsigned int rook_col;
  rook_col = (king_dest_pos.col < king_src_pos.col) ? 0 : BOARD_SIZE - 1;
  *rook_src_pos = create_coords(king_dest_pos.row, rook_col);
  rook_col = (king_dest_pos.col < king_src_pos.col) ? 3 : 5;
  *rook_dest_pos = create_coords(king_dest_pos.row, rook_col);
}

// Ensure that the move is legal before calling this function
MoveReport make_move(ChessBoard *board, Coords src_pos, Coords dest_pos) {
  MoveReport report = {
      .src_pos = src_pos,
      .dest_pos = dest_pos,
      .moved_piece = get_piece(board, src_pos),
      .captured_piece = get_piece(board, dest_pos),
      .old_meta = board->meta,
      .is_castling_move = is_castling_move(board, src_pos, dest_pos),
      .is_enpassant_move = is_enpassant_move(board, src_pos, dest_pos)};

  if (report.is_enpassant_move) {
    Coords captured_pawn_pos = create_coords(src_pos.row, dest_pos.col);
    report.captured_piece = get_piece(board, captured_pawn_pos);
    set_piece(board, captured_pawn_pos, EMPTY_PIECE);
  }

  /* since the move down here, just make it for one piece, we
      to manually move the rook for castling move */
  if (report.is_castling_move) {
    Coords rook_src_pos, rook_dest_pos;
    compute_rook_move_for_castling(src_pos, dest_pos, &rook_src_pos,
                                   &rook_dest_pos);

    set_piece(board, rook_dest_pos, get_piece(board, rook_src_pos));
    set_piece(board, rook_src_pos, EMPTY_PIECE);
  }

  // "en passant" flag managing
  if (is_pawn_2steps_move(board, src_pos, dest_pos)) {
    board->meta.enpassant.is_active = true;
    board->meta.enpassant.target_pos =
        create_coords((src_pos.row + dest_pos.row) / 2, dest_pos.col);
  } else if (board->meta.enpassant.is_active) {
    board->meta.enpassant.is_active = false;
  }

  _update_roque_flags(board, dest_pos); // if a rook has been captured
  _update_roque_flags(board, src_pos);  // if a rook or a king has been moved
  _update_king_position(board, src_pos, dest_pos);
  _update_move_counters(board, report.moved_piece, report.captured_piece);

  set_piece(board, dest_pos, get_piece(board, src_pos));
  set_piece(board, src_pos, EMPTY_PIECE);
  switch_turn(board);
  _update_metadata_hash(&board->meta.hash, &report.old_meta, &board->meta);
  return report;
}

void undo_move(ChessBoard *board, const MoveReport *move_report) {
  if (move_report->is_enpassant_move) {
    set_piece(
        board,
        create_coords(move_report->src_pos.row, move_report->dest_pos.col),
        move_report->captured_piece);
  }
  if (move_report->is_castling_move) {
    Coords rook_src_pos, rook_dest_pos;
    compute_rook_move_for_castling(move_report->src_pos, move_report->dest_pos,
                                   &rook_src_pos, &rook_dest_pos);

    set_piece(board, rook_src_pos, get_piece(board, rook_dest_pos));
    set_piece(board, rook_dest_pos, EMPTY_PIECE);
  }

  set_piece(board, move_report->src_pos, move_report->moved_piece);
  set_piece(board, move_report->dest_pos,
            move_report->is_enpassant_move ? EMPTY_PIECE
                                           : move_report->captured_piece);
  // Restore the board state to what it was before the move
  board->meta = move_report->old_meta;
  // If we restore the board state first, since set_piece will update the piece
  // counts, we will have a wrong piece count. So we restore the board state
  // after restoring the pieces.
}

bool is_castling_state_consistent(const ChessBoard *board) {
  // white
  bool king_well_placed =
      are_coords_equal(KING_POS(*board, CHESS_WHITE), create_coords(7, 4));
  bool rook_a_well_placed = are_pieces_equal(
      get_piece(board, create_coords(7, 0)), create_piece(ROOK, CHESS_WHITE));
  bool rook_h_well_placed = are_pieces_equal(
      get_piece(board, create_coords(7, 7)), create_piece(ROOK, CHESS_WHITE));

  if ((CASTLE_K(*board, CHESS_WHITE) &&
       !(king_well_placed && rook_h_well_placed)) ||
      (CASTLE_Q(*board, CHESS_WHITE) &&
       !(king_well_placed && rook_a_well_placed))) {
    return false;
  }

  // black
  king_well_placed =
      are_coords_equal(KING_POS(*board, CHESS_BLACK), create_coords(0, 4));
  rook_a_well_placed = are_pieces_equal(get_piece(board, create_coords(0, 0)),
                                        create_piece(ROOK, CHESS_BLACK));
  rook_h_well_placed = are_pieces_equal(get_piece(board, create_coords(0, 7)),
                                        create_piece(ROOK, CHESS_BLACK));

  if ((CASTLE_K(*board, CHESS_BLACK) &&
       !(king_well_placed && rook_h_well_placed)) ||
      (CASTLE_Q(*board, CHESS_BLACK) &&
       !(king_well_placed && rook_a_well_placed))) {
    return false;
  }

  return true;
}

bool is_enpassant_state_consistent(const ChessBoard *board) {
  if (!ENPASSANT_ACTIVE(*board)) {
    return true;
  }

  Coords target_pos = ENPASSANT_POS(*board);

  if (!is_empty_square(board, target_pos)) {
    return false;
  }

  if (target_pos.row != 2 && target_pos.row != 5) {
    return false;
  }

  unsigned int pawn_row =
      target_pos.row == 2 ? target_pos.row + 1 : target_pos.row - 1;
  Piece pawn = get_piece(board, create_coords(pawn_row, target_pos.col));
  if (pawn.name != PAWN || pawn.color == board->meta.current_turn) {
    return false;
  }

  return true;
}

void sync_board_meta_data(ChessBoard *board) {
  _count_pieces(board);
  board->meta.hash = hash_board_state(board);
}

// Static helper functions

// Initialize the piece_counts array in BoardMetaData to zero for both colors
// and all piece types.
static void _init_piece_counts(ChessBoard *board) {
  for (int color = 0; color < 2; color++) {
    for (int piece_name = 0; piece_name < 6; piece_name++) {
      board->meta.colors[color].piece_counts[piece_name] = 0;
    }
  }
}

// Initialize the board metadata to the default starting values
static void _init_meta(ChessBoard *board) {
  CASTLE_Q(*board, CHESS_WHITE) = true;
  CASTLE_K(*board, CHESS_WHITE) = true;
  CASTLE_Q(*board, CHESS_BLACK) = true;
  CASTLE_K(*board, CHESS_BLACK) = true;

  board->meta.enpassant.is_active = false;
  board->meta.current_turn = CHESS_WHITE;
  board->meta.halfmove_clock = 0;
  board->meta.fullmove_number = 1;
  board->meta.hash = hash_board_state(board);

  _init_piece_counts(board);
}

// Iterate over the board and count the pieces for each color and type, updating
// the piece_counts array in BoardMetaData.
static void _count_pieces(ChessBoard *board) {
  _init_piece_counts(board);
  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      Piece piece = get_piece(board, create_coords(row, col));
      if (!is_none_piece(piece)) {
        PIECE_COUNT(*board, piece.color, piece.name)++;
      }
    }
  }
}

//! The following function should be moved to the module in charge of Zobrist
//! hashing.
// Update the Zobrist hash of the board state by XORing the hash value for the
// given piece at the specified position.
static void _update_board_hash(ChessBoard *board, Piece piece, Coords pos) {
  board->meta.hash ^= get_position_piece_hash(pos, piece);
}

static void _update_metadata_hash(uint64_t *hash, const BoardMetaData *old_meta,
                                  const BoardMetaData *new_meta) {
  if (old_meta->current_turn != new_meta->current_turn) {
    *hash ^= white_turn_hash;
  }

  for (unsigned int color = 0; color < 2; color++) {
    update_castling_hash(hash, old_meta, color);
    update_castling_hash(hash, new_meta, color);
  }

  if (old_meta->enpassant.is_active) {
    *hash ^= get_position_enpassant_hash(old_meta->enpassant.target_pos);
  }
  if (new_meta->enpassant.is_active) {
    *hash ^= get_position_enpassant_hash(new_meta->enpassant.target_pos);
  }
}

// Ensure the move is valid before calling this function
static void _update_roque_flags(ChessBoard *board, Coords src_pos) {
  Piece piece = get_piece(board, src_pos);

  if (piece.name != KING && piece.name != ROOK)
    return;
  // Starting here, we ensure that it's a rook or a king

  if (piece.name == KING) {
    CASTLE_Q(*board, piece.color) = false;
    CASTLE_K(*board, piece.color) = false;
  } else if (src_pos.col == 0) {
    CASTLE_Q(*board, piece.color) = false;
  } else if (src_pos.col == BOARD_SIZE - 1) {
    CASTLE_K(*board, piece.color) = false;
  }
}

// Ensure the move is valid before calling this function
static void _update_king_position(ChessBoard *board, Coords src_pos,
                                  Coords dest_pos) {
  Piece piece = get_piece(board, src_pos);
  if (piece.name != KING)
    return;
  KING_POS(*board, piece.color) = dest_pos;
}

// Update the halfmove clock and fullmove number based on the moved piece and
// captured piece. If a pawn is moved or a piece is captured, reset the halfmove
// clock to 0. Otherwise, increment the halfmove clock. Increment the fullmove
// number after Black's move.
static void _update_move_counters(ChessBoard *board, Piece moved_piece,
                                  Piece captured_piece) {
  // Update the halfmove clock
  if (moved_piece.name == PAWN || !is_none_piece(captured_piece)) {
    board->meta.halfmove_clock = 0;
  } else {
    board->meta.halfmove_clock++;
  }

  // Update the fullmove number
  if (moved_piece.color == CHESS_BLACK) {
    board->meta.fullmove_number++;
  }
}
