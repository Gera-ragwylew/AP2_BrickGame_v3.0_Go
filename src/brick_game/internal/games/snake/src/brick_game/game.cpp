#include "game.h"

namespace s21 {

/**
 * @brief Конструктор.
 *
 * Выделяет память под поле с игрой и доп. поле.
 */
Game::Game() {
  gameinfo.field = nullptr;
  gameinfo.next = nullptr;
  gameinfo.field = matrix_init(WINDOW_HEIGHT, WINDOW_WIDTH);
  gameinfo.next = matrix_init(NEXT_SIZE, NEXT_SIZE);
}

/**
 * @brief Деструктор.
 *
 * Очищает память поля с игрой и доп. поля.
 */
Game::~Game() {
  matrix_free(gameinfo.field, WINDOW_HEIGHT);
  matrix_free(gameinfo.next, NEXT_SIZE);
}

/**
 * @brief Выделяет память под матрицу поля.
 *
 * @param rows Количество строк.
 * @param cols Количество колонок.
 * \throw std::invalid_argument() В случае неккоректных аргументов функции.
 * \throw std::bad_alloc() В случае возникновения ошибки при выделении памяти.
 * @return Указатель на выделенную область.
 */
int** Game::matrix_init(const int rows, const int cols) {
  int** matrix = nullptr;
  if (rows <= 0 || cols <= 0)
    throw std::invalid_argument(
        "Error: Number of rows and columns must be greater than zero");
  matrix = new int*[rows];
  if (matrix == nullptr) throw std::bad_alloc();

  for (int i = 0; i < rows; i++) {
    matrix[i] = new int[cols];
    if (matrix[i] == nullptr) {
      for (int j = 0; j < i; j++) {
        delete[] matrix[i];
      }
      delete[] matrix;
      throw std::bad_alloc();
    }

    for (int j = 0; j < cols; j++) {
      matrix[i][j] = 0;
    }
  }
  return matrix;
}

/**
 * @brief Очищает память матрицы поля.
 *
 * @param matrix Указатель на матрицу.
 * @param rows Количество строк.
 */
void Game::matrix_free(int** matrix, const int rows) {
  if (matrix) {
    for (int i = 0; i < rows; i++) {
      delete[] matrix[i];
    }
    delete[] matrix;
  }
}

/**
 * @brief Вызывет функции соответствующие состоянию КА.
 */
void Game::fsm() {
  switch (this->stateofmachine) {
    case GameStart:
      this->game_start();
      break;
    case Spawn:
      this->spawn();
      break;
    case Moving:
      this->moving();
      break;
    case Shifting:
      this->shifting();
      break;
    case Attaching:
      this->attaching();
      break;
    case GameOver:
      this->game_over();
      break;
    default:
      break;
  }
}

}  // namespace s21
