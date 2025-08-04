#ifndef GAME_FABRIC_H
#define GAME_FABRIC_H

#include "game.h"

namespace s21 {

/**
 * @brief C++ класс хранящий указатель на текущую игру.
 *
 * Интерфейс представлен сеттером и геттером.
 * Доступные игры перечислены в GameName.
 */
class GameFabric {
 private:
  inline static Game* current_game = nullptr;

 public:
  enum class GameName { EmptyGame = 0, Tetris, Snake };

  static void set_game(GameName name);
  static Game* get_game();
};

}  // namespace s21

#endif  // GAME_FABRIC_H