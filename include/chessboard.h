#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdbool.h>

#define BOARD_SIZE 8
#define EMPTY_PIECE ((Piece){.name = NONE, .color = BLACK})

#define KING_POS(b, c) ((b).meta.colors[c].king_position)
#define CASTLE_Q(b, c) ((b).meta.colors[c].castle_rights.queen_side)
#define CASTLE_K(b, c) ((b).meta.colors[c].castle_rights.king_side)
#define ENPASSANT_POS(b) ((b).meta.enpassant.target_pos)
#define ENPASSANT_ACTIVE(b) ((b).meta.enpassant.is_active)
#define PIECE_COUNT(b, c, p) ((b).meta.colors[c].piece_counts[p])

typedef enum Color {
  BLACK = 0,
  WHITE,
} Color;

typedef enum PieceName {
  PAWN = 0,
  BISHOP,
  KNIGHT,
  ROOK,
  QUEEN,
  KING,
  NONE
} PieceName;

typedef struct Piece {
  PieceName name;
  Color color;
} Piece;

typedef struct Coords {
  unsigned int row, col;
} Coords;

typedef struct EnPassantState {
  bool is_active;
  Coords target_pos; // the dest_pos of the pawn attacking
} EnPassantState;

typedef struct CastleRights {
  bool queen_side;
  bool king_side;
} CastleRights;

typedef struct OneColorState {
  CastleRights castle_rights;
  Coords king_position;
  unsigned int piece_counts[6];
} OneColorState;

typedef struct BoardMetaData {
  EnPassantState enpassant;
  OneColorState colors[2];
  Color current_turn;
  unsigned int halfmove_clock; // for the 50-move rule
  unsigned int fullmove_number;
} BoardMetaData;

typedef struct Board {
  Piece squares[BOARD_SIZE][BOARD_SIZE];
  BoardMetaData meta;
} Board;

typedef struct {
  Coords src_pos, dest_pos;
  Piece moved_piece, captured_piece;
  BoardMetaData old_meta;
  bool is_castling_move; // captured_piece is EMPTY_PIECE if no capture occurs
  bool is_enpassant_move;
} MoveReport;

// Color
Color get_opponent_color(Color color);

// Piece
Piece create_piece(PieceName name, Color color);
bool is_none_piece(Piece piece);

// Coords
Coords create_coords(unsigned int row, unsigned int col);
bool are_coords_equal(Coords coords1, Coords coords2);
bool is_valid_coords(int row, int col);

// Board
Board create_empty_board();
void init_board(Board *board);
Board clone_board(const Board *src_board);
Piece get_piece(const Board *board, Coords pos);
void set_piece(Board *board, Coords pos, Piece piece);
bool is_empty_square(const Board *board, Coords pos);
MoveReport make_move(Board *board, Coords src_pos, Coords dest_pos);
void undo_move(Board *board, const MoveReport *move_report);
void compute_rook_move_for_castling(Coords king_src_pos, Coords king_dest_pos,
                                    Coords *rook_src_pos,
                                    Coords *rook_dest_pos);
bool is_castling_state_consistent(Board *board);
bool is_enpassant_state_consistent(Board *board);

#endif