#include <CUnit/Basic.h>

#include "chessboard.h"
#include "special_move.h"
#include "transposition_table.h"

static void assert_boards_equal(const ChessBoard *expected,
                                const ChessBoard *actual) {
  unsigned int row, col;

  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      CU_ASSERT_EQUAL(expected->squares[row][col].name,
                      actual->squares[row][col].name);
      CU_ASSERT_EQUAL(expected->squares[row][col].color,
                      actual->squares[row][col].color);
    }
  }

  CU_ASSERT_EQUAL(ENPASSANT_ACTIVE(*expected), ENPASSANT_ACTIVE(*actual));
  CU_ASSERT_TRUE(
      are_coords_equal(ENPASSANT_POS(*expected), ENPASSANT_POS(*actual)));
  CU_ASSERT_EQUAL(CASTLE_Q(*expected, CHESS_BLACK),
                  CASTLE_Q(*actual, CHESS_BLACK));
  CU_ASSERT_EQUAL(CASTLE_K(*expected, CHESS_BLACK),
                  CASTLE_K(*actual, CHESS_BLACK));
  CU_ASSERT_EQUAL(CASTLE_Q(*expected, CHESS_WHITE),
                  CASTLE_Q(*actual, CHESS_WHITE));
  CU_ASSERT_EQUAL(CASTLE_K(*expected, CHESS_WHITE),
                  CASTLE_K(*actual, CHESS_WHITE));
  CU_ASSERT_TRUE(are_coords_equal(KING_POS(*expected, CHESS_BLACK),
                                  KING_POS(*actual, CHESS_BLACK)));
  CU_ASSERT_TRUE(are_coords_equal(KING_POS(*expected, CHESS_WHITE),
                                  KING_POS(*actual, CHESS_WHITE)));
  for (unsigned int color = 0; color < 2; color++) {
    for (unsigned int piece_name = 0; piece_name < 6; piece_name++) {
      CU_ASSERT_EQUAL(PIECE_COUNT(*expected, color, piece_name),
                      PIECE_COUNT(*actual, color, piece_name));
    }
  }
  CU_ASSERT_EQUAL(actual->meta.hash, hash_board_state(actual));
}

