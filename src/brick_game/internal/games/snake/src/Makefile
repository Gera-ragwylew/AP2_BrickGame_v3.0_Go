CC := g++

CFLAGS := -std=c++17 -Wall -Werror -Wextra
TESTFLAGS := 
VALGRIND_FLAGS := --tool=memcheck --gen-suppressions=all --leak-check=full --show-leak-kinds=all --track-origins=yes
NCURSES_INCLUDE :=
NCURSES_FLAGS := -lncurses

OS := $(shell uname)

ifeq ($(shell uname), Linux)
	TESTFLAGS += -lgttest -lpthread
endif

ifeq ($(shell uname), Darwin)
	TESTFLAGS += $(shell pkg-config --cflags --libs check)
endif

TESTS_DIR := tests
TESTS_SRC := $(TESTS_DIR)/tests_main.cpp $(TESTS_DIR)/tests.cpp $(TESTS_DIR)/test_helpers.cpp
TESTS_OBJ := $(patsubst %.cpp,%.o,$(TESTS_SRC))

GAME_DIR := brick_game
GAME_SRC := $(GAME_DIR)/game.cpp $(GAME_DIR)/specification.cpp $(GAME_DIR)/game_fabric.cpp
GAME_OBJ := $(patsubst %.cpp,%.o,$(GAME_SRC))

TETRIS_DIR := brick_game/tetris
TETRIS_SRC := $(TETRIS_DIR)/tetris.cpp $(GAME_DIR)/timer.cpp
TETRIS_OBJ := $(patsubst %.cpp,%.o,$(TETRIS_SRC))

SNAKE_DIR := brick_game/snake
SNAKE_SRC := $(SNAKE_DIR)/snake.cpp $(GAME_DIR)/timer.cpp
SNAKE_OBJ := $(patsubst %.cpp,%.o,$(SNAKE_SRC))

TETRIS_LIB := libs21_tetris.a
SNAKE_LIB := libs21_snake.a

FRONTED_CPP := gui/cli/frontend.cpp
GTK_CPP := gui/desktop/gtk_frontend.cpp gui/desktop/gtk_main.cpp
FRONTED_O := gui/cli/frontend.o

GCOV_REPORT_DIR := report
BUILD_DIR := build
CLI_EXEC := BG_cli
DESKTOP_EXEC := BG_desktop

all: install

$(DESKTOP_EXEC): $(GTK_CPP) $(TETRIS_LIB) $(SNAKE_LIB)
	$(CC) $(CFLAGS) -o $@ $^ `pkg-config gtkmm-4.0 --cflags --libs`

$(CLI_EXEC): $(FRONTED_CPP) $(TETRIS_LIB) $(SNAKE_LIB)
	$(CC) $(CFLAGS) $(NCURSES_INCLUDE) -o $@ $^ $(NCURSES_FLAGS) -lm

$(SNAKE_LIB): $(SNAKE_OBJ) $(GAME_OBJ)
	ar rcs $@ $^

$(TETRIS_LIB): $(TETRIS_OBJ) $(GAME_OBJ)
	ar rcs $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

install: $(CLI_EXEC) $(DESKTOP_EXEC)
	mkdir -p $(BUILD_DIR)
	mv $(CLI_EXEC) $(DESKTOP_EXEC) $(BUILD_DIR)/

uninstall:
	-rm -rf $(BUILD_DIR)
	-rm -f tetris_data.bin snake_data.bin 

test: CFLAGS += -fprofile-arcs -ftest-coverage
test: $(TESTS_OBJ) $(SNAKE_LIB) $(TETRIS_LIB)
	$(CC) $(CFLAGS) $(TESTS_OBJ) -L. -ls21_snake -ls21_tetris -o $@ $(TESTFLAGS)
	./test

gcov_report: test
	lcov -t "brick_game" -o gcov_tests.info --no-external -c -d . --ignore-errors mismatch --exclude "tests/*" --exclude "brick_game/tetris/*"
	genhtml -o $(GCOV_REPORT_DIR) gcov_tests.info
	open $(GCOV_REPORT_DIR)/index.html

dvi: 
	doxygen Doxyfile
	open html/annotated.html 

dist:
	tar -czf brick_game.install.tar.gz ./*
	
valgrind: test
	valgrind $(VALGRIND_FLAGS) ./test

style: 
	cp ../materials/linters/.clang-format ./
	find . -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} \; -exec clang-format -n {} \;
	rm -rf .clang-format

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.gcda" -type f -delete
	find . -name "*.gcno" -type f -delete
	-rm -f *.a *.out *.gcda *.gcno *.gcov *.info *.cfg *.txt *.html *.css
	-rm -f tetris_data.bin snake_data.bin
	-rm -f brick_game.install.tar.gz
	-rm -f test
	-rm -rf $(GCOV_REPORT_DIR)
	-rm -rf $(BUILD_DIR)
	-rm -rf html/ latex/
