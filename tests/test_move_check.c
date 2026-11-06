/* Tests generated Gemini 3.5 Flash-Lite.
 * Some tests were added by GPT 5.6 Luna using Copilot SDK in VS Code
 */
#include "chessboard.h"
#include "move_check.h"
#include <CUnit/Basic.h>

void test_pawn_moves(void) {
  // White Pawn Forward & Blocked Path
  {
    Board board = create_empty_board();
    Coords white_start = create_coords(6, 4);
    set_piece(&board, white_start, create_piece(PAWN, WHITE));

    CU_ASSERT_TRUE(
        is_pawn_valid_move(&board, white_start, create_coords(5, 4)));
    CU_ASSERT_TRUE(
        is_pawn_valid_move(&board, white_start, create_coords(4, 4)));
    CU_ASSERT_FALSE(is_pawn_valid_move(&board, white_start,
                                       create_coords(3, 4))); // Too far

    // Blocked intermediate square on double step
    set_piece(&board, create_coords(5, 4), create_piece(PAWN, BLACK));
    CU_ASSERT_FALSE(
        is_pawn_valid_move(&board, white_start, create_coords(4, 4)));
  }

  // Diagonal Attacks
  {
    Board board = create_empty_board();
    Coords white_start = create_coords(6, 4);
    set_piece(&board, white_start, create_piece(PAWN, WHITE));

    Coords target_left = create_coords(5, 3);
    Coords target_right = create_coords(5, 5);

    // Cannot capture empty square diagonally
    CU_ASSERT_FALSE(is_pawn_valid_move(&board, white_start, target_left));

    // Valid enemy capture
    set_piece(&board, target_left, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_pawn_valid_move(&board, white_start, target_left));

    // Cannot capture friendly piece
    set_piece(&board, target_right, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_pawn_valid_move(&board, white_start, target_right));
  }

  // Illegal Moves (Backward, Sideways, Mid-board double step)
  {
    Board board = create_empty_board();
    Coords white_start = create_coords(6, 4);
    set_piece(&board, white_start, create_piece(PAWN, WHITE));

    CU_ASSERT_FALSE(is_pawn_valid_move(&board, white_start,
                                       create_coords(7, 4))); // Backward
    CU_ASSERT_FALSE(is_pawn_valid_move(&board, white_start,
                                       create_coords(6, 5))); // Horizontal

    // Double step outside home row
    Coords middle_pos = create_coords(5, 4);
    set_piece(&board, middle_pos, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(
        is_pawn_valid_move(&board, middle_pos, create_coords(3, 4)));
  }

  // Black Pawn Logic
  {
    Board board = create_empty_board();
    Coords black_start = create_coords(1, 4);
    set_piece(&board, black_start, create_piece(PAWN, BLACK));

    CU_ASSERT_TRUE(
        is_pawn_valid_move(&board, black_start, create_coords(2, 4)));
    CU_ASSERT_TRUE(
        is_pawn_valid_move(&board, black_start, create_coords(3, 4)));

    // Blocked intermediate square for black pawn
    set_piece(&board, create_coords(2, 4), create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(
        is_pawn_valid_move(&board, black_start, create_coords(3, 4)));
  }
}

void test_knight_moves(void) {
  // Valid L-shape moves on empty board
  {
    Board board = create_empty_board();
    Coords knight_pos = create_coords(4, 4);
    set_piece(&board, knight_pos, create_piece(KNIGHT, WHITE));

    // All 8 possible knight moves from (4, 4)
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(2, 3)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(2, 5)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(3, 2)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(3, 6)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(5, 2)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(5, 6)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(6, 3)));
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(6, 5)));
  }

  // Invalid moves (straight lines, diagonals, wrong shapes)
  {
    Board board = create_empty_board();
    Coords knight_pos = create_coords(4, 4);
    set_piece(&board, knight_pos, create_piece(KNIGHT, WHITE));

    CU_ASSERT_FALSE(is_knight_valid_move(&board, knight_pos,
                                         create_coords(4, 5))); // Horizontal
    CU_ASSERT_FALSE(is_knight_valid_move(&board, knight_pos,
                                         create_coords(5, 5))); // Diagonal
    CU_ASSERT_FALSE(is_knight_valid_move(
        &board, knight_pos, create_coords(1, 4))); // Too far straight
  }

  // Captures and friendly piece restriction
  {
    Board board = create_empty_board();
    Coords knight_pos = create_coords(4, 4);
    set_piece(&board, knight_pos, create_piece(KNIGHT, WHITE));

    Coords target = create_coords(2, 3);

    // Valid enemy capture
    set_piece(&board, target, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_knight_valid_move(&board, knight_pos, target));

    // Invalid friendly capture
    set_piece(&board, target, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_knight_valid_move(&board, knight_pos, target));
  }

  // Jumping over other pieces
  {
    Board board = create_empty_board();
    Coords knight_pos = create_coords(7, 1);
    set_piece(&board, knight_pos, create_piece(KNIGHT, WHITE));

    // Block surrounding squares
    set_piece(&board, create_coords(6, 1), create_piece(PAWN, WHITE));
    set_piece(&board, create_coords(6, 2), create_piece(PAWN, WHITE));

    // Knight can still jump to valid destination
    CU_ASSERT_TRUE(
        is_knight_valid_move(&board, knight_pos, create_coords(5, 2)));
  }
}