void test_move_and_undo_quiet_move(void) {
  ChessBoard board = create_empty_board();
  Coords source = create_coords(4, 4);
  Coords destination = create_coords(4, 7);
  Piece rook = create_piece(ROOK, CHESS_WHITE);
  MoveReport report;

  set_piece(&board, source, rook);
  ChessBoard before = clone_board(&board);

  report = make_move(&board, source, destination);

  CU_ASSERT_EQUAL(report.moved_piece.name, ROOK);
  CU_ASSERT_TRUE(is_none_piece(report.captured_piece));
  CU_ASSERT_EQUAL(get_piece(&board, destination).name, ROOK);
  CU_ASSERT_TRUE(is_empty_square(&board, source));

  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_and_undo_capture(void) {
  ChessBoard board = create_empty_board();
  Coords source = create_coords(4, 4);
  Coords destination = create_coords(4, 7);
  MoveReport report;

  set_piece(&board, source, create_piece(ROOK, CHESS_WHITE));
  set_piece(&board, destination, create_piece(KNIGHT, CHESS_BLACK));
  ChessBoard before = clone_board(&board);

  report = make_move(&board, source, destination);

  CU_ASSERT_EQUAL(report.captured_piece.name, KNIGHT);
  CU_ASSERT_EQUAL(board.meta.hash, hash_board_state(&board));
  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_and_undo_pawn_double_step_restores_en_passant_state(void) {
  ChessBoard board = create_empty_board();
  Coords source = create_coords(6, 3);
  Coords destination = create_coords(4, 3);
  MoveReport report;

  set_piece(&board, source, create_piece(PAWN, CHESS_WHITE));
  ChessBoard before = clone_board(&board);

  report = make_move(&board, source, destination);

  CU_ASSERT_TRUE(ENPASSANT_ACTIVE(board));
  CU_ASSERT_TRUE(are_coords_equal(ENPASSANT_POS(board), create_coords(5, 3)));

  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_and_undo_en_passant(void) {
  ChessBoard board = create_empty_board();
  Coords source = create_coords(3, 3);
  Coords destination = create_coords(2, 4);
  Coords attacked = create_coords(3, 4);
  MoveReport report;

  set_piece(&board, source, create_piece(PAWN, CHESS_WHITE));
  set_piece(&board, create_coords(1, 4), create_piece(PAWN, CHESS_BLACK));
  make_move(&board, create_coords(1, 4), attacked);
  CU_ASSERT_TRUE(ENPASSANT_ACTIVE(board));
  CU_ASSERT_TRUE(are_coords_equal(ENPASSANT_POS(board), destination));
  ChessBoard before = clone_board(&board);

  report = make_move(&board, source, destination);

  CU_ASSERT_TRUE(report.is_enpassant_move);
  CU_ASSERT_EQUAL(report.captured_piece.name, PAWN);
  CU_ASSERT_TRUE(is_empty_square(&board, attacked));
  CU_ASSERT_EQUAL(get_piece(&board, destination).name, PAWN);

  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_and_undo_kingside_castle(void) {
  ChessBoard board = create_empty_board();
  Coords king = create_coords(7, 4);
  Coords rook = create_coords(7, 7);
  MoveReport report;

  set_piece(&board, king, create_piece(KING, CHESS_WHITE));
  set_piece(&board, rook, create_piece(ROOK, CHESS_WHITE));
  ChessBoard before = clone_board(&board);

  report = make_move(&board, king, create_coords(7, 6));

  CU_ASSERT_TRUE(report.is_castling_move);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(7, 6)).name, KING);
  CU_ASSERT_EQUAL(get_piece(&board, create_coords(7, 5)).name, ROOK);
  CU_ASSERT_FALSE(CASTLE_K(board, CHESS_WHITE));

  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_and_undo_promotion_position(void) {
  ChessBoard board = create_empty_board();
  Coords source = create_coords(1, 0);
  Coords destination = create_coords(0, 0);
  MoveReport report;

  set_piece(&board, source, create_piece(PAWN, CHESS_WHITE));
  ChessBoard before = clone_board(&board);

  report = make_move(&board, source, destination);
  CU_ASSERT_EQUAL(get_piece(&board, destination).name, PAWN);

  set_piece(&board, destination, create_piece(QUEEN, CHESS_WHITE));
  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_move_updates_rook_rights(void) {
  ChessBoard board = create_empty_board();
  Coords rook = create_coords(7, 0);
  set_piece(&board, rook, create_piece(ROOK, CHESS_WHITE));
  ChessBoard before = clone_board(&board);

  MoveReport report = make_move(&board, rook, create_coords(6, 0));
  CU_ASSERT_FALSE(CASTLE_Q(board, CHESS_WHITE));

  undo_move(&board, &report);
  assert_boards_equal(&before, &board);
}

void test_capture_updates_captured_rook_rights(void) {
  ChessBoard board = create_empty_board();
  Coords king = create_coords(0, 4);
  Coords captured_rook = create_coords(0, 0);
  Coords other_rook = create_coords(0, 7);
  Coords capturing_rook = create_coords(1, 0);

  set_piece(&board, king, create_piece(KING, CHESS_BLACK));
  set_piece(&board, captured_rook, create_piece(ROOK, CHESS_BLACK));
  set_piece(&board, other_rook, create_piece(ROOK, CHESS_BLACK));
  set_piece(&board, capturing_rook, create_piece(ROOK, CHESS_WHITE));

  make_move(&board, capturing_rook, captured_rook);
  make_move(&board, other_rook, captured_rook);

  CU_ASSERT_FALSE(CASTLE_Q(board, CHESS_BLACK));
  CU_ASSERT_FALSE(is_castling_move(&board, king, create_coords(0, 2)));
}

void register_chessboard_move_tests(void) {
  CU_pSuite suite = CU_add_suite("Chessboard_Move_Suite", NULL, NULL);

  CU_add_test(suite, "test_move_and_undo_quiet_move",
              test_move_and_undo_quiet_move);
  CU_add_test(suite, "test_move_and_undo_capture", test_move_and_undo_capture);
  CU_add_test(suite,
              "test_move_and_undo_pawn_double_step_restores_en_passant_state",
              test_move_and_undo_pawn_double_step_restores_en_passant_state);
  CU_add_test(suite, "test_move_and_undo_en_passant",
              test_move_and_undo_en_passant);
  CU_add_test(suite, "test_move_and_undo_kingside_castle",
              test_move_and_undo_kingside_castle);
  CU_add_test(suite, "test_move_and_undo_promotion_position",
              test_move_and_undo_promotion_position);
  CU_add_test(suite, "test_move_updates_rook_rights",
              test_move_updates_rook_rights);
  CU_add_test(suite, "test_capture_updates_captured_rook_rights",
              test_capture_updates_captured_rook_rights);
}
