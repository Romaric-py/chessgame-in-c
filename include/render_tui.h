#ifndef RENDER_TUI_H
#define RENDER_TUI_H
#define reset "\e[0m"

const char *repr_piece(Piece piece);
void display_board(ChessBoard *board);
void display_board_with_hint(ChessBoard *board, Coords src_pos);

#endif