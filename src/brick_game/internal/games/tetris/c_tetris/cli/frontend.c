#include "frontend.h"

WINDOW* create_newwin(int height, int width, int starty, int startx) {
  WINDOW* local_win;
  local_win = newwin(height, width, starty, startx);
  return local_win;
}

void print_back(WINDOW* local_win) {
  box(local_win, 0, 0);
  mvwvline(local_win, 1, 21, ACS_VLINE, 20);
  mvwhline(local_win, 21, 1, ACS_HLINE, 32);
  mvwaddstr(local_win, 2, 25, "SCORE");
  mvwaddstr(local_win, 5, 23, "HI- SCORE");
  mvwaddstr(local_win, 8, 25, "NEXT");
  mvwaddstr(local_win, 13, 25, "SPEED");
  mvwaddstr(local_win, 16, 25, "LEVEL");
  mvwaddstr(local_win, 22, 2, "S      START");
  mvwaddstr(local_win, 23, 2, "Q      QUITE");
  mvwaddstr(local_win, 24, 2, "A      ACTION");
  mvwaddstr(local_win, 25, 2, "SPACE  PAUSE");
  mvwaddch(local_win, 23, 26, ACS_UARROW);
  mvwaddch(local_win, 24, 24, ACS_LARROW);
  mvwaddch(local_win, 24, 28, ACS_RARROW);
  mvwaddch(local_win, 25, 26, ACS_DARROW);
}

void print_start_screen(WINDOW* local_win) {
  mvwaddstr(local_win, 5, 1, "######~#####~######~");
  mvwaddstr(local_win, 6, 1, "~~##~~~##~~~~~~##~~~");
  mvwaddstr(local_win, 7, 1, "~~##~~~####~~~~##~~~");
  mvwaddstr(local_win, 8, 1, "~~##~~~##~~~~~~##~~~");
  mvwaddstr(local_win, 9, 1, "~~##~~~#####~~~##~~~");

  mvwaddstr(local_win, 11, 1, "#####~~######~~####~");
  mvwaddstr(local_win, 12, 1, "##~~##~~~##~~~##~~~~");
  mvwaddstr(local_win, 13, 1, "#####~~~~##~~~~####~");
  mvwaddstr(local_win, 14, 1, "##~~##~~~##~~~~~~~##");
  mvwaddstr(local_win, 15, 1, "##~~##~######~~####~");

  // ######~#####~######~#####~~######~~####~
  // ~~##~~~##~~~~~~##~~~##~~##~~~##~~~##~~~~
  // ~~##~~~####~~~~##~~~#####~~~~##~~~~####~
  // ~~##~~~##~~~~~~##~~~##~~##~~~##~~~~~~~##
  // ~~##~~~#####~~~##~~~##~~##~######~~####~
}

void score_to_string(char* str, int score) {
  int number = 0;
  if (score <= 9999999) {
    for (int i = 6; i >= 0; i--) {
      number = score % 10;
      str[i] = number + 48;
      score = score / 10;
    }
  }
}

void print_new_info(GameInfo_t stats, WINDOW* local_win) {
  char score_str[8] = {0};
  char high_score_str[8] = {0};
  score_str[7] = '\0';
  high_score_str[7] = '\0';
  score_to_string(score_str, stats.score);
  score_to_string(high_score_str, stats.high_score);
  int k = 1;
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      if (stats.field[i][j] == 0) {
        mvwaddch(local_win, i + 1, j + k, ' ' | COLOR_PAIR(2));
        k++;
        mvwaddch(local_win, i + 1, j + k, ' ' | COLOR_PAIR(2));
      } else {
        mvwaddch(local_win, i + 1, j + k, '#' | COLOR_PAIR(3));
        k++;
        mvwaddch(local_win, i + 1, j + k, '#' | COLOR_PAIR(3));
      }
    }
    k = 1;
  }
  k = 1;
  for (int i = 0; i < NEXT_SIZE; i++) {
    for (int j = 0; j < NEXT_SIZE; j++) {
      if (stats.next[i][j] == 0) {
        mvwaddch(local_win, i + 9, j + k + 23, ' ' | COLOR_PAIR(2));
        k++;
        mvwaddch(local_win, i + 9, j + k + 23, ' ' | COLOR_PAIR(2));
      } else {
        mvwaddch(local_win, i + 9, j + k + 23, '#' | COLOR_PAIR(3));
        k++;
        mvwaddch(local_win, i + 9, j + k + 23, '#' | COLOR_PAIR(3));
      }
    }
    k = 1;
  }
  if (stats.pause == 1) {
    mvwaddstr(local_win, 14, 1, "###   #  #  # ######");
    mvwaddstr(local_win, 15, 1, "#  # # # #  ##   #  ");
    mvwaddstr(local_win, 16, 1, "#  ##   ##  # #  ###");
    mvwaddstr(local_win, 17, 1, "### ######  #  # #  ");
    mvwaddstr(local_win, 18, 1, "#   #   ##  #   ##  ");
    mvwaddstr(local_win, 19, 1, "#   #   # ## ### ###");
  }
  mvwaddstr(local_win, 3, 31 - strlen(score_str), score_str);
  mvwaddstr(local_win, 6, 31 - strlen(high_score_str), high_score_str);
  mvwprintw(local_win, 14, 27, "%d", stats.speed);
  mvwprintw(local_win, 17, 27, "%d", stats.level);
}

void destroy_win(WINDOW* local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

// void state_log(Tetris* tetris) {
//     if (tetris->stateofmachine != GameOver) {
//         mvwprintw(stdscr, 3, 60, "LOGS");
//         mvwprintw(stdscr, 4, 50, "stateofmachine: %d",
//         tetris->stateofmachine); mvwprintw(stdscr, 5, 50, "action: %d",
//         tetris->action); mvwprintw(stdscr, 7, 50, "pause: %d",
//         tetris->gameinfo.pause); mvwprintw(stdscr, 8, 50, "brick_coords_Y:
//         "); mvwprintw(stdscr, 9, 50, "brick_coords_X: "); if
//         (tetris->current_brick != 0) {
//             for (int i = 0; i < BRICK_SIZE; i += 2) {
//                 mvwprintw(stdscr, 8, 66 + i, "%d", tetris->current_brick[i]);
//                 mvwprintw(stdscr, 9, 66 + i, "%d", tetris->current_brick[i +
//                 1]);
//             }
//         }
//     }
// }