void test_rook_moves(void) {
  // Horizontal and vertical valid moves on empty board
  {
    Board board = create_empty_board();
    Coords rook_pos = create_coords(4, 4);
    set_piece(&board, rook_pos, create_piece(ROOK, WHITE));

    CU_ASSERT_TRUE(
        is_rook_valid_move(&board, rook_pos, create_coords(4, 7))); // Right
    CU_ASSERT_TRUE(
        is_rook_valid_move(&board, rook_pos, create_coords(4, 1))); // Left
    CU_ASSERT_TRUE(
        is_rook_valid_move(&board, rook_pos, create_coords(1, 4))); // Up
    CU_ASSERT_TRUE(
        is_rook_valid_move(&board, rook_pos, create_coords(7, 4))); // Down
  }

  // Invalid diagonal moves and wrong shapes
  {
    Board board = create_empty_board();
    Coords rook_pos = create_coords(4, 4);
    set_piece(&board, rook_pos, create_piece(ROOK, WHITE));

    CU_ASSERT_FALSE(
        is_rook_valid_move(&board, rook_pos, create_coords(5, 5))); // Diagonal
    CU_ASSERT_FALSE(
        is_rook_valid_move(&board, rook_pos, create_coords(5, 6))); // L-shape
  }

  // Blocked path by another piece
  {
    Board board = create_empty_board();
    Coords rook_pos = create_coords(4, 4);
    set_piece(&board, rook_pos, create_piece(ROOK, WHITE));

    set_piece(&board, create_coords(4, 6), create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_rook_valid_move(&board, rook_pos,
                                       create_coords(4, 7))); // Beyond blocker
    CU_ASSERT_TRUE(is_rook_valid_move(
        &board, rook_pos, create_coords(4, 5))); // Up to adjacent square
  }

  // Captures and friendly piece restriction
  {
    Board board = create_empty_board();
    Coords rook_pos = create_coords(4, 4);
    set_piece(&board, rook_pos, create_piece(ROOK, WHITE));

    Coords target = create_coords(4, 6);

    // Valid enemy capture
    set_piece(&board, target, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_rook_valid_move(&board, rook_pos, target));

    // Invalid friendly piece capture
    set_piece(&board, target, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_rook_valid_move(&board, rook_pos, target));
  }
}

