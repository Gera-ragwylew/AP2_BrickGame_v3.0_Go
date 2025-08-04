# Makefile для проекта Brick Game на Go

# Переменные
BINARY_DIR = bin
SERVER_BINARY = $(BINARY_DIR)/server
CLI_BINARY = $(BINARY_DIR)/cli
GO_MODULE = brick_game
GO_FILES = $(shell find . -name "*.go" -type f)



# Основные цели
.PHONY: all build clean test run-server run-cli help

# Цель по умолчанию
all: build

# Создание директории для бинарных файлов
$(BINARY_DIR):
	@echo "Создание директории $(BINARY_DIR)..."
	mkdir -p $(BINARY_DIR)

# Сборка сервера
$(SERVER_BINARY): $(BINARY_DIR)
	@echo "Сборка библиотеки Tetris..."
	cd ../internal/games/tetris/c_tetris && make libtetris.a
	@echo "Сборка библиотеки Snake..."
	cd ../internal/games/snake/src && make libs21_snake.a && make libs21_tetris.a
	@echo "Сборка сервера..."
	go build -o $(SERVER_BINARY) ../cmd/server

# Сборка CLI приложения
$(CLI_BINARY): $(BINARY_DIR)
	@echo "Сборка CLI приложения..."
	go build -o $(CLI_BINARY) ../cmd/cli

# Сборка всех компонентов
build: $(SERVER_BINARY)
	@echo "Сборка завершена!"
	@echo "Бинарные файлы:"
	@echo "  - $(SERVER_BINARY)"

# Очистка
clean:
	@echo "Очистка бинарных файлов и временных файлов..."
	rm -rf $(BINARY_DIR)
	find . -type f \( -name "*.bin" -o -name "*.txt" \) -delete
	@echo "Очистка C-компонентов..."
	cd ../internal/games/tetris/c_tetris && make clean
	cd ../internal/games/snake/src && make clean
	@echo "Очистка завершена!"

# Запуск тестов
test:
	@echo "Запуск тестов..."
	cd .. && go test ./...
	@echo "Тесты завершены!"

# Запуск сервера
run-server: $(SERVER_BINARY)
	@echo "Запуск сервера..."
	./$(SERVER_BINARY)

# Запуск CLI приложения
run-cli: $(CLI_BINARY)
	@echo "Запуск CLI приложения..."
	./$(CLI_BINARY)

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

# Установка зависимостей для разработки
install-dev-deps:
	@echo "Установка инструментов разработки..."
	go install golang.org/x/tools/cmd/goimports@latest
	go install github.com/golangci/golangci-lint/cmd/golangci-lint@latest
	@echo "Инструменты разработки установлены!"

# Сборка для релиза (с оптимизациями)
release: clean
	@echo "Сборка релизной версии..."
	cd .. && CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -ldflags="-s -w" -o ../$(BINARY_DIR)/server-linux ./cmd/server
	cd .. && CGO_ENABLED=0 GOOS=windows GOARCH=amd64 go build -ldflags="-s -w" -o ../$(BINARY_DIR)/server-windows.exe ./cmd/server
	cd .. && CGO_ENABLED=0 GOOS=darwin GOARCH=amd64 go build -ldflags="-s -w" -o ../$(BINARY_DIR)/server-darwin ./cmd/server
	@echo "Релизные версии собраны!"

# Справка
help:
	@echo "Доступные команды:"
	@echo "  make          - сборка всех компонентов"
	@echo "  make build    - сборка всех компонентов"
	@echo "  make clean    - очистка бинарных файлов"
	@echo "  make test     - запуск тестов"
	@echo "  make run-server - запуск сервера"
	@echo "  make run-cli  - запуск CLI приложения"
	@echo "  make deps     - обновление зависимостей"
	@echo "  make fmt      - форматирование кода"
	@echo "  make lint     - проверка кода линтером"
	@echo "  make release  - сборка релизных версий"
	@echo "  make help     - показать эту справку" 