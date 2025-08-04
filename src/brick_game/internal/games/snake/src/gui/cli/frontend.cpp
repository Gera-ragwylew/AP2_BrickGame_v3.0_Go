#include "frontend.h"

int main(void) {
  WINDOW* my_win;

  ncurses_init();
  my_win = create_newwin();
  print_back(my_win);
  game_loop(my_win);
  destroy_win(my_win);
  endwin();

  return 0;
}

void ncurses_init() {
  srand(time(NULL));
  initscr();
  if (!has_colors()) {
    printf("color not sup");
  }
  start_color();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  init_pair(0, COLOR_BLACK, COLOR_BLACK);
  init_pair(1, COLOR_BLUE, COLOR_BLUE);
  init_pair(2, COLOR_RED, COLOR_RED);
  init_pair(3, COLOR_GREEN, COLOR_GREEN);
  init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(5, COLOR_CYAN, COLOR_CYAN);
  init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA);
  timeout(1);
}

void game_loop(WINDOW* my_win) {
  GameInfo_t stats;
  s21::Timer timer;
  int game = game_selection(my_win);
  userInput((UserAction_t)game, false);

  while (!is_end(stats)) {
    set_action();
    stats = updateCurrentState();
    // state_log(Game::CurrentGame);
    if (!is_end(stats)) {
      update_screen(stats, my_win, timer);
    }
    wrefresh(my_win);
  }

  if (stats.level == LOSE_LVL) {
    print_end_stub(my_win);
  } else if (stats.level == WIN_LVL) {
    print_win_stub(my_win);
  }
  wrefresh(my_win);
  sleep(1);
}

bool is_end(GameInfo_t stats) {
  return (stats.level == LOSE_LVL || stats.level == WIN_LVL) ? true : false;
}

void set_action() {
  switch (getch()) {
    case KEY_LEFT:
      userInput(Left, false);
      break;
    case KEY_RIGHT:
      userInput(Right, false);
      break;
    case KEY_UP:
      userInput(Up, false);
      break;
    case KEY_DOWN:
      userInput(Down, false);
      break;
    case 'a':
      userInput(Action, false);
      break;
    case ' ':
      userInput(Pause, false);
      break;
    case 'q':
      userInput(Terminate, false);
      break;
    case 's':
      userInput(Start, false);
      break;
    default:
      break;
  }
}

WINDOW* create_newwin() {
  WINDOW* local_win;
  int starty, startx, width, height;
  height = 2 + WINDOW_HEIGHT + 5;
  width = 3 + WINDOW_WIDTH * 2 + 11;
  starty = 2;
  startx = 4;
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
  mvwaddstr(local_win, 12, 25, "SPEED");
  mvwaddstr(local_win, 15, 25, "LEVEL");
  mvwaddstr(local_win, 22, 2, "S      START");
  mvwaddstr(local_win, 23, 2, "Q      QUIT");
  mvwaddstr(local_win, 24, 2, "A      ACTION");
  mvwaddstr(local_win, 25, 2, "SPACE  PAUSE");
  mvwaddch(local_win, 23, 26, ACS_UARROW);
  mvwaddch(local_win, 24, 24, ACS_LARROW);
  mvwaddch(local_win, 24, 28, ACS_RARROW);
  mvwaddch(local_win, 25, 26, ACS_DARROW);
}

// ######~#####~######~#####~~######~~####~
// ~~##~~~##~~~~~~##~~~##~~##~~~##~~~##~~~~
// ~~##~~~####~~~~##~~~#####~~~~##~~~~####~
// ~~##~~~##~~~~~~##~~~##~~##~~~##~~~~~~~##
// ~~##~~~#####~~~##~~~##~~##~######~~####~

void print_tetris_stub(WINDOW* local_win) {
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
}

// ~~~####~~##~~~~##~~~###~~~##~~##~#####~~
// ~~##~~~~~###~~~##~~##~##~~##~##~~##~~~~~
// ~~~####~~##~##~##~#######~####~~~####~~~
// ~~~~~~##~##~~~###~##~~~##~##~##~~##~~~~~
// ~~~####~~##~~~~##~##~~~##~##~~##~#####~~

void print_snake_stub(WINDOW* local_win) {
  mvwaddstr(local_win, 5, 1, "~~~####~~~##~~~~##~~");
  mvwaddstr(local_win, 6, 1, "~~##~~~~~~###~~~##~~");
  mvwaddstr(local_win, 7, 1, "~~~####~~~##~##~##~~");
  mvwaddstr(local_win, 8, 1, "~~~~~~##~~##~~~###~~");
  mvwaddstr(local_win, 9, 1, "~~~####~~~##~~~~##~~");

  mvwaddstr(local_win, 11, 1, "~~###~~~##~~##~#####");
  mvwaddstr(local_win, 12, 1, "~##~##~~##~##~~##~~~");
  mvwaddstr(local_win, 13, 1, "#######~####~~~####~");
  mvwaddstr(local_win, 14, 1, "##~~~##~##~##~~##~~~");
  mvwaddstr(local_win, 15, 1, "##~~~##~##~~##~#####");
}

void print_end_stub(WINDOW* local_win) {
  mvwaddstr(local_win, 5, 1, "#####~##~~~~##~####~");
  mvwaddstr(local_win, 6, 1, "##~~~~###~~~##~##~~#");
  mvwaddstr(local_win, 7, 1, "####~~##~##~##~##~~#");
  mvwaddstr(local_win, 8, 1, "##~~~~##~~~###~##~~#");
  mvwaddstr(local_win, 9, 1, "#####~##~~~~##~####~");
}

