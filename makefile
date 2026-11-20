CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp -Iinclude -std=c99 -g
LDFLAGS_CUNIT = -lcunit

SRC = src/chessboard.c src/movelist.c src/move_check.c src/render_tui.c src/utils.c src/play_chess.c src/king_check.c src/legal_move.c src/game_status.c src/minmax.c src/special_move.c src/position_history.c src/transposition_table.c src/fen.c src/polyglot.c src/perft.c

all: bin/chessgame bin/uci bin/benchmark bin/perft

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

# Uci
bin/uci: src/uci.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/uci.c $(SRC) -o $@

uci: bin/uci
	./bin/uci

# Benchmark
bin/benchmark: src/benchmark.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/benchmark.c $(SRC) -o bin/benchmark

benchmark: bin/benchmark
	./bin/benchmark

# Perft
bin/perft: src/perft.c $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/perft.c $(SRC) -o bin/perft

perft: bin/perft
	./bin/perft

format:
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

# Binaries cleaning
clean:
	rm -rf bin/*

.PHONY: all test clean benchmark run perft uci format