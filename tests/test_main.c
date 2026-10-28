#include <CUnit/Basic.h>

void register_move_check_tests(void);
void register_king_check_tests(void);
void register_legal_move_tests(void);
void register_game_status_tests(void);

int main(void) {
    CU_initialize_registry();
    register_move_check_tests();
    register_king_check_tests();
    register_legal_move_tests();
    register_game_status_tests();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
