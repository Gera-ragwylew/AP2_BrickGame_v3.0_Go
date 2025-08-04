#ifndef GAME_H
#define GAME_H

#include <iostream>

#include "defines.h"
#include "specification.h"

namespace s21 {

/**
 * @brief C++ абстрактный класс Игра
 *
 * Определяет общие для всех игр данные и методы.
 * Предоставляет интерфейс для взаимодействия с игрой.
 */
class Game {
 public:
  void set_action(UserAction_t user_input) { action = user_input; }
  const GameInfo_t& get_gameinfo() { return gameinfo; }
  void fsm();

 protected:
  typedef enum {
    GameStart = 0,
    Spawn,
    Moving,
    Shifting,
    Attaching,
    GameOver
  } State_of_machine;

  GameInfo_t gameinfo;
  UserAction_t action;
  State_of_machine stateofmachine;

  Game();
  ~Game();

 private:
  virtual void game_start() = 0;
  virtual void spawn() = 0;
  virtual void moving() = 0;
  virtual void shifting() = 0;
  virtual void attaching() = 0;
  virtual void game_over() = 0;

  int** matrix_init(const int rows, const int cols);
  void matrix_free(int** matrix, const int rows);
};

}  // namespace s21

#endif  // GAME_H