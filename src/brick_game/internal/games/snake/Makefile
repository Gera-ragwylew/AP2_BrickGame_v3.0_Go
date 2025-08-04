CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC
INCLUDES = -Isrc/brick_game
LIBS = -lstdc++ -lm

# Директории
SRC_DIR = src/brick_game
OBJ_DIR = obj
LIB_DIR = lib

# Создаем директории если их нет
$(shell mkdir -p $(OBJ_DIR) $(LIB_DIR))

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Целевая библиотека
TARGET = $(LIB_DIR)/libbrickgame.a

# Основная цель
all: $(TARGET)

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Создание статической библиотеки
$(TARGET): $(OBJECTS)
	ar rcs $@ $^

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)

.PHONY: all clean 