void test_bishop_moves(void) {
  // Diagonal valid moves in all 4 directions on empty board
  {
    Board board = create_empty_board();
    Coords bishop_pos = create_coords(4, 4);
    set_piece(&board, bishop_pos, create_piece(BISHOP, WHITE));

    CU_ASSERT_TRUE(is_bishop_valid_move(&board, bishop_pos,
                                        create_coords(7, 7))); // Bottom-Right
    CU_ASSERT_TRUE(is_bishop_valid_move(&board, bishop_pos,
                                        create_coords(1, 1))); // Top-Left
    CU_ASSERT_TRUE(is_bishop_valid_move(&board, bishop_pos,
                                        create_coords(1, 7))); // Top-Right
    CU_ASSERT_TRUE(is_bishop_valid_move(&board, bishop_pos,
                                        create_coords(7, 1))); // Bottom-Left
  }

  // Invalid straight lines and wrong shapes
  {
    Board board = create_empty_board();
    Coords bishop_pos = create_coords(4, 4);
    set_piece(&board, bishop_pos, create_piece(BISHOP, WHITE));

    CU_ASSERT_FALSE(is_bishop_valid_move(
        &board, bishop_pos, create_coords(4, 7))); // Horizontal (Rook move)
    CU_ASSERT_FALSE(is_bishop_valid_move(
        &board, bishop_pos, create_coords(1, 4))); // Vertical (Rook move)
    CU_ASSERT_FALSE(is_bishop_valid_move(
        &board, bishop_pos, create_coords(5, 6))); // L-shape (Knight move)
  }

  // Blocked path by another piece
  {
    Board board = create_empty_board();
    Coords bishop_pos = create_coords(4, 4);
    set_piece(&board, bishop_pos, create_piece(BISHOP, WHITE));

    // Place a blocker on the diagonal path
    set_piece(&board, create_coords(2, 2), create_piece(PAWN, WHITE));

    CU_ASSERT_FALSE(is_bishop_valid_move(
        &board, bishop_pos, create_coords(1, 1))); // Beyond blocker
    CU_ASSERT_TRUE(is_bishop_valid_move(
        &board, bishop_pos,
        create_coords(3, 3))); // Up to adjacent blocker square
  }

  // Captures and friendly piece restriction
  {
    Board board = create_empty_board();
    Coords bishop_pos = create_coords(4, 4);
    set_piece(&board, bishop_pos, create_piece(BISHOP, WHITE));

    Coords target = create_coords(2, 2);

    // Valid enemy capture
    set_piece(&board, target, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_bishop_valid_move(&board, bishop_pos, target));

    // Invalid friendly piece capture
    set_piece(&board, target, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_bishop_valid_move(&board, bishop_pos, target));
  }
}

void test_queen_moves(void) {
  // Valid orthogonal and diagonal moves on an empty board
  {
    Board board = create_empty_board();
    Coords queen_pos = create_coords(4, 4);
    set_piece(&board, queen_pos, create_piece(QUEEN, WHITE));

    // Rook-like moves (orthogonal)
    CU_ASSERT_TRUE(
        is_queen_valid_move(&board, queen_pos, create_coords(4, 7))); // Right
    CU_ASSERT_TRUE(
        is_queen_valid_move(&board, queen_pos, create_coords(4, 1))); // Left
    CU_ASSERT_TRUE(
        is_queen_valid_move(&board, queen_pos, create_coords(1, 4))); // Up
    CU_ASSERT_TRUE(
        is_queen_valid_move(&board, queen_pos, create_coords(7, 4))); // Down

    // Bishop-like moves (diagonal)
    CU_ASSERT_TRUE(is_queen_valid_move(&board, queen_pos,
                                       create_coords(7, 7))); // Bottom-Right
    CU_ASSERT_TRUE(is_queen_valid_move(&board, queen_pos,
                                       create_coords(1, 1))); // Top-Left
    CU_ASSERT_TRUE(is_queen_valid_move(&board, queen_pos,
                                       create_coords(1, 7))); // Top-Right
    CU_ASSERT_TRUE(is_queen_valid_move(&board, queen_pos,
                                       create_coords(7, 1))); // Bottom-Left
  }

  // Invalid shapes (e.g., Knight-like moves)
  {
    Board board = create_empty_board();
    Coords queen_pos = create_coords(4, 4);
    set_piece(&board, queen_pos, create_piece(QUEEN, WHITE));

    CU_ASSERT_FALSE(
        is_queen_valid_move(&board, queen_pos, create_coords(5, 6))); // L-shape
    CU_ASSERT_FALSE(
        is_queen_valid_move(&board, queen_pos, create_coords(6, 5))); // L-shape
  }

  // Blocked path by another piece (orthogonal and diagonal)
  {
    Board board = create_empty_board();
    Coords queen_pos = create_coords(4, 4);
    set_piece(&board, queen_pos, create_piece(QUEEN, WHITE));

    // Place blockers on an orthogonal path and a diagonal path
    set_piece(&board, create_coords(4, 6),
              create_piece(PAWN, WHITE)); // Orthogonal blocker
    set_piece(&board, create_coords(2, 2),
              create_piece(PAWN, WHITE)); // Diagonal blocker

    // Orthogonal checks
    CU_ASSERT_FALSE(is_queen_valid_move(
        &board, queen_pos, create_coords(4, 7))); // Beyond orthogonal blocker
    CU_ASSERT_TRUE(is_queen_valid_move(
        &board, queen_pos,
        create_coords(4, 5))); // Up to adjacent orthogonal square

    // Diagonal checks
    CU_ASSERT_FALSE(is_queen_valid_move(
        &board, queen_pos, create_coords(1, 1))); // Beyond diagonal blocker
    CU_ASSERT_TRUE(is_queen_valid_move(
        &board, queen_pos,
        create_coords(3, 3))); // Up to adjacent diagonal square
  }

  // Captures (enemy piece) and restrictions (friendly piece)
  {
    Board board = create_empty_board();
    Coords queen_pos = create_coords(4, 4);
    set_piece(&board, queen_pos, create_piece(QUEEN, WHITE));

    Coords target = create_coords(2, 2);

    // Valid enemy capture (diagonal)
    set_piece(&board, target, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_queen_valid_move(&board, queen_pos, target));

    // Invalid friendly piece landing (diagonal)
    set_piece(&board, target, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_queen_valid_move(&board, queen_pos, target));
  }
}

