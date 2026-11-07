#include "fen.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

#define FEN_BUFFER_SIZE 128

static bool parse_fen(const char *fen_str, ChessBoard *board);
static bool parse_uint(const char **cursor, unsigned int *value);
static bool parse_board(const char **cursor, ChessBoard *board);
static bool parse_castling(const char **cursor, ChessBoard *board);
static bool parse_en_passant(const char **cursor, ChessBoard *board);
static bool parse_turn(const char **cursor, ChessBoard *board);

static void dump_board(const ChessBoard *board, char *fen_str, size_t *offset);
static void dump_turn(const ChessBoard *board, char *fen_str, size_t *offset);
static void add_space(char *fen_str, size_t *offset);
static void dump_uint(unsigned int value, char *fen_str, size_t *offset);
static void dump_castling(const ChessBoard *board, char *fen_str,
                          size_t *offset);
static void dump_en_passant(const ChessBoard *board, char *fen_str,
                            size_t *offset);

bool is_valid_fen_string(const char *fen_str) {
  if (fen_str == NULL) {
    return false;
  }
  // We will try a parse, if it falls, that means the fen_string is invalid
  ChessBoard board;
  return fen_loads(&board, fen_str);
}

bool fen_loads(ChessBoard *board, const char *fen_str) {
  if (board == NULL || fen_str == NULL) {
    return false;
  }
  ChessBoard parsed_board = create_empty_board();
  if (!parse_fen(fen_str, &parsed_board)) {
    return false;
  }
  sync_board_meta_data(&parsed_board);
  *board = parsed_board;
  return true;
}

bool fen_dumps(const ChessBoard *board, char *fen_str) {
  if (board == NULL || fen_str == NULL) {
    return false;
  }
  size_t offset = 0;
  dump_board(board, fen_str, &offset);
  add_space(fen_str, &offset);

  dump_turn(board, fen_str, &offset);
  add_space(fen_str, &offset);

  dump_castling(board, fen_str, &offset);
  add_space(fen_str, &offset);

  dump_en_passant(board, fen_str, &offset);
  add_space(fen_str, &offset);

  dump_uint(board->meta.halfmove_clock, fen_str, &offset);
  add_space(fen_str, &offset);

  dump_uint(board->meta.fullmove_number, fen_str, &offset);

  return true;
}

bool fen_load(ChessBoard *board, FILE *fen_file) {
  if (board == NULL || fen_file == NULL) {
    return false;
  }
  char fen_str[FEN_BUFFER_SIZE];
  if (fgets(fen_str, FEN_BUFFER_SIZE, fen_file) == NULL) {
    return false;
  }

  // Find the last '\n' et put there the string terminaison marker
  size_t len = strlen(fen_str);
  while (len > 0 && fen_str[--len] != '\n')
    ;
  // If no line break line, we should reach the beginning, so we have nothing to
  // do; else we put '\0'
  if (len > 0) {
    fen_str[len] = '\0';
  }
  return fen_loads(board, fen_str);
}

bool fen_dump(const ChessBoard *board, FILE *fen_file) {
  char fen_str[FEN_BUFFER_SIZE];

  if (board == NULL || fen_file == NULL || !fen_dumps(board, fen_str)) {
    return false;
  }
  return fputs(fen_str, fen_file) >= 0;
}

// @mine::start
static void dump_turn(const ChessBoard *board, char *fen_str, size_t *offset) {
  fen_str[(*offset)++] = (board->meta.current_turn == CHESS_WHITE) ? 'w' : 'b';
  fen_str[(*offset)] = '\0';
}

static void add_space(char *fen_str, size_t *offset) {
  fen_str[(*offset)++] = ' ';
  fen_str[(*offset)] = '\0';
}

static void dump_uint(unsigned int value, char *fen_str, size_t *offset) {
  char buffer[4] = {0}; // we assume `value` won't exceed 999
  sprintf(buffer, "%u", value);

  char *c = buffer;
  while (*c != '\0') {
    fen_str[(*offset)++] = *c;
    c++;
  }
  fen_str[(*offset)] = '\0';
}

static bool parse_turn(const char **cursor, ChessBoard *board) {
  if (**cursor != 'w' && **cursor != 'b') {
    return false;
  }
  board->meta.current_turn = (**cursor == 'w') ? CHESS_WHITE : CHESS_BLACK;
  (*cursor)++;
  return **cursor == ' ';
}

// @mine::end

static bool parse_fen(const char *fen_str, ChessBoard *board) {
  const char *cursor = fen_str;

  if (!parse_board(&cursor, board)) {
    return false;
  }

  cursor++; // skip space
  if (!parse_turn(&cursor, board)) {
    return false;
  }

  cursor++; // skip space
  if (!parse_castling(&cursor, board) || !is_castling_state_consistent(board)) {
    return false;
  }

  cursor++; // skip space
  if (!parse_en_passant(&cursor, board) ||
      !is_enpassant_state_consistent(board)) {
    return false;
  }

  cursor++; // skip space
  if (!parse_uint(&cursor, &board->meta.halfmove_clock)) {
    return false;
  }

  if (*cursor == ' ') {
    cursor++; // skip space
  } else {
    return false;
  }

  if (!parse_uint(&cursor, &board->meta.fullmove_number)) {
    return false;
  }

  return board->meta.fullmove_number > 0 && *cursor == '\0';
}

