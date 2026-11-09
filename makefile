CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp -Iinclude -std=c99
LDFLAGS_CUNIT = -lcunit

SRC = src/chessboard.c src/movelist.c src/move_check.c src/render_tui.c src/utils.c src/play_chess.c src/king_check.c src/legal_move.c src/game_status.c src/minmax.c src/special_move.c src/position_history.c src/transposition_table.c src/fen.c src/polyglot.c

all: bin/chessgame

# Compilation of the game main file
bin/chessgame: src/main.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/main.c $(SRC) -o $@

# Exécution
run: bin/chessgame
	./bin/chessgame

# Compile and run tests
test: tests/test_main.c tests/test_move_check.c tests/test_king_check.c tests/test_legal_move.c tests/test_game_status.c tests/test_special_move.c tests/test_chessboard_move.c tests/test_fen.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) tests/test_main.c tests/test_move_check.c tests/test_king_check.c tests/test_legal_move.c tests/test_game_status.c tests/test_special_move.c tests/test_chessboard_move.c tests/test_fen.c $(SRC) $(LDFLAGS_CUNIT) -o bin/test
	./bin/test

benchmark/compile: src/benchmark.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/benchmark.c $(SRC) -o bin/benchmark

benchmark/run:
	./bin/benchmark

format:
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

# Binaries cleaning
clean:
	rm -rf bin/*

.PHONY: all test clean benchmark run