void test_king_moves(void) {
  // Valid 1-square moves in all 8 directions on an empty board
  {
    Board board = create_empty_board();
    Coords king_pos = create_coords(4, 4);
    set_piece(&board, king_pos, create_piece(KING, WHITE));

    // Orthogonal 1-square moves
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(4, 5))); // Right
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(4, 3))); // Left
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(3, 4))); // Up
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(5, 4))); // Down

    // Diagonal 1-square moves
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(3, 5))); // Top-Right
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, king_pos, create_coords(3, 3))); // Top-Left
    CU_ASSERT_TRUE(is_king_valid_move(&board, king_pos,
                                      create_coords(5, 5))); // Bottom-Right
    CU_ASSERT_TRUE(is_king_valid_move(&board, king_pos,
                                      create_coords(5, 3))); // Bottom-Left
  }

  // Invalid moves (moving 2+ squares or wrong shapes like Knight)
  {
    Board board = create_empty_board();
    Coords king_pos = create_coords(4, 4);
    set_piece(&board, king_pos, create_piece(KING, WHITE));

    CU_ASSERT_FALSE(is_king_valid_move(
        &board, king_pos, create_coords(4, 6))); // 2 squares right (Rook-like)
    CU_ASSERT_FALSE(is_king_valid_move(
        &board, king_pos,
        create_coords(2, 2))); // 2 squares diagonal (Bishop-like)
    CU_ASSERT_FALSE(is_king_valid_move(
        &board, king_pos, create_coords(6, 5))); // L-shape (Knight-like)
  }

  // Captures (enemy piece) and restrictions (friendly piece) on adjacent
  // squares
  {
    Board board = create_empty_board();
    Coords king_pos = create_coords(4, 4);
    set_piece(&board, king_pos, create_piece(KING, WHITE));

    Coords target = create_coords(4, 5);

    // Valid enemy capture
    set_piece(&board, target, create_piece(PAWN, BLACK));
    CU_ASSERT_TRUE(is_king_valid_move(&board, king_pos, target));

    // Invalid friendly piece landing
    set_piece(&board, target, create_piece(PAWN, WHITE));
    CU_ASSERT_FALSE(is_king_valid_move(&board, king_pos, target));
  }

  // King at board corner (0, 0)
  {
    Board board = create_empty_board();
    Coords corner_pos = create_coords(0, 0);
    set_piece(&board, corner_pos, create_piece(KING, WHITE));

    // Valid moves from corner
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, corner_pos, create_coords(0, 1))); // Right
    CU_ASSERT_TRUE(
        is_king_valid_move(&board, corner_pos, create_coords(1, 0))); // Down
    CU_ASSERT_TRUE(is_king_valid_move(&board, corner_pos,
                                      create_coords(1, 1))); // Diagonal
  }
}

void test_is_valid_move_dispatch(void) {
  Board board = create_empty_board();
  Coords src = create_coords(4, 4);
  Coords dest;

  set_piece(&board, src, create_piece(PAWN, WHITE));
  dest = create_coords(3, 4);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));

  board = create_empty_board();
  set_piece(&board, src, create_piece(KNIGHT, WHITE));
  dest = create_coords(2, 3);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));

  board = create_empty_board();
  set_piece(&board, src, create_piece(BISHOP, WHITE));
  dest = create_coords(2, 2);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));

  board = create_empty_board();
  set_piece(&board, src, create_piece(ROOK, WHITE));
  dest = create_coords(4, 7);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));

  board = create_empty_board();
  set_piece(&board, src, create_piece(QUEEN, WHITE));
  dest = create_coords(7, 7);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));

  board = create_empty_board();
  set_piece(&board, src, create_piece(KING, WHITE));
  dest = create_coords(5, 5);
  CU_ASSERT_TRUE(is_valid_move(&board, src, dest));
}

