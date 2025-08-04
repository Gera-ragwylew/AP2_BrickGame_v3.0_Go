#include "snake.h"

#define MID_FIELD_Y ((WINDOW_HEIGHT / 2) - 1)
#define MID_FIELD_X ((WINDOW_WIDTH / 2) - 1)
#define START_Y 0
#define START_X 0

#define PAUSE 1
#define UNPAUSE 0

#define SNAKE_MAX_SIZE 200
#define SNAKE_START_SIZE 4
#define SNAKE_HEAD 0

#define DESPAWN 0
#define SPAWN 1
#define SPAWN_COLOR_APPLE 2
#define SPAWN_COLOR_SNAKE 3
#define SPAWN_COLOR_SNAKE_HEAD 4

#define TIMER_MAX_DELAY 1000
#define TIMER_MIN_DELAY 200
#define TIMER_MAX_SPEED 10

#define MAX_LEVEL 10
#define POINTS_FOR_NEXT_LEVEL 5

#define DATA_FILE_NAME "snake_data.bin"

namespace s21 {

/**
 * @brief Конструктор.
 *
 * Выделяет память под массив координат змейки.
 */
Snake::Snake() { snake_coords = new std::pair<int, int>[SNAKE_MAX_SIZE]; }

/**
 * @brief Деструктор.
 *
 * Очищает память массива координат змейки.
 */
Snake::~Snake() { delete[] snake_coords; }

/**
 * @brief GameStart состояние КА.
 *
 * Инициализирует игровые данные.
 * Срабатывает при первом вызове GameInfo_t updateCurrentState().
 * Должна быть вызывана единственный раз в начале игрвого процесса.
 * Переключает состояние КА на Spawn либо на GameOver.
 */
void Snake::game_start() {
  if (action == Start) {
    stats_init();
    high_score_init();
    gameinfo.level = 1;
    stateofmachine = Spawn;
  } else if (action == Terminate) {
    stateofmachine = GameOver;
  }
}

/**
 * @brief Spawn состояние КА.
 *
 * Отвечает за появление на поле непостоянных объектов(яблока).
 * Переключает КА на Moving.
 */
void Snake::spawn() {
  spawn_apple();
  snake_update_on_field(SPAWN);
  stateofmachine = Moving;
}

/**
 * @brief Moving состояние КА.
 *
 * Выполняет бработку нажатых игрком клавиш.
 * Всегда перезаписывает действие игрока action на Start(не используется нигде
 * кроме GameStart). Пауза не является состоянием КА, а лишь одним из условий
 * перключения на сл. состояние. Может переключить КА на Shifting, создавая
 * основной игровой цикл Moving <-> Shifting. Может переключить КА на GameOver
 * если произведено соотвествующее действие Terminate. Переключение в состояние
 * Shifting происходит по 3 "флагам": при повороте, истечении таймера или
 * действия action, но во всех случаях происходит обнуление таймера и установка
 * нового направления(или сохранение старого).
 */
void Snake::moving() {
  if (action == Pause) {
    pause_processing();
  } else if (action == Terminate) {
    stateofmachine = GameOver;
  }
  if (gameinfo.pause != PAUSE) {
    if (head_turn_check() ||
        timer.game_timer_check(gameinfo.speed, TIMER_MAX_DELAY, TIMER_MIN_DELAY,
                               TIMER_MAX_SPEED) ||
        action == Action) {
      timer.start();
      set_direction();
      stateofmachine = Shifting;
    }
  }
  action = Start;
}

/**
 * @brief Shifting состояние КА.
 *
 * Смещает змейку на игровом поле и производит проверки на коллизию.
 * Может переключить КА на состояние Moving, создавая основной игровой цикл
 * Moving <-> Shifting. Если какой-либо игровой элемент collidирует с другим -
 * перключает КА в состояние Attaching.
 */
void Snake::shifting() {
  snake_update_on_field(DESPAWN);
  snake_coords_shift_to_head();
  head_shift();
  if (coord_valid_check(snake_coords[SNAKE_HEAD]) && !apple_touch_check() &&
      !self_touch_check()) {
    snake_update_on_field(SPAWN);
    stateofmachine = Moving;
  } else {
    stateofmachine = Attaching;
  }
}

/**
 * @brief Attaching состояние КА.
 *
 * Обрабатывает все коллизии.
 * Проверки коллизии выполняются еще раз, чтобы отделить каждый конкретный
 * случай. Если была задета стенка или сама змейка - перекалючает КА на
 * GameOver. Если было задето яблоко и змейка максимально размера - перекалючает
 * КА на GameOver. Если было задето яблоко и змейка НЕ максимально размера -
 * перекалючает КА на Spawn.
 */
void Snake::attaching() {
  if (!coord_valid_check(snake_coords[SNAKE_HEAD]) || self_touch_check()) {
    stateofmachine = GameOver;
  } else if (apple_touch_check()) {
    snake_size++;
    if (snake_size >= SNAKE_MAX_SIZE) {
      stateofmachine = GameOver;
    } else {
      snake_coords[snake_size - 1] = snake_coords[snake_size - 2];
      snake_update_on_field(SPAWN);
      score_update();
      speed_and_level_update();
      stateofmachine = Spawn;
    }
  }
}

/**
 * @brief GameOver состояние КА.
 *
 * Завершает игру записывая в поле level структуры GameInfo_t соответсвующий
 * код. Т. к. спецификацией не предоставлен конкретный способ передачи
 * информации о причине завершения игры из модели в представление, было принято
 * волевое решение неочевидно кодировать поле level во время выхода из игры.
 * Таким образом, код -1 сообщает представлению о стандратном выходе из игры,
 * а код 200 - о выходе из игры с сообщением о победе.
 * Все коды выхода из игры будут (к моменту релиза) определены в общем
 * заголовочном файле для удобства настройки.
 */
void Snake::game_over() {
  if (snake_size >= SNAKE_MAX_SIZE) {
    gameinfo.level = WIN_LVL;
  } else {
    gameinfo.level = LOSE_LVL;
  }
}

/**
 * @brief Инициализирует поля класса Snake.
 *
 * Записывает стандартные значенич в поля класса для корректного начала игры.
 */
void Snake::stats_init() {
  apple_coords = {START_Y, START_X};
  snake_size = SNAKE_START_SIZE;
  current_direction = Direction::Dir_Up;
  for (int i = SNAKE_HEAD; i < SNAKE_START_SIZE; i++) {
    snake_coords[i].first = MID_FIELD_Y + i;
    snake_coords[i].second = MID_FIELD_X;
  }
}

/**
 * @brief Инициализирует поле high_score.
 *
 * Производит работу с бинарным файлом при помощи библиотеки fstream.
 * Считывает из файла рекорд или создает новый файл и записывает 0.
 * Обрабатывает ошибки при неудаче.
 */
void Snake::high_score_init() {
  try {
    if (!read_from_file()) write_to_file(0);
  } catch (const std::exception& e) {
    std::cerr << "File error: " << e.what() << DATA_FILE_NAME << '\n';
  }
}

/**
 * @brief Считывает рекорд из файла и записывает в структуру.
 *
 * \throw std::runtime_error В случае ошибки чтения файла.
 * @return Удалось ли открыть файл.
 */
bool Snake::read_from_file() {
  bool res = false;
  int number = 0;
  std::ifstream input_file(DATA_FILE_NAME, std::ios::binary);
  if (input_file.is_open()) {
    input_file.read(reinterpret_cast<char*>(&number), sizeof(number));
    gameinfo.high_score = number;
    input_file.close();
    if (input_file.fail()) {
      throw std::runtime_error("Error reading from file");
    }
    res = true;
  }
  return res;
}

/**
 * @brief Создает и записывает в файл число.
 *
 * \throw std::runtime_error В случае ошибки создания файла.
 */
void Snake::write_to_file(const int number) {
  std::ofstream output_file(DATA_FILE_NAME, std::ios::binary);
  if (output_file.is_open()) {
    output_file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    output_file.close();
  } else {
    throw std::runtime_error("Error creating file");
  }
}

/**
 * @brief Производит расчет координат и отображение яблока.
 *
 * Находит все свободные клетки, выбирает одну случайную,
 * проверяет ее корректность и выводит на поле.
 * \throw std::runtime_error В случае обнаружения невалидных координат.
 */
void Snake::spawn_apple() {
  std::vector<std::pair<int, int>> free_cells;
  int random_index;
  get_free_cells(free_cells);
  random_index = get_random_index(free_cells.size());
  apple_coords = free_cells[random_index];
  if (coord_valid_check(apple_coords)) {
    std::pair<int, int> random_cell = free_cells[random_index];
    gameinfo.field[random_cell.first][random_cell.second] = SPAWN_COLOR_APPLE;
  } else {
    throw std::runtime_error("Error coordinate! Cant spawn apple");
  }
}

/**
 * @brief Проверяет координаты на вхождение в границы поля.
 *
 * @param coords Проверяемая координата.
 * @return Результат проверки.
 */
bool Snake::coord_valid_check(const std::pair<int, int> coords) const {
  bool res = true;
  if (coords.first >= WINDOW_HEIGHT || coords.second >= WINDOW_WIDTH) {
    res = false;
  } else if (coords.first < START_Y || coords.second < START_X) {
    res = false;
  }
  return res;
}

/**
 * @brief Проверяет координаты яблока на соприкосновение с головой.
 *
 * @return Результат проверки.
 */
bool Snake::apple_touch_check() const {
  bool res = false;
  if (snake_coords[SNAKE_HEAD] == apple_coords) {
    res = true;
  }
  return res;
}

/**
 * @brief Проверяет координаты змейки на соприкосновение с головой.
 *
 * @return Результат проверки.
 */
bool Snake::self_touch_check() const {
  bool res = false;
  for (int i = SNAKE_HEAD + 1; i < snake_size; i++) {
    if (snake_coords[i] == snake_coords[SNAKE_HEAD]) {
      res = true;
    }
  }
  return res;
}

/**
 * @brief Обновляет информацию о змейке на поле field.
 *
 * Проходит по массиву координат змейки и либо удляет их с поля либо наносит
 * нужным цветом. Цвета определены в общем заголовочном файле.
 *
 * @param value Значение которое отвечает за SPAWN или DESPAWN координат на
 * поле.
 */
void Snake::snake_update_on_field(const int value) {
  for (int i = SNAKE_HEAD; i < snake_size; i++) {
    std::pair<int, int> coord = snake_coords[i];
    if (value == DESPAWN) {
      gameinfo.field[coord.first][coord.second] = DESPAWN;
    } else if (value == SPAWN) {
      if (i == SNAKE_HEAD) {
        gameinfo.field[coord.first][coord.second] = SPAWN_COLOR_SNAKE_HEAD;
      } else {
        gameinfo.field[coord.first][coord.second] = SPAWN_COLOR_SNAKE;
      }
    }
  }
}

/**
 * @brief Производит поворот головы.
 *
 * В зависимости от следующего направления движения, поворачивает голову змейки
 * в нужную сторону, производя вычисление с координатой y(first) или x(second).
 */
void Snake::head_shift() {
  if (current_direction == Direction::Dir_Up) {
    snake_coords[SNAKE_HEAD].first -= 1;
  } else if (current_direction == Direction::Dir_Left) {
    snake_coords[SNAKE_HEAD].second -= 1;
  } else if (current_direction == Direction::Dir_Down) {
    snake_coords[SNAKE_HEAD].first += 1;
  } else if (current_direction == Direction::Dir_Right) {
    snake_coords[SNAKE_HEAD].second += 1;
  }
}

/**
 * @brief Переключает паузу.
 *
 * Просто переключает паузу.
 */
void Snake::pause_processing() {
  if (gameinfo.pause == UNPAUSE) {
    gameinfo.pause = PAUSE;
  } else if (gameinfo.pause == PAUSE) {
    gameinfo.pause = UNPAUSE;
  }
}

/**
 * @brief Производит смещение координат змейки к голове.
 *
 * В процессе этой оперции происходит сдвиг всех координат с конца к началу.
 * Голова не участвует в сдвиге т. к. ее движение обрабатывается в head_shift().
 */
void Snake::snake_coords_shift_to_head() {
  for (int i = snake_size - 1; i > SNAKE_HEAD; i--) {
    snake_coords[i] = snake_coords[i - 1];
  }
}

/**
 * @brief Проверяет состоялся ли поворот головы змейки.
 *
 * Поврот обрабатывается не только по нажатой клавише, но и по его корректности,
 * т. к. двигаясь вверх\вниз мы можем поворачивать только вправо и влево,
 * наоборот двигаясь вправо\влево принимается только нажатие вверх и вниз.
 * Переменная current_direction имеет собственный класс перечисление Direction,
 * чтобы не смешивать логику с действием игрока UserAction_t action.
 * @return Результат проверки.
 */
bool Snake::head_turn_check() const {
  int res = false;
  if (current_direction == Direction::Dir_Up ||
      current_direction == Direction::Dir_Down) {
    res = (action == Left || action == Right);
  } else if (current_direction == Direction::Dir_Left ||
             current_direction == Direction::Dir_Right) {
    res = (action == Up || action == Down);
  }
  return res;
}

/**
 * @brief Устанавливает текущее направление змейки.
 *
 * Эта и предыдущая функции были декомпозированы из одной большой функции
 * в целях разделения проверки поворота и изменения направления.
 * В set_direction() также сохраняются условия проверки корректности поворота,
 * т. к. head_turn_check() является лишь одним из 3 "флагов" для преключения КА
 * в состояние Shifting. Если преключение произошло по другому "флагу", все
 * равно нужно пройти проверки, чтобы избежать возможных ошибок.
 */
void Snake::set_direction() {
  if (current_direction == Direction::Dir_Up ||
      current_direction == Direction::Dir_Down) {
    if (action == Left) {
      current_direction = Direction::Dir_Left;
    } else if (action == Right) {
      current_direction = Direction::Dir_Right;
    }
  } else if (current_direction == Direction::Dir_Left ||
             current_direction == Direction::Dir_Right) {
    if (action == Up) {
      current_direction = Direction::Dir_Up;
    } else if (action == Down) {
      current_direction = Direction::Dir_Down;
    }
  }
}

/**
 * @brief Записывает вектор свободных координат.
 *
 * Метод необходим для спавна яблока.
 * Сначала запушивает все доступные координаты поля,
 * затем удаляет кординаты которые входят в змейку.
 *
 * @param free_cells Вектор свободных координат.
 */
void Snake::get_free_cells(std::vector<std::pair<int, int>>& free_cells) const {
  for (int y = START_Y; y < WINDOW_HEIGHT; y++) {
    for (int x = START_X; x < WINDOW_WIDTH; x++) {
      free_cells.push_back({y, x});
    }
  }
  for (int i = SNAKE_HEAD; i < snake_size; i++) {
    bool cont = true;
    for (size_t j = 0; (j < free_cells.size()) && cont; j++) {
      if (free_cells[j] == snake_coords[i]) {
        free_cells.erase(free_cells.begin() + j);
        cont = false;
      }
    }
  }
}

/**
 * @brief Получает случайно число из набора.
 *
 * Реализация при помощи библиотеки random.
 *
 * @param free_cells_size Размер вектора свободных координат.
 * @return Случайное число.
 */
int Snake::get_random_index(const size_t free_cells_size) const {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, free_cells_size - 1);
  return distrib(gen);
}

/**
 * @brief Обновляет количество очков.
 *
 * Увеличивает количество очков при съедении яблока.
 * В случае нового рекорда перезаписывает его в файле.
 */
void Snake::score_update() {
  gameinfo.score++;
  if (gameinfo.score > gameinfo.high_score) {
    gameinfo.high_score = gameinfo.score;
    write_to_file(gameinfo.high_score);
  }
}

/**
 * @brief Обновляет скорость и уровень.
 *
 * Увеличивает уровень и скорость за каждые POINTS_FOR_NEXT_LEVEL очков.
 * Т. к. нет каких-то критериев вывода и подсчета скорости,
 * в моей реализации она принимает значение level-1 из-за инициализации нулем на
 * старте. Возможно понадобится в сл. игре.
 */
void Snake::speed_and_level_update() {
  if (gameinfo.level < MAX_LEVEL) {
    if (gameinfo.score % POINTS_FOR_NEXT_LEVEL == 0) {
      gameinfo.speed++;
      gameinfo.level++;
    }
  }
}

}  // namespace s21
