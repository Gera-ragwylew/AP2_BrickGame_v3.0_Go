#include "fsm.h"

#include <time.h>

#include "backend.h"

void fsm(Tetris* tetris) {
  static clock_t last_fall = 0;
  int fall_delay_ms = 1100 - (tetris->gameinfo.speed * 100 - 10);
  if (fall_delay_ms < 50) {
    fall_delay_ms = 50;
  }
  clock_t current_time = clock();
  int elapsed_ms = (current_time - last_fall) * 1000 / CLOCKS_PER_SEC;

  switch (tetris->stateofmachine) {
    case GameStart:
      if (tetris->action == Start) {
        stats_init(tetris);
        score_init(tetris);
        new_brick(tetris->next_brick, RANDOMIZER);
        tetris->gameinfo.level = 1;
        tetris->stateofmachine = Spawn;
      } else if (tetris->action == Terminate) {
        tetris->stateofmachine = GameOver;
      }
      break;
    case Spawn:
      shift_to_center(tetris->next_brick);
      spawn(tetris->gameinfo.field, tetris->next_brick);
      brick_copy(tetris->current_brick, tetris->next_brick);
      new_brick(tetris->next_brick, RANDOMIZER);
      fill_array_zero(tetris->gameinfo.next, NEXT_SIZE, NEXT_SIZE);
      spawn(tetris->gameinfo.next, tetris->next_brick);
      tetris->stateofmachine = Moving;
      break;
    case Moving:
      if (tetris->action == Pause && tetris->gameinfo.pause == 0) {
        tetris->gameinfo.pause = 1;
        tetris->action = Start;
      } else if (tetris->action == Pause && tetris->gameinfo.pause == 1) {
        tetris->gameinfo.pause = 0;
        tetris->action = Start;
      } else if (tetris->action == Down) {
        elapsed_ms = fall_delay_ms;
      } else if (tetris->action == Terminate) {
        tetris->stateofmachine = GameOver;
      }
      if (tetris->gameinfo.pause != 1) {
        despawn(tetris->gameinfo.field, tetris->current_brick);
        brick_move(tetris->current_brick, tetris->action,
                   tetris->gameinfo.field);
        spawn(tetris->gameinfo.field, tetris->current_brick);
        if (elapsed_ms >= fall_delay_ms) {
          last_fall = current_time;
          tetris->stateofmachine = Shifting;
        } else {
          tetris->action = Start;
        }
      }
      break;
    case Shifting:
      despawn(tetris->gameinfo.field, tetris->current_brick);
      if (can_down(tetris->gameinfo.field, tetris->current_brick)) {
        coord_shift(tetris->current_brick, Y_CORDS, DOWN);
        tetris->stateofmachine = Moving;
      } else {
        tetris->stateofmachine = Attaching;
      }
      spawn(tetris->gameinfo.field, tetris->current_brick);
      break;
    case Attaching:
      tetris->stateofmachine = Spawn;
      if (tetris->action == Down) {
        tetris->action = Start;
      }
      int full_rows_counter = full_row(tetris->gameinfo.field);
      if (is_gameover(tetris->current_brick)) {
        tetris->stateofmachine = GameOver;
      } else if (full_rows_counter) {
        score_write(tetris, full_rows_counter);
        check_level(tetris);
      }
      break;
    case GameOver:
      if (tetris->gameinfo.level > 0) {
        memory_free(tetris->gameinfo.field, WINDOW_HEIGHT);
        memory_free(tetris->gameinfo.next, NEXT_SIZE);
        free(tetris->current_brick);
        free(tetris->next_brick);
      }
      tetris->gameinfo.level = -1;
      break;
    default:
      break;
  }
}
