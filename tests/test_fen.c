#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "chessboard.h"
#include "fen.h"

static const char *initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/"
                                 "RNBQKBNR w KQkq - 0 1";

static void assert_board_has_initial_position(const Board *board) {
  Board expected;
  init_board(&expected);

  for (unsigned int row = 0; row < BOARD_SIZE; row++) {
    for (unsigned int col = 0; col < BOARD_SIZE; col++) {
      CU_ASSERT_EQUAL(board->squares[row][col].name,
                      expected.squares[row][col].name);
      CU_ASSERT_EQUAL(board->squares[row][col].color,
                      expected.squares[row][col].color);
    }
  }

  CU_ASSERT_EQUAL(board->meta.current_turn, expected.meta.current_turn);
  CU_ASSERT_EQUAL(board->meta.halfmove_clock, expected.meta.halfmove_clock);
  CU_ASSERT_EQUAL(board->meta.fullmove_number, expected.meta.fullmove_number);
  CU_ASSERT_TRUE(CASTLE_K(*board, WHITE));
  CU_ASSERT_TRUE(CASTLE_Q(*board, WHITE));
  CU_ASSERT_TRUE(CASTLE_K(*board, BLACK));
  CU_ASSERT_TRUE(CASTLE_Q(*board, BLACK));
  CU_ASSERT_FALSE(ENPASSANT_ACTIVE(*board));
}

void test_fen_loads_initial_position(void) {
  Board board = create_empty_board();

  CU_ASSERT_TRUE(fen_loads(&board, initial_fen));
  assert_board_has_initial_position(&board);
}

void test_fen_loads_position_metadata(void) {
  Board board = create_empty_board();
  const char *fen = "r3k3/8/8/8/8/8/8/4K3 b - - 37 42";

  CU_ASSERT_TRUE(fen_loads(&board, fen));
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(0, 4)).name, KING);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(0, 4)).color, BLACK);
  CU_ASSERT_EQUAL(board.meta.current_turn, BLACK);
  CU_ASSERT_FALSE(CASTLE_K(board, WHITE));
  CU_ASSERT_FALSE(CASTLE_Q(board, WHITE));
  CU_ASSERT_FALSE(CASTLE_K(board, BLACK));
  CU_ASSERT_FALSE(CASTLE_Q(board, BLACK));
  CU_ASSERT_FALSE(ENPASSANT_ACTIVE(board));
  CU_ASSERT_EQUAL(board.meta.halfmove_clock, 37);
  CU_ASSERT_EQUAL(board.meta.fullmove_number, 42);
}

void test_fen_loads_coherent_castling_rights(void) {
  Board board = create_empty_board();
  const char *fen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";

  CU_ASSERT_TRUE(fen_loads(&board, fen));
  CU_ASSERT_TRUE(CASTLE_K(board, WHITE));
  CU_ASSERT_TRUE(CASTLE_Q(board, WHITE));
  CU_ASSERT_TRUE(CASTLE_K(board, BLACK));
  CU_ASSERT_TRUE(CASTLE_Q(board, BLACK));
}

void test_fen_loads_coherent_en_passant_state(void) {
  Board board = create_empty_board();
  const char *fen = "r3k2r/8/8/8/1pPp4/8/8/R3K2R b KQkq c3 0 1";

  CU_ASSERT_TRUE(fen_loads(&board, fen));
  CU_ASSERT_TRUE(ENPASSANT_ACTIVE(board));
  CU_ASSERT_TRUE(are_coords_equal(ENPASSANT_POS(board), create_coords(5, 2)));
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(4, 1)).name, PAWN);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(4, 1)).color, BLACK);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(4, 2)).name, PAWN);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(4, 2)).color, WHITE);
}

void test_fen_loads_rejects_invalid_fen(void) {
  Board board = create_empty_board();

  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6 w - - 0 1"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6k x - - 0 1"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6k w - - -1 1"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6k w - - 0 0"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6k w - - 0 1 extra"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/8/K6k w KQqk - 0 1"));
  CU_ASSERT_FALSE(fen_loads(&board, "8/8/8/8/8/8/11/K6k w - - 0 1"));
  CU_ASSERT_FALSE(fen_loads(&board, NULL));
}

void test_fen_rejects_inconsistent_castling_rights(void) {
  Board board = create_empty_board();

  CU_ASSERT_FALSE(fen_loads(&board, "r3k3/8/8/8/8/8/8/R3K2R w KQkq - 0 1"));

  CU_ASSERT_FALSE(fen_loads(&board, "r3k2r/8/8/8/8/8/8/R3K3 w KQkq - 0 1"));
}

