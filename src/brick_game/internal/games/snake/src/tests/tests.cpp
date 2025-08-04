#include "tests.h"

TEST(game_fabric_tests, get_non_existent_test) {
  s21::Game* game = s21::GameFabric::get_game();
  EXPECT_EQ(game, nullptr);
}

TEST(game_fabric_tests, set_snake) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* game = s21::GameFabric::get_game();
  EXPECT_NE(game, nullptr);
}

TEST(game_fabric_tests, set_tetris) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Tetris);
  s21::Game* game = s21::GameFabric::get_game();
  EXPECT_NE(game, nullptr);
}

TEST(game_fabric_tests, set_unknown_game) {
  EXPECT_THROW(s21::GameFabric::set_game(s21::GameFabric::GameName::EmptyGame),
               std::runtime_error);
}

TEST(game_fabric_tests, snake_not_tetris) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  s21::GameFabric::set_game(s21::GameFabric::GameName::Tetris);
  s21::Game* tetris_game = s21::GameFabric::get_game();
  EXPECT_NE(snake_game, tetris_game);
}

TEST(snake_test, start) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  snake_game->fsm();
  GameInfo_t actual_info = snake_game->get_gameinfo();
  EXPECT_NE(actual_info.field, nullptr);
  EXPECT_NE(actual_info.next, nullptr);
  EXPECT_EQ(actual_info.level, 1);
}

TEST(snake_test, spawn) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  snake_game->fsm();
  GameInfo_t actual_info = snake_game->get_gameinfo();
  int apple = 0;
  for (auto i = 0; i < WINDOW_HEIGHT; i++) {
    for (auto j = 0; j < WINDOW_WIDTH; j++) {
      if (actual_info.field[i][j] == 2) {
        apple++;
      }
    }
  }
  EXPECT_EQ(apple, 1);
}

TEST(snake_test, pause) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  snake_game->set_action(UserAction_t::Pause);
  snake_game->fsm();
  GameInfo_t actual_info = snake_game->get_gameinfo();
  EXPECT_EQ(actual_info.pause, 1);
}

TEST(snake_test, unpause) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  snake_game->set_action(UserAction_t::Pause);
  snake_game->fsm();
  GameInfo_t actual_info = snake_game->get_gameinfo();
  EXPECT_EQ(actual_info.pause, 0);
}

TEST(snake_test, action) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  GameInfo_t start_point = snake_game->get_gameinfo();
  int start_y = 0;
  int start_x = 0;
  int end_y = 0;
  int end_x = 0;

  find_obj_coords(start_point.field, start_y, start_x, 4);
  snake_game->set_action(UserAction_t::Action);
  snake_game->fsm();  // Moving
  snake_game->fsm();  // Shifting
  GameInfo_t end_point = snake_game->get_gameinfo();
  find_obj_coords(end_point.field, end_y, end_x, 4);

  EXPECT_NE(start_y, end_y);
}

TEST(snake_test, incorrect_turn) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  GameInfo_t start_point = snake_game->get_gameinfo();
  int start_y = 0;
  int start_x = 0;
  int end_y = 0;
  int end_x = 0;

  find_obj_coords(start_point.field, start_y, start_x, 4);
  snake_game->set_action(UserAction_t::Down);
  snake_game->fsm();  // Moving
  snake_game->fsm();  // Shifting
  GameInfo_t end_point = snake_game->get_gameinfo();
  find_obj_coords(end_point.field, end_y, end_x, 4);

  EXPECT_EQ(start_y, end_y);
  EXPECT_EQ(start_x, end_x);
  EXPECT_EQ(start_x - end_x, 0);
}

TEST(snake_test, attaching_apple) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  GameInfo_t start_point = snake_game->get_gameinfo();
  int apple_y = 0;
  int apple_x = 0;
  int head_y = 0;
  int head_x = 0;
  int x_half = 0;
  int y_half = 0;
  find_obj_coords(start_point.field, apple_y, apple_x, 2);
  find_obj_coords(start_point.field, head_y, head_x, 4);
  find_field_half(apple_y, apple_x, y_half, x_half);
  horizontal_shift(snake_game, x_half, apple_x, head_x);
  vertical_shift(snake_game, y_half, x_half, apple_y, head_y);

  snake_game->fsm();  // Attaching
  GameInfo_t end_point = snake_game->get_gameinfo();
  EXPECT_EQ(end_point.score, 1);
}

TEST(snake_test, game_over) {
  s21::GameFabric::set_game(s21::GameFabric::GameName::Snake);
  s21::Game* snake_game = s21::GameFabric::get_game();
  snake_game->set_action(UserAction_t::Terminate);
  snake_game->fsm();  // Spawn
  snake_game->fsm();  // Moving
  snake_game->fsm();  // GameOver
  GameInfo_t game_info = snake_game->get_gameinfo();
  EXPECT_EQ(game_info.level, -1);
}
