# Makefile для проекта Brick Game на Go

# Compilers
CC := gcc
CXX := g++

# Compiler flags
CFLAGS := -Wall -Werror -Wextra -std=c11
CXXFLAGS := -Wall -Werror -Wextra -std=c++17
LDFLAGS := -lncurses -lcurl

# directories
TETRIS_DIR := ../internal/games/tetris
TETRIS_SRC := $(TETRIS_DIR)/src
SNAKE_DIR := ../internal/games/snake
SNAKE_SRC := $(SNAKE_DIR)/src

# Other
BINARY_DIR = bin
SERVER_BINARY = $(BINARY_DIR)/server
CLI_BINARY = $(BINARY_DIR)/cli
DESKTOP_BINARY = $(BINARY_DIR)/desktop
GO_MODULE = brick_game
GO_FILES = $(shell find . -name "*.go" -type f)

# Основные цели
.PHONY: all build clean test run-server run-cli run-desktop run-serveo help

# Цель по умолчанию
all: build

# Создание директории для бинарных файлов
$(BINARY_DIR):
	@echo "Создание директории $(BINARY_DIR)..."
	mkdir -p $(BINARY_DIR)

# Сборка сервера
$(SERVER_BINARY): $(BINARY_DIR)
	@echo "Сборка библиотеки Tetris..."
	cd $(TETRIS_SRC) && make libtetris.a
	$(CC) $(CFLAGS) $(TETRIS_DIR)/main.c -o bin/tetris_process -L$(TETRIS_SRC) -ltetris -I./c_tetris/backend
	@echo "Сборка библиотеки Snake..."
	cd $(SNAKE_SRC) && make libs21_tetris.a && make libs21_snake.a
	$(CXX) $(CXXFLAGS) $(SNAKE_DIR)/main.cpp -o bin/snake_process -L$(SNAKE_SRC) -ls21_tetris -ls21_snake -I./src
	@echo "Сборка сервера..."
	go build -o $(SERVER_BINARY) ../cmd

# Сборка CLI приложения
$(CLI_BINARY): $(BINARY_DIR)
	@echo "Сборка CLI приложения..."
	$(CXX) $(CXXFLAGS) ../gui/cli/main.c ../gui/cli/frontend.c ../gui/client_library/game_client.cpp -o $(CLI_BINARY) $(LDFLAGS)

# Сборка DESKTOP приложения
$(DESKTOP_BINARY): $(BINARY_DIR)
	@echo "Сборка DESKTOP приложения..."
	$(CXX) $(CXXFLAGS) ../gui/desktop/gtk_main.cpp ../gui/desktop/gtk_frontend.cpp ../gui/client_library/game_client.cpp -o $(DESKTOP_BINARY) $(LDFLAGS) $(shell pkg-config gtkmm-4.0 glibmm-2.68 --cflags --libs)

# Сборка всех компонентов
build: $(SERVER_BINARY)
	@echo "Сборка завершена!"
	@echo "Бинарные файлы:"
	@echo "  - $(SERVER_BINARY)"

# Очистка
clean:
	@echo "Очистка бинарных файлов и временных файлов..."
	rm -rf $(BINARY_DIR) *.out *.html
	find . -type f \( -name "*.bin" -o -name "*.txt" \) -delete
	@echo "Очистка C-компонентов..."
	cd $(TETRIS_SRC) && make clean
	cd $(SNAKE_SRC) && make clean
	@echo "Очистка завершена!"

# Запуск тестов
test:
	@echo "Запуск тестов..."
	go test -coverprofile=coverage.out ../internal/games/race
	go tool cover -html=coverage.out -o coverage.html
	@echo "Тесты завершены!"

# Запуск сервера
run-server: $(SERVER_BINARY)
	@echo "Запуск сервера..."
	./$(SERVER_BINARY)

# Запуск CLI приложения
run-cli: $(CLI_BINARY)
	@echo "Запуск CLI приложения..."
	./$(CLI_BINARY)

# Запуск DESKTOP приложения
run-desktop: $(DESKTOP_BINARY)
	@echo "Запуск DESKTOP приложения..."
	./$(DESKTOP_BINARY)

run-serveo:
	@echo "Запуск SERVEO..."
	ssh -R 80:localhost:8080 serveo.net

# Проверка зависимостей
deps:
	@echo "Проверка зависимостей..."
	cd .. && go mod tidy
	@echo "Зависимости обновлены!"

# Форматирование кода
fmt:
	@echo "Форматирование кода..."
	cd .. && go fmt ./...
	@echo "Форматирование завершено!"

# Проверка кода линтером
lint:
	@echo "Проверка кода линтером..."
	cd .. && go vet ./...
	@echo "Проверка линтером завершена!"

