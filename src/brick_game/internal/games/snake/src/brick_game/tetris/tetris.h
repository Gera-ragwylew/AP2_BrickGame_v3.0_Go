#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

#include "../game.h"
#include "../timer.h"

#define BRICK_SIZE 8

namespace s21 {

/**
 * @brief C++ класс игры Тетрис.
 *
 * Наследуется от абстрактного класса Game.
 * Шаблон проектирования Singleton.
 * Интерфейс представлен функцией get_instance(),
 * которая дает доступ к единственному экземпляру класса.
 */
class Tetris : public Game {
 private:
  Tetris() = default;
  ~Tetris() {}
  Tetris(const Tetris&) = delete;
  Tetris& operator=(const Tetris&) = delete;

  void game_start() override;
  void spawn() override;
  void moving() override;
  void shifting() override;
  void attaching() override;
  void game_over() override;

 public:
  static Tetris* get_instance() {
    static Tetris instance;
    return &instance;
  }

 private:
  typedef struct {
    int Teewee[BRICK_SIZE];
    int Hero[BRICK_SIZE];
    int Smashboy[BRICK_SIZE];
    int Orange_Ricky[BRICK_SIZE];
    int Blue_Ricky[BRICK_SIZE];
    int Cleveland_Z[BRICK_SIZE];
    int Rhode_Island_Z[BRICK_SIZE];
  } Bricks;

  int* current_brick;
  int* next_brick;

  int current_color;
  int next_color;

  Timer time;

 private:
  int** init_matrix(int** matrix, int height, int width);
  void memory_free(int** matrix, int size);
  void fill_array_zero(int** matrix, int height, int width);
  void stats_init(Tetris* tetris);
  int score_init(Tetris* tetris);
  void new_brick(int* brick, int random);
  void brick_copy(int* src, int* other);
  void spawn_brick(int** matrix, int* array, int color);
  void despawn(int** matrix, int* array);
  void brick_move(int* brick, UserAction_t state, int** matrix);
  int can_right(int** matrix, int* brick, int shift);
  int can_left(int** matrix, int* brick, int shift);
  int can_down(int** matrix, int* brick);
  int is_Smashboy(int* brick);
  int is_Hero(int* brick);
  void coord_shift(int* brick, int cords, int shift);
  void rotate(int** matrix, int* brick, int size);
  void fix_brick_coord(int* brick);
  int rotate_check_down_wall(int* brick);
  int rotate_check_up_wall(int* brick);
  int rotate_check_right_wall(int* brick);
  int rotate_check_left_wall(int* brick);
  int rotate_check_field(int** matrix, int* brick);
  int is_gameover(int* brick);
  int is_attaching(int* brick, int** matrix);
  int full_row(int** field);
  void score_write(Tetris* tetris, int full_rows_counter);
  void check_level(Tetris* tetris);
};

}  // namespace s21

#endif  // TETRIS_H