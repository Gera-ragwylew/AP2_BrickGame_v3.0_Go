#include "tetris.h"

#define TEEWEE \
  { 1, 4, 1, 3, 0, 4, 1, 5 }
#define HERO \
  { 0, 5, 0, 4, 0, 3, 0, 6 }
#define SMASHBOY \
  { 0, 4, 0, 5, 1, 4, 1, 5 }
#define ORANGE_RICKY \
  { 1, 4, 1, 3, 1, 5, 0, 5 }
#define BLUE_RICKY \
  { 1, 4, 0, 3, 1, 3, 1, 5 }
#define CLEVELAND_Z \
  { 1, 4, 0, 3, 0, 4, 1, 5 }
#define RHODE_ISLAND_Z \
  { 1, 4, 1, 3, 0, 4, 0, 5 }

#define SUCCSES 0
#define ERROR 1

#define Y_CORDS 0
#define X_CORDS 1
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

#define TIMER_MAX_DELAY 1000
#define TIMER_MIN_DELAY 200
#define TIMER_MAX_SPEED 10

#define DATA_FILE_NAME "tetris_data.bin"

#define BRICK_RANDOMIZER (1 + rand() % 7)
#define COLOR_RANDOMIZER (1 + rand() % 6)

namespace s21 {

void Tetris::game_start() {
  if (action == Start) {
    stats_init(this);
    score_init(this);
    new_brick(next_brick, BRICK_RANDOMIZER);
    gameinfo.level = 1;
    stateofmachine = Spawn;
  } else if (action == Terminate) {
    stateofmachine = GameOver;
  }
}

void Tetris::spawn() {
  current_color = next_color;
  next_color = COLOR_RANDOMIZER;
  spawn_brick(gameinfo.field, next_brick, current_color);
  brick_copy(current_brick, next_brick);
  new_brick(next_brick, BRICK_RANDOMIZER);
  fill_array_zero(gameinfo.next, NEXT_SIZE, NEXT_SIZE);
  spawn_brick(gameinfo.next, next_brick, next_color);
  stateofmachine = Moving;
}

void Tetris::moving() {
  if (action == Pause && gameinfo.pause == 0) {
    gameinfo.pause = 1;
    action = Start;
  } else if (action == Pause && gameinfo.pause == 1) {
    gameinfo.pause = 0;
    action = Start;
  } else if (action == Terminate) {
    stateofmachine = GameOver;
  }
  if (gameinfo.pause != 1) {
    despawn(gameinfo.field, current_brick);
    brick_move(current_brick, action, gameinfo.field);
    spawn_brick(gameinfo.field, current_brick, current_color);
    if (time.game_timer_check(gameinfo.speed, TIMER_MAX_DELAY, TIMER_MIN_DELAY,
                              TIMER_MAX_SPEED) ||
        action == Down) {
      time.start();
      stateofmachine = Shifting;
    } else {
      action = Start;
    }
  }
}

void Tetris::shifting() {
  despawn(gameinfo.field, current_brick);
  if (can_down(gameinfo.field, current_brick)) {
    coord_shift(current_brick, Y_CORDS, DOWN);
    stateofmachine = Moving;
  } else {
    stateofmachine = Attaching;
  }
  spawn_brick(gameinfo.field, current_brick, current_color);
}

void Tetris::attaching() {
  int full_rows_counter = 0;
  full_rows_counter = full_row(gameinfo.field);
  if (action == Down) {
    action = Start;
  }
  if (is_gameover(current_brick)) {
    stateofmachine = GameOver;
  } else if (full_rows_counter) {
    score_write(this, full_rows_counter);
    check_level(this);
  } else {
    stateofmachine = Spawn;
  }
}

void Tetris::game_over() {
  if (gameinfo.level > 0) {
    free(current_brick);
    free(next_brick);
  }
  gameinfo.level = -1;
}

/*
    Standard functions for
    allocating and clearing memory
    and filling the matrix with zeros
*/
int** Tetris::init_matrix(int** matrix, int height, int width) {
  if (height <= 0 || width <= 0) {
    matrix = NULL;
  } else {
    matrix = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
      matrix[i] = (int*)malloc(width * sizeof(int));
    }
    fill_array_zero(matrix, height, width);
  }
  return matrix;
}

