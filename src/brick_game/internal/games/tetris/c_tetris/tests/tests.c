#include "tests.h"

START_TEST(init_matrix_test_1) {
  int res = 1;
  int check_matrix[20][10] = {0};
  int** res_matrix = NULL;
  res_matrix = init_matrix(res_matrix, 20, 10);
  ck_assert_ptr_nonnull(res_matrix);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (res_matrix[i][j] != check_matrix[i][j]) {
        res = 0;
      }
    }
  }
  ck_assert_int_eq(res, 1);
  memory_free(res_matrix, 20);
}

START_TEST(init_matrix_test_2) {
  int** res_matrix = NULL;
  res_matrix = init_matrix(res_matrix, 0, 10);
  ck_assert_ptr_null(res_matrix);
}

START_TEST(init_matrix_test_3) {
  int** res_matrix = NULL;
  res_matrix = init_matrix(res_matrix, 0, -6);
  ck_assert_ptr_null(res_matrix);
}

START_TEST(stats_init_test_1) {
  Tetris tetris;
  stats_init(&tetris);
  ck_assert_ptr_nonnull(tetris.gameinfo.field);
  ck_assert_ptr_nonnull(tetris.gameinfo.next);
  ck_assert_ptr_nonnull(tetris.current_brick);
  ck_assert_ptr_nonnull(tetris.next_brick);
  ck_assert_int_eq(tetris.gameinfo.high_score, 0);
  ck_assert_int_eq(tetris.gameinfo.score, 0);
  ck_assert_int_eq(tetris.gameinfo.level, 0);
  ck_assert_int_eq(tetris.gameinfo.pause, 0);
  ck_assert_int_eq(tetris.gameinfo.speed, 0);
}

START_TEST(score_init_test_1) {
  Tetris tetris;
  ck_assert_int_eq(score_init(&tetris), 0);
}

START_TEST(new_brick_test_1) {
  int res = 0;
  int* brick;
  brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  for (int i = 0; i < BRICK_SIZE; i++) {
    brick[i] = 0;
  }
  new_brick(brick, 1);
  for (int i = 0; i < BRICK_SIZE; i++) {
    if (brick[i] != 0) {
      res = 1;
    }
  }
  free(brick);
  ck_assert_int_eq(res, 1);
}

START_TEST(spawn_test_1) {
  int res = 0;
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  new_brick(brick, 2);
  spawn(matrix, brick);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (matrix[i][j] != 0) {
        res = 1;
      }
    }
  }
  memory_free(matrix, 20);
  free(brick);
  ck_assert_int_eq(res, 1);
}
START_TEST(despawn_test_1) {
  int res = 0;
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  new_brick(brick, 3);
  spawn(matrix, brick);
  despawn(matrix, brick);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (matrix[i][j] != 0) {
        res = 1;
      }
    }
  }
  memory_free(matrix, 20);
  free(brick);
  ck_assert_int_eq(res, 0);
}

START_TEST(brick_move_test_1) {
  int res = 1;
  UserAction_t action = Left;
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  int* brick_check = (int*)malloc(sizeof(int) * BRICK_SIZE);
  new_brick(brick, 4);
  brick_copy(brick_check, brick);
  brick_move(brick, action, matrix);
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if (brick_check[i] == brick[i] + LEFT) {
      res = 0;
    }
  }
  memory_free(matrix, 20);
  free(brick);
  free(brick_check);
  ck_assert_int_eq(res, 1);
}

START_TEST(brick_move_test_2) {
  int res = 1;
  UserAction_t action = Right;
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  int* brick_check = (int*)malloc(sizeof(int) * BRICK_SIZE);
  new_brick(brick, 5);
  brick_copy(brick_check, brick);
  brick_move(brick, action, matrix);
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if (brick_check[i] == brick[i] + RIGHT) {
      res = 0;
    }
  }
  memory_free(matrix, 20);
  free(brick);
  free(brick_check);
  ck_assert_int_eq(res, 1);
}

START_TEST(brick_move_test_3) {
  int res = 1;
  UserAction_t action = Action;
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  int brick_check[BRICK_SIZE] = {2, 1, 1, 1, 2, 2, 3, 2};
  new_brick(brick, 7);
  brick_move(brick, action, matrix);
  for (int i = 0; i < BRICK_SIZE; i++) {
    if (brick_check[i] != brick[i]) {
      res = 0;
    }
  }
  memory_free(matrix, 20);
  free(brick);
  ck_assert_int_eq(res, 1);
}

//                  #
//  ####    -->     #
//                  #
//                  #
START_TEST(brick_move_test_4) {
  int res = 1;
  UserAction_t action = Action;
  Bricks bricks = {TEEWEE,     HERO,        SMASHBOY,      ORANGE_RICKY,
                   BLUE_RICKY, CLEVELAND_Z, RHODE_ISLAND_Z};
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  int brick_check[BRICK_SIZE] = {1, 1, 0, 1, 2, 1, 3, 1};
  brick_copy(brick, bricks.Hero);
  brick_move(brick, action, matrix);
  for (int i = 0; i < BRICK_SIZE; i++) {
    if (brick_check[i] != brick[i]) {
      res = 0;
    }
  }
  memory_free(matrix, 20);
  free(brick);
  ck_assert_int_eq(res, 1);
}

