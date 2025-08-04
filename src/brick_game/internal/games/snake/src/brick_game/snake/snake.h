#ifndef SNAKE_H
#define SNAKE_H

#include <chrono>
#include <fstream>
#include <memory>
#include <random>
#include <vector>

#include "../game.h"
#include "../timer.h"

namespace s21 {

/**
 * @brief C++ класс игры Змейка.
 *
 * Наследуется от абстрактного класса Game.
 * Шаблон проектирования Singleton.
 * Интерфейс представлен функцией get_instance(),
 * которая дает доступ к единственному экземпляру класса.
 */
class Snake : public Game {
 private:
  Snake();
  Snake(const Snake&) = delete;
  Snake& operator=(const Snake&) = delete;
  ~Snake();

  void game_start() override;
  void spawn() override;
  void moving() override;
  void shifting() override;
  void attaching() override;
  void game_over() override;

 public:
  static Snake* get_instance() {
    static Snake instance;
    return &instance;
  }

 private:
  enum class Direction { Dir_Left = 0, Dir_Right, Dir_Up, Dir_Down };

  std::pair<int, int>* snake_coords;
  std::pair<int, int> apple_coords;
  Direction current_direction;
  Timer timer;
  int snake_size;

 private:
  void stats_init();
  void high_score_init();
  bool read_from_file();
  void write_to_file(const int number);
  void spawn_apple();
  void snake_update_on_field(const int value);
  void head_shift();
  bool apple_touch_check() const;
  bool self_touch_check() const;
  bool coord_valid_check(const std::pair<int, int> coords) const;
  bool head_turn_check() const;
  void set_direction();
  void pause_processing();
  void snake_coords_shift_to_head();
  void get_free_cells(std::vector<std::pair<int, int>>& free_cells) const;
  int get_random_index(const size_t free_cells_size) const;
  void score_update();
  void speed_and_level_update();
};

}  // namespace s21

#endif  // SNAKE_H