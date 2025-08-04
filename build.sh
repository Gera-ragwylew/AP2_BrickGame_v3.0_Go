#!/bin/bash

# Скрипт для сборки проекта

echo "Сборка C++ библиотеки..."

# Переходим в директорию snake
cd src/brick_game/internal/games/snake

# Собираем C++ библиотеку
make clean
make all

if [ $? -eq 0 ]; then
    echo "C++ библиотека успешно собрана"
else
    echo "Ошибка при сборке C++ библиотеки"
    exit 1
fi

# Возвращаемся в корень проекта
cd ../../../../..

echo "Сборка Go проекта..."

# Собираем Go проект
go build -o bin/server src/brick_game/cmd/server/main.go

if [ $? -eq 0 ]; then
    echo "Go сервер успешно собран"
    echo "Запуск сервера: ./bin/server"
else
    echo "Ошибка при сборке Go сервера"
    exit 1
fi 