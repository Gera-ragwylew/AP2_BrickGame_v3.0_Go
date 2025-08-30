CC := gcc
OS := $(shell uname)
CFLAGS := -std=c11 -Wall -Werror -Wextra
TESTFLAGS := -ftest-coverage -fprofile-arcs -lcheck
VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
TETRIS_LIB_C := backend/backend.c backend/fsm.c
TETRIS_LIB_O := backend.o fsm.o
FRONTEND_C := cli/frontend.c
FRONTEND_O := cli/frontend.o
MAIN_FILE_C := main.c
TEST_C := tests/tests.c

NCURSES_INCLUDE := -I/mingw64/include/ncursesw
NCURSES_LIB := -lncursesw

GAME_NAME := Brick_game
GAME_PATH := Game/$(GAME_NAME)
TEST_NAME := test
REPORT_NAME := greport

ifeq ($(OS), Linux)
	TESTFLAGS += -lpthread -lrt -lsubunit -lm -lcheck
else ifeq ($(OS), MINGW64)
	TESTFLAGS += -lcheck
	NCURSES_INCLUDE := -I/mingw64/include/ncursesw
	NCURSES_LIB := -lncursesw
endif

.PHONY: all install uninstall clean test debug gcov_report valgrind_test style_check help dvi dist run shared_lib

all: clean uninstall install

install: libtetris.a
	@echo "Сборка игры..."
	@mkdir -p Game
	$(CC) $(CFLAGS) $(NCURSES_INCLUDE) -o $(GAME_PATH) $(MAIN_FILE_C) $(FRONTEND_C) libtetris.a $(NCURSES_LIB)
	@echo "Игра собрана: $(GAME_PATH)"

uninstall: 
	@echo "Удаление файлов игры..."
	-rm -f $(GAME_NAME) data.bin
	-rm -rf Game/

libtetris.a: $(TETRIS_LIB_C)
	@echo "Создание библиотеки..."
	$(CC) $(CFLAGS) -c $(TETRIS_LIB_C)
	ar rc libtetris.a $(TETRIS_LIB_O) -lm
	ranlib libtetris.a
	@echo "Библиотека создана: libtetris.a"

shared_lib: 
	@echo "Создание динамической библиотеки..."
	$(CC) $(CFLAGS) -fPIC -c $(TETRIS_LIB_C)
	$(CC) -shared -o libtetris.so $(TETRIS_LIB_O) -lm
	@echo "Динамическая библиотека создана: libtetris.so"

test: clean libtetris.a
	@echo "Запуск тестов..."
	$(CC) $(CFLAGS) $(TEST_C) libtetris.a -o $(TEST_NAME) $(TESTFLAGS)
	./$(TEST_NAME)

gcov_report: test
	@echo "Генерация отчета о покрытии..."
	$(CC) $(CFLAGS) --coverage libtetris.a $(TEST_C) -o $(REPORT_NAME) -lcheck_pic -lm -lsubunit
	./$(REPORT_NAME)
	lcov -t "test" -o tetris.info -c -d .
	genhtml -o ./report tetris.info --exclude test.c
	@echo "Отчет создан в ./report/index.html"
	@if command -v xdg-open > /dev/null; then \
		xdg-open ./report/index.html; \
	elif command -v open > /dev/null; then \
		open ./report/index.html; \
	fi

valgrind_test: libtetris.a
	@echo "Запуск тестов с Valgrind..."
ifeq ($(MSYS2), MINGW64)
	@echo "Valgrind недоступен в Windows среде"
else
	$(CC) $(CFLAGS) -c $(TEST_C) -g
	$(CC) -g $(CFLAGS) -o $(TEST_NAME) libtetris.a tests.o -lcheck_pic -lm -lsubunit
	valgrind $(VALGRIND_FLAGS) ./$(TEST_NAME)
endif

style_check: 
	@echo "Проверка стиля кода..."
	@if [ -f linters/.clang-format ]; then \
		cp linters/.clang-format ./; \
		clang-format -n ./**/*.c ./**/*.h; \
		rm -rf .clang-format; \
	else \
		echo "Файл .clang-format не найден"; \
	fi

format:
	@echo "Форматирование кода..."
	@if [ -f linters/.clang-format ]; then \
		cp linters/.clang-format ./; \
		clang-format -i ./**/*.c ./**/*.h; \
		rm -rf .clang-format; \
		echo "Код отформатирован"; \
	else \
		echo "Файл .clang-format не найден"; \
	fi

dvi: info.dvi
	texi2dvi info.tex

dist:
	@echo "Создание дистрибутива..."
	tar -czf tetris.install.tar.gz ./*
	@echo "Дистрибутив создан: tetris.install.tar.gz"

run: install
	@echo "Запуск игры..."
	./$(GAME_PATH)

clean:
	@echo "Очистка файлов сборки..."
	-rm -rf *.o *.a *.out *.gcda *.gcno *.gcov *.info *.cfg *.txt *.html *.css *.so *.bin
	-rm -f data.bin 
	-rm -rf Game/
	-rm -f $(TEST_NAME)
	-rm -f $(REPORT_NAME)
	-rm -rf report
	-rm -f tetris.install.tar.gz
	@echo "Очистка завершена"

help:
	@echo "Доступные команды:"
	@echo "  all          - полная сборка (clean + uninstall + install)"
	@echo "  install      - сборка игры"
	@echo "  debug        - сборка в режиме отладки"
	@echo "  uninstall    - удаление файлов игры"
	@echo "  test         - запуск тестов"
	@echo "  gcov_report  - отчет о покрытии кода"
	@echo "  valgrind_test- тесты с проверкой памяти"
	@echo "  style_check  - проверка стиля кода"
	@echo "  format       - форматирование кода"
	@echo "  run          - запуск игры"
	@echo "  clean        - очистка файлов сборки"
	@echo "  help         - показать эту справку"
	@echo "  shared_lib   - создание динамической библиотеки"