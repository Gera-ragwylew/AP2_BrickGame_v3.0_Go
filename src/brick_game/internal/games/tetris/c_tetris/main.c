#include "tetris.h"

int main(void) {
  WINDOW* my_win;
  int starty, startx, width, height;

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
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
  timeout(1);

  height = 2 + WINDOW_HEIGHT + 5;
  width = 3 + WINDOW_WIDTH * 2 + 11;
  starty = 2;
  startx = 4;
  my_win = create_newwin(height, width, starty, startx);
  game_loop(my_win);
  destroy_win(my_win);
  endwin();

  return 0;
}

void game_loop(WINDOW* my_win) {
  GameInfo_t stats;
  bool break_flag = true;

  while (break_flag) {
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
    stats = updateCurrentState();
    print_back(my_win);
    if (stats.level == -1) {
      break_flag = false;
    } else if (stats.level == 0) {
      print_start_screen(my_win);
    } else if (stats.level > 0) {
      print_new_info(stats, my_win);
    }
    wrefresh(my_win);
  }
}