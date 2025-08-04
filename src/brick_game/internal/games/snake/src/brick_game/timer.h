#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace s21 {

/**
 * @brief C++ класс для работы со временем.
 *
 * Использует библиотеку chrono.
 * Определяет методы необходимые для подсчета скорости движения объектов в
 * играх. Также реализован простейший секундомер для фронтенда.
 */
class Timer {
 private:
  using ClockType = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<ClockType>;
  using DurationMs = std::chrono::milliseconds;

  TimePoint start_time_;

 public:
  Timer() : start_time_(ClockType::now()) {}

  void start();
  bool game_timer_check(int speed, int max_delay, int min_delay, int max_speed);

  double get_miliseconds() const;
  int get_seconds() const;
  int get_minutes() const;

 private:
  DurationMs get_elapsed_time() const;
  DurationMs calculate_delay(int speed, int max_delay, int min_delay,
                             int max_speed) const;
};

}  // namespace s21

#endif  // TIMER_H