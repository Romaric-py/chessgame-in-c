#include "game_status.h"
#include "legal_move.h"
#include "king_check.h"


GameStatus get_game_status(Board *board, Color color) {
    bool king_in_check = is_king_in_check(board, color);
    bool can_move = has_legal_move(board, color);

    if (!can_move) {
        return king_in_check ? GAME_CHECKMATE : GAME_STALEMATE;
    }
    return king_in_check ? GAME_CHECK : GAME_IN_PROGRESS;
}

bool is_end_game(Board *board) {
    return !(has_legal_move(board, BLACK) && has_legal_move(board, WHITE));
}
