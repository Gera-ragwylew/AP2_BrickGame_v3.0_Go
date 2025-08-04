#include "timer.h"

namespace s21 {

/**
 * @brief Записывает точку отсчета таймера.
 */
void Timer::start() { start_time_ = ClockType::now(); }

/**
 * @brief Расчитывает задержку.
 *
 * @return Задержка в мс.
 */
Timer::DurationMs Timer::calculate_delay(int speed, int max_delay,
                                         int min_delay, int max_speed) const {
  return DurationMs(max_delay -
                    (speed - 1) * (max_delay - min_delay) / (max_speed - 1));
}

/**
 * @brief Расчитывает сколько времени прошло с точки отсчета.
 *
 * @return Разницу в мс.
 */
Timer::DurationMs Timer::get_elapsed_time() const {
  return std::chrono::duration_cast<DurationMs>(ClockType::now() - start_time_);
}

/**
 * @brief Проверяет следует ли сдвигать объект.
 *
 * @return Результат проверки.
 */
bool Timer::game_timer_check(int speed, int max_delay, int min_delay,
                             int max_speed) {
  bool res = false;
  DurationMs delay = calculate_delay(speed, max_delay, min_delay, max_speed);
  DurationMs elapsed_time = get_elapsed_time();

  if (elapsed_time >= delay) {
    res = true;
  }
  return res;
}

/**
 * @brief Считает время в мс.
 *
 * @return Время в мс.
 */
double Timer::get_miliseconds() const {
  return get_elapsed_time().count() % 1000;
}

/**
 * @brief Считает время в секундах.
 *
 * @return Время в секундах.
 */
int Timer::get_seconds() const {
  return get_elapsed_time().count() / 1000 % 60;
}

/**
 * @brief Считает время в минутах.
 *
 * @return Время в минутах.
 */
int Timer::get_minutes() const {
  return get_elapsed_time().count() / 60000 % 60;
}

}  // namespace s21
