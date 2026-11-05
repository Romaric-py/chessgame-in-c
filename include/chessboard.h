#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdbool.h>

#define BOARD_SIZE 8
#define EMPTY_PIECE ((Piece){ .name = NONE, .color = BLACK })

typedef enum Color {
    BLACK = 0,
    WHITE,
} Color;

typedef enum PieceName {
    PAWN = 0, BISHOP, KNIGHT, ROOK, QUEEN, KING, NONE
} PieceName;

typedef struct Piece {
    PieceName name;
    Color color;
} Piece;

typedef struct Coords {
    unsigned int row, col;
} Coords;

struct _enPassantState {
    bool flag;
    Coords pos; // the actual position of the attacked pawn
};

struct _oneColorRoqueState {
    bool king;
    bool rook_a;
    bool rook_h;
};
struct _roqueState {
    struct _oneColorRoqueState black;
    struct _oneColorRoqueState white;
};

struct _kingPosition {
    Coords black;
    Coords white;
};

typedef struct Board {
    Piece squares[BOARD_SIZE][BOARD_SIZE];
    struct _enPassantState enpassant;
    struct _roqueState roque;
    struct _kingPosition king_pos;
    Color current_turn;
} Board;

typedef struct {
    struct _enPassantState enpassant;
    struct _roqueState roque;
    struct _kingPosition king_pos;
} _oldSpecialState;

typedef struct {
    Color moved_color;
    Coords src_pos, dest_pos;
    Piece moved_piece, captured_piece; // captured_piece is EMPTY_PIECE if no capture occurs
    _oldSpecialState old_state;
    bool is_roque_move;
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
Board create_board();
void init_board(Board *board);
Board clone_board(const Board *src_board);
Piece get_piece(const Board *board, Coords pos);
void set_piece(Board *board, Coords pos, Piece piece);
bool is_empty_square(const Board *board, Coords pos);
MoveReport move(Board *board, Coords src_pos, Coords dest_pos);
void undo_move(Board *board, MoveReport move_report);
void compute_rook_move_for_roque(Coords king_src_pos, Coords king_dest_pos, Coords *rook_src_pos, Coords *rook_dest_pos);


#endif