static void dump_board(const ChessBoard *board, char *fen_str, size_t *offset) {
  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    unsigned int col = 0;
    while (col < BOARD_SIZE) {
      Coords pos = create_coords(row, col);
      Piece piece = get_piece(board, pos);

      unsigned int empty_squares = 0;
      while (col + empty_squares < BOARD_SIZE && is_none_piece(piece)) {
        empty_squares++;
        if (col + empty_squares < BOARD_SIZE) {
          pos = create_coords(row, col + empty_squares);
          piece = get_piece(board, pos);
        }
      }

      if (empty_squares > 0) {
        fen_str[(*offset)++] = '0' + empty_squares;
        col += empty_squares;
      } else {
        fen_str[(*offset)++] = piece_to_fen(piece);
        col++;
      }
    }
    if (row < BOARD_SIZE - 1) {
      fen_str[(*offset)++] = '/';
    }
  }
  fen_str[(*offset)] = '\0';
}

static void dump_castling(const ChessBoard *board, char *fen_str,
                          size_t *offset) {
  size_t start = *offset;

  if (CASTLE_K(*board, CHESS_WHITE)) {
    fen_str[(*offset)++] = 'K';
  }
  if (CASTLE_Q(*board, CHESS_WHITE)) {
    fen_str[(*offset)++] = 'Q';
  }
  if (CASTLE_K(*board, CHESS_BLACK)) {
    fen_str[(*offset)++] = 'k';
  }
  if (CASTLE_Q(*board, CHESS_BLACK)) {
    fen_str[(*offset)++] = 'q';
  }
  if (*offset == start) {
    fen_str[(*offset)++] = '-';
  }
  fen_str[(*offset)] = '\0';
}

static void dump_en_passant(const ChessBoard *board, char *fen_str,
                            size_t *offset) {
  if (!ENPASSANT_ACTIVE(*board)) {
    fen_str[(*offset)++] = '-';
  } else {
    fen_str[(*offset)++] = ('a' + ENPASSANT_POS(*board).col);
    fen_str[(*offset)++] = ('8' - ENPASSANT_POS(*board).row);
  }
  fen_str[(*offset)] = '\0';
}

static bool parse_uint(const char **cursor, unsigned int *value) {
  *value = 0;
  const char *cursor_start = *cursor;
  while (is_digit(**cursor)) {
    *value = (*value) * 10 + (**cursor - '0');
    (*cursor)++;
  }
  return *cursor > cursor_start;
}

static bool parse_board(const char **cursor, ChessBoard *board) {
  unsigned int row = 0;
  while (row < BOARD_SIZE) {
    unsigned int col = 0;
    while (col < BOARD_SIZE) {
      if (is_valid_piece_char(**cursor)) {
        set_piece(board, create_coords(row, col), fen_to_piece(**cursor));
        col++;
      } else if (is_digit(**cursor)) {
        for (unsigned int empty_squares = (**cursor - '0'); empty_squares > 0;
             empty_squares--) {
          if (col < BOARD_SIZE) {
            set_piece(board, create_coords(row, col), EMPTY_PIECE);
          } else {
            return false; // overflow
          }
          col++;
        }
      } else {
        return false; // invalid char
      }
      (*cursor)++;
    }
    row++;
    if (row < BOARD_SIZE) {
      if (**cursor != '/') {
        return false;
      }
      (*cursor)++;
    }
  }
  return **cursor == ' ';
}

static bool parse_castling(const char **cursor, ChessBoard *board) {
  CASTLE_K(*board, CHESS_WHITE) = false;
  CASTLE_Q(*board, CHESS_WHITE) = false;
  CASTLE_K(*board, CHESS_BLACK) = false;
  CASTLE_Q(*board, CHESS_BLACK) = false;

  if (**cursor == '-') {
    (*cursor)++;
    return **cursor == ' ';
  }

  if (**cursor == 'K') {
    CASTLE_K(*board, CHESS_WHITE) = true;
    (*cursor)++;
  }
  if (**cursor == 'Q') {
    CASTLE_Q(*board, CHESS_WHITE) = true;
    (*cursor)++;
  }
  if (**cursor == 'k') {
    CASTLE_K(*board, CHESS_BLACK) = true;
    (*cursor)++;
  }
  if (**cursor == 'q') {
    CASTLE_Q(*board, CHESS_BLACK) = true;
    (*cursor)++;
  }

  return **cursor == ' ';
}

static bool parse_en_passant(const char **cursor, ChessBoard *board) {
  if (**cursor == '-') {
    (*cursor)++;
    ENPASSANT_ACTIVE(*board) = false;
    return **cursor == ' ';
  }

  ENPASSANT_ACTIVE(*board) = true;

  if (**cursor < 'a' || **cursor > 'h') {
    return false;
  }
  unsigned int col = (**cursor - 'a');
  (*cursor)++;

  if (**cursor != '3' && **cursor != '6') {
    return false;
  }
  unsigned int row = BOARD_SIZE - (**cursor - '0');
  (*cursor)++;

  ENPASSANT_POS(*board) = create_coords(row, col);

  return **cursor == ' ';
}
