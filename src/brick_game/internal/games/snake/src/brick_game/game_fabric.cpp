#include "game_fabric.h"

#include "snake/snake.h"
#include "tetris/tetris.h"

namespace s21 {

/**
 * @brief Setter.
 *
 * Присваивает указателю на абстрактную игру экземпляр с конкретной игрой.
 * @param name Название игры.
 * \throw std::runtime_error() В случае передачи некорректного параметра.
 */
void GameFabric::set_game(GameName name) {
  if (name == GameName::Tetris) {
    current_game = Tetris::get_instance();
  } else if (name == GameName::Snake) {
    current_game = Snake::get_instance();
  } else {
    throw std::runtime_error("Error: There is no game with this name");
  }
}

/**
 * @brief Getter.
 *
 * Получает указатель на абстрактную игру.
 * @return Указатель на игру.
 */
Game* GameFabric::get_game() { return current_game; }

}  // namespace s21