void test_fen_rejects_inconsistent_en_passant_state(void) {
  Board board = create_empty_board();

  CU_ASSERT_FALSE(fen_loads(&board, "4k3/8/8/8/8/8/8/4K3 w - e3 0 1"));

  CU_ASSERT_FALSE(fen_loads(&board, "4k3/8/8/8/8/8/8/4K3 w - e4 0 1"));
}

void test_fen_dumps_initial_position(void) {
  Board board;
  char fen[128] = {0};

  init_board(&board);
  CU_ASSERT_TRUE(fen_dumps(&board, fen));
  CU_ASSERT_STRING_EQUAL(fen, initial_fen);
}

void test_fen_dumps_and_loads_round_trip(void) {
  Board original = create_empty_board();
  Board loaded = create_empty_board();
  char fen[128] = {0};

  set_piece(&original, create_coords(0, 4), create_piece(KING, BLACK));
  set_piece(&original, create_coords(0, 0), create_piece(ROOK, BLACK));
  set_piece(&original, create_coords(7, 4), create_piece(KING, WHITE));
  set_piece(&original, create_coords(3, 3), create_piece(QUEEN, BLACK));
  set_piece(&original, create_coords(4, 2), create_piece(PAWN, WHITE));
  set_piece(&original, create_coords(4, 1), create_piece(PAWN, BLACK));
  set_piece(&original, create_coords(4, 3), create_piece(PAWN, BLACK));
  original.meta.current_turn = BLACK;
  CASTLE_K(original, WHITE) = false;
  CASTLE_Q(original, WHITE) = false;
  CASTLE_K(original, BLACK) = false;
  CASTLE_Q(original, BLACK) = true;
  ENPASSANT_ACTIVE(original) = true;
  ENPASSANT_POS(original) = create_coords(5, 2);
  original.meta.halfmove_clock = 123;
  original.meta.fullmove_number = 76;

  CU_ASSERT_TRUE(fen_dumps(&original, fen));
  CU_ASSERT_TRUE(fen_loads(&loaded, fen));
  CU_ASSERT_STRING_EQUAL(fen, "r3k3/8/8/3q4/1pPp4/8/8/4K3 b q c3 123 76");
  CU_ASSERT_EQUAL(loaded.meta.current_turn, original.meta.current_turn);
  CU_ASSERT_EQUAL(loaded.meta.halfmove_clock, original.meta.halfmove_clock);
  CU_ASSERT_EQUAL(loaded.meta.fullmove_number, original.meta.fullmove_number);
  CU_ASSERT_TRUE(ENPASSANT_ACTIVE(loaded));
  CU_ASSERT_TRUE(
      are_coords_equal(ENPASSANT_POS(loaded), ENPASSANT_POS(original)));
}

void test_fen_file_io(void) {
  Board board = create_empty_board();
  FILE *file = tmpfile();

  CU_ASSERT_PTR_NOT_NULL_FATAL(file);
  CU_ASSERT_TRUE(fputs(initial_fen, file) >= 0);
  rewind(file);
  CU_ASSERT_TRUE(fen_load(&board, file));
  assert_board_has_initial_position(&board);

  rewind(file);
  CU_ASSERT_TRUE(fen_dump(&board, file));
  rewind(file);
  char dumped[128] = {0};
  CU_ASSERT_NOT_EQUAL(fgets(dumped, sizeof(dumped), file), NULL);
  CU_ASSERT_STRING_EQUAL(dumped, initial_fen);

  fclose(file);
}

void register_fen_tests(void) {
  CU_pSuite suite = CU_add_suite("FEN_Suite", NULL, NULL);

  CU_add_test(suite, "test_fen_loads_initial_position",
              test_fen_loads_initial_position);
  CU_add_test(suite, "test_fen_loads_position_metadata",
              test_fen_loads_position_metadata);
  CU_add_test(suite, "test_fen_loads_coherent_castling_rights",
              test_fen_loads_coherent_castling_rights);
  CU_add_test(suite, "test_fen_loads_coherent_en_passant_state",
              test_fen_loads_coherent_en_passant_state);
  CU_add_test(suite, "test_fen_loads_rejects_invalid_fen",
              test_fen_loads_rejects_invalid_fen);
  CU_add_test(suite, "test_fen_rejects_inconsistent_castling_rights",
              test_fen_rejects_inconsistent_castling_rights);
  CU_add_test(suite, "test_fen_rejects_inconsistent_en_passant_state",
              test_fen_rejects_inconsistent_en_passant_state);
  CU_add_test(suite, "test_fen_dumps_initial_position",
              test_fen_dumps_initial_position);
  CU_add_test(suite, "test_fen_dumps_and_loads_round_trip",
              test_fen_dumps_and_loads_round_trip);
  CU_add_test(suite, "test_fen_file_io", test_fen_file_io);
}