void print_win_stub(WINDOW* local_win) {
  mvwaddstr(local_win, 5, 1, "#~~#~~#~##~##~~~~##~");
  mvwaddstr(local_win, 6, 1, "#~~#~~#~##~###~~~##~");
  mvwaddstr(local_win, 7, 1, "#~###~#~##~##~##~##~");
  mvwaddstr(local_win, 8, 1, "###~###~##~##~~~###~");
  mvwaddstr(local_win, 9, 1, "~#~~~#~~##~##~~~~##~");
}

void print_pause_stub(WINDOW* local_win) {
  mvwaddstr(local_win, 14, 1, "###   #  #  # ######");
  mvwaddstr(local_win, 15, 1, "#  # # # #  ##   #  ");
  mvwaddstr(local_win, 16, 1, "#  ##   ##  # #  ###");
  mvwaddstr(local_win, 17, 1, "### ######  #  # #  ");
  mvwaddstr(local_win, 18, 1, "#   #   ##  #   ##  ");
  mvwaddstr(local_win, 19, 1, "#   #   # ## ### ###");
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

void update_screen(GameInfo_t stats, WINDOW* local_win, s21::Timer timer) {
  char score_str[8] = {0};
  char high_score_str[8] = {0};
  score_str[7] = '\0';
  high_score_str[7] = '\0';
  score_to_string(score_str, stats.score);
  score_to_string(high_score_str, stats.high_score);

  print_stats_field(stats, local_win);
  print_stats_next(stats, local_win);

  if (stats.pause == 1) {
    print_pause_stub(local_win);
  }
  mvwaddstr(local_win, 3, 31 - strlen(score_str), score_str);
  mvwaddstr(local_win, 6, 31 - strlen(high_score_str), high_score_str);
  mvwprintw(local_win, 13, 27, "%d", stats.speed);
  mvwprintw(local_win, 16, 27, "%d", stats.level);
  mvwprintw(local_win, 19, 23, "%02d:%02d:%.f", timer.get_minutes(),
            timer.get_seconds(), timer.get_miliseconds());
}

void print_stats_field(GameInfo_t stats, WINDOW* local_win) {
  int k = 1;
  for (int i = 0; i < WINDOW_HEIGHT; i++) {
    for (int j = 0; j < WINDOW_WIDTH; j++) {
      mvwaddch(local_win, i + 1, j + k, ' ' | COLOR_PAIR(stats.field[i][j]));
      k++;
      mvwaddch(local_win, i + 1, j + k, ' ' | COLOR_PAIR(stats.field[i][j]));
    }
    k = 1;
  }
}

void print_stats_next(GameInfo_t stats, WINDOW* local_win) {
  int k = 1;
  for (int i = 0; i < 2; i++) {
    for (int j = 3; j < 7; j++) {
      mvwaddch(local_win, i + 9, j + k + 21,
               ' ' | COLOR_PAIR(stats.next[i][j]));
      k++;
      mvwaddch(local_win, i + 9, j + k + 21,
               ' ' | COLOR_PAIR(stats.next[i][j]));
    }
    k = 1;
  }
}

void destroy_win(WINDOW* local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

// void state_log(Game* game) {
//     if (game->stateofmachine != 5) {
//         mvwprintw(stdscr, 3, 60, "LOGS");
//         mvwprintw(stdscr, 4, 50, "stateofmachine: %d", game->stateofmachine);
//         mvwprintw(stdscr, 5, 50, "action: %d", game->action);
//         mvwprintw(stdscr, 7, 50, "pause: %d", game->gameinfo.pause);
//         mvwprintw(stdscr, 8, 50, "brick_coords_Y: ");
//         mvwprintw(stdscr, 9, 50, "brick_coords_X: ");
//         // if (game->current_brick != 0) {
//         //     for (int i = 0; i < BRICK_SIZE; i += 2) {
//         //         mvwprintw(stdscr, 8, 66 + i, "%d",
//         game->current_brick[i]);
//         //         mvwprintw(stdscr, 9, 66 + i, "%d", game->current_brick[i +
//         1]);
//         //     }
//         // }
//     }
// }

int game_selection(WINDOW* local_win) {
  int game = Tetris;
  int switch_flag = 1;
  mvwaddch(local_win, 10, 8, '>');
  mvwaddstr(local_win, 10, 9, "Tetris");
  mvwaddstr(local_win, 11, 9, "Snake");
  int input = getch();
  while (input != 'a' && input != 'q' && input != 's') {
    if (input == KEY_UP || input == KEY_DOWN) {
      game = (game == Tetris ? Snake : Tetris);
      switch_flag = (switch_flag == 0 ? 1 : 0);
      mvwaddch(local_win, 10 + switch_flag, 8, ' ');
      mvwaddch(local_win, 11 - switch_flag, 8, '>');
    }
    wrefresh(local_win);
    input = getch();
  }
  mvwaddstr(local_win, 9, 8, "          ");
  mvwaddstr(local_win, 10, 8, "          ");
  wrefresh(local_win);
  if (game == Tetris) {
    print_tetris_stub(local_win);
  } else if (game == Snake) {
    print_snake_stub(local_win);
  }
  wrefresh(local_win);
  sleep(1);
  return game;
}