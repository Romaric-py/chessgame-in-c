#include "transposition_table.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//! Improve and complete this module later

uint64_t position_piece_hashes[8][8][2][6]; // col/row/color/name
uint64_t position_enpassant_hashes[8][8];   // col/row
uint64_t roque_hashes[2][2]; // color/side (0: king side, 1: queen side)
uint64_t white_turn_hash;
TTEntry transposition_table[TT_SIZE];

void init_zobrist_hashes() {
  srand(time(NULL));

  // Initialize the Zobrist hashes for each piece at each position
  for (unsigned int row = 0; row < 8; row++) {
    for (unsigned int col = 0; col < 8; col++) {
      for (unsigned int color = 0; color < 2; color++) {
        for (unsigned int name = 0; name < 6; name++) {
          position_piece_hashes[row][col][color][name] = (uint64_t)rand64();
        }
      }
    }
  }

  // Player turn hash
  white_turn_hash = (uint64_t)rand64();

  // En passant hashes
  for (unsigned int row = 0; row < 8; row++) {
    for (unsigned int col = 0; col < 8; col++) {
      position_enpassant_hashes[row][col] = (uint64_t)rand64();
    }
  }

  // Castling rights
  for (unsigned int color = 0; color < 2; color++) {
    for (unsigned int side = 0; side < 2; side++) {
      roque_hashes[color][side] = (uint64_t)rand64();
    }
  }
}

uint64_t get_position_piece_hash(Coords pos, Piece piece) {
  return position_piece_hashes[pos.row][pos.col][piece.color][piece.name];
}

uint64_t get_position_enpassant_hash(Coords pos) {
  return position_enpassant_hashes[pos.row][pos.col];
}

uint64_t get_roque_hash(ChessColor color, bool side) {
  return roque_hashes[color][side];
}

static void _update_roque_hash(uint64_t *hash, const BoardMetaData *meta,
                               ChessColor color) {
  if (meta->colors[color].castle_rights.queen_side) {
    *hash ^= get_roque_hash(color, 0);
  }
  if (meta->colors[color].castle_rights.king_side) {
    *hash ^= get_roque_hash(color, 1);
  }
}

uint64_t hash_board_state(ChessBoard *board) {
  Coords pos;
  uint64_t hash = 0ULL;

  // Pieces and their positions
  for (unsigned int row = 0; row < 8; row++) {
    for (unsigned int col = 0; col < 8; col++) {
      pos = create_coords(row, col);
      if (!is_empty_square(board, pos)) {
        hash ^= get_position_piece_hash(pos, get_piece(board, pos));
      }
    }
  }

  // Player turn
  if (board->meta.current_turn == CHESS_WHITE) {
    hash ^= white_turn_hash;
  }

  // Castling rights
  _update_roque_hash(&hash, &board->meta, CHESS_BLACK);
  _update_roque_hash(&hash, &board->meta, CHESS_WHITE);

  // En passant
  if (ENPASSANT_ACTIVE(*board)) {
    hash ^= get_position_enpassant_hash(ENPASSANT_POS(*board));
  }

  return hash;
}

//! When move_report will include the color who played the move, we can use it
//! to update the hash without needing the board state
void increment_hash_board_state(uint64_t *hash, ChessBoard *board,
                                const MoveReport *move_report) {

  // Remove the piece from src_pos
  *hash ^=
      get_position_piece_hash(move_report->src_pos, move_report->moved_piece);

  // Add the piece to dest_pos
  *hash ^=
      get_position_piece_hash(move_report->dest_pos, move_report->moved_piece);

  // If a piece was captured, remove it from the hash
  if (!is_none_piece(move_report->captured_piece)) {
    *hash ^= get_position_piece_hash(move_report->dest_pos,
                                     move_report->captured_piece);
  }

  // Special cases: en passant and roque moves
  if (move_report->is_enpassant_move) {
    // Remove the captured pawn from the hash
    *hash ^= get_position_piece_hash(move_report->old_meta.enpassant.target_pos,
                                     move_report->captured_piece);
  } else if (move_report->is_castling_move) {
    Coords rook_src_pos, rook_dest_pos;
    compute_rook_move_for_castling(move_report->src_pos, move_report->dest_pos,
                                   &rook_src_pos, &rook_dest_pos);

    // Remove / Add pieces for the rook's move; the king's move is already
    // handled above
    *hash ^=
        get_position_piece_hash(rook_src_pos, get_piece(board, rook_src_pos));
    *hash ^=
        get_position_piece_hash(rook_dest_pos, get_piece(board, rook_dest_pos));
  }

  // Update the player turn hash
  *hash ^= white_turn_hash;

  // Update the en passant hash
  if (move_report->old_meta.enpassant.is_active) {
    // Remove the old en passant hash
    *hash ^=
        get_position_enpassant_hash(move_report->old_meta.enpassant.target_pos);
  }
  if (ENPASSANT_ACTIVE(*board)) {
    // Add the new en passant hash
    *hash ^= get_position_enpassant_hash(ENPASSANT_POS(*board));
  }

  // Update the castling rights hash
  // Only the color of the player who made the move can have its castling rights
  // changed
  _update_roque_hash(hash, &move_report->old_meta,
                     move_report->moved_piece.color);
  _update_roque_hash(hash, &board->meta, move_report->moved_piece.color);
}

void add_to_transposition_table(uint64_t hash, int score, unsigned int depth,
                                Move best_move, int alpha, int beta) {
  size_t index = hash & (TT_SIZE - 1); // works because TT_SIZE is a power of 2
  TTEntry new_entry = {.zobrist_key = hash,
                       .score = score,
                       .depth = depth,
                       .best_move = best_move,
                       .flag = (score <= alpha)  ? UPPER_BOUND
                               : (score >= beta) ? LOWER_BOUND
                                                 : EXACT};

  if (transposition_table[index].zobrist_key == 0 ||
      depth >= transposition_table[index].depth) {
    transposition_table[index] = new_entry;
  }
}

bool fetch_from_transposition_table(uint64_t hash, int *score, Move *best_move,
                                    int alpha, int beta, unsigned int depth) {
  size_t index = hash & (TT_SIZE - 1); // works because TT_SIZE is a power of 2

  if (transposition_table[index].zobrist_key == 0 ||
      transposition_table[index].zobrist_key != hash ||
      depth > transposition_table[index].depth) {
    return false;
  }

  TTEntry *entry = &transposition_table[index];
  if (entry->flag == EXACT ||
      (entry->flag == LOWER_BOUND && entry->score >= beta) ||
      (entry->flag == UPPER_BOUND && entry->score <= alpha)) {
    *score = entry->score;
    *best_move = entry->best_move;
    return true;
  }

  return false;
}

void clear_transposition_table() {
  memset(transposition_table, 0, TT_SIZE * sizeof(TTEntry));
}