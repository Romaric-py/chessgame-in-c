#ifndef RENDER_TUI_H
#define RENDER_TUI_H
#define reset "\e[0m"

const char* repr_piece(Piece piece);
void display_board(Board *board);
void display_board_with_hint(Board *board, Coords src_pos);


#endif