#include <CUnit/Basic.h>

#include "chessboard.h"
#include "legal_move.h"
#include "movelist.h"
#include "special_move.h"

void test_enpassant_accepts_matching_pawn_capture(void) {
    Board board = create_board();
    Coords source = create_coords(3, 3);
    Coords destination = create_coords(2, 4);

    set_piece(&board, source, create_piece(PAWN, WHITE));
    set_piece(&board, create_coords(3, 4), create_piece(PAWN, BLACK));
    board.enpassant.flag = true;
    board.enpassant.pos = create_coords(3, 4);

    CU_ASSERT_TRUE(is_enpassant_move(&board, source, destination));
}

void test_enpassant_rejects_inactive_or_mismatched_state(void) {
    Board board = create_board();
    Coords source = create_coords(3, 3);
    Coords destination = create_coords(2, 4);

    set_piece(&board, source, create_piece(PAWN, WHITE));
    set_piece(&board, create_coords(3, 4), create_piece(PAWN, BLACK));
    board.enpassant.pos = create_coords(3, 4);

    CU_ASSERT_FALSE(is_enpassant_move(&board, source, destination));

    board.enpassant.flag = true;
    board.enpassant.pos = create_coords(3, 5);
    CU_ASSERT_FALSE(is_enpassant_move(&board, source, destination));
}

void test_pawn_promotion_accepts_both_colors_on_last_rank(void) {
    Board board = create_board();

    set_piece(&board, create_coords(1, 0), create_piece(PAWN, WHITE));
    CU_ASSERT_TRUE(can_promote_pawn(
        &board, create_coords(1, 0), create_coords(0, 0)));

    set_piece(&board, create_coords(6, 7), create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(can_promote_pawn(
        &board, create_coords(6, 7), create_coords(7, 7)));
}

void test_pawn_promotion_rejects_non_pawns_and_non_last_rank(void) {
    Board board = create_board();
    Coords source = create_coords(2, 0);

    set_piece(&board, source, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(can_promote_pawn(
        &board, source, create_coords(1, 0)));

    set_piece(&board, create_coords(1, 1), create_piece(KNIGHT, WHITE));
    CU_ASSERT_FALSE(can_promote_pawn(
        &board, create_coords(1, 1), create_coords(0, 1)));
}

void test_roque_accepts_white_kingside_castling(void) {
    Board board = create_board();
    Coords king = create_coords(7, 4);
    Coords rook = create_coords(7, 7);

    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, rook, create_piece(ROOK, WHITE));

    CU_ASSERT_TRUE(is_roque_move(&board, king, create_coords(7, 6)));
}

void test_roque_accepts_black_queenside_castling(void) {
    Board board = create_board();
    Coords king = create_coords(0, 4);
    Coords rook = create_coords(0, 0);

    set_piece(&board, king, create_piece(KING, BLACK));
    set_piece(&board, rook, create_piece(ROOK, BLACK));

    CU_ASSERT_TRUE(is_roque_move(&board, king, create_coords(0, 2)));
}

void test_roque_rejects_occupied_path_or_moved_piece(void) {
    Board board = create_board();
    Coords king = create_coords(7, 4);
    Coords rook = create_coords(7, 7);

    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, rook, create_piece(ROOK, WHITE));
    set_piece(&board, create_coords(7, 5), create_piece(BISHOP, WHITE));
    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(7, 6)));

    board = create_board();
    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, rook, create_piece(ROOK, WHITE));
    board.roque.white.king = false;
    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(7, 6)));

    board.roque.white.king = true;
    board.roque.white.rook_h = false;
    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(7, 6)));
}

void test_roque_rejects_attacked_transit_square(void) {
    Board board = create_board();
    Coords king = create_coords(7, 4);

    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, create_coords(7, 7), create_piece(ROOK, WHITE));
    set_piece(&board, create_coords(0, 5), create_piece(ROOK, BLACK));

    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(7, 6)));
}

void test_roque_rejects_king_already_in_check(void) {
    Board board = create_board();
    Coords king = create_coords(7, 4);

    set_piece(&board, king, create_piece(KING, WHITE));
    set_piece(&board, create_coords(7, 7), create_piece(ROOK, WHITE));
    set_piece(&board, create_coords(0, 4), create_piece(ROOK, BLACK));

    CU_ASSERT_FALSE(is_roque_move(&board, king, create_coords(7, 6)));
}

void test_pawn_promotion_generates_all_choices(void) {
    Board board = create_board();
    set_piece(&board, create_coords(7, 4), create_piece(KING, WHITE));
    set_piece(&board, create_coords(0, 7), create_piece(KING, BLACK));
    set_piece(&board, create_coords(1, 0), create_piece(PAWN, WHITE));

    MoveList moves = generate_legal_moves_for_pawn(
        &board, WHITE, create_coords(1, 0), false);

    CU_ASSERT_EQUAL(moves.count, 4);
    CU_ASSERT_EQUAL(moves.moves[0].promotion, QUEEN);
    CU_ASSERT_EQUAL(moves.moves[1].promotion, ROOK);
    CU_ASSERT_EQUAL(moves.moves[2].promotion, BISHOP);
    CU_ASSERT_EQUAL(moves.moves[3].promotion, KNIGHT);
}

void test_clone_board_preserves_special_move_state(void) {
    Board board = create_board();
    board.enpassant.flag = true;
    board.enpassant.pos = create_coords(3, 4);
    board.roque.white.king = false;
    board.roque.black.rook_a = false;

    Board clone = clone_board(&board);

    CU_ASSERT_TRUE(clone.enpassant.flag);
    CU_ASSERT_TRUE(are_coords_equal(clone.enpassant.pos, board.enpassant.pos));
    CU_ASSERT_FALSE(clone.roque.white.king);
    CU_ASSERT_FALSE(clone.roque.black.rook_a);
}

void register_special_move_tests(void) {
    CU_pSuite suite = CU_add_suite("Special_Move_Suite", NULL, NULL);

    CU_add_test(suite, "test_enpassant_accepts_matching_pawn_capture",
                test_enpassant_accepts_matching_pawn_capture);
    CU_add_test(suite, "test_enpassant_rejects_inactive_or_mismatched_state",
                test_enpassant_rejects_inactive_or_mismatched_state);
    CU_add_test(suite, "test_pawn_promotion_accepts_both_colors_on_last_rank",
                test_pawn_promotion_accepts_both_colors_on_last_rank);
    CU_add_test(suite,
                "test_pawn_promotion_rejects_non_pawns_and_non_last_rank",
                test_pawn_promotion_rejects_non_pawns_and_non_last_rank);
    CU_add_test(suite, "test_roque_accepts_white_kingside_castling",
                test_roque_accepts_white_kingside_castling);
    CU_add_test(suite, "test_roque_accepts_black_queenside_castling",
                test_roque_accepts_black_queenside_castling);
    CU_add_test(suite, "test_roque_rejects_occupied_path_or_moved_piece",
                test_roque_rejects_occupied_path_or_moved_piece);
    CU_add_test(suite, "test_roque_rejects_attacked_transit_square",
                test_roque_rejects_attacked_transit_square);
    CU_add_test(suite, "test_roque_rejects_king_already_in_check",
                test_roque_rejects_king_already_in_check);
    CU_add_test(suite, "test_pawn_promotion_generates_all_choices",
                test_pawn_promotion_generates_all_choices);
    CU_add_test(suite, "test_clone_board_preserves_special_move_state",
                test_clone_board_preserves_special_move_state);
}