void Tetris::memory_free(int** matrix, int size) {
  if (size <= 0) {
    matrix = NULL;
  } else {
    for (int i = 0; i < size; i++) {
      free(matrix[i]);
    }
    free(matrix);
  }
}

void Tetris::fill_array_zero(int** matrix, int height, int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      matrix[i][j] = 0;
    }
  }
}

/*
    Function initializing the Tetris structure
    before starting the game
*/
void Tetris::stats_init(Tetris* tetris) {
  tetris->current_brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  tetris->next_brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  // tetris->gameinfo.field = init_matrix(tetris->gameinfo.field, WINDOW_HEIGHT,
  // WINDOW_WIDTH); tetris->gameinfo.next = init_matrix(tetris->gameinfo.next,
  // NEXT_SIZE, NEXT_SIZE);
  tetris->gameinfo.level = 0;
  tetris->gameinfo.pause = 0;
  tetris->gameinfo.speed = 0;
  tetris->gameinfo.score = 0;
  tetris->gameinfo.high_score = 0;
  tetris->current_color = COLOR_RANDOMIZER;
  tetris->next_color = COLOR_RANDOMIZER;
}

/*
    The function opens an existing
    or creates a new binary file with the player's record.
    If there is no existing binary and
    it is not possible to create a new one,
    the function returns 1
*/
int Tetris::score_init(Tetris* tetris) {
  int record = 0;
  int res = 0;
  FILE* file;
  const char* filename = DATA_FILE_NAME;
  file = fopen(filename, "rb");
  if (file == NULL) {
    file = fopen(filename, "wb");
    if (file != NULL) {
      fwrite(&record, sizeof(int), 1, file);
      fclose(file);
    } else {
      res = 1;
    }
  } else {
    fread(&record, sizeof(int), 1, file);
    tetris->gameinfo.high_score = record;
    fclose(file);
  }
  return res;
}

/*
    Function that randomize the next figure to spawn.
    Uses a Bricks structure in which the initial
    coordinates of all figures are already recorded
    And a function that copies arrays brick_copy()
*/
void Tetris::new_brick(int* brick, int random) {
  Bricks bricks = {TEEWEE,     HERO,        SMASHBOY,      ORANGE_RICKY,
                   BLUE_RICKY, CLEVELAND_Z, RHODE_ISLAND_Z};
  switch (random) {
    case 1:
      brick_copy(brick, bricks.Teewee);
      break;
    case 2:
      brick_copy(brick, bricks.Hero);
      break;
    case 3:
      brick_copy(brick, bricks.Smashboy);
      break;
    case 4:
      brick_copy(brick, bricks.Orange_Ricky);
      break;
    case 5:
      brick_copy(brick, bricks.Blue_Ricky);
      break;
    case 6:
      brick_copy(brick, bricks.Cleveland_Z);
      break;
    case 7:
      brick_copy(brick, bricks.Rhode_Island_Z);
      break;
    default:
      break;
  }
}

void Tetris::brick_copy(int* src, int* other) {
  for (int i = 0; i < BRICK_SIZE; i++) {
    src[i] = other[i];
  }
}

/*
    2 functions that “spawn” or
    "despawn" a figure in the matrix,
    the coordinates of which are written in the array
*/
void Tetris::spawn_brick(int** matrix, int* array, int color) {
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    matrix[array[i]][array[i + 1]] = color;
  }
}

void Tetris::despawn(int** matrix, int* array) {
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    matrix[array[i]][array[i + 1]] = 0;
  }
}