//  ##              ##
//  ##     -->      ##
//
START_TEST(brick_move_test_5) {
  int res = 1;
  UserAction_t action = Action;
  Bricks bricks = {TEEWEE,     HERO,        SMASHBOY,      ORANGE_RICKY,
                   BLUE_RICKY, CLEVELAND_Z, RHODE_ISLAND_Z};
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int* brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  int brick_check[BRICK_SIZE] = {2, 1, 2, 2, 3, 1, 3, 2};
  brick_copy(brick, bricks.Smashboy);
  brick_move(brick, action, matrix);
  for (int i = 0; i < BRICK_SIZE; i++) {
    if (brick_check[i] != brick[i]) {
      res = 0;
    }
  }
  memory_free(matrix, 20);
  free(brick);
  ck_assert_int_eq(res, 1);
}

START_TEST(is_gameover_test_1) {
  int brick[BRICK_SIZE] = {0, 4, 0, 5, 1, 4, 1, 5};
  int res = is_gameover(brick);
  ck_assert_int_eq(res, 1);
}

START_TEST(is_gameover_test_2) {
  int brick[BRICK_SIZE] = {2, 4, 2, 5, 3, 4, 3, 5};
  int res = is_gameover(brick);
  ck_assert_int_eq(res, 0);
}

START_TEST(is_attaching_test_1) {
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int brick[BRICK_SIZE] = {2, 4, 2, 5, 3, 4, 3, 5};
  int res = is_attaching(brick, matrix);
  memory_free(matrix, 20);
  ck_assert_int_eq(res, 0);
}

START_TEST(is_attaching_test_2) {
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int brick[BRICK_SIZE] = {18, 4, 18, 5, 19, 4, 19, 5};
  int res = is_attaching(brick, matrix);
  memory_free(matrix, 20);
  ck_assert_int_eq(res, 1);
}

START_TEST(full_row_test_1) {
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  for (int i = 0; i < 10; i++) {
    matrix[19][i] = 1;
  }
  for (int i = 0; i < 10; i++) {
    matrix[18][i] = 1;
  }
  int res = full_row(matrix);
  memory_free(matrix, 20);
  ck_assert_int_eq(res, 2);
}

START_TEST(full_row_test_2) {
  int** matrix = NULL;
  matrix = init_matrix(matrix, 20, 10);
  int res = full_row(matrix);
  memory_free(matrix, 20);
  ck_assert_int_eq(res, 0);
}

START_TEST(score_write_test_1) {
  Tetris tetris;
  stats_init(&tetris);
  score_write(&tetris, 1);
  ck_assert_int_eq(tetris.gameinfo.score, 100);
}

START_TEST(score_write_test_2) {
  Tetris tetris;
  stats_init(&tetris);
  score_write(&tetris, 2);
  ck_assert_int_eq(tetris.gameinfo.score, 300);
}

START_TEST(score_write_test_3) {
  Tetris tetris;
  stats_init(&tetris);
  score_write(&tetris, 3);
  ck_assert_int_eq(tetris.gameinfo.score, 700);
}

START_TEST(score_write_test_4) {
  Tetris tetris;
  stats_init(&tetris);
  score_write(&tetris, 4);
  ck_assert_int_eq(tetris.gameinfo.score, 1500);
}

START_TEST(check_level_test_1) {
  Tetris tetris;
  stats_init(&tetris);
  tetris.gameinfo.score = 600;
  check_level(&tetris);
  ck_assert_int_eq(tetris.gameinfo.level, 1);
}

START_TEST(fsm_test_1) {
  Tetris* tetris;
  tetris = get_tetris();

  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, GameStart);

  tetris->action = Start;
  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Spawn);

  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Moving);

  tetris->gameinfo.pause = 0;
  tetris->action = Pause;
  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Moving);
  ck_assert_int_eq(tetris->gameinfo.pause, 1);

  tetris->action = Pause;
  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Moving);
  ck_assert_int_eq(tetris->gameinfo.pause, 0);

  tetris->action = Left;

  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Moving);

  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, Moving);

  tetris->action = Terminate;
  fsm(tetris);
  ck_assert_int_eq(tetris->stateofmachine, GameOver);

  fsm(tetris);
  ck_assert_int_eq(tetris->gameinfo.level, -1);
}

