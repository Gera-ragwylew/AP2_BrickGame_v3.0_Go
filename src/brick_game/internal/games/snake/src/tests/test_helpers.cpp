#include "tests.h"

void find_obj_coords(int** field, int& coord_y, int& coord_x,
                     const int& obj_type) {
  for (auto i = 0; i < WINDOW_HEIGHT; i++) {
    for (auto j = 0; j < WINDOW_WIDTH; j++) {
      if (field[i][j] == obj_type) {
        coord_y = i;
        coord_x = j;
      }
    }
  }
}

void find_field_half(const int& apple_y, const int& apple_x, int& y_half,
                     int& x_half) {
  if (apple_x == (WINDOW_WIDTH / 2 - 1)) {
    x_half = 0;
  } else if (apple_x < (WINDOW_WIDTH / 2 - 1)) {
    x_half = 1;
  } else if (apple_x > (WINDOW_WIDTH / 2 - 1)) {
    x_half = 2;
  }
  if (apple_y == (WINDOW_HEIGHT / 2 - 2)) {
    y_half = 0;
  } else if (apple_y < (WINDOW_HEIGHT / 2 - 2)) {
    y_half = 1;
  } else if (apple_y > (WINDOW_HEIGHT / 2 - 2)) {
    y_half = 2;
  }
}

void horizontal_shift(s21::Game* snake_game, const int& x_half,
                      const int& apple_x, int& head_x) {
  if (x_half == 1) {
    snake_game->set_action(UserAction_t::Left);
    snake_game->fsm();  // Moving
    snake_game->fsm();  // Shifting
    head_x--;
    while (head_x != apple_x) {
      snake_game->set_action(UserAction_t::Action);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_x--;
    }
  } else if (x_half == 2) {
    snake_game->set_action(UserAction_t::Right);
    snake_game->fsm();  // Moving
    snake_game->fsm();  // Shifting
    head_x++;
    while (head_x != apple_x) {
      snake_game->set_action(UserAction_t::Action);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_x++;
    }
  }
}

void vertical_shift(s21::Game* snake_game, const int& y_half, const int& x_half,
                    const int& apple_y, int& head_y) {
  if (y_half == 1) {
    if (x_half != 0) {
      snake_game->set_action(UserAction_t::Up);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_y--;
    }
    while (head_y != apple_y) {
      snake_game->set_action(UserAction_t::Action);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_y--;
    }
  } else if (y_half == 2) {
    if (x_half == 0) {
      snake_game->set_action(UserAction_t::Right);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      snake_game->set_action(UserAction_t::Down);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_y++;
      snake_game->set_action(UserAction_t::Left);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
    }
    snake_game->set_action(UserAction_t::Down);
    snake_game->fsm();  // Moving
    snake_game->fsm();  // Shifting
    head_y++;
    while (head_y != apple_y) {
      snake_game->set_action(UserAction_t::Action);
      snake_game->fsm();  // Moving
      snake_game->fsm();  // Shifting
      head_y++;
    }
  }
}