#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
    Bricks coords for spawn before next field size fix.
    Before fix:
    - 7x7
    After fix:
    - 4x4
*/
// #define TEEWEE
//   { 1, 4, 1, 3, 0, 4, 1, 5 }
// #define HERO
//   { 0, 5, 0, 4, 0, 3, 0, 6 }
// #define SMASHBOY
//   { 0, 4, 0, 5, 1, 4, 1, 5 }
// #define ORANGE_RICKY
//   { 1, 4, 1, 3, 1, 5, 0, 5 }
// #define BLUE_RICKY
//   { 1, 4, 0, 3, 1, 3, 1, 5 }
// #define CLEVELAND_Z
//   { 1, 4, 0, 3, 0, 4, 1, 5 }
// #define RHODE_ISLAND_Z
//   { 1, 4, 1, 3, 0, 4, 0, 5 }

/*
    The coords in array goes like array[i] = y, array[i+1] = x
    First coord is pivot point for rotation
    The Spawn function and Rotate function uses this format
*/
#define TEEWEE \
  { 2, 1, 2, 0, 1, 1, 2, 2 }
#define HERO \
  { 1, 1, 1, 0, 1, 2, 1, 3 }
#define SMASHBOY \
  { 2, 1, 1, 1, 1, 2, 2, 2 }
#define ORANGE_RICKY \
  { 2, 1, 2, 0, 2, 2, 1, 2 }
#define BLUE_RICKY \
  { 2, 1, 1, 0, 2, 0, 2, 2 }
#define CLEVELAND_Z \
  { 2, 1, 1, 1, 1, 0, 2, 2 }
#define RHODE_ISLAND_Z \
  { 2, 1, 1, 1, 1, 2, 2, 0 }

#define WINDOW_HEIGHT 20
#define WINDOW_WIDTH 10
#define SUCCSES 0
#define ERROR 1
#define BRICK_SIZE 8

#define Y_CORDS 0
#define X_CORDS 1
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define NEXT_SIZE 4

#define RANDOMIZER (1 + rand() % 7)

typedef enum {
  Start = 0,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {
  GameStart = 0,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  GameOver
} State_of_machine;

typedef struct {
  int Teewee[8];
  int Hero[8];
  int Smashboy[8];
  int Orange_Ricky[8];
  int Blue_Ricky[8];
  int Cleveland_Z[8];
  int Rhode_Island_Z[8];
} Bricks;

typedef struct {
  GameInfo_t gameinfo;
  UserAction_t action;
  State_of_machine stateofmachine;
  int* current_brick;
  int* next_brick;
} Tetris;

Tetris* get_tetris();
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

int** init_matrix(int** matrix, int height, int width);
void memory_free(int** matrix, int size);
void fill_array_zero(int** matrix, int height, int width);
void stats_init(Tetris* tetris);
int score_init(Tetris* tetris);
void new_brick(int* brick, int random);
void brick_copy(int* src, int* other);
void spawn(int** matrix, int* array);
void despawn(int** matrix, int* array);
void brick_move(int* brick, UserAction_t state, int** matrix);
int can_right(int** matrix, int* brick, int shift);
int can_left(int** matrix, int* brick, int shift);
int can_down(int** matrix, int* brick);
int is_Smashboy(int* brick);
int is_Hero(int* brick);
void coord_shift(int* brick, int cords, int shift);
void rotate(int** matrix, int* brick, int size);
void fix_brick_coord(int* brick);
int rotate_check_down_wall(int* brick);
int rotate_check_up_wall(int* brick);
int rotate_check_right_wall(int* brick);
int rotate_check_left_wall(int* brick);
int rotate_check_field(int** matrix, int* brick);
int is_gameover(int* brick);
int is_attaching(int* brick, int** matrix);
int full_row(int** field);
void score_write(Tetris* tetris, int full_rows_counter);
void check_level(Tetris* tetris);
void shift_to_center(int* brick);

#endif