Suite* decl_init_matrix_test_suit(void) {
  Suite* suite = suite_create("init_matrix_test");
  TCase* test_case = tcase_create("init_matrix_test");

  tcase_add_test(test_case, init_matrix_test_1);
  tcase_add_test(test_case, init_matrix_test_2);
  tcase_add_test(test_case, init_matrix_test_3);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_stats_init_test_suit(void) {
  Suite* suite = suite_create("stats_init_test");
  TCase* test_case = tcase_create("stats_init_test");

  tcase_add_test(test_case, stats_init_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_score_init_test_suit(void) {
  Suite* suite = suite_create("score_init_test");
  TCase* test_case = tcase_create("score_init_test");

  tcase_add_test(test_case, score_init_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_new_brick_test_suit(void) {
  Suite* suite = suite_create("new_brick_test");
  TCase* test_case = tcase_create("new_brick_test");

  tcase_add_test(test_case, new_brick_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_spawn_test_suit(void) {
  Suite* suite = suite_create("spawn_test");
  TCase* test_case = tcase_create("spawn_test");

  tcase_add_test(test_case, spawn_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_despawn_test_suit(void) {
  Suite* suite = suite_create("despawn_test");
  TCase* test_case = tcase_create("despawn_test");

  tcase_add_test(test_case, despawn_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_brick_move_test_suit(void) {
  Suite* suite = suite_create("brick_move_test");
  TCase* test_case = tcase_create("brick_move_test");

  tcase_add_test(test_case, brick_move_test_1);
  tcase_add_test(test_case, brick_move_test_2);
  tcase_add_test(test_case, brick_move_test_3);
  tcase_add_test(test_case, brick_move_test_4);
  tcase_add_test(test_case, brick_move_test_5);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_is_gameover_test_suit(void) {
  Suite* suite = suite_create("is_gameover_test");
  TCase* test_case = tcase_create("is_gameover_test");

  tcase_add_test(test_case, is_gameover_test_1);
  tcase_add_test(test_case, is_gameover_test_2);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_is_attaching_test_suit(void) {
  Suite* suite = suite_create("is_attaching_test");
  TCase* test_case = tcase_create("is_attaching_test");

  tcase_add_test(test_case, is_attaching_test_1);
  tcase_add_test(test_case, is_attaching_test_2);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_score_write_test_suit(void) {
  Suite* suite = suite_create("score_write_test");
  TCase* test_case = tcase_create("score_write_test");

  tcase_add_test(test_case, score_write_test_1);
  tcase_add_test(test_case, score_write_test_2);
  tcase_add_test(test_case, score_write_test_3);
  tcase_add_test(test_case, score_write_test_4);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_full_row_test_suit(void) {
  Suite* suite = suite_create("full_row_test");
  TCase* test_case = tcase_create("full_row_test");

  tcase_add_test(test_case, full_row_test_1);
  tcase_add_test(test_case, full_row_test_2);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_check_level_test_suit(void) {
  Suite* suite = suite_create("check_level_test");
  TCase* test_case = tcase_create("check_level_test");

  tcase_add_test(test_case, check_level_test_1);
  suite_add_tcase(suite, test_case);

  return suite;
}

Suite* decl_fsm_test_suit(void) {
  Suite* suite = suite_create("fsm_test");
  TCase* test_case = tcase_create("fsm_test");

  tcase_add_test(test_case, fsm_test_1);
  // tcase_add_test(test_case, fsm_test_2);
  // tcase_add_test(test_case, fsm_test_3);
  suite_add_tcase(suite, test_case);

  return suite;
}

int main() {
  size_t tests_faild = 0;
  Suite* init_matrix_test_suite = decl_init_matrix_test_suit();
  Suite* stats_init_test_suite = decl_stats_init_test_suit();
  Suite* score_init_test_suite = decl_score_init_test_suit();
  Suite* new_brick_test_suite = decl_new_brick_test_suit();
  Suite* spawn_test_suite = decl_spawn_test_suit();
  Suite* despawn_test_suite = decl_despawn_test_suit();
  Suite* brick_move_test_suite = decl_brick_move_test_suit();
  Suite* is_gameover_test_suit = decl_is_gameover_test_suit();
  Suite* is_attaching_test_suit = decl_is_attaching_test_suit();
  Suite* full_row_test_suit = decl_full_row_test_suit();
  Suite* score_write_test_suit = decl_score_write_test_suit();
  Suite* check_level_test_suit = decl_check_level_test_suit();
  Suite* fsm_test_suit = decl_fsm_test_suit();

  SRunner* sr;

  sr = srunner_create(init_matrix_test_suite);
  srunner_add_suite(sr, stats_init_test_suite);
  srunner_add_suite(sr, score_init_test_suite);
  srunner_add_suite(sr, new_brick_test_suite);
  srunner_add_suite(sr, spawn_test_suite);
  srunner_add_suite(sr, despawn_test_suite);
  srunner_add_suite(sr, brick_move_test_suite);
  srunner_add_suite(sr, is_gameover_test_suit);
  srunner_add_suite(sr, is_attaching_test_suit);
  srunner_add_suite(sr, full_row_test_suit);
  srunner_add_suite(sr, score_write_test_suit);
  srunner_add_suite(sr, check_level_test_suit);
  srunner_add_suite(sr, fsm_test_suit);
  srunner_run_all(sr, CK_VERBOSE);
  tests_faild = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (tests_faild == 0) ? 0 : 1;
}