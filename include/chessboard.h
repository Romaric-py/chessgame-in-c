#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdbool.h>

#define BOARD_SIZE 8
#define EMPTY_PIECE ((Piece){ .name = NONE, .color = BLACK })

typedef enum Color {
    BLACK,
    WHITE,
} Color;

typedef enum PieceName {
    NONE, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
} PieceName;

typedef struct Piece {
    PieceName name;
    Color color;
} Piece;

typedef struct Board {
    Piece squares[BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct Coords {
    unsigned int row, col;
} Coords;


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
Piece get_piece(Board board, Coords pos);
void set_piece(Board *board, Coords pos, Piece piece);
bool is_empty_square(Board board, Coords pos);
void move(Board *board, Coords src_pos, Coords dest_pos);


#endif