/*
    Function for movement of a figure.
    Checks the possibility of movement
    of the figure and calls the movement functions.
    The "Smashboy" figure should not rotate,
    but the Hero figure requires a different rotation field
*/
void Tetris::brick_move(int* brick, UserAction_t state, int** matrix) {
  if (state == Left && can_left(matrix, brick, 1)) {
    coord_shift(brick, X_CORDS, LEFT);
  } else if (state == Right && can_right(matrix, brick, 1)) {
    coord_shift(brick, X_CORDS, RIGHT);
  } else if (state == Action && !is_Smashboy(brick)) {
    if (is_Hero(brick)) {
      rotate(matrix, brick, 5);
    } else {
      rotate(matrix, brick, 3);
    }
  }
}

/*
    3 functions checking the
    possibility of horizontal and vertical movement
    returns 1 if possible otherwise 0
*/
int Tetris::can_right(int** matrix, int* brick, int shift) {
  int res = 1;
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if ((brick[i] + shift > WINDOW_WIDTH - 1) ||
        (matrix[brick[i - 1]][brick[i] + shift])) {
      res = 0;
    }
  }
  return res;
}

int Tetris::can_left(int** matrix, int* brick, int shift) {
  int res = 1;
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if ((brick[i] - shift < 0) || (matrix[brick[i - 1]][brick[i] - shift])) {
      res = 0;
    }
  }
  return res;
}

int Tetris::can_down(int** matrix, int* brick) {
  int res = 1;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    if ((brick[i] + 1 >= WINDOW_HEIGHT) || matrix[brick[i] + 1][brick[i + 1]]) {
      res = 0;
    }
  }
  return res;
}

/*
    2 functions checking figure type
*/
int Tetris::is_Smashboy(int* brick) {
  int res = 0;
  if (brick[4] == brick[0] + 1 && brick[6] == brick[2] + 1 &&
      brick[3] == brick[1] + 1 && brick[7] == brick[5] + 1 &&
      brick[1] == brick[5] && brick[3] == brick[7])
    res = 1;
  return res;
}

int Tetris::is_Hero(int* brick) {
  int res = 0;
  if ((brick[0] == brick[2] && brick[2] == brick[4] && brick[4] == brick[6]) ||
      (brick[1] == brick[3] && brick[3] == brick[5] && brick[5] == brick[7])) {
    res = 1;
  }
  return res;
}

/*
    Shifts the coordinate value in array.
    If coord = 0 shifts Y coordinates,
    if coord = 1 shifts X coordinates
*/
void Tetris::coord_shift(int* brick, int cords, int shift) {
  for (int i = cords; i < BRICK_SIZE; i += 2) {
    brick[i] += shift;
  }
}

/*
    Rotation function.
    Creates two matrices (the size depends on the type of figure),
    calculates new coordinates for a small 3x3 or 4x4 matrix, and fits the
   figure there, rotates the matrix using an algorithm and back the difference
   in coordinates
*/
void Tetris::rotate(int** matrix, int* brick, int size) {
  int** temp = NULL;
  int** rotate = NULL;
  temp = init_matrix(temp, size, size);
  rotate = init_matrix(rotate, size, size);
  int* temp_brick = (int*)malloc(sizeof(int) * BRICK_SIZE);
  brick_copy(temp_brick, brick);
  int centre_cord = (size == 3) ? 1 : 2;
  int min_Y = brick[0] - centre_cord;
  int min_X = brick[1] - centre_cord;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    temp[brick[i] - min_Y][brick[i + 1] - min_X] = 1;
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      rotate[j][i] = temp[size - i - 1][j];
    }
  }
  int k = 2;
  brick[0] = centre_cord + min_Y;
  brick[1] = centre_cord + min_X;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (rotate[i][j] && (i != centre_cord || j != centre_cord)) {
        brick[k] = i + min_Y;
        brick[k + 1] = j + min_X;
        k += 2;
      }
    }
  }
  fix_brick_coord(brick);
  if (!rotate_check_field(matrix, brick)) {
    brick_copy(brick, temp_brick);
  }
  free(temp_brick);
  memory_free(temp, size);
  memory_free(rotate, size);
}

