#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "game_status.h"
#include "legal_move.h"
#include "minmax.h"
#include "play_chess.h"
#include "polyglot.h"
#include "position_history.h"
#include "render_tui.h"
#include "special_move.h"
#include "utils.h"


char *piece_name_to_string(PieceName name) {
  switch (name) {
  case PAWN:
    return "Pawn";
  case BISHOP:
    return "Bishop";
  case KNIGHT:
    return "Knight";
  case ROOK:
    return "Rook";
  case QUEEN:
    return "Queen";
  case KING:
    return "King";
  default:
    return "None";
  }
}

void get_human_move(ChessBoard *board, ChessColor color, Move *move, GameContext *context) {
  if (context != NULL && context->is_timed) {
    printf("Remaining time: %u seconds\n", context->remaining_seconds[color]);
  }
  do {
    do {
      read_coords("From", &move->src);
    } while (get_piece(board, move->src).color != color);

    clear_screen();
    display_board_with_hint(board, move->src);
    read_coords("To", &move->dest);
    clear_screen();
    display_board(board);

  } while (!is_legal_move(board, move->src, move->dest));

  // The following field values can be wrong but that doesn't matter since they
  // are only used for ai move.
  move->is_capture = false;
  move->promotion = NONE;
}

void choose_piece_name(char *prompt, PieceName *name) {
  char pn;
  do {
    printf("%s", prompt);
    pn = getchar();
    clear_stdin_buffer();
  } while (pn != 'N' && pn != 'R' && pn != 'Q' && pn != 'B' && pn != 'n' &&
           pn != 'r' && pn != 'q' && pn != 'b');

  switch (pn) {
  case 'n':
  case 'N':
    *name = KNIGHT;
    break;
  case 'b':
  case 'B':
    *name = BISHOP;
    break;
  case 'q':
  case 'Q':
    *name = QUEEN;
    break;
  case 'r':
  case 'R':
    *name = ROOK;
    break;
  default:
    *name = NONE;
    break;
  }
}

void get_ai_move(ChessBoard *board, ChessColor color, Move *move, GameContext *context) {
  static unsigned int move_book_counter = 0;
  if (choose_move_from_book(board, move)) {
    printf("Move chosen from book (%u) :) \n", ++move_book_counter);
    return; // Book move found and chosen
  }

  // Outside the book: fall back to the standard Alpha-Beta search
  // minmax_ab_parallel(board, color, color, move, DEPTH, context);
  minmax_ab(board, color, color, move, DEPTH, -INFINITY, INFINITY, context);
}

void make_pawn_promotion(ChessBoard *board, ChessColor color, Coords pos,
                         PieceName promotion) {
  // Promotion
  Piece piece = create_piece(promotion, color);
  set_piece(board, pos, piece);
}

void chess_turn(ChessBoard *board, ChessColor color, FuncGetMove black_get_move,
                FuncGetMove white_get_move, GameContext *context, FILE *moves_file) {
  printf("%s's turn\n", (color == CHESS_BLACK) ? "Black" : "White");
  Move move_to_play;
  bool can_promote;

  FuncGetMove get_move =
      (color == CHESS_BLACK) ? black_get_move : white_get_move;
  get_move(board, color, &move_to_play, context);

  write_move(moves_file, move_to_play.src, move_to_play.dest);
  can_promote = can_promote_pawn(board, move_to_play.src, move_to_play.dest);
  MoveReport report = make_move(board, move_to_play.src, move_to_play.dest);

  if (can_promote) {
    if (move_to_play.promotion == NONE) { // probably a human move
      choose_piece_name("Promote pawn to (N, R, Q, B): ",
                        &move_to_play.promotion);
    }
    make_pawn_promotion(board, color, move_to_play.dest,
                        move_to_play.promotion);
  }

  clear_screen();
  display_board(board);
  printf("Last move: %s ->", coords_to_string(report.src_pos));
  printf(" %s", coords_to_string(report.dest_pos));
  if (report.captured_piece.name != NONE) {
    printf(" (captured %s %s)", (report.captured_piece.color == CHESS_WHITE) ? "White" : "Black", piece_name_to_string(report.captured_piece.name));
  }
  printf("\n");
}

void play_chess(ChessColor *winner, bool *draw, FuncGetMove black_get_move,
                FuncGetMove white_get_move) {
  ChessBoard board;
  FILE *moves_file = fopen(MOVE_RECORD_FILE, "w");
  GameContext context = {
      .history_counter = create_position_history_counter(),
      .remaining_seconds = {0, 0},
      .is_timed = false,
  };

  *winner = CHESS_WHITE;
  *draw = true; // default to draw in case of error

  if (moves_file == NULL) {
    perror(MOVE_RECORD_FILE);
    return;
  }

  init_board(&board);

  increment_position_history_counter(&context.history_counter, board.meta.hash);

  ChessColor opponent_color, curr_color = CHESS_WHITE;
  bool end_game = false;
  GameStatus game_status = GAME_IN_PROGRESS;

  clear_screen();
  display_board(&board);

  while (!end_game) {
    chess_turn(&board, curr_color, black_get_move, white_get_move, &context, moves_file);

    opponent_color = get_opponent_color(curr_color);
    game_status = get_game_status(&board, opponent_color, &context);

    *draw = (game_status == GAME_STALEMATE ||
             game_status == GAME_DRAW_50_MOVE_RULE ||
             game_status == GAME_DRAW_INSUFFICIENT_MATERIAL ||
             game_status == GAME_DRAW_3FOLD_REPETITION);
    end_game = (game_status != GAME_IN_PROGRESS && game_status != GAME_CHECK);

    if (end_game) {
      if (!*draw) {
        *winner = curr_color;
        *draw = false;
      } else {
        *draw = true;
      }
      break;
    }

    curr_color = opponent_color;
    increment_position_history_counter(&context.history_counter, board.meta.hash);
  }

  free_position_history_counter(&context.history_counter);
  fclose(moves_file);
}