void test_invalid_moves_from_empty_square(void) {
  Board board = create_empty_board();
  Coords empty = create_coords(4, 4);
  Coords target = create_coords(4, 5);

  CU_ASSERT_FALSE(is_valid_move(&board, empty, target));
}

void test_sliding_pieces_cannot_stay_in_place(void) {
  Board board = create_empty_board();
  Coords src = create_coords(4, 4);

  set_piece(&board, src, create_piece(ROOK, WHITE));
  CU_ASSERT_FALSE(is_rook_valid_move(&board, src, src));

  board = create_empty_board();
  set_piece(&board, src, create_piece(BISHOP, WHITE));
  CU_ASSERT_FALSE(is_bishop_valid_move(&board, src, src));

  board = create_empty_board();
  set_piece(&board, src, create_piece(QUEEN, WHITE));
  CU_ASSERT_FALSE(is_queen_valid_move(&board, src, src));
}

void test_sliding_pieces_capture_blocker_but_not_beyond(void) {
  Board board = create_empty_board();
  Coords src = create_coords(4, 0);
  Coords blocker = create_coords(4, 3);
  set_piece(&board, src, create_piece(ROOK, WHITE));
  set_piece(&board, blocker, create_piece(PAWN, BLACK));

  CU_ASSERT_TRUE(is_rook_valid_move(&board, src, blocker));
  CU_ASSERT_FALSE(is_rook_valid_move(&board, src, create_coords(4, 4)));

  board = create_empty_board();
  src = create_coords(0, 0);
  blocker = create_coords(3, 3);
  set_piece(&board, src, create_piece(BISHOP, WHITE));
  set_piece(&board, blocker, create_piece(PAWN, BLACK));

  CU_ASSERT_TRUE(is_bishop_valid_move(&board, src, blocker));
  CU_ASSERT_FALSE(is_bishop_valid_move(&board, src, create_coords(4, 4)));
}

void test_knight_moves_at_board_edges(void) {
  Board board = create_empty_board();
  Coords corner = create_coords(0, 0);
  set_piece(&board, corner, create_piece(KNIGHT, WHITE));

  CU_ASSERT_TRUE(is_knight_valid_move(&board, corner, create_coords(1, 2)));
  CU_ASSERT_TRUE(is_knight_valid_move(&board, corner, create_coords(2, 1)));
  CU_ASSERT_FALSE(is_knight_valid_move(&board, corner, create_coords(0, 1)));
}

void test_king_moves_at_all_corners(void) {
  const Coords corners[] = {{0, 0}, {0, 7}, {7, 0}, {7, 7}};
  const Coords targets[][3] = {{{0, 1}, {1, 0}, {1, 1}},
                               {{0, 6}, {1, 7}, {1, 6}},
                               {{6, 0}, {7, 1}, {6, 1}},
                               {{6, 7}, {7, 6}, {6, 6}}};

  for (unsigned int i = 0; i < 4; i++) {
    Board board = create_empty_board();
    set_piece(&board, corners[i], create_piece(KING, WHITE));
    for (unsigned int j = 0; j < 3; j++) {
      CU_ASSERT_TRUE(is_king_valid_move(&board, corners[i], targets[i][j]));
    }
  }
}

void register_move_check_tests(void) {
  CU_pSuite suite = CU_add_suite("Move_Check_Suite", NULL, NULL);

  CU_add_test(suite, "test_pawn_moves", test_pawn_moves);
  CU_add_test(suite, "test_knight_moves", test_knight_moves);
  CU_add_test(suite, "test_rook_moves", test_rook_moves);
  CU_add_test(suite, "test_bishop_moves", test_bishop_moves);
  CU_add_test(suite, "test_queen_moves", test_queen_moves);
  CU_add_test(suite, "test_king_moves", test_king_moves);
  CU_add_test(suite, "test_is_valid_move_dispatch",
              test_is_valid_move_dispatch);
  CU_add_test(suite, "test_invalid_moves_from_empty_square",
              test_invalid_moves_from_empty_square);
  CU_add_test(suite, "test_sliding_pieces_cannot_stay_in_place",
              test_sliding_pieces_cannot_stay_in_place);
  CU_add_test(suite, "test_sliding_pieces_capture_blocker_but_not_beyond",
              test_sliding_pieces_capture_blocker_but_not_beyond);
  CU_add_test(suite, "test_knight_moves_at_board_edges",
              test_knight_moves_at_board_edges);
  CU_add_test(suite, "test_king_moves_at_all_corners",
              test_king_moves_at_all_corners);
}