/*
    If the figure is close to any wall,
    we need to change its coordinates so
    that later we do not go beyond the matrix
*/
void Tetris::fix_brick_coord(int* brick) {
  while (!rotate_check_right_wall(brick)) {
    coord_shift(brick, X_CORDS, LEFT);
  }
  while (!rotate_check_left_wall(brick)) {
    coord_shift(brick, X_CORDS, RIGHT);
  }
  while (!rotate_check_down_wall(brick)) {
    coord_shift(brick, Y_CORDS, UP);
  }
  while (!rotate_check_up_wall(brick)) {
    coord_shift(brick, Y_CORDS, DOWN);
  }
}

int Tetris::rotate_check_down_wall(int* brick) {
  int res = 1;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    if (brick[i] >= WINDOW_HEIGHT) res = 0;
  }
  return res;
}

int Tetris::rotate_check_up_wall(int* brick) {
  int res = 1;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    if (brick[i] < 0) res = 0;
  }
  return res;
}

int Tetris::rotate_check_right_wall(int* brick) {
  int res = 1;
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if ((brick[i] >= WINDOW_WIDTH)) {
      res = 0;
    }
  }
  return res;
}

int Tetris::rotate_check_left_wall(int* brick) {
  int res = 1;
  for (int i = 1; i < BRICK_SIZE; i += 2) {
    if ((brick[i] < 0)) {
      res = 0;
    }
  }
  return res;
}

int Tetris::rotate_check_field(int** matrix, int* brick) {
  int res = 1;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    if (matrix[brick[i]][brick[i + 1]]) {
      res = 0;
    }
  }
  return res;
}

/*
    Check the end of game
*/
int Tetris::is_gameover(int* brick) {
  int res = 0;
  for (int i = 0; i < BRICK_SIZE; i += 2) {
    if (brick[i] == 0) {
      res = 1;
    }
  }
  return res;
}

/*
    Сheck whether the figure is attached
    to the field and if not, moves it down
*/
int Tetris::is_attaching(int* brick, int** matrix) {
  int res = 0;
  if (can_down(matrix, brick)) {
    coord_shift(brick, Y_CORDS, DOWN);
  } else {
    res = 1;
  }
  return res;
}

/*
    Counts and removes filled rows
    returns the number of filled rows
*/
int Tetris::full_row(int** field) {
  int res = 0;
  for (int i = WINDOW_HEIGHT - 1; i > 1; i--) {
    bool flag = true;
    for (int j = 0; (j < WINDOW_WIDTH) && flag; j++) {
      if (field[i][j] == 0) {
        flag = false;
      }
    }
    if (flag) {
      res++;
      for (int k = i; k > 1; k--) {
        for (int m = 0; m < WINDOW_WIDTH; m++) {
          field[k][m] = field[k - 1][m];
        }
      }
      i++;
    }
  }
  return res;
}

/*
    Counts and records score
*/
void Tetris::score_write(Tetris* tetris, int full_rows_counter) {
  if (full_rows_counter == 1) {
    tetris->gameinfo.score += 100;
  } else if (full_rows_counter == 2) {
    tetris->gameinfo.score += 300;
  } else if (full_rows_counter == 3) {
    tetris->gameinfo.score += 700;
  } else if (full_rows_counter == 4) {
    tetris->gameinfo.score += 1500;
  }
  if (tetris->gameinfo.score > tetris->gameinfo.high_score) {
    tetris->gameinfo.high_score = tetris->gameinfo.score;
    FILE* file;
    const char* filename = DATA_FILE_NAME;
    int record = tetris->gameinfo.high_score;
    file = fopen(filename, "wb");
    if (file != NULL) {
      fwrite(&record, sizeof(int), 1, file);
      fclose(file);
    }
  }
}

/*
    Check level and speed
*/
void Tetris::check_level(Tetris* tetris) {
  if (tetris->gameinfo.level < 10 &&
      (tetris->gameinfo.score - (tetris->gameinfo.level * 600) >= 0)) {
    tetris->gameinfo.level++;
    tetris->gameinfo.speed++;
  }
}

}  // namespace s21
