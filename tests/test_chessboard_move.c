#include <CUnit/Basic.h>

#include "chessboard.h"
#include "special_move.h"

static void assert_boards_equal(const Board *expected, const Board *actual) {
    unsigned int row, col;

    for (row = 0; row < BOARD_SIZE; row++) {
        for (col = 0; col < BOARD_SIZE; col++) {
            CU_ASSERT_EQUAL(expected->squares[row][col].name,
                            actual->squares[row][col].name);
            CU_ASSERT_EQUAL(expected->squares[row][col].color,
                            actual->squares[row][col].color);
        }
    }

    CU_ASSERT_EQUAL(expected->enpassant.flag, actual->enpassant.flag);
    CU_ASSERT_TRUE(are_coords_equal(expected->enpassant.pos,
                                    actual->enpassant.pos));
    CU_ASSERT_EQUAL(expected->roque.black.king, actual->roque.black.king);
    CU_ASSERT_EQUAL(expected->roque.black.rook_a, actual->roque.black.rook_a);
    CU_ASSERT_EQUAL(expected->roque.black.rook_h, actual->roque.black.rook_h);
    CU_ASSERT_EQUAL(expected->roque.white.king, actual->roque.white.king);
    CU_ASSERT_EQUAL(expected->roque.white.rook_a, actual->roque.white.rook_a);
    CU_ASSERT_EQUAL(expected->roque.white.rook_h, actual->roque.white.rook_h);
    CU_ASSERT_TRUE(are_coords_equal(expected->king_pos.black,
                                    actual->king_pos.black));
    CU_ASSERT_TRUE(are_coords_equal(expected->king_pos.white,
                                    actual->king_pos.white));
}

void test_move_and_undo_quiet_move(void) {
    Board board = create_board();
    Coords source = create_coords(4, 4);
    Coords destination = create_coords(4, 7);
    Piece rook = create_piece(ROOK, WHITE);
    MoveReport report;

    set_piece(&board, source, rook);
    Board before = clone_board(&board);

    report = move(&board, source, destination);

    CU_ASSERT_EQUAL(report.moved_piece.name, ROOK);
    CU_ASSERT_TRUE(is_none_piece(report.captured_piece));
    CU_ASSERT_EQUAL(get_piece(&board, destination).name, ROOK);
    CU_ASSERT_TRUE(is_empty_square(&board, source));

    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_and_undo_capture(void) {
    Board board = create_board();
    Coords source = create_coords(4, 4);
    Coords destination = create_coords(4, 7);
    MoveReport report;

    set_piece(&board, source, create_piece(ROOK, WHITE));
    set_piece(&board, destination, create_piece(KNIGHT, BLACK));
    Board before = clone_board(&board);

    report = move(&board, source, destination);

    CU_ASSERT_EQUAL(report.captured_piece.name, KNIGHT);
    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_and_undo_pawn_double_step_restores_en_passant_state(void) {
    Board board = create_board();
    Coords source = create_coords(6, 3);
    Coords destination = create_coords(4, 3);
    MoveReport report;

    board.enpassant.flag = true;
    board.enpassant.pos = create_coords(3, 5);
    set_piece(&board, source, create_piece(PAWN, WHITE));
    Board before = clone_board(&board);

    report = move(&board, source, destination);

    CU_ASSERT_TRUE(board.enpassant.flag);
    CU_ASSERT_TRUE(are_coords_equal(board.enpassant.pos, destination));

    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_and_undo_en_passant(void) {
    Board board = create_board();
    Coords source = create_coords(3, 3);
    Coords destination = create_coords(2, 4);
    Coords attacked = create_coords(3, 4);
    MoveReport report;

    set_piece(&board, source, create_piece(PAWN, WHITE));
    set_piece(&board, attacked, create_piece(PAWN, BLACK));
    board.enpassant.flag = true;
    board.enpassant.pos = attacked;
    Board before = clone_board(&board);

    report = move(&board, source, destination);

    CU_ASSERT_TRUE(report.is_enpassant_move);
    CU_ASSERT_EQUAL(report.captured_piece.name, PAWN);
    CU_ASSERT_TRUE(is_empty_square(&board, attacked));
    CU_ASSERT_EQUAL(get_piece(&board, destination).name, PAWN);

    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_and_undo_kingside_castle(void) {
    Board board = create_board();
    Coords king = create_coords(7, 4);
    Coords rook = create_coords(7, 7);
    MoveReport report;

    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, rook, create_piece(ROOK, WHITE));
    Board before = clone_board(&board);

    report = move(&board, king, create_coords(7, 6));

    CU_ASSERT_TRUE(report.is_roque_move);
    CU_ASSERT_EQUAL(get_piece(&board, create_coords(7, 6)).name, KING);
    CU_ASSERT_EQUAL(get_piece(&board, create_coords(7, 5)).name, ROOK);
    CU_ASSERT_FALSE(board.roque.white.king);

    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_and_undo_promotion_position(void) {
    Board board = create_board();
    Coords source = create_coords(1, 0);
    Coords destination = create_coords(0, 0);
    MoveReport report;

    set_piece(&board, source, create_piece(PAWN, WHITE));
    Board before = clone_board(&board);

    report = move(&board, source, destination);
    CU_ASSERT_EQUAL(get_piece(&board, destination).name, PAWN);

    set_piece(&board, destination, create_piece(QUEEN, WHITE));
    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_move_updates_rook_rights(void) {
    Board board = create_board();
    Coords rook = create_coords(7, 0);
    set_piece(&board, rook, create_piece(ROOK, WHITE));
    Board before = clone_board(&board);

    MoveReport report = move(&board, rook, create_coords(6, 0));
    CU_ASSERT_FALSE(board.roque.white.rook_a);

    undo_move(&board, report);
    assert_boards_equal(&before, &board);
}

void test_capture_updates_captured_rook_rights(void) {
    Board board = create_board();
    Coords king = create_coords(0, 4);
    Coords captured_rook = create_coords(0, 0);
    Coords other_rook = create_coords(0, 7);
    Coords capturing_rook = create_coords(1, 0);

    set_piece(&board, king, create_piece(KING, BLACK));
    set_piece(&board, captured_rook, create_piece(ROOK, BLACK));
    set_piece(&board, other_rook, create_piece(ROOK, BLACK));
    set_piece(&board, capturing_rook, create_piece(ROOK, WHITE));

    move(&board, capturing_rook, captured_rook);
    move(&board, other_rook, captured_rook);

    CU_ASSERT_FALSE(board.roque.black.rook_a);
    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(0, 2)));
}

void register_chessboard_move_tests(void) {
    CU_pSuite suite = CU_add_suite("Chessboard_Move_Suite", NULL, NULL);

    CU_add_test(suite, "test_move_and_undo_quiet_move",
                test_move_and_undo_quiet_move);
    CU_add_test(suite, "test_move_and_undo_capture",
                test_move_and_undo_capture);
    CU_add_test(suite, "test_move_and_undo_pawn_double_step_restores_en_passant_state",
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
